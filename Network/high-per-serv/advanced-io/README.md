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
- 两个fd默认阻塞
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

[示例程序-CGI服务器原理](6-1_dup_cgi.cpp)

关键代码段:
```c
close(STDOUT_FILENO);
dup(connfd); // `connfd` 是 `accept` 的一个连接
printf("abcd\n");
close(connfd);
```

- 上述程序首先关闭了标准输出(值为1)的fd，于是`dup`就会选择最小的可用fd，即为1; `printf`调用实际上是将字符串输出到值为1的FD，因此此处会将`abcd\n`输出到`connfd`对应的网络连接中
- **但后面`close(connfd)`是有问题的**，`connfd`所指向连接被`dup`复制了一份，因此单纯关闭`connfd`只是连接的引用计数-1，并没有关闭连接
- 如果使用`shutdown`或者对复制的socket也使用`close`则会关闭网络连接
- 但如此做会发现并没有将`abcd\n`发送到client，这是因为将标准输入关闭以后，类似重定向的操作，`printf`由行缓冲变成了全缓冲，该调用并不能使缓冲区占满，可参考[这个博客](https://blog.csdn.net/yanhuan136675/article/details/79394756)；解决方法是调用`fflush`刷新缓冲区
- TODO: 疑问: 有没有办法将关闭的STDOUT再开回来，或者说STDOUT的初始化流程是怎么样的

# readv 和 writev

相当于简化版的 `sendmsg` 和 `recvmsg` (见[socket读写](basic-api/socket-read-write.md))

```c
#include <sys/uio.h>
ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
```

[HTTP-response示例程序(using writev)](6-2_web_response.cpp)

# sendfile

直接在两个fd间传数据，而无需经过读写操作(这样会带来内核缓冲区<->用户缓冲区的数据拷贝)，是一种 **零拷贝** 函数；而这个函数顾名思义，是将一个真实文件的数据传到一个连接中

```c
#include <sys/sendfile.h>
ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
```

- `out_fd` 必须是支持`mmap`操作的fd，即指向真实的文件(不能是pipe、socket); `in_fd` 必须是一个socket (从Linux2.6.33开始已经可以是任意fd了)
- `offset` 指定读文件的偏移量，置为`NULL`表示从头; `count` 指定读取的字节数
- 返回成功传输的字节数

TODO: 用`sendfile`改造上面的6-2程序

# mmap 和 munmap

> 非常细的内存知识点，需要结合APUE和实际开发好好归纳; 这部分应该写到os里去

`mmap`申请一段内存，用于进程间通信的共享内存 / 将文件直接映射到内存中; `munmap`释放空间

```c
#include <sys/mman.h>

void *mmap(void *addr, size_t length, int prot, int flags,
          int fd, off_t offset);
int munmap(void *addr, size_t length);
```

`mmap`:
- 用户可使用`addr`指定要使用的内存段地址(as a hint)，如果设为`NULL`将会自动分配; `length`指定内存段长度(bytes); 需注意`addr`和`length`都必须是页对齐的(返回页大小:`sysconf(_SC_PAGE_SIZE)`)
- `prot`用于设置内存段的访问权限; 分别对应可读、可写、可执行、不可访问(详见man)
- `flags` 指定程序修改这段内存以后，对其他同样映射这段内存的程序 & 内存所对应的映射文件 的影响; 多个参数可按位或(`|`)，但其中有一些显然互斥不能共同设定(比如`MAP_SHARED`和`MAP_PRIVATE`); 列出一些典型的，其他见man
  |参数|含义|
  |--|--|
  |`MAP_SHARED`| 进程间共享这段内存，**修改对各进程可见，且会更新到映射的文件**(使用`msync()`配置更新时机)
  |`MAP_PRIVATE`| 创建一个copy-on-write的内存，也即**修改该内存对其他进程不可见，不更新到映射的文件**(修改文件是否影响内存是未定义的)
  |`MAP_ANONYMOUS`| 指定该内存空间**不是从文件映射**，并将内存全初始化为0; `fd`应设为-1，`offset`应设为0 (尽管通常这两个参数会被忽略)
  |`MAP_FIXED`| *Don't interpret addr as a hint*，**即分配的内存段起始位置必须正好为`addr`**，这要求`addr`是页对齐的(页大小为4096bytes) <br>**!慎用，使用前需详细阅读man相关提示以适应跨平台**
  |`MAP_HUGETLB`|使用大页表分配，默认值看`/proc/meminfo`的`Hugepagesize`，但更有意思的是两个宏(看看是怎么实现的):`MAP_HUGE_2MB`, `MAP_HUGE_1GB` (since Linux 3.8)
- `fd`是被映射文件的描述符，`offset`设置文件偏移量
- 返回指向目标区域的指针，失败返回`MAP_FAILED`(`(void*)-1`)并设置errno 

> `(void*)-1` = `(void*)(-1)` = `0xFFFFFFFF`

`munmap`则对应释放空间, 成功返回0, 失败返回-1并设置errno

# splice

和`sendfile`一样也是零拷贝操作，不过`splice`用于从管道导出数据或将数据发送到管道，也即input和output至少一个得是管道

```c
#include <fcntl.h>
ssize_t splice(int fd_in, loff_t *off_in, int fd_out,
        loff_t *off_out, size_t len, unsigned int flags);
```

- 如果`fd`(`fd_in`或`fd_out`, 下同)为pipe, 则对应`off`要设为NULL
- 如果`fd`不是pipe, 则对应`off`对应偏移量
> TODO: 如果有offset, 那被跳过的区域数据会怎么样，fd的偏移量还保持原位的话，再`read`会怎么样? 而另一方面，`write`又会怎么样? (可以写个pipe验证一下) (offset没弄明白怎么用)
> - 猜测由于fd偏移量保持不变, `fd_in`会将被跳过的区域读出; `fd_out`会从偏移位置覆盖?不确定(如果socket咋覆盖啊都发出去了)
- `len`指定移动数据的长度
- `flags`控制数据的移动方式(按位或`|`)
  - `SPLICE_F_MOVE`: 移动内存页而不是复制数据，想法挺好，可惜实现有问题现在失效了
  - `SPLICE_F_NONBLOCK`: `splice`被设为非阻塞，但如果操作的`fd`会阻塞就还是会阻塞 (TODO: `splice`非阻塞的效果)
  - `SPLICE_F_MORE`: 给内核的提示，指示后续会有更多的`splice`来移动数据，对于`fd_out`是socket的时候很有帮助
    > (see also the description of MSG_MORE in send(2), and the description of TCP_CORK in tcp(7))
- 成功时返回移动的字节数, 可能为0, 表示没有数据要移动(当`fd_in`为管道而该管道中无数据)

[示例-回射服务器程序](6-4_splice_reflect.cpp)

# tee

在管道之间复制数据(也即不消耗管道内的原始数据，仍可以被读), 所以input和output必须都是管道, 也是零拷贝操作。
```c
#include <fcntl.h>
ssize_t tee(int fd_in, int fd_out, size_t len, unsigned int flags);
```
- 除了`fd_in`和`fd_out`都必须是管道意外，别的参数含义和`splice`函数一致
- `flags`也同`splice`函数

[示例 - 使用splice和tee实现bash的tee命令](6-5_tee.cpp)

> 这是manpage的示例代码, 使用示例:`date |./a.out out.log | cat`

# fcntl

对fd的控制操作
```c
#include <fcntl.h>
int fcntl(int fd, int cmd, ... /* arg */ );
```
详见man手册

示例程序 - 将fd改为非阻塞：
```c
#include <unistd.h>
#include <fcntl.h>

int setnonblocking(int fd)
{
  int old_option = fcntl(fd, F_GETFL);
  int new_option = old_option | O_NONBLOCK;
  fcntl(fd, new_option);
  return old_option; /* 返回旧状态以备用 */
}

```