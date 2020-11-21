# Go-Http服务 源码分析

# 更新记录

|时间|说明|
|--|--|
|2018/11/17|first commit|
|2020/11/14|- 基于新版本的src作内容修正<br>- 深入`Listen`调用

# 概述

Go语言的HTTP服务有官方的包：`net/http`。简单来说，它是利用Go语言封装的一套非常易用的http服务处理框架。

首先先简单介绍一下Web服务器的运作过程: 

1. 首先在Web服务器运行初始化的时候，它会建立一个Socket并开始监听世界各地传过来的http请求
2. 紧接着某个地方的一个client用Socket来请求和服务器连接
3. 在侦听到这个请求以后，监听Socket会分配新的一个Socket给这个client的请求，并以此建立其友好的TCP连接
4. 然后Client就利用这个TCP连接向Server传送 http request；Server读取request并进行逻辑处理，随后利用TCP连接向Client传输 http response

> 现在回看这里是不准确的，如果每个client分配一个新的socket，会导致服务器负载压力很大；当然如何解决负载问题还没深入去研究；初步了解到这部分和 I/O 复用 有关系。

> 2020/11/14 回看*2，这就是准确的..socket只是一层薄薄的系统调用接口，当时的我还深陷“一个socket会占用一个端口”这样的错误观念，其实就是很多个socket。

> 疑问：是server主动断联，还是client断联


## 以曲线进入主题 - 路由

[这里](goHttp.go)是某个教程里的一个简单的、使用Go原生`net/http`包实现的Web服务器，也是这篇博客分析该包的入口代码。


首先看到其`main`函数的第一句
```Go
http.HandleFunc("/", sayhelloName) 
```
这一句从语义即可得知这条语句会给 "/" 这个路由绑定处理函数 `sayhelloName`。值得注意的是，`HandleFunc`并不是某个对象的方法，而是http包的一个函数（相当于C++的static函数）。这就很奇怪了，按照正常的思路，不应该是先建立一个http服务连接对象(本质上是一条TCP连接以及利用这个连接实现的http通信服务)，然后再调用这个对象的方法给这个连接配置一些路由处理函数吗？这时候我们看一下 `net.http.HandleFunc`的源码：

```Go
func HandleFunc(pattern string, handler func(ResponseWriter, *Request)) {
	DefaultServeMux.HandleFunc(pattern, handler)
}
```
这个`DefaultServeMux`是一个`ServeMux`类对象（当然把它叫做对象其实是有争议的——Go定义里是没有类和对象的，但从概念上来理解他的确就是一个"对象"）。学过数电的对multiplexer应该有印象，就是 "数据选择器" 的意思。这里类比到我们的Web服务，其实就是Web服务器里的路由：

```Go
// DefaultServeMux is the default ServeMux used by Serve.
var DefaultServeMux = &defaultServeMux
var defaultServeMux ServeMux
```

```Go
type ServeMux struct {
	mu    sync.RWMutex
	m     map[string]muxEntry
	hosts bool // whether any patterns contain hostnames
}

```

我们看到`ServeMux`类里有一个 `map[string]muxEntry`对象也大概可以理解了，这就是把不同的url作为key映射到不同的处理函数上，如果不出意外的话，我们入口代码`goHttp.go`里面的`net.http.HandleFunc("/", sayhelloName)`最终就会向`DefaultServeMux`的这个`map`对象写入数据。

结果也的确如此：我们对源码进行追踪，最终可以在`func (mux *ServeMux) Handle`函数里找到这个插入处理

```Go
mux.m[pattern] = muxEntry{h: handler, pattern: pattern}
```
PS:这个函数里还有加锁解锁的操作，保证函数的原子性（why?）：

```Go
mux.mu.Lock()
defer mux.mu.Unlock()
```

另一方面，我们还要带着一个思考：在什么时候会把`DeafultServeMux`绑定给我们具体的http服务？这就需要我们接下去研究 `net/http`包具体是怎么实现连接的建立和http请求的监听了。

