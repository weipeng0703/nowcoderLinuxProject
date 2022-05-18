/*
 * @Descripttion: 
 * @version: 1.0
 * @Author: weipeng
 * @Date: 2022-05-18 14:45:07
 * @LastEditors: weipeng
 * @LastEditTime: 2022-05-18 19:59:44
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

};

#endif