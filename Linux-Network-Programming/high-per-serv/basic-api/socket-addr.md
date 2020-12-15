# socket地址API

## 字节序

网络字节统一使用 **大端字节序** (big endian byte order)，因此 **大端字节序也称为网络字节序**。

现代PC大多采用小端字节序，因此**小端字节序又称为主机字节序**。

以32-bit CPU为例解释字节序, 累加器每次能装载一个32-bit int
- 6(十进制) --> b0....0110(32-bit二进制)
- 大端
```
------------- (high addr mem)
0
0
...more 0...
1
1
0
------------- (low addr mem)
```
- **需留意大端虽然高位在低地址，但每字节(8-bit)里还是高位在高地址**，直接的现象就是从按位的地址空间来看这个数**不连续**，以`127.0.0.1`的整数表示为例：
```
for 127.0.0.1:
------------- (high addr mem)
0                                               1
0                   --> for this 8 byte, not    0
...more 0...                                    ...more 0...
1                                               0
-------------
0
...more 0...
0
-------------
0
...more 0...
0
-------------
0                                               1
1                   --> for this 8 byte, not    ...more 1...
...more 1...                                    1
1                                               0
------------- (low addr mem)
```
- 小端
```
------------- (high addr mem)
0
1
1
...more 0...
0
0
------------- (low addr mem)
```

[src 机器字节序判断](./5-1_byteorder.c)

Linux 提供4个函数完成两种字节序的转换，`htonl`, `htons`, `ntohl`, `ntohs`, **任何格式化的数据通过网络传输时都应使用这些函数转换字节序**

## 通用socket地址

通用socket地址解决在 `void*`(通用指针类型 from ANSI C)出现之前, socket函数需接受不同协议族的专用socket地址结构的问题。这要求调用socket函数时把专用socket地址强制类型转换成`sockaddr`
```c
#include <bits/socket.h>
struct sockaddr
{
    sa_family_t sa_family;  // u_int16_t
    char sa_data[14];       // u_int8_t
}
```
> 留意到Linux的定义和《Unix网络编程(3rd)》相比少了个`unint8_t sa_len`变量

`sa_family` 是地址族类型，通常与协议族类型对应(通常??); 常见协议族(protocol family/**domain**)和地址族的对应(这两者定义在`bits/socket.h`，具有完全相同的值，所以经常混用)

|协议族|地址族|描述|地址值含义|长度|
|--|--|--|--|--|
|PF_UNIX|AF_UNIX|UNIX本地域协议族|FILE path| max 108bytes|
|PF_INET|AF_INET|TCP/IPv4协议族|16bit-port + 32bit-IPv4addr| 6bytes|
|PF_INET6|AF_INET6|TCP/IPv6协议族|16bit-port <br>+ 32bit-流标识(?) <br>+ 128bit-IPv6addr <br>+ 32bit-范围ID(?)|26bits

显然从上面可以看到`sockkaddr`中的`char sa_data[14]`(14bytes)不够大，因此Linux定义了新的通用socket地址结构：
```c
#include <bits/socket.h>
struct sockaddr_storage
{
    sa_family_t sa_family;          // u_int16_t
    unsigned long int __ss_align;   // u_int64_t
    char __ss_padding[128 - sizeof(u_int16_t) - sizeof(__ss_align)]
}
```
该结构还用了`__ss_align`来提供不同地址结构的内存对齐
> 书里关于`__ss_padding`没有减去 `sizeof(u_int16_t)`, 但我看实际的定义里是有的; TODO: 这个通用地址的内存对齐是怎么做的?

## 专用socket地址

UNIX本地域协议族:
```c
#include <sys/un.h>
struct sockaddr_un
{
    sa_family_t sun_family;  // AF_UNIX
    char sun_path[108];     // FILE path
}
```

IPv4:
```c
#include <netinet/in.h>
struct in_addr
{
    u_int32_t s_addr;   // IPv4地址，需表示为网络字节序
};
struct sockaddr_in
{
    sa_family_t sin_family;     // AF_INET
    u_int16_t sin_port;			// port, 需表示为网络字节序
    struct in_addr sin_addr;	// IPv4地址struct(见上面定义)

    /* Pad to size of `struct sockaddr'.  */
    unsigned char sin_zero[sizeof (struct sockaddr) -
			   __SOCKADDR_COMMON_SIZE -
			   sizeof (in_port_t) -
			   sizeof (struct in_addr)];    // TODO: what is this for?
};
```

IPv6:
```c
#include <netinet/in.h>
struct in6_addr
{
    union
    {
        uint8_t __u6_addr8[16];
        uint16_t __u6_addr16[8];
        uint32_t __u6_addr32[4];
    } __in6_u;  /* 共用体的每个成员大小都是16bytes，因此所占内存为16bytes */
    /* TODO: 为什么in6_addr要用union存多种形式 */
};
struct sockaddr_in
{
    sa_family_t sin6_family;    // AF_INET6
    u_int16_t sin6_port;        // port, 需表示为网络字节序
    struct in6_addr sin6_addr;	// IPv6地址struct(见上面定义)
    u_int32_t sin6_flowinfo;    /* TODO: IPv6 流信息，应置为0 */
    u_int32_t sin6_scope_id;	/* TODO: IPv6 scope ID，尚处于试验阶段? */
};
```

## IP地址转换函数

一组用于【IP地址 字符串形式 <--> 二进制数形式(网络字节序)】的函数

IPv4 点十进制字符串(xxx.xxx.xxx.xxx)和网络字节序整数的转换:
```c
#include <arpa/inet.h>
in_addr_t inet_addr (const char *cp);
int inet_aton (const char *cp, struct in_addr *inp);
char* inet_ntoa (struct in_addr in);
```

- `inet_addr` 将点十进制的IPv4地址转化为网络字节序整数
  - 失败时返回`INADDR_NONE`，通常是32位为1的值，这意味着`255.255.255.255`不能由该函数处理
  - **已被废弃**
- `inet_aton` 功能和 `inet_addr` 一样，但会将结果存到`inp`指向的地址结构中，并以1(true)或0(false)返回执行状态
- `inet_ntoa` 即反过来，返回一个`char*`字符串，但需要注意，**该函数内部用一个静态变量存储转化结果，因此是不可重入的**

上面的函数只能IPv4使用，下面的两个函数是IPv4和IPv6通用的(最最最常用):
```c
#include <arpa/inet.h>
int inet_pton (int af, const char* cp, void* buf);
const char* inet_ntop (int af, const void* cp, char* buf, socklen_t len);
```
- `inet_pton` 用于将点分十进制的IPv4 / 十六进制字符串(?)表示的字符串 `cp` 转为对应的整数 `dst`，注意到这里存结果用的是`void*`，成功返回1，失败返回0并设置`errno`
- `inet_ntop` 则是数转字符串，传入数据和上面类似用了个`void*`，同时需要用`len`指定保存字符串结果的`buf`的大小
- 留意到`inet_ntop`返回了一个`const char*`, 该指针仍指向`buf`

[src 地址转换函数](./5-1_addr_translate.c)


TODO: Linux errno, what & how APUE1.7

TODO: const void *__restrict; [MS doc](https://docs.microsoft.com/en-us/cpp/cpp/extension-restrict?view=msvc-160https://docs.microsoft.com/en-us/cpp/cpp/extension-restrict?view=msvc-160), [MS doc-restrict](https://docs.microsoft.com/en-us/cpp/cpp/restrict?view=msvc-160)

TODO: char*, const char*

TODO: extern