/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: weipeng
 * @Date: 2022-05-18 14:47:05
 * @LastEditors: weipeng
 * @LastEditTime: 2022-05-19 19:59:37
 */

#include "http_conn.h"
#include <pthread.h>
#include <exception>
#include <semaphore.h>
#include <pthread.h>
#include <exception>
#include <list>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>

int http_conn::m_epollfd = -1;
int http_conn::m_user_count = 0;

// 设置文件描述符非阻塞
void setnonblocking(int fd) {
    int old_flag = fcntl(fd, F_GETFL);
    int new_flag = old_flag | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_flag);
}

// 添加需要监听的文件描述符到epoll中
void addfd(int epollfd, int fd, bool one_shot) {
    epoll_event event;
    event.data.fd = fd;
    // event.events = EPOLLIN | EPOLLRDHUP;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;

    if (one_shot) {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

// 从epoll中删除需要监听的文件描述符
void removefd(int epollfd, int fd) {
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

// 修改epoll中的文件描述符,主要目的是重置socket上的EPOLLONESHOT事件,以确保下一次可读时, EPOLLIN事件能被触发
void modfd(int epollfd, int fd, int ev) {
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &event);
}

// 初始化连接
void http_conn::init(int sockfd, const sockaddr_in & addr) {
    m_sockfd = sockfd;
    m_address = addr;

    // 设置端口复用
    int reuse = 1;
    setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // 添加到epoll对象中
    addfd(m_epollfd, m_sockfd, true);
    m_user_count++;     // 连接的用户数++

    init();
}

void http_conn::init() {
    m_check_state = CHECK_STATE_REQUESTLINE;        // 初始化状态为解析请求首行
    m_checked_index = 0;
    m_start_line = 0;
    m_read_index = 0;

    m_method = GET;
    m_url = 0;
    m_version = 0;
    m_linger = false;

    m_content_length = 0;
    m_read_idx = 0;
    m_checked_idx = 0;

    bzero(m_read_buf, READ_BUFFER_SIZE);
}

// 关闭连接
void http_conn::close_conn() {
    if(m_sockfd != -1) {
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        m_user_count--;     // 连接的客户端数减一
    }
}

// 循环读取客户数据，直到无数据可读或者对方关闭连接
bool http_conn::read() {
    if (m_read_index >= READ_BUFFER_SIZE) return false;

    // 读取到的字节
    int bytes_read = 0;
    while (true) {
        bytes_read = recv(m_sockfd, m_read_buf + m_read_index, READ_BUFFER_SIZE - m_read_index, 0);
        
        if (bytes_read == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 没有数据
                break;
            }
            return false;
        } else if (bytes_read == 0) {
            // 对方关闭连接
            return false;
        }
        m_read_index += bytes_read;
    }
    std::cout << "读取到数据:" << m_read_buf << std::endl;
    return true;
}

// 解析http请求
// 主状态机,解析请求
http_conn::HTTP_CODE http_conn::process_read() {
    
    // 定义初始状态
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;

    char * text = 0;

    while (((line_status = parse_line()) == LINE_OK) || 
            ((m_check_state == CHECK_STATE_CONTENT) && (line_status == LINE_OK))) {
                // 解析到了一行完整的数据， 或者解析到了请求体也是完成的数据

                // 获取一行数据
                text = get_line();

                m_start_line = m_checked_index;
                std::cout << "得到一行http信息: " << text << std::endl;

                // 根据主状态机不同的情况做不同的处理
                switch (m_check_state)
                {
                case CHECK_STATE_REQUESTLINE:
                    ret = parse_request_line(text);
                    if (ret == BAD_REQUEST) return BAD_REQUEST;

                case CHECK_STATE_HEADER:
                    ret = parse_headers(text);
                    if (ret == BAD_REQUEST) return BAD_REQUEST;
                    else if (ret = GET_REQUEST) return do_request();
                    
                case CHECK_STATE_CONTENT:
                    ret = parse_content(text);
                    if (ret == GET_REQUEST) return do_request();
                    line_status = LINE_OPEN;
                    break;

                default:
                    return INTERNAL_ERROR;
                    break;
                }
                return NO_REQUEST;
    }
}
// 解析HTTP请求首行,获取请求方法、目标URL和HTTP版本
http_conn::HTTP_CODE http_conn::parse_request_line(char* text) {
    // 例如 GET /index.html HTTP/1.1
    m_url = strpbrk(text, " \t");       // strpbrk(string1, string2)方法寻找string1中string2第一次出现的位置
    // 变为 GET\0/index.html HTTP/1.1
    *m_url++ = '\0';

    char * method = text;
    if (strcasecmp(method, "GET") == 0) {   // 忽略大小写的比较
        m_method = GET;
    } else {
        return BAD_REQUEST;
    }
    // 变为 index.html HTTP/1.1
    m_version = strpbrk(m_url, " \t");
    if (!m_version) {
        return BAD_REQUEST;
    }
    // 变为 index.html\0HTTP/1.1
    *m_version++ = '\0';

    if (strcasecmp(m_version, "HTTP/1.1") != 0) {
        return BAD_REQUEST;
    }
    // http://192.168.1.1:10000/index.html
    if (strncasecmp(m_url, "http://", 7) == 0) {
        m_url += 7;     // 192.168.1.1:10000/index.html
        m_url = strchr(m_url, '/');     // /index.html
    }

    if (!m_url || m_url[0] != '/') {
        return BAD_REQUEST;
    } 

    m_check_state = CHECK_STATE_HEADER;    // 检查状态变成检查请求头
    return NO_REQUEST;
}

// 解析请求头
http_conn::HTTP_CODE http_conn::parse_headers(char* text) {
    // 遇到空行，表示头部字段解析完毕
    if (text[0] == '\0') {
        // 如果HTTP请求有消息体，还需要读取m_content_length字节的消息体
        // 状态机转移到CKECK_STATE_CONTENT
        if (m_content_length != 0) {
            m_check_state = CHECK_STATE_CONTENT;
            return NO_REQUEST;
        }
        // 否则说明我们已经得到了一个完整的HTTP请求
        return GET_REQUEST;
    } else if ((strncasecmp(text, "Connection:", 11))== 0) {
        // 处理connection的头部字段
        text += 11;
        text += strspn(text, "\t");
        if (strcasecmp(text, "keep-alive") == 0) {
            m_linger = true;
        }
    } else if ((strncasecmp(text, "Content-Length:", 15))== 0) {
        // 处理Content-Length的头部字段
        text += 15;
        text += strspn(text, "\t");
        m_content_length = atol(text);
    } else if ((strncasecmp(text, "Host:", 5))== 0) {
        // 处理Host的头部字段
        text += 55;
        text += strspn(text, "\t");
        m_host = text;
    } else {
        std::cout << "UNKNOWN HEADER: " << text << std::endl;
    }
    
    
}
// 我们没有真正解析HTTP的请求体，只是判断它是否被完整读入
http_conn::HTTP_CODE http_conn::parse_content(char* text) {
    if (m_read_idx >= (m_content_length + m_checked_idx)) {
        text[m_content_length] = '\0';
        return GET_REQUEST;
    }
    return NO_REQUEST;
}
// 解析具体的某行，判断依据是 \r\n
http_conn::LINE_STATUS http_conn::parse_line() {

    char temp;

    for (; m_checked_index < m_read_index; m_checked_index++) {
        temp = m_read_buf[m_checked_index];
        if (temp == '\r') {
            if ((m_checked_index + 1) == m_read_index) {
                return LINE_OPEN;
            } else if (m_read_buf[m_checked_index + 1] == '\n') {
                m_read_buf[m_checked_index++] = '\0';
                m_read_buf[m_checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        } else if (temp == '\n') {
            if ((m_checked_index > 1) && (m_read_buf[m_checked_index - 1] == '\r')) {
                m_read_buf[m_checked_index - 1] = '\0';
                m_read_buf[m_checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
        return LINE_OPEN;
    }

}

http_conn::HTTP_CODE http_conn::do_request() {
    // /root/nowcoderLinuxProject/webServer/resources
}

// 由线程池中的工作线程调用,这是处理http请求的入口函数
void http_conn::process() {
    std::cout << "解析请求，创建响应" << std::endl;

    // 解析http请求
    HTTP_CODE read_ret = process_read();
    if (read_ret == NO_REQUEST) {
        modfd(m_epollfd, m_sockfd, EPOLLIN);
        return;
    }


    // 生成响应
    

}

// 非阻塞的写
bool http_conn::write() {
    std::cout << "一次性写完数据" << std::endl;
    return true;
}