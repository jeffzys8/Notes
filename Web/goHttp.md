# Go Http 源码分析

Go语言的HTTP服务有官方的包：```net/http```。简单来说，它是利用Go语言底层的Socket网络编程实现的一套http服务处理框架。(P.S. 至于Socket，以及里面频繁用到的并发知识，推荐食用 Unix环境高级编程 & Unix网络编程-套接字编程。但有多好吃我是不知道的，但大家都说好）。

首先先简单介绍一下(其实大家都懂)Web服务器的运作过程(而且我还可能会说错): 

1. 首先在Web服务器运行初始化的时候，它会建立一个Socket并开始监听世界各地传过来的http请求
2. 紧接着某个地方的一个小client也用Socket来请求和服务器连接
3. 在侦听到这个请求以后，监听Socket会分配新的一个Socket给这个client的请求，并以此建立其友好的TCP连接
4. 然后Client就利用这个TCP连接向Server传送 http request；反之，Server利用TCP连接向Client传输 http response
5. 最后信息传完，服务器会删掉这个tcp连接


## 以曲线进入主题 - 路由

[这里](goHttp.go)是某个教程里的一个简单的、使用Go原生```net/http```包实现的Web服务器，也是这篇博客分析该包的入口代码。


首先看到其```main```函数的第一句
```Go
/* goHttp.go: 24 */
http.HandleFunc("/", sayhelloName) 
```
这一句从语义即可得知这条语句会给 "/" 这个路由绑定处理函数 ```sayhelloName```。值得注意的是，```HandleFunc```并不是某个对象的方法，而是http包的一个函数（相当于C++的static函数）。这就很奇怪了，按照正常的思路，不应该是先建立一个http服务连接对象(本质上是一条TCP连接以及利用这个连接实现的http通信服务)，然后再调用这个对象的方法给这个连接配置一些路由处理函数吗？这时候我们看一下 ```http.HandleFunc```的源码：

```Go
/* net/http/server.go: 2406 */
func HandleFunc(pattern string, handler func(ResponseWriter, *Request)) {
	DefaultServeMux.HandleFunc(pattern, handler)
}
```
这个```DefaultServeMux```是一个```ServeMux```类对象（当然说这话要很小心——Go定义里是没有类和对象的，这不过是我们附加的概念而已）。学过数电的对multiplexer应该有印象，就是”数据选择器”的意思。这里类比到我们的Web服务，其实就是Web服务器里的路由：

```Go
/* net/http/server.go: 2163 */
// DefaultServeMux is the default ServeMux used by Serve.
var DefaultServeMux = &defaultServeMux
var defaultServeMux ServeMux
```

```Go
/* net/http/server.go: 2149 */
type ServeMux struct {
	mu    sync.RWMutex
	m     map[string]muxEntry
	hosts bool // whether any patterns contain hostnames
}

```

我们看到```ServeMux```类里有一个 ```map[string]muxEntry```对象也大概可以理解了，这就是把不同的url作为key映射到不同的处理函数上，如果不出意外的话，我们入口代码```goHttp.go```里面的```http.HandleFunc()```最终就会向```DefaultServeMux```的这个```map```对象写入数据。

结果也的确如此：我们对源码进行追踪，最终可以在```func (mux *ServeMux) Handle```函数里找到这个插入处理。Socket

```Go
/* net/http/server.go: 2383 */
mux.m[pattern] = muxEntry{h: handler, pattern: pattern}
```
PS:这个函数里还有加锁解锁的操作，保证函数的原子性（why?）：

```Go
mux.mu.Lock()
defer mux.mu.Unlock()
```

另一方面，我们还要带着一个思考：在什么时候会把```DeafultServeMux```绑定给我们具体的http服务？这就需要我们接下去研究 ```net/http```包具体是怎么实现连接的建立和http请求的监听了。

## Web服务的主题 - 连接与请求监听

我们继续分析```goHttp.go```，到现在为止也就分析了一行代码(orz)

```Go
/* goHttp.go: 25 */
err := http.ListenAndServe(":9090", nil) //设置监听的端口
if err != nil {
    log.Fatal("ListenAndServe: ", err)
}
```

> 小的试验：将端口绑定到已有服务。port 13 是约定俗成的时间服务器，如果我这里将9090改成13，就会报错：

```shell
ListenAndServe: listen tcp :13: bind: permission denied
```

```ListenAndServe```实际做了三件事情：

1. 创建Listen Socket, 监听指定的端口, 等待客户端请求到来。

	```Go
	/* net/http/server.go: 2760 */
	ln, err := net.Listen("tcp", addr)
	```
	```net.Listen```底层用TCP协议(Socket编程)搭建了一个服务，然后监控我们设置的端口。那这个底层的实现具体是怎么样的？这个放到后面再讲。而这个函数创建的一套服务接口，就由一个Listener(上面的```ln```)来暴露了。而后调用一个```Serve```函数来处理这个Listener，也即处理到达这个底层监控服务的客户请求

	```Go
	/* net/http/server.go: 2764 */
	return srv.Serve(tcpKeepAliveListener{ln.(*net.TCPListener)})
	```
	(这个语法比较迷...一时没有看懂)

2. Listen Socket接受客户端的请求, 得到Client Socket, 接下来通过Client Socket与客户端通信。

	```Go
	/* net/http/server.go: 2826 */
	rw, e := l.Accept()
	```

	我们可以看到```Serve```函数内循环地进行```Accept```操作，这个操作实际上是阻塞式地等待底层监听服务监听到一个客户请求，然后分配一个连接(```rw```)；值得注意的是，这里的连接是底层服务层面的，有别于下面又新建的一个连接，相当于这个底层连接的一个HTTP包裹。

	```Go
	/* net/http/server.go: 2849 */
	c := srv.newConn(rw)
	```

	最后单独开了一个go routine，把这个请求的数据当做参数扔给这个HTTP conn(对底层conn的封装)去服务。这个就是高并发体现了，用户的每一次请求都是在一个新的协程去服务，相互不影响

	```Go
	/* net/http/server.go: 2851 */
	go c.serve(ctx)
	```

3. 处理客户端的请求, 首先从Client Socket读取HTTP请求的协议头, 如果是POST方法, 还可能要读取客户端提交的数据, 然后交给相应的handler处理请求, handler处理完毕准备好客户端需要的数据, 通过Client Socket写给客户端。

> To be continued.

[教程分析](https://github.com/astaxie/build-web-application-with-golang/blob/master/zh/03.3.md)

- [ ] ```net.Listen```的具体实现

<hr>

Any suggestions? 欢迎各类批评建议～

**zys980808@126.com**