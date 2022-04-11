/*
库是一种特殊的程序，不能单独运行
静态库和动态库（共享库）：
静态库：在程序的链接阶段被复制到程序中；
动态库：在程序的链接阶段没有被复制到程序中，而是程序在运行时由系统动态加载到内存中供程序调用；
库的好处：1.代码保密 2.方便部署和分发

◼ 命名规则：
    ◆ Linux : libxxx.a
        lib : 前缀（固定）
        xxx : 库的名字，自己起
        .a : 后缀（固定）
    ◆ Windows : libxxx.lib

◼ 静态库的制作：
    ◆ gcc 使用命令 gcc -c .cpp文件  获得 .o 文件（库代码）
    ◆ 将 .o 文件打包，使用 ar 工具（archive）
        ar rcs libxxx.a xxx.o xxx.o
        r – 将文件插入备存文件中
        c – 建立备存文件
        s – 索引
◼ 动态库的使用
-I 表示去哪里搜索包含的头文件
-L 指定当前库的路径
-l 指定库的名称
gcc main.c -o app -I ./include/ -L ./lib/ -l calc

*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<iostream>
#include<sstream>
#include<stack> 
#include<set>
#include<vector>
#include<stdio.h>
#include<iomanip>
#include<algorithm>
#include<unordered_map>
// #include<priority_queue>

