# linux网络编程基础api

> 这里的代码要跟着敲一遍呀

- [socket地址API](socket-addr.md)
- [socket基本生命周期函数](socket-life-cycle.md)

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