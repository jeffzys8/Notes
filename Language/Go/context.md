# context

https://segmentfault.com/a/1190000024441501

https://golang.org/pkg/context/


> 这段真的翻译有问题啊
在 Go 语言中 `context` 包允许您传递一个 "context" 到您的程序。 Context 如超时或截止日期（deadline）或通道，来指示停止运行和返回。例如，如果您正在执行一个 web 请求或运行一个系统命令，定义一个超时对生产级系统通常是个好主意。因为，如果您依赖的API运行缓慢，你不希望在系统上备份（back up）请求，因为它可能最终会增加负载并降低所有请求的执行效率。导致级联效应。这是超时或截止日期 context 派上用场的地方。

每一个 Context 都会从最顶层的 Goroutine 一层一层传递到最下层，这也是 Golang 中上下文最常见的使用方式，如果没有 Context，当上层执行的操作出现错误时，下层其实不会收到错误而是会继续执行下去。

Go 语言中的 context.Context 的主要作用还是在多个 Goroutine 组成的树中同步取消信号以减少对资源的消耗和占用，虽然它也有传值的功能，但是这个功能我们还是很少用到。
