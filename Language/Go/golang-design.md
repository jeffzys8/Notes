# go 语言原本

用来记录go的一些实现原理。

https://golang.design/under-the-hood/zh-cn

## Channel

Channel 底层结构
实现 Channel 的结构并不神秘，本质上就是一个 mutex 锁加上一个环状缓存、 一个发送方队列和一个接收方队列：

```Go
// src/runtime/chan.go
type hchan struct {
	qcount   uint           // 队列中的所有数据数
	dataqsiz uint           // 环形队列的大小
	buf      unsafe.Pointer // 指向大小为 dataqsiz 的数组
	elemsize uint16         // 元素大小
	closed   uint32         // 是否关闭
	elemtype *_type         // 元素类型
	sendx    uint           // 发送索引
	recvx    uint           // 接收索引
	recvq    waitq          // recv 等待列表，即（ <-ch ）
	sendq    waitq          // send 等待列表，即（ ch<- ）
	lock mutex
}
type waitq struct { // 等待队列 sudog 双向队列
	first *sudog
	last  *sudog
}
```

> to be continued

## 协程

https://www.cnblogs.com/zkweb/p/7815600.html

> to be continued