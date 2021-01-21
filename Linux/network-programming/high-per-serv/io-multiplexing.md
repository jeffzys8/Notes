# I/O 复用

IO复用的使用场景：
- 同时处理多个socket (TODO: client非阻塞connect)
- 同时处理用户输入和网络连接 (TODO: 聊天室程序)
- **server同时处理监听socket和连接socket**
- server同时处理TCP请求和UDP请求 (TODO: 回射服务器)
- server同时监听多个端口/处理多个服务(?) (xinetd服务器) 

## 资料

写的真的好：
https://segmentfault.com/a/1190000003063859

这个也很形象：
https://zhuanlan.zhihu.com/p/87843750

为什么IO复用高效:
https://www.v2ex.com/t/511406
