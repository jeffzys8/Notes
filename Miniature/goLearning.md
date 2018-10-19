- [HelloWorld](#helloworld)
- [基础](#基础)
	- [包](#包)
	- [函数](#函数)
	- [变量](#变量)
	- [控制语句](#控制语句)
	- [指针](#指针)
	- [结构体](#结构体)
	- [数组和切片](#数组和切片)
	- [映射](#映射)
	- [函数值](#函数值)
	- [函数闭包](#函数闭包)
- [方法和接口](#方法和接口)
	- [方法](#方法)
	- [接口](#接口)

# HelloWorld

```Go
package main
/*
Every Go program is made up of packages.

Programs start running in package main.
*/

import "fmt"

func main() {
	fmt.Println("Hello, 世界")
	
}
```

# 基础

## 包

按照约定，包名与导入路径的最后一个元素一致。例如："math/rand"下的包源码均以 ```package rand``` 语句开始

- 导入一个包
```Go
import "fmt"
```

- 导入多个包
```Go
import (
	"fmt"
	"math/rand"
)

import "fmt"
import "math"
```

包中大写为“导出”

```Go
fmt.Println(math.pi) //报错
fmt.Println(math.Pi) //正确
```

## 函数

简单的例子：
```Go
package main

import "fmt"

/*
- variable type: after name
- return value type : after parameters
*/
func add(x int, y int) int {
	return x + y
}

func main() {
	fmt.Println(add(42, 13))
}
```

相同类型形参可省略一个,例如上面
```Go
x int, y int
```
可写作
```Go
x, y int
```

返回值可为多个
```Go
func swap(x, y string) (string, string) {
	return y, x
}

func main() {
	a, b := swap("hello", "world")  //简洁赋值，等价于var a, b string = swap(...)
	fmt.Println(a, b)
}
```

命名返回值，直接返回这些命名；长函数会影响可读性
```Go
func split(sum int) (x, y int) {
	x = sum * 4 / 9
	y = sum - x
	return
}
```

## 变量
又是一个例子
```Go
package main

import "fmt"

var c, python, java bool

func main() {
	var i int
	fmt.Println(i, c, python, java)
}

/* 输出结果：
0 false false false

看来是有默认初始化的过程
*/
```

基本类型

```Go
bool    //1 byte

string 

int  int8  int16  int32  int64
uint uint8 uint16 uint32 uint64 uintptr

/*
其中，int, uint 和 uintptr（尚未知道用处）在32位系统上为32位(4bytes)宽，64->64。需要整数时尽量使用int
*/

byte // alias for uint8

rune // alias for int32
     // represents a Unicode code point

float32 float64
//注意这里没有单纯的float了

complex64 complex128
```


多个变量同时赋值
```Go
var i, j int = 1, 2
var c, python, java = true, false, "no!" //省略类型，自动从值获取
```

简洁赋值语句；不能在函数外使用（函数外每个语句必须以关键字开始，如var, func）
```Go
k := 3
//等价于
var k = 3
```

语法块变量声明
```Go
var (
	ToBe   bool       = true
	MaxInt uint64     = 1<<64 - 1
	z      complex128 = cmplx.Sqrt(-5 + 12i)
)
```

默认初始化值
```Go
func main() {
	var i int
	var f float64
	var b bool
	var s string
    fmt.Printf("%v %v %v %q\n", i, f, b, s)
}

/* 输出结果
0 0 false ""
*/
```

类型转换：和C++类似，但不存在隐式转换
```Go
i := 42PP
f := float64(i)
u := uint(f)

//下面会报错
i := 42
var f float64 = i
```

常量const关键字，不能用:=赋值
```Go
const World = "世界"
```

## 控制语句

Go中循环结构只有 for; for无小括号，但一定要有大括号
```Go
sum := 0
for i := 0; i < 10; i++ {
	sum += i
}
fmt.Println(sum)
```

for内三个语句都是optional（可作while语句）
```Go
for ;sum<1000;{}
for sum<1000 {} //分号再去掉，形如while
for {}  //无限循环
```

条件语句，也是无需小括号，必须大括号
```Go
if x < 0 {}
```

if条件判断前可执行一个简单语句, 该语句声明的变量仅在if内
```Go
if v := math.Pow(x, n); v < lim {
	return v
}
```

练习 - 利用牛顿法实现平方根函数

```Go
package main

import (
    "fmt"
    "math"
)

func Sqrt(x float64) float64{
    z := 1.0
    for math.Abs(z*z - x) > 1e-15{
        z -= (z*z - x) / (2*z)
    }
    return z
}

func main(){
    fmt.Println(Sqrt(2))
    fmt.Println(math.Sqrt(2))
}
```

switch 语句

- 不需要break，只会运行选中的case
- 可通过在结尾加 fallthrough 语句延续分支
- case无需为常量，取值不必为整数

```Go
fmt.Print("Go runs on ")
switch os := runtime.GOOS; os {
case "darwin":
    fmt.Println("OS X.")
case "linux":
    fmt.Println("Linux.")
default:
    // freebsd, openbsd,
    // plan9, windows...
    fmt.Printf("%s.", os)
}
```

switch 可以不带条件（等价于switch(true))，可当做if then else结构，更加清晰
``` Go
t := time.Now()
switch {
case t.Hour() < 12:
    fmt.Println("Good morning!")
case t.Hour() < 17:
    fmt.Println("Good afternoon.")
default:
    fmt.Println("Good evening.")
}
```

defer，在外层函数返回后执行，其实参会立即求值（压入栈中）
```Go
func main() {
	fmt.Println("counting")

	for i := 0; i < 10; i++ {
		defer fmt.Println(i)
	}

	fmt.Println("done")
}

/* 输出结果：
    counting
    done
    9
    8
    ...
    0
*/
```

## 指针

指针零值为```nil```,声明如下：
```Go
var p *int
```

& * 操作和 C比较类似，但没有指针运算
```Go
//&操作
i := 42
var p *int = &i
//*操作
fmt.Println(*p)
*p = 21
```

## 结构体
简单例子
```Go
type Vertex struct {
	X int
	Y int
}

func main() {
    fmt.Println(Vertex{1, 2})
    //注意其赋值使用大括号
}
```
通过名字赋值
```Go
v1 = Vertex{1, 2}
v2 = Vertex{X: 1}   //v2.Y = 0
```
点号访问成员
```Go
v := Vertex{1, 2}
v.X = 4
fmt.Println(v.X)
```
指针访问结构体，```(*p).X``` 等价于 ```p.X```
```Go
v := Vertex{1, 2}
p := &v
(*p).X = 1e9
p.X = 1e9
fmt.Println(v)
```

## 数组和切片
数组例子，大小需为常数
```Go
func main() {
	var a [2]string
	a[0] = "Hello"
	a[1] = "World"
	fmt.Println(a[0], a[1])
	fmt.Println(a)

	primes := [6]int{2, 3, 5, 7, 11, 13}
	fmt.Println(primes)
}
```

切片很常用
- 左闭右开
- 就像数组的局部引用，本身不存数据
- 上下界可忽略（下界默认0，上界默认数组/切片长度）
```Go
func main() {
	primes := [6]int{2, 3, 5, 7, 11, 13}

	var s []int = primes[1:4]
	fmt.Println(s)
}
/* 输出结果
    [3, 5, 7]
*/
```

值得注意的是，这是一个数组：
```Go
[3]bool{true, true, false}
```

而这创建了同样的数组，再构建了它的切片
```Go
[]bool{true, true, false}
```

结构体和切片结合的例子
```Go
s := []struct {
		i int
		b bool
	}{
		{2, true},
		{3, false},
		{5, true},
		{7, true},
		{11, false},
		{13, true},
	}
```

切片的长度：包含的元素个数
切片的容量：从它包含的第一个元素开始，到其底层数组末尾的个数（倒是暂时不懂有什么用）
```Go
s := []int{2, 3, 5, 7, 11, 13}
len(s)	//6
cap(s)	//6
```

切片空 - nil
```Go
var s []int
fmt.Println(s, len(s), cap(s))
if s == nil {
	fmt.Println("nil!")
}
```

用make创建切片 - 相当于动态创建数组，初始值都为0
```Go
a := make([]int, 5)  // len(a)=5
b := make([]int, 0, 5) // len(b)=0, cap(b)=5
```

切片中的切片以及修改的例子

```Go
// Create a tic-tac-toe board.
board := [][]string{
	[]string{"_", "_", "_"},
	[]string{"_", "_", "_"},
	[]string{"_", "_", "_"},
}

// The players take turns.
board[0][0] = "X"
board[2][2] = "O"
board[1][2] = "X"
board[1][0] = "O"
board[0][2] = "X"

// modify the first line (by myself)
board[0] = board[0][0:1]
board[0] = board[0][:cap(board[0])]
```

向切片追加元素，注意：这个操作会导致底层数组被挤占，如果cap不够，会分配一个更大的数组

```Go
var k = [5]int{1,2,3,4,5}
	//k: [1 2 3 4 5]
m := k[:3]
	//m: [1 2 3]
m = append(m, 200)
	//m: [1 2 3 200]; k: [1 2 3 200 5]
m = append(m, 300)
	//m/k: [1 2 3 200 300]
m = append(m, 400)
	//m: [1 2 3 200 300 400]; k: [1 2 3 200 300]
```

```Go
var k = [5]int{1,2,3,4,5}
	//k: [1 2 3 4 5]
m := k[:3]
	//m: [1 2 3]
m = append(m, 200, 300, 400)
	//m: [1 2 3 200 300 400]
	//k: [1 2 3 4 5]
```

Range遍历切片
```Go
var pow = []int{1, 2, 4, 8, 16, 32, 64, 128}

func main() {
	//index + value
	for i, v := range pow {
		fmt.Printf("2**%d = %d\n", i, v)
	}
	//只有index (i)
	for i := range pow {
		pow[i] = 1 << uint(i) // == 2**i
	}
	//只有value(value), index用_省略掉
	for _, value := range pow {
		fmt.Printf("%d\n", value)
	}

}
```

Slice 练习 —— 构造给定长度的二维数组并绘图
```Go
package main

import "golang.org/x/tour/pic"

func Pic(dx, dy int) [][]uint8 {
	result := [][]uint8{}
	for y := 0; y < dy; y++ {
		temp := []uint8{}
		for x := 0; x < dx; x++{
			temp = append(temp,uint8((x+y)/2))
		}
		result = append(result, temp)
	}
	return result
}

func main() {
	pic.Show(Pic)
}
```

## 映射

一个例子 —— 基本类型
```Go
k := make(map[string]int)
k["hello"] = 3
fmt.Println(k["hello"])
```

另一个例子 —— 结构体
```Go
type Vertex struct {
	Lat, Long float64
	Short float64
}

func main() {
	m := make(map[string]Vertex)
	m["Bell Labs"] = Vertex{1,2,3}
	fmt.Println(m["Bell Labs"])
}
```

直接初始化
```Go
type Vertex struct {
	Lat, Long float64
}

var m = map[string]Vertex{
	"Bell Labs": Vertex{
		40.68433, -74.39967,
	},
	"Google": Vertex{
		37.42202, -122.08408,
	},
	//倒也诡异，这里的","不能省略
}

//简化
var m = map[string]Vertex{
	"Bell Labs": {40.68433, -74.39967},
	"Google":    {37.42202, -122.08408},
}
```

对映射的修改
```Go
//在映射 m 中插入或修改元素：
m[key] = elem

//获取元素：
/*	
	如元素不存在，则返回该元素类型的零值：
	例如 int 返回 0, string 返回 ""
*/
elem := m[key]

//获取并检测元素是否存在
elem, ok := m[key]

//只检测元素是否存在
_, ok := m[key]

//删除元素：
delete(m, key)
```
自创的和if复合使用
```Go
if _,ok:=m["Answer"];ok==true{}
```

练习：单词检测
```Go
package main

import (
	"golang.org/x/tour/wc"
	"strings"
)

func WordCount(s string) map[string]int {
	aMap := map[string]int{}
	aArray := strings.Fields(s)
	for _,v := range aArray{
		aMap[v]++;
	}
	return aMap
}

func main() {
	wc.Test(WordCount)
}
```

## 函数值

```Go
package main

import (
	"fmt"
	"math"
)

//作为形式参数
func compute(fn func(float64, float64) float64) float64 {
	return fn(3, 4)
}

func main() {
	hypot := func(x, y float64) float64 {
		return math.Sqrt(x*x + y*y)
	}
	fmt.Println(hypot(5, 12))

	//作为实际参数
	fmt.Println(compute(hypot))
	fmt.Println(compute(math.Pow))
}
```

## 函数闭包

函数闭包指的是这样的“**函数值**”：该函数值代表的函数使用了函数体外（非全局）的函数；这样就使得这个函数某种意义上来说“附于”这个/这些变量（有种对象的感觉了，只不过这个对象只有一个方法）

```Go
/*注意这里 adder 并不是一个函数闭包，adder()才是*/
func adder() func(int) int {
	sum := 0
	return func(x int) int {
		sum += x
		return sum
	}
}

func main() {
	pos:= adder()
	fmt.Println(pos(1))
	fmt.Println(pos(100))
}
```

练习：斐波那契数列
```Go
func fibonacci() func() int {
	former := 0
	latter := 1
	return func() int {
		temp := former
		temp2 := latter
		latter += former
		former = temp2
		return temp
	}
}

func main() {
	f := fibonacci()
	for i := 0; i < 10; i++ {
		fmt.Println(f())
	}
}
```

# 方法和接口

## 方法
Go 没有类。不过你可以为结构体类型定义方法。方法就是一类带特殊的 接收者 参数的函数。

打个栗子
```Go
import (
	"fmt"
	"math"
)

type Vertex struct {
	X, Y float64
}

func (v Vertex) Abs() float64 {
	return math.Sqrt(v.X*v.X + v.Y*v.Y)
}

func main() {
	v := Vertex{3, 4}
	fmt.Println(v.Abs())
}
```

为非结构体声明方法

```Go
type MyFloat float64
//直接为float64声明方法是非法的，因为它是包外的

func (f MyFloat) Abs() float64 {
	if f < 0 {
		return float64(-f)
	}
	return float64(f)
}
```

指针接收者：值得注意的是，如果用值接收者的话，方法是无法对这个接收者——天..还是叫对象吧——方法是无法对这个对象进行修改的，而指针接收者可以。

因此显然是指针接收者更为常用（我认为，要保护数据的时候就最好不用指针）
```Go
func (v *Vertex) Scale(f float64) {
	//如果不是*Vertex而是Vertex,这里修改对原对象无效
	v.X = v.X * f
	v.Y = v.Y * f
}

func main() {
	v := Vertex{3, 4}
	v.Scale(10)
	fmt.Println(v.Abs())
}
```

使用指针接收者时，实际参数无论是对象还是对象的指针，最终都会转化为指针传给函数(而单纯的函数是做不到这一点的，是否指针很严格)

```Go
func (v *Vertex) Scale(f float64) {
	v.X = v.X * f
	v.Y = v.Y * f
}

func main() {
	v := Vertex{3, 4}
	v.Scale(2)
	p := &Vertex{3, 4}
	p.Scale(2)
	fmt.Println(v, p)
	//可以看到由于指针的重定向，两个方式调用方法起到了同样的效果
}
```

同样地，在方法声明里这样
```Go
func (v Vertex) Scale(f float64)
```
实参也是既可以是指针也可以是值，当然其区别就在于这种声明无法修改对象的值；

**总而言之，方法是否能修改对象，要看函数声明的形参，和实参没有关系**

## 接口

接口类型的变量可以保存任何实现了这些方法的值

一个例子：
```Go
type Abser interface {
	Abs() float64
}

func main() {
	var a Abser
	f := MyFloat(-math.Sqrt2)
	v := Vertex{3, 4}

	a = f  // a MyFloat 实现了 Abser
	a = &v // a *Vertex 实现了 Abser

	// 下面一行，v 是一个 Vertex（而不是 *Vertex）
	// 所以没有实现 Abser。
	//a = v

	fmt.Println(a.Abs())
}

type MyFloat float64

func (f MyFloat) Abs() float64 {
	if f < 0 {
		return float64(-f)
	}
	return float64(f)
}

type Vertex struct {
	X, Y float64
}

func (v *Vertex) Abs() float64 {
	return math.Sqrt(v.X*v.X + v.Y*v.Y)
}

```

```接口值```也是一个对象，包含了具体对象的值以及它的类型，可以用printf输出；<br>
未赋值的接口值其对象值和类型都为```<nil>```
```Go
type I interface {
	M()
}

type T struct {
	S string
}

func (t T) M() {
	fmt.Println(t.S)
}

func main() {
	var i I

	i = T{"Hello"}
	describe(i)
	i.M()
	fmt.Printf("(%v, %T)\n", i, i)
}

/*
结果：({Hello}, main.T)
*/
```

如果将某个类型的```nil```对象赋予某个接口，方法依然会被调用
```Go
var t *T
i = t
fmt.Printf("(%v, %T)\n", i, i)
i.M()

/*
输出(<nil>, *main.T), 但M方法仍然会被调用
*/
```

对```nil```接口（注意不是和上面的```nil```对象值不同，这里是连类型都是```nil```）调用方法会panic

```Go
var i I
i.M() //报错
```

空接口的妙用：可以保存任意类型的值！（怎么有种Javascript的感觉了）
```Go
var i interface{}
i = 42
i = "hello"
```

类型断言：判断接口是不是某个类型，并且赋值
```Go
var i interface{} = "hello"	//string 类型

/* 若 i 保存了一个 T，那么 t 将会是其底层值，而 ok 为 true */
s, ok := i.(string)	
fmt.Println(s, ok)

/* 若 i 保存的不是一个 T，ok 将为 false 而 t 将为 T 类型的零值 */
f, ok := i.(float64)
fmt.Println(f, ok)

/* 不是T，又没有OK => panic */
f = i.(float64) 
fmt.Println(f)
```

接口类型选择(switch)
```Go
switch v := i.(type) {
case int:
	fmt.Printf("Twice %v is %v\n", v, v*2)
case string:
	fmt.Printf("%q is %v bytes long\n", v, len(v))
default:
	fmt.Printf("I don't know about type %T!\n", v)
}
```

**Stringer**类型，在```fmt```中的，是最普遍的接口之一。比如一个Print一个对象，默认会输出对象数据，但可以通过实现 ```String()```方法改变输出

```Go
type Person struct {
	Name string
	Age  int
}

func (p Person) String() string {
	return fmt.Sprintf("%v (%v years)", p.Name, p.Age)
}

func main() {
	a := Person{"Arthur Dent", 42}
	fmt.Println(a)
}

/*
当前输出： Arthur Dent (42 years)
如果不实现String()方法：{Arthur Dent 42}
*/
```

练习：输出IP地址。用到了fmt.Sprintf
```Go
package main
import "fmt"

type IPAddr [4]byte

// TODO: Add a "String() string" method to IPAddr
func (i IPAddr) String() string{
	var result string
	result = fmt.Sprintf("%v.%v.%v.%v",i[0],i[1],i[2],i[3])
	return result
}

func main() {
	hosts := map[string]IPAddr{
		"loopback":  {127, 0, 0, 1},
		"googleDNS": {8, 8, 8, 8},
	}
	for name, ip := range hosts {
		fmt.Printf("%v: %v\n", name, ip)
	}
}
```
