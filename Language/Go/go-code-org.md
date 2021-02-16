# Go 代码组织


- [How to write go codes(with go modules)](https://golang.org/doc/code.html)
- [How to write go codes(with `$GOPATH`)](https://golang.org/doc/gopath_code.html)
> `$GOPATH`是旧的方式，在使用`godep`之类的工具时还会用到


## Go Modules


Go code is grouped into packages, and packages are grouped into modules.

Your package's module specifies the context Go needs to run the code, including the Go version the code is written for and the set of other modules it requires.


```
go mod init examples/greetings
```

import本地module

```
replace example.com/greetings => ../greetings
```