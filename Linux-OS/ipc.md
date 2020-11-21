# 进程间通信

> 线程和进程一起讨论

## 消息传递

> 同样是囫囵吞枣，需要一行一行代码好好看~

### 管道

- 半双工(某些系统支持全双工)：数据只支持单向流动
- 使用者需具有公共祖先，更常见的是父子使用(FIFO无此局限)

```c
int pipe(init fd[2]);
```
其中`fd[0]`为读打开，`fd[1]`为写打开；写入到1，从0中读出

fork以后各自关闭一个fd，更严格控制读写操作的角色(APUE page 431)

`popen` & `pclose`； 
我们经常创建一个连接到另一个进程的管道，然后读其output或向其input发送数据。
创建一个管道，fork一个进程， 关闭未使用的管道端，执行一个shell命令，然后等待命令终止。

[天天都在用的管道命令里面到底怎么实现的？](https://zhuanlan.zhihu.com/p/47168082) 简单来说就是`pipe` + `dup2`

协同进程

> TODO: 类似`dup2`的高级IO函数

### FIFO

命名管道
FIFO是一种文件类型(???`stat`结构的`st_mode`成员)，因此类似创建文件

```c
int mkfifo(const char *path, mode_t mode);
int mkfifoat(int fd, const cahr *path, mode_t mode);
```

TODO

### 消息队列

消息的链接表，存储在内核中

## 同步

自旋锁、互斥锁、条件变量、读写锁、信号量

参考[线程](thread.md)的线程同步部分

## 共享内存

不需要在进程之间复制数据，因此是最快的一种IPC，但需要自己保证同步。

```c
int shmget(key_t key, size_t size, int flag);
```
返回一个共享存储的标识

## RPC

## socket