## 网络服务的主题 - 监听请求并服务连接

我们继续分析`goHttp.go`，到现在为止也就分析了一行代码(orz)

```Go
err := http.ListenAndServe(":9090", nil) //设置监听的端口
if err != nil {
    log.Fatal("ListenAndServe: ", err)
}
```

> 小的试验：将端口绑定到已有服务。port 13 是约定俗成的时间服务器，如果我这里将9090改成13，就会报错：

```shell
ListenAndServe: listen tcp :13: bind: permission denied
```

`http.ListenAndServe`会创建一个`http.(Server)`实例，同时调用其`http.(*Server).ListenAndServe`方法，该方法实际做了两件事情：

1. **创建socket来监听指定的端口，等待客户端请求到来。**

```Go
ln, err := net.Listen("tcp", addr)
```
`net.Listen`创建了一个TCP socket并对该端口进行监听（如果是Linux的话，就是对socket完成了`bind`和`listen`的系统调用）。这个监听的具体过程是怎么样的？这个很重要也比较复杂，放到后面再讲。而这个函数创建的一套服务接口，就由一个`net.Listener`来暴露了。值得注意的是，`net.Listener`只是一个interface，封装了下面几个函数。

```GO
type Listener interface {
// Accept waits for and returns the next connection to the listener.
Accept() (Conn, error)

// Close closes the listener.
// Any blocked Accept operations will be unblocked and return errors.
Close() error

// Addr returns the listener's network address.
Addr() Addr
}
```

通过深入代码可以发现，这里其实返回的是一个`net.TCPListener`，这个我们后面再讲。


2. **处理`Listener`监听到的新客户连接**
   	
在获取到`Listener`以后，会调用一个`(*http.Server).Serve(net.Listner)`函数来处理到达这个底层监控服务的客户请求连接。

```Go
return srv.Serve(ln)
```

`Serve`负责循环地接受客户连接(`Accept`)并分配go程对该连接进行处理。从Linux角度来看，就是类似调用`epoll_wait`获取新的连接socket，同时分配一个线程对该socket进行读(request)、写(response)操作。当然go程是比内核实现的线程更轻量级的存在。
> TODO: 自己归纳一下go程/贴个链接

> 其实就是Reactor模式，可以参考《Linux高性能服务器编程》

我们将错误处理等操作去掉，保留核心代码来观察一下`Serve`这个函数

```Go
for {
	rw, err := l.Accept()
	c := srv.newConn(rw)
	c.setState(c.rwc, StateNew) // before Serve can return
	go c.serve(connCtx)
}
```

下面将进行逐句分析：
```Go
rw, e := l.Accept()
```
`net.(Listner).Accept`会阻塞地等待`Listener`返回一个`net.Conn`接口型变量。`net.Conn`为流式网络连接(其实主要就是TCP socket)提供统一的接口，还需支持多个go程并发对它进行读写。
- `net.Conn`的定义
	```Go
	// Conn is a generic stream-oriented network connection.
	// Multiple goroutines may invoke methods on a Conn simultaneously.
	type Conn interface {
		Read(b []byte) (n int, err error)
		Write(b []byte) (n int, err error)
		Close() error
		LocalAddr() Addr
		RemoteAddr() Addr
		...超时相关...
	}
	```

> 疑问，多个go程并发地读或写Conn，会不会造成数据乱序? 这是不是得取决于它定义的IO模式(阻塞/非阻塞，同步/异步)，后面研究Accept时好好看看。

```Go
c := srv.newConn(rw)
c.setState(c.rwc, StateNew) // before Serve can return
```

调用`net.http.(*Server).newConn`将`Conn`对象(`rw`)封装起来；那为什么要封装起来呢？我们前面看到`Conn`其实是一个TCP连接，属于传输层，因此要对它进行HTTP操作还需要进行一层HTTP应用层包裹，就是封装后的`net.http.conn`。

