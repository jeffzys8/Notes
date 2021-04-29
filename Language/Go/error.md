# Go 错误处理

https://blog.golang.org/go1.13-errors#TOC_2.1.

https://www.jianshu.com/p/f30da01eea97

```Go
package main

import (
	"errors"
	"fmt"
)

type ErrorParent struct {
	err error
}

func (ErrorParent) Error() string {
	return "a parent error"
}

func (e ErrorParent) Unwrap() error { return e.err }

type ErrorGrand struct {
	msg string
}

var (
	ErrorParentNoValue = ErrorGrand{"no such value"}
	ErrorParentUnknown = ErrorGrand{"unknown error"}
)

func (ErrorGrand) Error() string {
	return "a grand error"
}

func main() {

	grand := ErrorParentNoValue
	parent := ErrorParent{grand}

	fmt.Println(errors.Is(parent, ErrorParentNoValue))

}
```