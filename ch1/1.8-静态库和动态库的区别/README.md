/*
静态库
    ◼ 优点：
        ◆ 静态库被打包到应用程序中加载速度快
        ◆ 发布程序无需提供静态库，移植方便
    ◼ 缺点：
        ◆ 消耗系统资源，浪费内存
        ◆ 更新、部署、发布麻烦
动态库
    ◼ 优点：
        ◆ 可以实现进程间资源共享（共享库），其在内存中动态加载，其他应用程序使用时不需要重新加载
        ◆ 更新、部署、发布简单
        ◆ 可以控制何时加载动态库
    ◼ 缺点：
        ◆ 加载速度比静态库慢
        ◆ 发布程序时需要提供依赖的动态库
*/