# 头文件

原本以为头文件就是一个定义类的地方；如果有变量的话也得定义到类里的static变量；其实可以用`extern`声明变量;

头文件只能有声明，不能有定义，但有三个例外：
- 定义类：以往使用的static变量的生存地
- 定义值在编译时就能确定的const对象：有点像宏
- 定义inline函数 ("宏函数", 且必须是在头文件定义)

头文件保护符——防止多重包含引起的重复定义

```c++
#ifndef SALESITEM_H
#define SALESITEM_H
//进行头文件的定义
#endif
```

# string

字符处理见于 p77，这是C标准库的`cctype` (在C语言中为`ctype.h`)

string也是具有`iterator`的

# vector

像`size`这样小型的函数一般都被定义为inline, 所以为了安全, 在for循环比较比较的时候不要去提前存储size值，在每次比较的时候调用`vector::size()`即可

如果vector为空, `vector::begin()` 等于 `vecotr::end()`

两个迭代器之间的距离: `difference_type`

`const_iterator`: 用于只读操作；`*const_iterator` 得到一个指向const对象的引用，对迭代器本身(const对象的指针)的修改是允许的(因此才能有for遍历中的++操作)，但对其解引用的对象(const对象)进行修改是不允许的<br>
- 要注意区分`const_iterator`和`const iterator`，前者是本身可修改，解引用值不可修改；后者是本身不可修改，解引用值可修改

# bitset

用string给bitset初始化的时候，需要记得bitset在赋值完成后是反向的

# 数组

数组长度固定，且无法知道数组长度（只能知道分配了多少`sizeof`)

> 记得留意数组的初始化方法

**初始化**
- `int ia[5] = {0, 1, 2}`，未覆盖部分初始化为0
- 在函数体内定义的内置数组 (局部变量)，无初始化；<br>在函数体外定义的内置数组 (类成员、全局)，初始化为0
- `int ia[] = {0, 1, 2}`

# 指针

`void`指针可以保存任何类型对象的地址，但不允许操纵其指向的对象

**指针和引用的比较**：引用必须初始化指向某个对象，且不能再修改指向的对象（可以理解为某个变量的别名）；而指针指向的是某个对象的地址，它可以通过赋值指向其他对象的地址。



（自以为）指向const对象的指针：
- 必须也有const特性：`const double *cptr`，不能修改其指向对象值
- `const int` 和 `int const` 是一样的
- 注意这里的const并不是指指针为const，而是说明它**指向的对象类型为const**
- 因此不需要对`cptr`进行初始化，也可以重新对指针赋值

const指针:
- 指针不能变，但可以操纵其指向值（除非指向值为const）
- `double *const cptr` ，注意*的位置

（自以为）指向const对象的const指针
- 既不能修改指针值，也不能操作指针指向的值
- `const int *const ptr` 或 `int const *const`

关于typedef和指针：
```cpp
typedef int *pt_type;   //int是pt_type的解引用，因此pt_type是int的指针
pt_type pt_m = &i;
cout << *pt_m << endl;

typedef int *pint;    //pint是int的指针
const pint c = &i; 
// 这里不能直接文本翻译为 const int *c
// 事实上，const修饰了pint，所以c应该是一个const指针
// 写作 int *const c;
```