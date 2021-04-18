# socket选项

`fcntl`是控制fd属性的通用POSIX方法，以下是专用于控制socket属性的：

```c
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

int getsockopt(int sockfd, int level, int optname,
                void *optval, socklen_t *optlen);
int setsockopt(int sockfd, int level, int optname,
                const void *optval, socklen_t optlen);
```

> For a description of the available socket options see socket(7) and the appropriate protocol man pages.

上面的意思是，具体每一个协议的选项到`socket(7)`里去查，关于socket选项都是在 DESCRIPTION --> Socket options 栏目内(manpage正在变得越来越重要)
- 比如我要查socket通用选项，就`man 7 socket` 然后就可以在里面找到 `SO_XXX` 形式的各种选项了；可以看到对应的`level`是`SOL_SOCKET`；
- 再比如TCP就是`man 7 tcp`；而可以看到对应的`level`是 `IPPROTO_TCP`

另外，不同socket选项的调用时机是不同的。比如有一些需要在bind前调用e.g. `SO_REUSEADDR`），而有一些是对accept后的socket才“真正”调用（e.g. TCP的最大报文段选项`TCP_MAXSEG`是在三次握手时的同步报文中传输的）。对于后面的情况所涉及的选项，Linux实现了这样的方案：只要对监听socket设置这些选项，accept获得的socket自动继承。包含的socket不一一展开，可以看书5.11

## SO_REUSEADDR 选项

```
SO_REUSEADDR
      Indicates that the rules used in validating addresses supplied in a bind(2) call should  allow  reuse  of  local  ad‐
      dresses.   For  AF_INET  sockets  this  means that a socket may bind, except when there is an active listening socket
      bound to the address.  When the listening socket is bound to INADDR_ANY with a specific port then it is not  possible
      to bind to this port for any local address.  Argument is an integer boolean flag.
```
使`bind`调用可以复用被TIME_WAIT状态连接占用的地址，但不对正处在监听状态的地址起作用。

[示例程序 - 复用accpet的代码](5-5_accpet_errconn.cpp)

> 重复一下之前的疑问：TIME_WAIT只限制`bind`但不限制远端同ip+port的`connect`?

## SO_RCVBUF 和 SO_SNDBUFF 选项

用于设定和获取当前的 **接收缓冲区** 和 **发送缓冲区** 的大小

需留意，当使用`setsockopt`设定这两个缓冲区大小的时候，对应的值会被double；获取的时候就是实际值不会/2；另外接收缓冲区的值最小是256B，发送缓冲区的值最小是2MB。这样能确保一个TCP连接有足够的空闲缓冲区处理拥塞。

可以通过`/proc/sys/net/ipv4/tcp_rmem`和`/proc/sys/net/ipv4/tcp_wmem`强制修改而没有最小值限制

TODO: 示例程序，包括了窗口扩大因子、首次ACK不立即扩大发送窗口等过程，值得留意

## SO_RCVLOWAT 和 SO_SNDLOWAT 选项

TCP判断socket是否可读或可写的低水位标记，一般是被 I/O复用 使用，默认都为 1byte

## SO_LINGER 选项

用于控制`close`在关闭TCP连接时的行为。

默认情况下，当使用`close`调用关闭一个socket时，函数将立即返回，内核TCP模块负责把发送缓冲区里剩余的数据发给对方。

该选项使用的是`linger`类型结构体：
```c
struct linger
{
    int l_onoff;		/* Nonzero to linger on close.  */
    int l_linger;		/* Time to linger.  */
};
```

- `l_onoff == 0`，使用默认行为close
- `l_onoff != 0 && l_linger == 0`，close立即返回，丢弃缓冲区剩余数据，并发给对方RST**指示异常终止连接**
- `l_onoff != 0 && l_linger > 0`，会等待`l_linger`时间发送数据。
  - 对于阻塞socket， `close`会阻塞这么长时间，如果没发完(TCP当然是得得到ACK才算发完)就返回-1并设置errno为`EWOULDBLOCK`l
  - 对于非阻塞socket，`close`会直接返回，我们需要根据其返回值(???)和errno来判断数据是否发送完 TODO: 怎么会还看返回值呢