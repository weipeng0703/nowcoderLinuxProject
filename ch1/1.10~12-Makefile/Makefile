# 1.定义变量
# src = sub.o add.o div.o mult.o main.o
# target = app

# 3.使用函数
src = $(wildcard ./*.c)
objs = $(patsubst %.c, %.o, $(src))		# 依赖文件
target = app

$(target) : $(objs)
	$(CC) $(objs) -o $(target)
# 若没找到上面的依赖，会继续向下查找其他依赖
# sub.o : sub.c
# 	gcc -c sub.c -o sub.o

# add.o : add.c
# 	gcc -c add.c -o add.o

# div.o : div.c
# 	gcc -c div.c -o div.o

# mult.o : mult.c
# 	gcc -c mult.c -o mult.o

# main.o : main.c
# 	gcc -c main.c -o main.o

# 2.模式匹配
%.o : %.c
	$(CC) -c $< -o $@

# 4.删除依赖文件
# 且将clean文件定义为伪目标
.PHONY : clean
clean :
	rm $(objs) -f