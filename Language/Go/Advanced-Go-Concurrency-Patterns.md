# Advanced Go Concurrency Patterns

> 本篇是 2013 Go talks - Advanced Go Concurrency Patterns 的翻译+自己的一些注解。Go并发模型对于传统的多线程并发代码架构是很大的颠覆，当然其实自己对于多线程编程的经验也不够多，之后在深入学习Linux C++网络编程时再回看和比较。

## 资料

- [Slides](https://talks.golang.org/2013/advconc.slide#1)
  - 比较有意思的是，这个演示文稿里集成了一个小ide，可以调试代码
- [Codes](https://talks.golang.org/2013/advconc)
- [Video](https://www.youtube.com/watch?v=QDDwwePbDtw)

补充资料：

- [Go Concurrency Patterns](https://talks.golang.org/2012/concurrency.slide#1)
  - 这个没有仔细去看，但好像是对并发的基础知识介绍。
- [Go Talks](https://talks.golang.org/)
  - 里面的topic应该都蛮值得好好看的。


## 概要

这个讲座是关于如何用Go编写**快速响应、资源有效管理、可读性高的并发程序**。

Go是在语言层面和运行时实现的并发，并非一个库。这改变了我们组织Go代码的方式。
Go程(Goroutine)在和主线程相同的地址空间执行函数。而信道(Channel)则可以帮助我们在Go程间进行**状态同步**和**数据交换**。
> 数据交换这个功能对于信道来说是显而易见的，但对于信道在状态同步中的应用这个talk有比较深入的示例进行说明。

## 从乒乓球游戏到溢出 :)

这里用信道抽象出球桌的概念(`table`)，并用结构体构造一个球的概念(`Ball`)

两个球员(`player`函数)分别(`go`)从球桌上等待球的到来`ball := <- table`，在获得球以后击打一次，并将击打数(`hits`)记录到球实例中。随后又将球传回桌上(`table <- ball`)。

最后回收游戏相当于从俩球员的球桌中抢球`<-table`。

```Go
type Ball struct{ hits int }

func main() {
    table := make(chan *Ball)
    go player("ping", table)
    go player("pong", table)

    table <- new(Ball) // game on; toss the ball
    time.Sleep(1 * time.Second)
    <-table // game over; grab the ball
}

func player(name string, table chan *Ball) {
    for {
        ball := <-table
        ball.hits++
        fmt.Println(name, ball.hits)
        time.Sleep(100 * time.Millisecond)
        table <- ball
    }
}
```

如果我们不把球放上球桌(注释掉`table <- new(Ball)`)，主程序会在`Sleep`以后死锁并触发panic，因为无法从球桌信道中获得任何数据。Go会在死锁时提供调用栈信息用于debug，非常有用。

> 程序可以主动调用`panic("msg u want to print")`来查看栈信息，不过这回导致程序异常退出，还有其他机制可以看调用栈信息，但这里不深究。

```
fatal error: all goroutines are asleep - deadlock!

goroutine 1 [chan receive]:
main.main()
        /Users/zhengyusen/Desktop/Workspace/Notes/Language/Go/test.go:20 +0xd1

goroutine 6 [chan receive]:
main.player(0x10cd270, 0x4, 0xc00006a060)
        /Users/zhengyusen/Desktop/Workspace/Notes/Language/Go/test.go:25 +0x45
created by main.main
        /Users/zhengyusen/Desktop/Workspace/Notes/Language/Go/test.go:15 +0x76

goroutine 7 [chan receive]:
main.player(0x10cd274, 0x4, 0xc00006a060)
        /Users/zhengyusen/Desktop/Workspace/Notes/Language/Go/test.go:25 +0x45
created by main.main
        /Users/zhengyusen/Desktop/Workspace/Notes/Language/Go/test.go:16 +0xad
exit status 2
```

最后回收球的动作显然的是存在问题的，球没有了，两个`player`函数就阻塞在信道接收部分了，而主程序此时已经退出，所以这造成了溢出。**对于持久程序而言，这样的代码是不能接受的**。

> TODO: 改写乒乓球游戏，让他在游戏结束时正常地回收goroutine。看完这个视频首先得想法就是给Ball包一层，外面加一个指示游戏是否还在继续的信号（看到这感兴趣的话你也动手试试？）

## Feed Reader（小型“今日头条”）

对于更复杂的并发程序（包含了创建、交互、周期事件、取消等动作），用传统多线程编程的方式将会比较困难，而对于Go来说，简化这些交互的核心在于`select`语句。该语句类似`switch`，不过会阻塞地等待其中一个case可执行。

### 功能需求和接口定义

在这里演讲者要实现一个小型的摘要订阅器（这里还涉及一些RSS标准之类的概念，可以自行了解一下，但这不重要），简单来说就是要周期地从用户订阅的urls中获取文章的摘要(maybe)，喂给用户。

以下是获取摘要的函数定义(`Fetch`)，以及获取到的数据的格式(`Item`)
```Go
// Fetch fetches Items for uri and returns the time when the next
// fetch should be attempted.  On failure, Fetch returns an error.
func Fetch(uri string) (items []Item, next time.Time, err error)

type Item struct{
    Title, Channel, GUID string // a subset of RSS fields
}
```

获取的数据希望是一个“流”，通过信道去实现。
```Go
<-chan Item
```

> 开始实际的代码设计部分

首先是一个初步的接口定义，方便我们后续进行实现和测试（先画饼）：
```Go
type Fetcher interface {
    Fetch() (items []Item, next time.Time, err error)
}

func Fetch(domain string) Fetcher {...} // fetches Items from domain
```
然后我们需要一个 **订阅(`Subscription`)** 的抽象，我们可以通过`Updates`浏览该订阅的新数据，以及`Close`关闭该订阅。同时在这之上我们需要实现订阅某个流、合并订阅的操作。
```Go
type Subscription interface {
    Updates() <-chan Item // stream of Items
    Close() error         // shuts down the stream
}

func Subscribe(fetcher Fetcher) Subscription {...} // converts Fetches to a stream

func Merge(subs ...Subscription) Subscription {...} // merges several streams
```

这里的关键在于当我们指示关闭订阅的时候，订阅里包含的周期性的、延时的操作应当立即停止并被清理掉。

### 主程调用

这里先写出主程序调用订阅函数之类实现feed reader功能的代码。有点TDD的意思。
程序订阅了三个url并合并为一个订阅`merged`，随后设定了一个延时匿名函数，在3秒后会调用`merged.Close()`来关闭所有订阅。在这三秒内，程序会不断轮询`merged.Updates()`来获取新的摘要并输出。

这里值得再提一下，`for range`遍历信道会在信道被关闭`close(chan)`以后结束。


```Go
func main() {
    // Subscribe to some feeds, and create a merged update stream.
    merged := Merge(
        Subscribe(Fetch("blog.golang.org")),
        Subscribe(Fetch("googleblog.blogspot.com")),
        Subscribe(Fetch("googledevelopers.blogspot.com")))

    // Close the subscriptions after some time.
    time.AfterFunc(3*time.Second, func() {
        fmt.Println("closed:", merged.Close())
    })

    // Print the stream.
    for it := range merged.Updates() {
        fmt.Println(it.Channel, it.Title)
    }

    panic("show me the stacks")
}
```

### 实现订阅功能

这个Talk将主要集中在`Subscribe`函数（时间因素），`Merge`的部分会在网上公开代码，据说也很有趣。
> TODO：既然如此，不如自己也先试一下实现 `Merge`?

- 声明了`sub`结构体用于implement`Subscription`接口
  - `fetcher` 即前面说明的 `Fetcher` 函数
  - `updates` 即前面说明的用户用来遍历的摘要数据流
- 定义`Subscribe`函数用于创建`sub`
  - 会启动`loop`函数，这是最复杂的函数，因为他需要周期性地`Fetch`、将数据传给`updates`，同时还要处理关闭的状况，还得是响应式的..

```Go
func Subscribe(fetcher Fetcher) Subscription {
    s := &sub{
        fetcher: fetcher,
        updates: make(chan Item), // for Updates
    }
    go s.loop()
    return s
}

// sub implements the Subscription interface.
type sub struct {
    fetcher Fetcher   // fetches items
    updates chan Item // delivers items to the user
}

// loop fetches items using s.fetcher and sends them
// on s.updates.  loop exits when s.Close is called.
func (s *sub) loop() {...}
```

`sub`需要分别定义两个函数以实现`Subscription`接口。`Updates`很简单，返回一下数据信道即可；关键在于`Close`，它得让周期性`fetch`的动作（`loop`中实现）停下来，还要返回在`fetch`时发现的错误。
```Go
func (s *sub) Updates() <-chan Item {
    return s.updates
}
func (s *sub) Close() error {
    // TODO: make loop exit
    // TODO: find out about any error
    return err
}
```

下面就是重头戏了，`loop`函数，涉及三个动作：周期性fetch、处理关闭、接收并传递错误信息。

### 幼稚的`loop`实现

演讲者先展示了一种幼稚的实现：

```Go
func (s *sub) loop() {
    for {
        if s.closed {
            close(s.updates)
            return
        }
        items, next, err := s.fetcher.Fetch()
        if err != nil {
            s.err = err                 
            time.Sleep(10 * time.Second)
            continue
        }
        for _, item := range items {
            s.updates <- item
        }
        if now := time.Now(); next.After(now) {
            time.Sleep(next.Sub(now))
        }
    }
}

func (s *naiveSub) Close() error {
    s.closed = true
    return s.err   
}
```
这个代码有好几个问题（甚至我觉得演讲者还没有覆盖其全部问题）。


**数据竞争**，这出现在`s.err`和`s.closed`两个变量上，这两个变量的读写操作(位于`loop`和`Close`)没有加锁保护（这里的`s.err`没有在前文定义

> partially written types 是什么(partial write?)

Go 提供了检测数据竞争的工具（这个东西的原理值得更深入的研究）
```sh
go run -race naivemain.go
```

**非响应式的Sleep**，这出现在`time.Sleep(next.Sub(now))`，假如下一个时间是很长时间以后，这个go程就完全阻塞住了。另外前面对于拉数据错误时的睡眠10s虽然短暂，但也会导致对`Close`的不及时相应。

**阻塞在发送数据信道**，假如用户在`Subscription`go程正在获取数据时调用了`Close`并且不再遍历`updates`信道，随后获取到数据的订阅go程会尝试从`updates`中发送数据，这就直接永远阻塞住了（死锁）。

### 使用`select`

使用`select`来构造程序，可以将上面的问题一次性同步地解决！不用写并发的锁，不用去处理回调函数！
> 这对于我来说是很大的程序设计颠覆。每次for循环前的状态变量又有点前端React的感觉

```Go
func (s *sub) loop() {
    ... declare mutable state ...
    for {
        ... set up channels for cases ...
        select {
        case <-c1:
            ... read/write state ...
        case c2 <- x:
            ... read/write state ...
        case y := <-c3:
            ... read/write state ...
        }
    }
}
```

下面将分几个部分介绍如何用`select`将原有的存在bug的写法逐个组合起来

### 处理`Close`

将原本`bool`类型的`closed`变量，改造为一个`chan chan error`类型的变量`closing`，利用它来实现 **(请求-响应)结构**，以传递订阅结束状态。在这里`loop`可以视为一个service，主程即为client。具体的过程是这样：
- service循环地等待结束请求的到来
- client向service发送结束请求，同时阻塞地等待service返回响应
- service返回error(如果有的话)给client，指示订阅结束

具体代码：
```Go
type sub struct {
    closing chan chan error
}

func (s *sub) Close() error {
    errc := make(chan error)
    s.closing <- errc
    return <-errc
}
```

`loop`内在接收到结束请求时，返回结束消息并结束循环：
```Go
func (s *sub) loop() {
    var err error // set when Fetch fails
    for {
        select {
        case errc := <-s.closing:
            errc <- err
            close(s.updates) // tells receiver we're done
            return
        }
    }
}
```

### 包装`Fetch`

> 未完待续

- 需要实现nextTime
- 需要缓存获取到的数据
- 需要不阻塞

> 仍然存在问题，如果Fetch长时间阻塞？

> 我也有个问题，假如在Next时间到达之前Closed了，而且Next的时间很长，`time.After` channel不也leak了吗

```Go
func (s *sub) loop() {
    var pending []Item // appended by fetch; consumed by send
    var next time.Time // initially January 1, year 0
    var err error
    for {
        var fetchDelay time.Duration // initally 0 (no delay)
        if now := time.Now(); next.After(now) {
            fetchDelay = next.Sub(now)
        }
        startFetch := time.After(fetchDelay)

        select {
        case <-startFetch:
            var fetched []Item
            fetched, next, err = s.fetcher.Fetch()
            if err != nil {
                next = time.Now().Add(10 * time.Second)
                break
            }
            pending = append(pending, fetched...)
        }
    }
}
```

###  将数据发送到用户channel

TODO: 仔细阅读是怎么解决在无数据可send的时候用 nil channel 屏蔽发送case的


## TODO: 完整代码走读

> naive, fake, dedup, real

## 其他知识

### 匿名函数
```go
go func() { c <- 3 }()
```

### pkg: time

- time.AfterFunc
- time.After

### nil select