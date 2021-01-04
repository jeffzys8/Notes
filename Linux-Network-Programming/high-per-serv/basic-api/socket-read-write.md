# socket读写

用于文件读写的`read`和`write`同样适用于socket，但socket读写有专门的接口，可以对读写过程提供更好的控制。

## TCP数据读写

```c
#include <sys/types.h>
#include <sys/socket.h>
ssize_t recv(int __fd, void *__buf, size_t __n, int __flags);
ssize_t send(int __fd, const void *__buf, size_t __n, int __flags);
```

`recv`：
- 正常读返回读取数据的长度(可能小于给定的`len`，因此可能需要多次调用才能取得完整数据)
- 返回0意味着通信对方关闭了连接
- 出错时返回 -1 并设置errno

`send`:
- 成功时返回实际写入的数据长度
- 失败返回 -1 并设置errno

TODO: flags 和 带外数据的示例程序 + 5.9带外标记

## UDP数据读写

```c
#include <sys/types.h>
#include <sys/socket.h>
```

## 通用数据读写