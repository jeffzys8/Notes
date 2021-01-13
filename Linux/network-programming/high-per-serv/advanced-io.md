# 高级I/O

讨论几个和网络编程相关的高级I/O函数，分为三类
- 用于创建描述符: `pipe`, `dup`, `dup2`
- 用于读写数据: `readv`, `writev`, `sendflie`, `mmap`, `munmap`, `splice`, `tee`
- 用于控制I/O行为和属性: `fcntl`

## pipe

创建用于进程间通信的管道

```c
#include <unistd.h>
int pipe(int fd[2]);
```

- `fd[1]`为写端(只能写)，`fd[0]`为读端(只能读)
- 两个fd默认阻塞 (TODO: 将两个fd设为非阻塞的行为)
- `fd[1]`引用计数为0时，读`fd[0]`将返回0(即`EOF`)
- `fd[0]`引用计数为0时，写`fd[1]`将失败并引发`SIGPIPE`信号
- 管道默认大小为65536Bytes，通过`fcntl`可修改

`socketpair`可用来创建一个可双向读写(两个fd都能读写)的管道
```c
int sv[2];
socketpair(PF_UNIX, SOCK_STREAM, 0, sv);
```

## dup 和 dup2

复制文件描述符，用于比如 将`STDIN`重定向到某个文件(键盘键入同时写入文件) / 将`STDOUT`输出到某个socket(输出终端同时输出到网络连接)

```c
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);
```

`dup` 返回一个新的FD, 且和`oldfd`指向相同的文件/管道/网络连接, 返回的FD都是尽量小; 而`dup2`区别就是返回的FD不小于`newfd`

示例程序-CGI服务器原理(关键代码段)
```c
close(STDOUT_FILENO);
dup(connfd); // `connfd` 是 `accept` 的一个连接
printf("abcd\n");
close(connfd);
```

上述程序首先关闭了标准输出(值为1)的fd，于是`dup`就会选择最小的可用fd，即为1; `printf`调用实际上是将字符串输出到值为1的FD，因此此处会将`abcd\n`输出到`connfd`对应的网络连接中

- TODO: 疑问: 第二个`close`将`connfd`所指向连接的引用计数-1，那`dup`的连接会不会关闭? 进而这个网络连接会不会关闭?
- TODO: 疑问: 有没有办法将关闭的STDOUT再开回来，或者说STDOUT的初始化流程是怎么样的

> TODO: 单就创建一个sock_addr --> bind --> listen 这一过程重复代码很多，可以写个库封装一下

# readv 和 writev

相当于简化版的 `sendmsg` 和 `recvmsg` (见[socket读写](basic-api/socket-read-write.md))

```c
#include <sys/uio.h>
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
```

[HTTP-response示例程序(using writev)](6-2_web_response.cpp)

## 未整理

readv, writev; fd分散读入内存, 分散内存集中写入fd

sendfile; fd间传递数据，零拷贝(无kernel<->user拷贝);

mmap; 文件直接映射到内存空间; munmap释放

splice; 也是类似sendflie的零拷贝操作

tee; 在两个管道fd间复制数据(零拷贝)，源文件数据还在(有点Linux的tee的感觉)

fcntl; 对fd的控制操作