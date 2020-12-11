# Go 代码组织
[How to write go codes](https://golang.org/doc/code.html#Workspaces)

> 看起来这是旧版本的东西了(基于`GOPATH`那一套)


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