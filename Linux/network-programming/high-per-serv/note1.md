# Linux 高性能服务器编程

数据链路层、网络层、传输层在内核实现；少数服务器程序在内核实现，减少数据在user space和kernel space的来回复制。

TCP 面向连接，数据流，超时重传、数据确认保证可靠性，内核维护数据结构（连接状态、缓冲区、定时器等）

UDP 基于数据报，不可靠，数据报有长度；
> 理论上，IP数据报的最大长度是65535字节，这是由IP首部16比特总长度字段所限制的。 去除20字节的IP首部和8个字节的UDP首部， UDP数据报中用户数据的最长长度为65507字节

应用层协议
- ping -- ICMP(直接使用网络层协议)
- telnet -- TCP
- OSPF(开放最短路优先),用于动态更新路由信息 -- ?
- DNS -- UDP

UDP无需为应用层数据保存副本，发送即丢弃缓冲区数据。重发需从user space再次拷贝到kernel缓冲区。

一些名称
- TCP报文段(segment)
- UDP数据报(datagram)
- IP数据报(datagram)
- 以太网帧(frame)

以太网帧使用物理地址(MAC)通信(6+6=12bytes)，4bytes CRC字段提供循环冗余校验（忘了）;
MTU = 帧最大传输单元，以太网帧MTU = 1500bytes；IP分片传输(fragment)

下层协议header需要有一个protocol字段标识上层协议；比如IP-header有16位的protocol来区分ICMP, TCP, UDP等


tcpdump可以用来抓取以太网帧，1.5.3介绍了观察telnet运作，以后要回看。还提前清掉了ARP缓存

Linux下`/etc/resolv.conf`存放DNS服务器地址

tcpdump观察DNS 1.6.3

socket是一组系统调用，使应用程序能访问在内核实现的数据链路层、网络层、传输层协议。socket核心两个功能就是读写内核缓冲区，以及控制内核中各层协议的header信息。socket不止可以访问TCP/IP协议栈，还能访问X.25, UNIX本地域协议栈(这个看着像挺重要的东西)

