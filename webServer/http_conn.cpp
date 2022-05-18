/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: weipeng
 * @Date: 2022-05-18 14:47:05
 * @LastEditors: weipeng
 * @LastEditTime: 2022-05-18 20:29:10
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
    event.events = EPOLLIN | EPOLLRDHUP;

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
}

// 关闭连接
void http_conn::close_conn() {
    if(m_sockfd != -1) {
        removefd(m_epollfd, m_sockfd);
        m_sockfd = -1;
        m_user_count--;     // 连接的客户端数减一
    }
}

// 非阻塞的读
bool http_conn::read() {
    std::cout << "一次性读完数据" << std::endl;
    return true;
}

// 非阻塞的写
bool http_conn::write() {
    std::cout << "一次性写完数据" << std::endl;
    return true;
}

// 由线程池中的工作线程调用,这是处理http请求的入口函数
void http_conn::process() {

    // 解析http请求,生成响应
    std::cout << "parse requests, create response" << std::endl;

}