<!-- TODO: 这里突然想到了关于HTTP短连接长连接的事情，赶紧去看一下别忘了。 -->

同时也能看到`net.http.conn`维护了一套有限状态机，在这里进行了初步的状态设定`StateNew`，我们这里就不深入展开了。


```Go
go c.serve(connCtx)
```

最后单独开了一个go程去服务该HTTP连接，而接下来就是应用层HTTP的操作了。

# HTTP请求处理

承上而言，每个HTTP连接都分配单独的一个go程，而在每个go程里处理对该连接的request读取、解析，调用相应的处理函数生成response并再通过该连接写回。

> to be continued.

<!-- TODO -->

# `Listen`

> 关键词: tcp, bind, listen, netpoll, epoll, kqueue

在前面我们简要介绍了`net.Listen`，该函数会返回一个`net.(Listner)`接口类型对象供程序处理到来的连接。接下来我们详细讲一下`Listen`的底层原理，由于本篇是讲HTTP服务，所以这里只关注TCP连接，解释该函数具体是如何创建并监听连接的。

同样的，下文中的代码只会保留核心代码，对于一些错误处理之类的操作暂时略去，对于比较关键的也会作说明但不展开。


`net.Listen`函数创建了一个用于记录该连接各项信息的`ListenConfig`，并调用`net.(*ListenConfig).Listen`执行实际的监听操作

> 客户端使用的`net.Dial`函数的注释可以详细查看`network`和`address`参数的详情；这里不作展开。

```Go
func (lc *ListenConfig) Listen(ctx context.Context, network, address string) (Listener, error){
	addrs, err := DefaultResolver.resolveAddrList(ctx, "listen", network, address, nil)
	sl := &sysListener{
		ListenConfig: *lc,
		network:      network,
		address:      address,
	}

	var l Listener
	la := addrs.first(isIPv4)
	switch la := la.(type) {
	case *TCPAddr:
		l, err = sl.listenTCP(ctx, la)
	...其他地址类型...
	}
	return l, nil
}
```
首先`resolveAddrList`会根据传入的`address`和操作类型(`dial`/`listen`)去做地址解析，这里对于`Dial`而言就是DNS操作了。
```Go
addrs, err := DefaultResolver.resolveAddrList(ctx, "listen", network, address, nil)
```

随后会从返回的地址列表`addrs`中选取第一个是IPv4类型的地址。
```Go
la := addrs.first(isIPv4)
```

**但这里其实有些奇怪，因为讲道理监听应该可以指定监听IPv6，如果Listen就是指定IPv6的话，那岂不是不能执行监听操作了?** 

