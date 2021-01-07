# socket读写

用于文件读写的`read`和`write`同样适用于socket，但socket读写有专门的接口，可以对读写过程提供更好的控制。

## TCP数据读写

```c
#include <sys/types.h>
#include <sys/socket.h>
ssize_t recv(int __fd, void *__buf, size_t __n, int __flags);
ssize_t send(int __fd, const void *__buf, size_t __n, int __flags);
```

`recv`：
- 正常读返回读取数据的长度(可能小于给定的`len`，因此可能需要多次调用才能取得完整数据)
- 返回0意味着通信对方关闭了连接
- 出错时返回 -1 并设置errno

`send`:
- 成功时返回实际写入的数据长度
- 失败返回 -1 并设置errno

TODO: flags 和 带外数据的示例程序 + 5.9带外标记

flags 仅对当前的函数调用(`send`/`recv`)有效，永久改变socket属性需要用`setsockopt`

## UDP数据读写

```c
#include <sys/types.h>
#include <sys/socket.h>
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr.*dest_addr, socklen_t addrlen);
```

## 通用数据读写

```c
#include <sys/types.h>
#include <sys/socket.h>
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);

struct msghdr
{
    void *msg_name;		/* Address to send to/receive from.  */
    socklen_t msg_namelen;	/* Length of address data.  */

    struct iovec *msg_iov;	/* Vector of data to send/receive into.  */
    size_t msg_iovlen;		/* Number of elements in the vector.  */

    void *msg_control;		/* Ancillary data (eg BSD filedesc passing). */
    size_t msg_controllen;	/* Ancillary data buffer length.
				   !! The type should be socklen_t but the
				   definition of the kernel is incompatible
				   with this.  */

    int msg_flags;		/* Flags on received message.  */
};

struct iovec
{
    void *iov_base;	/* Pointer to data.  */
    size_t iov_len;	/* Length of data.  */
};
```

- `msg_name` 和 `msg_namelen` 分别指向 socket_addr结构地址和对应结构长度
- `msg_iov` 指向读取或发送的数据，这是一个`iovec`类型数组指针；因此对`sendmsg`是将分散内存集中发送，即“集中写”(gather write)，对应的`recvmsg`是“分散读”(scatter read)
- TODO: `msg_control` & `msg_controllen` 辅助数据，chap13讨论
- `msg_flags` 由外层flags设置，无需自己设置