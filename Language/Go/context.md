# Context

https://golang.org/pkg/context/

https://blog.golang.org/context

https://blog.golang.org/pipelines

对于一个Go Server, 每个请求都有一组 gorountines (获取鉴权, 访问数据库, RPC请求等); 需要在这个请求的某一异步环节被取消或超时时，其他的goroutine都收到通知并取消

```Go
type Context interface {
    // `Done` 返回一个只读channel, 当该`context`被cancel或timeout时, 该channel会关闭.
    // 补充: channel关闭后, 读该channel会返回零值, 且 ok 为 false
    Done() <-chan struct{}

    // Err 在Done channel 被close以后(意味着context结束), 以error的形式说明该context为什么被cancel了
    Err() error

    // Deadline 返回timeout的具体时刻, 假如不存在 ok 会设为false
    Deadline() (deadline time.TIme, ok bool)

    // Value 按key查值, 用来传一些请求相关的 metadata
    Value(key interface{}) interface{}
}
```

- `Done` channel用来传递 "该context相关的函数应丢弃当前工作并返回" 的信号(Don't communicate with shared memory)
- 能注意到, `Context`接口本身并不包含 `Cancel`, 这意味着收到context的一方不能取消context, 而应该由发出context一方cancel(**这很合理, 否则派生context能把父context给取消了**); 同理, `Done` channel是只读的, 防止被context接收方修改了

## 派生context

可以通过`WithCancel`, `WithTimeout`这样的函数继承现有context并派生新的context，当父context被cancel时，其派生context也会被cancel.

`Background` 用于生成 Context root, **永远不会cancel**
```Go
func Background() Context
```

`WithCancel` 返回一个可调用`cancel`主动取消的派生context

```Go
func WithCancel(parent Context) (ctx Context, cancel CancelFunc)

type CancelFunc func()
```

`WithTimeout` 返回一个可主动取消, 且在设定时间后也会自动取消的派生context

```Go
func WithTimeout(parent Context, timeout time.Duration) (Context, CancelFunc)
```
`WithValue` 返回一个携带k-v数据的派生context
```Go
func WithValue(parent Context, key interface{}, val interface{}) Context
```

> TODO: 它给的例子里，`userip`包用`type`避免了key冲突，这可以归纳一下Go相等的判断(值,指针,`interface{}`)