> [IPv4 也是可以访问 IPv6 服务的](https://ms2008.github.io/2018/12/10/ipv6-bindv6only/) (这篇博文讲解了`Listen`默认监听IPv6，但也可以解析到来的ipv4请求，还用了`strace`对系统调用进行分析; 虽然和我这里遇到的问题不一样，但觉得有关联性且探究问题的方法很好，就mark下来了)

<!-- TODO: 自己做个实验试一下 -->

在获取完地址后，就会调用`(*sysListener).listenTCP`进行端口的TCP连接监听。

```Go
sl := &sysListener{
	ListenConfig: *lc,
	network:      network,
	address:      address,
}
l, err = sl.listenTCP(ctx, la)
```
注意到这里又封装了一层`sysListener`，观察源码可知它实现了多种网络的监听，并创建对应的`Listener`返回给调用方，所以可以理解为系统的**监听器工厂**。

另外值得注意的一个点是，在`(*sysListener).listenTCP`以前的函数都是**平台无关**的，也即所有平台都是调用同样的代码。但是从`(*sysListener).listenTCP`开始，就是**平台相关**的了，在MacOS中可以观察到它位于 `net/tcpsock_posix.go`，而在Windows可以观察到位于 `net/tcpsock_plan9.go`

> 这里又有新的问题：**在MacOS中我在源码内可以看到两个平台的`(*sysListener).listenTCP`实现，但用VSCode走读的时候都能准确地指向对应平台的代码，编译器(和代码提示器)怎么知道我需要编译哪份代码的呢?** 这个我暂时没有找到答案。感觉这个会和Go的runtime有关系，而VSCode的peeker辅助了一些runtime的功能。这个后面再去回看。

<!-- TODO: 找到这里平台相关的实现方法 -->

我们来看看这个函数做了什么：

```Go
func (sl *sysListener) listenTCP(ctx context.Context, laddr *TCPAddr) (*TCPListener, error) {
	fd, err := internetSocket(ctx, sl.network, laddr, nil, syscall.SOCK_STREAM, 0, "listen", sl.ListenConfig.Control)
	return &TCPListener{fd: fd, lc: sl.ListenConfig}, nil
}
```

到这又有点意思，本来`sysListener`整合了各种`listen`操作，而现在可见各种`listen`操作调用的又是统一的`internetSocket`接口。类似这样:
```
					--> listenTCP
	sysListener	-->					--> internetSocket
					--> listenUDP
```
<!-- TODO: 弄成图片吧，这也太丑了 -->
当然因为`interSocket`不仅处理`Listen`，也要处理`Dial`，所以这么做也是非常合理的。

```Go
func internetSocket(ctx context.Context, net string, laddr, raddr sockaddr, sotype, proto int, mode string, ctrlFn func(string, string, syscall.RawConn) error) (fd *netFD, err error) {
	if (runtime.GOOS == "aix" || runtime.GOOS == "windows" || runtime.GOOS == "openbsd") && mode == "dial" && raddr.isWildcard() {
		raddr = raddr.toLocal(net)
	}
	family, ipv6only := favoriteAddrFamily(net, laddr, raddr, mode)
	return socket(ctx, net, family, sotype, proto, ipv6only, laddr, raddr, ctrlFn)
}
```
<!-- TODO: local addr 和 remote addr -->

到这里就比较底层了，**也越来越接近最关键的位置了**。首先`net.favoriteAddrFamily`返回了地址族类型(ipv6/ipv4/..)，然后调用`net.socket`(MacOS位于`net/sock_posix.go`)。我们仔细观察一下这个函数，同样也只取我们关心的部分：

```Go
// socket returns a network file descriptor that is ready for
// asynchronous I/O using the network poller.
func socket(ctx context.Context, net string, family, sotype, proto int, ipv6only bool, laddr, raddr sockaddr, ctrlFn func(string, string, syscall.RawConn) error) (fd *netFD, err error) {
	s, err := sysSocket(family, sotype, proto)
	if err != nil {
		return nil, err
	}
	if err = setDefaultSockopts(s, family, sotype, ipv6only); err != nil {
		poll.CloseFunc(s)
		return nil, err
	}
	if fd, err = newFD(s, family, sotype, net); err != nil {
		poll.CloseFunc(s)
		return nil, err
	}

	// This function makes a network file descriptor for the
	// following applications:
	//
	// - An endpoint holder that opens a passive stream
	//   connection, known as a stream listener
	//
	// ...
	//
	// For stream and datagram listeners, they will only require
	// named sockets, so we can assume that it's just a request
	// from stream or datagram listeners when laddr is not nil but
	// raddr is nil. Otherwise we assume it's just for dialers or
	// the other connection holders.

	if laddr != nil && raddr == nil {
		switch sotype {
		case syscall.SOCK_STREAM, syscall.SOCK_SEQPACKET:
			if err := fd.listenStream(laddr, listenerBacklog(), ctrlFn); err != nil {
				fd.Close()
				return nil, err
			}
			return fd, nil
		case syscall.SOCK_DGRAM:
			...
		}
	}
	...
	//logic for dialing 
	return fd, nil
}
```

> to be continued.

---
## 未整理

> 疑问，已经用go程去处理每个Accept到的连接socket了，为什么还要epoll，epoll在这里难道不是只监控 listen Socket就够了吗？(完全可以自己动手试一下看看有没有问题呀~) 

> 不知道我这样理解对不对：go程不是线程！如果单纯把accept到的连接分给go程去调系统调用读写的话，还是会阻塞起来(如果是非阻塞调用的话就会导致不停空转，只要轮到这个时间片就浪费资源)，这样就和多线程去read/write没区别了。所以更好的做法是，统一使用类似epoll的方式去管理**监听socket+所有连接socket**，这样go程再去单独调用Accept的时候其实不会让线程阻塞在IO上了，

asynchronous I/O, network poller, laddr raddr, poll.CloseFunc, poll.fd, 引用计数

netpoll_epoll.go

`net.sysSocket`

```GO
syscall.ForkLock.RLock()
socketFunc(family, sotype, proto)
syscall.CloseOnExec(s)
syscall.ForkLock.RUnlock()
syscall.SetNonblock(s, true);
poll.CloseFunc(s) // when err, netpoll not yet monitoring
```
- [close-on-exec](https://blog.csdn.net/justmeloo/article/details/40184039)
- socket非阻塞

`net.(*netFD).listenStream` 

```Go
func setDefaultListenerSockopts(s int) error {
	// Allow reuse of recently-used addresses.
	return os.NewSyscallError("setsockopt", syscall.SetsockoptInt(s, syscall.SOL_SOCKET, syscall.SO_REUSEADDR, 1))
}
```

- [SO_REUSEADDR和SO_REUSEPORT作用](https://www.jianshu.com/p/141aa1c41f15)

`internal/poll.(*FD).Init`

```Go
// Set pollable to true if fd should be managed by runtime netpoll.
```

`internal/poll.pollDesc` 和底层epoll, kqueue交互的核心结构

`internal/poll.(*pollDesc).init`

```Go
serverInit.Do(runtime_pollServerInit)
ctx, errno := runtime_pollOpen(uintptr(fd.Sysfd))
```
可见是同一程序统一的poller

`internal/poll.runtime_pollServerXX()` 怎么runtime到 `runtime.poll_runtime_pollServerInit`的

```Go
//go:linkname poll_runtime_pollServerInit internal/poll.runtime_pollServerInit
func poll_runtime_pollServerInit() {
	netpollGenericInit()
}
```
这里又是如何指到kqueue/epoll的


# `Accept`

`net.Listen` 为我们返回了一个`Listner`变量，让我们可以执行`Accepet`操作，我们来看看调用它的时候发生了什么。

# 未整理

[教程分析](https://github.com/astaxie/build-web-application-with-golang/blob/master/zh/03.3.md)

- [ ] `net.Listen`的具体实现
- [ ] `net`包的`epoll`工作机制
- [ ] 无处不在的`context`

`(*http.Server).Serve(net.Listner)` TODO:
- 防止Listener被多次关闭（会有什么后果？）：
```Go
l = &onceCloseListener{Listener: l}
defer l.Close()
```
- HTTP 2相关，还提到了`Serve`有可能被并发调用
```Go
if err := srv.setupHTTP2_Serve(); err != nil {
return err
}
```
- `Server`可以同时监控多个`Listener?`
```Go
if !srv.trackListener(&l, true) {
return ErrServerClosed
}
defer srv.trackListener(&l, false)
```
- `context`是干嘛的（大致理解是跨进程、函数传递消息用的统一接口）
```Go
baseCtx := context.Background()
if srv.BaseContext != nil {
baseCtx = srv.BaseContext(origListener)
if baseCtx == nil {
	panic("BaseContext returned a nil context")
}
}
ctx := context.WithValue(baseCtx, ServerContextKey, srv)
```

<hr>

- https://segmentfault.com/a/1190000021812996
- https://zhuanlan.zhihu.com/p/31644462

Any suggestions? 欢迎各类批评建议～

**zys980808@126.com**