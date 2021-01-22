# I/O 复用

IO复用的使用场景：
- 同时处理多个socket (TODO: client非阻塞connect)
- 同时处理用户输入和网络连接 (TODO: 聊天室程序)
- **server同时处理监听socket和连接socket**
- server同时处理TCP请求和UDP请求 (TODO: 回射服务器)
- server同时监听多个端口/处理多个服务(?) (xinetd服务器)

I/O复用仍然是同步IO，其调用是阻塞的。主要有`select`, `poll`, `epoll`三种

## select

在一段时间内，监听用户指定fd集合上的可读、可写、异常事件，在就绪时`select`调用返回

```c
#include <sys/select.h>

int select(int nfds, fd_set *readfds, fd_set *writefds,
          fd_set *exceptfds, struct timeval *timeout);
```

- `nfds` 应设为最大fd+1
- 将需要监控的可读、可写、异常的fd分别用`fd_set`记录传给`select`调用，`select`调用将修改这三个`fd_set`来告诉应用程序就绪情况

```c
/* fd_set 定义 */
#define __NFDBITS	(8 * (int) sizeof (__fd_mask))
typedef struct
{
    /* XPG4.2 requires this member name.  Otherwise avoid the name
       from the global namespace.  */
#ifdef __USE_XOPEN
    __fd_mask fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->fds_bits)
#else
    __fd_mask __fds_bits[__FD_SETSIZE / __NFDBITS];
# define __FDS_BITS(set) ((set)->__fds_bits)
#endif
} fd_set;
```
从定义`fd_set`可看出就是一个bits数为 `FD_SETSIZE`的数组(单个)；
直接交给应用程序去操作位运算过于繁琐，应使用下列宏来操作`fd_set`
```c
void FD_CLR(int fd, fd_set *set);
int  FD_ISSET(int fd, fd_set *set);
void FD_SET(int fd, fd_set *set);
void FD_ZERO(fd_set *set);
```

- `timeout`传入超时时间，并在返回时更新`timeout`指示调用花费的时间
```c
struct timeval {
   long    tv_sec;         /* seconds */
   long    tv_usec;        /* microseconds */
};
```
若`timeval`两个成员都设为0，`select`将立即返回; 若`timeval`传`NULL`，`select`将一直阻塞直至有fd就绪

- 成功时返回就绪的fd数(3个`fd_set`总和); 失败返回-1并设置`errno`; **在阻塞期间收到信号也返回-1，`errno`设为`EINTR`**

### fd就绪条件

## 资料

写的真的好：
https://segmentfault.com/a/1190000003063859

这个也很形象：
https://zhuanlan.zhihu.com/p/87843750

为什么IO复用高效:
https://www.v2ex.com/t/511406
