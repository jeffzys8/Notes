# linux网络编程基础api

> 这里的代码要跟着敲一遍呀

- [socket地址API](socket-addr.md)


# socket连接操作

## `socket`

`socket`创建文件描述符
```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```

- `domain`: 指定协议族(=地址族), `PF_INET`, `PF_INET6`, `PF_UNIX`
- `type`: 指定服务类型, `SOCK_STREAM`, `SOCK_DATAGRAM`
  - 对于TCP/IP协议族而言则分别是 TCP 和 UDP
  - `&`来控制socket: `SOCK_NONBLOCK`, `SOCK_CLOEXEC`; 分别表示创建非阻塞socket、fork后子进程关闭该socket; 这些属性可以通过别的调用设置(`fcntl`)
- `protocol` = 0

成功返回一个`FD`，失败返回 `-1` 并设置 `errno`

TODO: errno又出现了

## `bind`

将一个socket与socket地址绑定(又叫命名);

server通常需要`bind`; 而client通常匿名，由OS自动分配socket地址

```c
#include <sys/types.h>
#include <sys/socket.h>

int bind (int sockfd, const struct sockaddr* addr, socklen_t addrlen);
```

成功返回`0`，失败返回`-1`并设置`errno`:
- `EACCES`: 被绑定的地址受保护，比如普通用户将`socket`绑定到知名服务端口(0~1023)
- `EADDRINUSE`: 被绑定的地址正在使用中，例如将socket绑定到一个处于`TIME_WAIT`状态的地址

TODO: `const struct sockaddr*` 不会放不下IPv6地址吗，不应该用`sockaddr_storage`?

TODO: `bind`可以不指定IP&port

## `listen`

server在`bind`以后还不能接受client连接，还需要通过`listen`系统调用来创建监听队列存放待处理的client连接

```c
#include <sys/socket.h>
int listen (int sockfd, int backlog);
```

成功时返回`0`，失败时返回`-1`并设置`errno`
- `backlog` 指示内核监听队列的最大长度。超出队列server不受理，而client会收到 `ECONNREFUSED` errno信息; 
- `backlog`表示处于 `ESTABLISHED`(完全连接)的socket上限, 处于 `SYN_RCVD`(半连接)状态的socket上限由 `proc/sys/net/ipv4/tcp_max_syn_backlog`参数定义
- 监听队列中完整连接的上限通常比`backlog`值略大?? (含义不明的模糊因子)


TODO: 示例listen程序

## `accept`

`accept`从前面`listen`创建的监听队列中取出一个连接进行处理

成功时返回一个新的连接socket

对对端网络断开毫不知情，只是从监听队列中取出连接，而不管连接处于何种状态(ESTABLISHED或者CLOSE_WAIT)

## `connect`

client直接使用`socket()`返回的`sockfd`与server建立连接，该`sockfd`唯一标识这个连接

## `close`

`close`并非总是立即关闭一个连接，而是将该fd的引用计数-1

多进程中，`fork`默认子进程打开了父进程的fd，因此引用计数+1(前面的`SOCK_CLOEXEC`可在执行`fork & exec`后避免这个问题)

使用`shutdown`系统调用可以直接关闭socket，并可以设置其行为

# 未整理



recv; send; /TCP

> read, write 同样可以用于socket, 但recv和send增加了flags进行额外控制

recvfrom; sendto; /UDP

recvmsg; 

sockatmark; /紧急数据

getsockname; getpeername; /获取本地、远端socket地址

getsockopt; setsockopt; /读取和设置socket fd属性(各层协议的具体值, header?)
这一部分具体展开讲了；以后要回看(包括取消TIME_WAIT，TCP缓冲区大小等)

gethostbyname, gethostbyaddr; /`/etc/hosts`, DNS

getservbyname, getservbyport; /`/etc/services`

getaddrinfo

getnameinfo