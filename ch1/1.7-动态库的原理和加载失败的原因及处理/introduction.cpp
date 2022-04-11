/*
库是一种特殊的程序，不能单独运行
静态库和动态库（共享库）：
静态库：在程序的链接阶段被复制到程序中；
动态库：在程序的链接阶段没有被复制到程序中，而是程序在运行时由系统动态加载到内存中供程序调用；
库的好处：1.代码保密 2.方便部署和分发

◼ 命名规则：
    ◆ Linux : libxxx.so
        lib : 前缀（固定）
        xxx : 库的名字，自己起
        .so : 后缀（固定）
        在Linux下是一个可执行文件
    ◆ Windows : libxxx.dll

◼ 动态库的制作：
    ◼ 动态库的制作：
    ◆ gcc 得到 .o 文件，得到和位置无关的代码
    gcc -c –fpic/-fPIC a.c b.c
    ◆ gcc 得到动态库
    gcc -shared a.o b.o -o libcalc.so
◼ 动态库的使用
-I 表示去哪里搜索包含的头文件
-L 指定当前库的路径
-l 指定库的名称
gcc main.c -o app -I ./include/ -L ./lib/ -l calc

◼ 动态库加载失败解决方式：
将动态库路径添加到环境变量之中
env 显示当前环境变量
echo $LD_LIBRARY_PATH 显示指定的环境变量
ldd 文件名称 查看该文件的关联路径
（1）配置临时环境变量
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/root/nowCoder/ch1/1.6-动态库的制作和使用/library/lib
（2）永久配置环境变量
    1.用户级别(.bashrc在根目录底下;如果要在其他目录下直接看环境变量要用 ~/.bashrc)
    vim .bashrc(vim ~/.bashrc)
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/root/nowCoder/ch1/1.6-动态库的制作和使用/library/lib
    . .bashrc(source ~/.bashrc)     使配置的环境变量生效
    2.系统级别
    sudo vim /etc/profile
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/root/nowCoder/ch1/1.6-动态库的制作和使用/library/lib
    source /etc/profile
*/