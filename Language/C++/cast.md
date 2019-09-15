# C++四种cast操作符

- 对于具有转换的简单类型而言C 风格转型工作得很好; 
- ANSI-C++标准定义了四个新的转换符: `reinterpret_cast`, `static_cast`, `dynamic_cast`和`const_cast`，目的在于**控制类和类指针之间的类型转换**

## reinpreter_cast

> 引用: https://zh.cppreference.com/w/cpp/language/reinterpret_cast

```c++
reinpreter_cast<type-id>(expression)
```

- **空间进行直接的内存拷贝，最通用的底层转换**
- 在类型之间转换不做任何类型的检查和转换
- **编译时指令, 不会生成具体CPU指令**，指示编译器将 表达式 视为如同具有 新类型 类型一样处理
- 转换不允许去掉常量性、易变性
- 用于：基本类型互转，指针互转，指针与整型互转 (**结果依赖平台**)

### 示例

```c++
int n=9;
double d=reinterpret_cast<double &>(n); 
// reinterpret_cast 仅仅是复制 n 的比特位到 d，因此d 包含无用值。
```

### 延伸思考

如何将任意的基本数据类型的二进制输出，然后比较一下 `reinterpret_cast` 以后是否有数据丢失 (int->double->int, double->int->double)

### 遗留的疑问

5) 任何对象指针类型 `T1*` 可转换成指向对象指针类型 `cv T2` 。这严格等价于 `static_cast<cv T2*>(static_cast<cv void*>(expression))`（这意味着，若 T2 的对齐要求不比 T1 的更严格，则指针值不改变，且将结果指针转换回原类型将生成其原值）。任何情况下，只有类型别名化（type aliasing）规则允许（见下文）时，结果指针才可以安全地解引用

> 这是在讲什么?


## const_cast

```c++
const_cast<type_id>(expression)
```

- **用于修改类型的 `const` 或 `volatile` 属性**，一般用于强制消除对象的常量性

## static_cast

```c++
static_cast<type-id>(expression)
```

- 和 `reinterpre_cast` 一样不允许转走常量性或易变性
- **用隐式和用户定义转换的组合在类型间转换**
- 最类似于强制类型转化?

// TODO: static_cast

## dynamic_cast

```c++
dynamic_cast<type-id>(expression)
```

- 沿继承层级向上、向下及侧向，**安全**地转换到其他类的指针和引用
- 和 `reinterpre_cast` 一样不允许转走常量性或易变性
- 各种运行时检查，**开销大**
- 亦可用 static_cast 进行向下转型，它避免运行时检查的开销，但只有在程序（通过某些其他逻辑）能够保证 表达式 所指向的对象肯定是 Derived 时才是安全的。

> 这东西好复杂, down_cast, side_cast, 必须要用例子来辅助理解

// TODO: dynamic_cast 举例

# 其他内容

## 向下转型

就是基类向转型; `dynamic_cast` 和 `static_cast` 都可以实现，但关键在于 `dynamic_cast` 有运行时检查，而 `static_cast` 需程序逻辑自行保证是derived（因此开销肯定是dynamic大了） 

## 侧向转型

// TODO: 侧向转型

## cv限定

### `const`

不能修改，修改会引起编译时错误

### `volatile`

禁止编译器对该变量做优化，每次读、写都得对保存的地址做直接调用

比如嵌入式的端口变量，如果变量访问被编译器优化了，那可能数据在端口发生变化的时候代码感知不到而导致错误

> // TODO: std::atomic, CAS原语, volatile 在线程安全中的使用（还有内存屏障的概念）