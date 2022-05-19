<!--
 * @Descripttion: 项目的几点优化
 * @version: 1.0
 * @Author: weipeng
 * @Date: 2022-05-19 21:48:54
 * @LastEditors: weipeng
 * @LastEditTime: 2022-05-19 22:29:57
-->

# 1. 对长时间不主动关闭的客户端自动关闭连接
对于不活跃的连接，如果不及时关闭会一直霸占文件描述符。新的连接请求便无法实现
定时检测不活跃的连接，将其关闭

# 2. to be continued

1. 现在只支持get请求，可以添加port请求

2. 加入数据库相关的功能

3. 测试使用Reactor模式

4. 测试使用epoll的LT/ET模式