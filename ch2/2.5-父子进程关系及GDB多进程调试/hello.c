#include <stdio.h>
#include <unistd.h>

int main() {

    printf("begin\n");

    if(fork() > 0) {

        printf("我是父进程：pid = %d, ppid = %d\n", getpid(), getppid());
 
        int i;
        for(i = 0; i < 10; i++) {
            printf("i = %d\n", i);
            sleep(1);
        }

    } else {

        printf("我是子进程：pid = %d, ppid = %d\n", getpid(), getppid());
        
        int j;
        for(j = 0; j < 10; j++) {
            printf("j = %d\n", j);
            sleep(1);
        }
    }

    return 0;
}

/*
父子进程之间的关系：
        区别：
            1.fork()函数的返回值不同
                父进程中: >0 返回的子进程的ID
                子进程中: =0
            2.pcb中的一些数据
                当前的进程的id pid
                当前的进程的父进程的id ppid
                信号集

        共同点：
            某些状态下：子进程刚被创建出来，还没有执行任何的写数据的操作
                - 用户区的数据
                - 文件描述符表
        
        父子进程对变量是不是共享的？
            - 刚开始的时候，是一样的，共享的。如果修改了数据，不共享了。
            - 读时共享（子进程被创建，两个进程没有做任何的写的操作），写时拷贝。
            - 所以父子进程不能使用变量进行通信
*/