/*
 * @Descripttion: My project
 * @version: 1.0
 * @Author: weipeng
 * @Date: 2022-04-08 16:32:18
 * @LastEditors: weipeng
 * @LastEditTime: 2022-04-08 16:32:18
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    //  1.判断管道文件是否存在
    int ret = access("pipe2chat1", F_OK);
    if(ret == -1)
    {
        printf("管道不存在，创建管道\n");

        ret = mkfifo("pipe2chat1", 0064);

        if(ret == -1)
        {
            perror("mkfifo");
            exit(0);
        }
    }

    ret = access("pipe2chat2", F_OK);
    if(ret == -1)
    {
        printf("管道不存在，创建管道\n");

        ret = mkfifo("pipe2chat2", 0064);

        if(ret == -1)
        {
            perror("mkfifo");
            exit(0);
        }
    }

    // 2.只写方式打开管道1
    int fdw = open("pipe2chat1", O_WRONLY);
    if(fdw == -1)
    {
        perror("open");
        exit(0);
    }
    printf("打开管道fifo1成功，等待写入...\n");
    
    // 3.只读方式打管道2
    int fdr = open("pipe2chat2", O_RDONLY);
    if(fdr == -1)
    {
        perror("open");
        exit(0);
    }
    printf("打开管道fifo2成功，等待读取...\n");

    
    pid_t pid = fork();
    //父进程，写fifo1
    if(pid > 0)
    {
        char bufw[128];
        while(1)
        {
            memset(bufw, 0, 128);
            fgets(bufw, 128, stdin);
            int ret2 = write(fdw, bufw, strlen(bufw));
            if(ret2 == -1) {
                perror("write");
                exit(0);
            }
        }
    }
    //子进程，读fifo2
    else if(pid == 0)
    {
        char bufr[128];
        while (1)
        {
            memset(bufr, 0 ,128);
            int ret3 = read(fdr, bufr, 128);
            if(ret3 <= 0) {
                perror("read");
                break;
            }
            printf("buf: %s\n", bufr);
        }
    }
    
    close(fdr);
    close(fdw);

    return 0;
}