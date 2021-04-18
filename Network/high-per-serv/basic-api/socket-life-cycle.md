# socket基本生命周期函数

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


[示例代码 - listen+backlog](5-3_listen_backlog.cpp)

## `accept`

```c
#include <sys/types.h>
#include <sys/socket.h>
int accept (int sockfd, struct sockaddr* addr, socklen_t *addrlen); // socklen_t = uint32
```

`accept`从前面`listen`创建的监听队列中取出一个连接进行处理

成功时返回一个新的连接socket

**对对端网络断开毫不知情，只是从监听队列中取出连接，而不管连接处于何种状态(ESTABLISHED或者CLOSE_WAIT)**

[示例代码 - accept异常的连接](5-5_accpet_errconn.cpp)

修改代码并使用netcat辅助调试后的一些现象：
1. TIME_WAIT状态的地址无法再bind
2. TIME_WAIT状态的同src+port地址可以直接再connect

**按照 UNP(page37) 的说法，TIME_WAIT状态应该是防止连接的化身再次连接，所以上述结果应该是1可bind(未指定dst)，而2不可(同src同dst属于连接的化身)，但结果确是相反的，这是为什么呢？**


> 另外有一些关于 nc 使用的小插曲。
> - 一开始在 server listen --> nc client connect --> server active close 以后，我使用[回车]断开连接，发现没有TIME_WAIT。
> - 然后通过tcpdump抓包发现使用[回车]会在连接FIN-->ACK以后再发送一段带FLAG=PUSH的数据出去，然后server返回RST，连接异常终止因此无TIME_WAIT。下面就是nc-client端回车后tcpdump抓到的内容
```shell

$ tcpdump -l -i lo | grep 1235
13:38:27.318508 IP jeff-Inspiron-5557.6666 > jeff-Inspiron-5557.1235: Flags [P.], seq 1:2, ack 2, win 512, options [nop,nop,TS val 1165005161 ecr 1164994945], length 1
13:38:27.318545 IP jeff-Inspiron-5557.1235 > jeff-Inspiron-5557.6666: Flags [R], seq 353796924, win 0, length 0
```

[tcpdump笔记](../../../Linux/tools/tcpdump.md)

## `connect`

client直接使用`socket()`返回的`sockfd`与server建立连接，该`sockfd`唯一标识这个连接

```c
#include <sys/types.h>
#include <sys/socket.h>
int connect(int sockfd, const sockaddr *serv_addr, socklen_t addrlen);
```

成功返回0，失败返回-1并设置errno，常见的两个errno:
- `ECONNREFUSED` 目标端口不存在，连接被拒绝
- `ETIMEDOUT` 连接超时

## `close`

`close`并非总是立即关闭一个连接，而是将某个fd的引用计数-1

多进程中，`fork`默认子进程打开了父进程的fd，因此默认需要在父子进程中都对socket执行`close`才能使连接关闭(前面的`SOCK_CLOEXEC`可在执行`fork & exec`后避免这个问题)

```c
#include <unistd.h>
int close(int fd);
```

使用`shutdown`系统调用可以直接关闭socket，并可以设置其行为；从引用的库也可以看出，`close`是文件通用的，而`shutdown`是网络编程专用的。同上也是成功时返回0，失败时返回-1并设置errno。

```c
#include <sys/socket.h>
int shutdown(int sockfd, int how);
```

`how`的可选值：
- `SHUT_RD` 关闭socket读端，并抛弃接受缓冲区数据(那能不能写呢)
- `SHUT_WR` 关闭socket写端，已在发送缓冲区的数据会发送完；随后连接处于半关闭状态。
- `SHUT_RDWR` 同时关闭读写