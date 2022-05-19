/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: weipeng
 * @Date: 2022-05-18 14:45:07
 * @LastEditors: weipeng
 * @LastEditTime: 2022-05-19 16:39:07
 */

#ifndef HTTPCONNECTION_H
#define HTTPCONNECTION_H
#include "locker.h"
#include <pthread.h>
#include <exception>
#include <semaphore.h>
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
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <sys/uio.h>



class http_conn {
public:

    static int m_epollfd;           // 所有socket上的事件都被注册到同一个epoll上
    static int m_user_count;        // 统计用户数量
    static const int READ_BUFFER_SIZE = 2048;
    static const int WRITE_BUFFER_SIZE = 1024;

     // HTTP请求方法，这里只支持GET
    enum METHOD {GET = 0, POST, HEAD, PUT, DELETE, TRACE, OPTIONS, CONNECT};
    
    /*
        解析客户端请求时，主状态机的状态(请求报文的三个部分)
        CHECK_STATE_REQUESTLINE:当前正在分析 请求行
        CHECK_STATE_HEADER:当前正在分析 头部字段
        CHECK_STATE_CONTENT:当前正在解析 请求体
    */
    enum CHECK_STATE { CHECK_STATE_REQUESTLINE = 0, CHECK_STATE_HEADER, CHECK_STATE_CONTENT };
    
    /*
        服务器处理HTTP请求的可能结果，报文解析的结果
        NO_REQUEST          :   请求不完整，需要继续读取客户数据
        GET_REQUEST         :   表示获得了一个完成的客户请求
        BAD_REQUEST         :   表示客户请求语法错误
        NO_RESOURCE         :   表示服务器没有资源
        FORBIDDEN_REQUEST   :   表示客户对资源没有足够的访问权限
        FILE_REQUEST        :   文件请求,获取文件成功
        INTERNAL_ERROR      :   表示服务器内部错误
        CLOSED_CONNECTION   :   表示客户端已经关闭连接了
    */
    enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, NO_RESOURCE, FORBIDDEN_REQUEST, FILE_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION };
    
    // 从状态机的三种可能状态，即行的读取状态，分别表示
    // 1.读取到一个完整的行 2.行出错 3.行数据尚且不完整
    enum LINE_STATUS { LINE_OK = 0, LINE_BAD, LINE_OPEN };

    http_conn() {}

    ~http_conn() {}

    // 处理客户端请求并响应
    void process();
    // 初始化新接收的连接
    void init(int sockfd, const sockaddr_in & addr);

    // 关闭连接
    void close_conn();

    // 非阻塞的读
    bool read();

    // 非阻塞的写
    bool write();


private:
    int m_sockfd;           // 该http连接的socket
    sockaddr_in m_address;  // 通信的socket地址

    char m_read_buf[READ_BUFFER_SIZE];  // 读缓冲区
    int m_read_index; // 标识读缓冲区及读入的客户端数据的最后一个字节的下一个位置

    int m_checked_index;        // 当前正在分析的字符在读缓冲区的位置
    int m_start_line;           // 当前正在解析的行的起始位置

    char * m_url;       // 请求目标文件的文件名
    char * m_version;       // 协议版本，只支持HTTP1.1
    METHOD m_method;       // 请求方法

    char * m_host;  // 主机名
    bool m_linger;  // http请求是否要保持连接


    CHECK_STATE m_check_state;  // 主状态机当前所处的状态

    void init();        // 初始化连接其余的信息

    // 解析http请求
    HTTP_CODE process_read();
    HTTP_CODE parse_request_line(char* text);        // 解析请求首行
    HTTP_CODE parse_headers(char* text);           // 解析请求头
    HTTP_CODE parse_content(char* text);           // 解析请求体
    LINE_STATUS parse_line();                        // 解析具体的某行
    HTTP_CODE do_request();

    char * get_line() { return m_read_buf + m_start_line; }
};

#endif