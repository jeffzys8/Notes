# 入门

## 编程范式

- 命令式编程 - 冯诺依曼 - java, c++
- 函数式编程 - lambda - scala部分

命令式涉及多线程的状态共享(锁), 函数式编程不会在线程共享状态, 无需锁;

Scala是多范式
- 纯粹的面对对象, 每一个操作都是方法
- 也是函数式 (函数作为值)

## HelloWorld

如果有对象 - 就必须先`scalac`编译; 和Java不一样, 编译的对象可以和文件名不一样, 从而出现这种环节:
```shell
$ scalac test.scala
$ scala -classpath . HelloWorld
```

# 基本语法

## 变量

val - 字面量, 声明时初始化且不可变
var - 可变, 也需要在声明时初始化

```scala
val myStr = "Hello"
var myStr2 : String = "Hello2"
var myStr3 : java.lang.String = "Hello3"
// 默认导入了 java.lang._
// val 不可变
```

## 数据类型和操作

不分基本数据类型和类 —— 全都是类, 都属于`scala`包; 所有操作符都是对象的方法, 例如以下等价:

```scala
scala> val sum = 5+3
sum: Int = 8

scala> val sum2 = (5).+(3)
sum2: Int = 8
```

富包装类 - 会隐式转换
```scala
3 max 5
```

## Range

创建数值序列
```scala
// 含终点, 步长1
scala> 1 to 5
res0: scala.collection.immutable.Range.Inclusive = Range(1, 2, 3, 4, 5)

// 不含终点, 步长1
scala> 1 until 5

// 步长为2
scala> 1 to 10 by 2
```

## 控制台输入输出

都属于对象 `scala.io.StdIn` 的方法, 使用前需先导入; `readInt, readDouble..., readLine`

# 控制结构

主要是for: `for (变量 <- 表达式)` (括号里称为生成器)
```scala
for (i <- 1 to 5) println(i)
```

守卫: 对循环变量进行判断 (自己在代码块里加也一样, 更简洁)
```scala
for (i <- 1 to 5 if i%2==0) println(i)
```

多个生成器(简洁的多重循环)+多个守卫
```scala
for (i <- 1 to 5 if i%2==0; j <- 1 to 3 if j!=i) println(i, j)
```

for 推导式, 返回一个生成器中集合类型的值; 用于通过for循环遍历一个或多个集合得到新的集合, 用于后续计算
```scala
val r=for(i <- 1 to 5 if i%=2) yield {println(i); i}
```

# 未归类

## 内联函数

```scala
val pythonLines = lines.filter(line => line.contains("Python"))
// lines: rdd.RDD[String]
```

# 疑问

- 单例对象
- 不支持java中的 "checked exception" (这是什么), 所有异常都当做运行时异常