# 基于共享变量的并发

> 有别于基于通信的并发，基于共享内存的并发是传统做法，也是重要的部分。

> TODO: channel的原理，怎么加的锁

**避免数据竞争的方法**

1. 避免从多个goroutine访问变量。
由于其它的goroutine不能够直接访问变量，它们只能使用一个channel来发送请求给指定的goroutine来查询更新变量。这也就是Go的口头禅“**不要使用共享数据来通信；使用通信来共享数据**”。（这个唯一能访问、操作变量的goroutine叫monitor go routine)

2. (传统方法)互斥

## sync.Mutex

前言: 使用buffer=1的channel构造二元信号量(binary semaphore):

```Go
var (
    sema    = make(chan struct{}, 1) // a binary semaphore guarding balance
    balance int
)

func Deposit(amount int) {
    sema <- struct{}{} // acquire token
    balance = balance + amount
    <-sema // release token
}

func Balance() int {
    sema <- struct{}{} // acquire token
    b := balance
    <-sema // release token
    return b
}
```

直接用`sync.Mutex`代替实现二元信号量:
```Go
import "sync"

var (
    mu      sync.Mutex // guards balance
    balance int
)

func Deposit(amount int) {
    mu.Lock()
    balance = balance + amount
    mu.Unlock()
}

func Balance() int {
    mu.Lock()
    b := balance
    mu.Unlock()
    return b
}
```

惯例来说，被mutex所保护的变量是在mutex变量声明之后立刻声明的。

在Lock和Unlock之间的代码段中的内容goroutine可以随便读取或者修改，这个代码段叫做临界区。

在临界区较长的时候，我们用defer来调用Unlock，临界区会隐式地延伸到函数作用域的最后，这样我们就从“总要记得在函数返回之后或者发生错误返回时要记得调用一次Unlock”这种状态中获得了解放。Go会自动帮我们完成这些事情。

```Go
func Balance() int {
    mu.Lock()
    defer mu.Unlock()
    return balance // 甚至在return语句读取balance之后, defer才执行
}
```

TODO: 一个通用的解决方案是将一个函数分离为多个函数，比如我们把Deposit分离成两个：一个不导出的函数deposit，这个函数假设锁总是会被保持并去做实际的操作，另一个是导出的函数Deposit，这个函数会调用deposit，但在调用前会先去获取锁。同理我们可以将Withdraw也表示成这种形式：

## sync.RWMutex

## 内存同步

```Go
var x, y int
go func() {
    x = 1 // A1
    fmt.Print("y:", y, " ") // A2
}()
go func() {
    y = 1                   // B1
    fmt.Print("x:", x, " ") // B2
}()
```
这样可能会输出
```
x:0 y:0
y:0 x:0
```
非常奇怪吧！
- 因为赋值和打印指向不同的变量，编译器可能会断定两条语句的顺序不会影响执行结果，并且会交换两个语句的执行顺序。
- 如果两个goroutine在不同的CPU上执行，每一个核心有自己的缓存，这样一个goroutine的写入对于其它goroutine的Print，在**主存同步**之前就是不可见的了。

## sync.Once

懒初始化的可重入实现，传入初始化函数。

```Go
var loadIconsOnce sync.Once
var icons map[string]image.Image
// Concurrency-safe.
func Icon(name string) image.Image {
    loadIconsOnce.Do(loadIcons) // loadIcons会去初始化icons
    return icons[name]
}
```
> 这不就是，懒汉单例模式了嘛~


使用一个mutex对

## 竞争条件检测

`-race` 

## Goroutines和线程

TODO!