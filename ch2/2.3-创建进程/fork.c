/*
    #include <sys/types.h>
    #include <unistd.h>

    pid_t fork(void);
        函数的作用：用于创建子进程。
        返回值：
            fork()的返回值会返回两次。一次是在父进程中，一次是在子进程中。
            在父进程中返回创建的子进程的ID, 在子进程中返回0

            如何区分父进程和子进程：通过fork的返回值。
            在父进程中返回-1，表示创建子进程失败，并且设置errno
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main() {

    int num = 10;

    // 创建子进程
    pid_t pid = fork();

    printf("pid : %d\n", pid);
    // 判断是父进程还是子进程
    if(pid > 0) {
        // 如果大于0，返回的是创建的子进程的进程号，当前是父进程
        printf("i am parent process, pid : %d, ppid : %d\n", getpid(), getppid());

        printf("parent num : %d\n", num);
        num += 10;
        printf("parent num += 10 : %d\n", num);


    } else if(pid == 0) {
        // 当前是子进程
        printf("i am child process, pid : %d, ppid : %d\n", getpid(),getppid());
       
        printf("child num : %d\n", num);
        num += 100;
        printf("child num += 100 : %d\n", num);
    }

    // for循环
    for(int i = 0; i < 3; i++) {
        printf("i : %d , pid : %d\n", i , getpid());
        sleep(1);
    }

    return 0;
}

/*
实际上，更准确来说，Linux 的 fork() 使用是通过写时拷贝 (copy- on-write) 实现。
写时拷贝是一种可以推迟甚至避免拷贝数据的技术。
内核此时并不复制整个进程的地址空间，而是让父子进程共享同一个地址空间。
只用在需要写入的时候才会复制地址空间，从而使各个进行拥有各自的地址空间。
也就是说，资源的复制是在需要写入的时候才会进行，在此之前，只有以只读方式共享。
注意：fork之后父子进程共享文件，
fork产生的子进程与父进程相同的文件文件描述符指向相同的文件表，引用计数增加，共享文件偏移指针。
*/