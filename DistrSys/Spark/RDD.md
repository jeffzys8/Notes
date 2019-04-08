# RDD

rdd: 可容错, 可并行的数据集

## 并行数据集

并行数据集 - 顾名思义了. 问题是该将数据集分多少个partitions? 一般这是Spark自动操控的, 集群里的每一个服务器都分得2-4个partitions (因为都是多核); 当然, 这个partition数值也是可以设置的 (**注意**, 有些地方可能把partition记为slices, 这是版本遗留问题)

(并行求和示例):
```scala
val data = Array(1, 2, 3, 4, 5)
val distData = sc.parallelize(data)
distData.reduce((a,b) => a+b)

val distData2 = sc.parallelize(data, 2)
```

## 外部数据集

Spark可以从Hadoop支持的外部存储源创建RDD: 本地fs, HDFS...

(map+reduce 计算所有单词的示例)
```scala
val distFile = sc.textFile("filename")
distFile.map(i => i.split(" ").length).reduce((a,b)=>a+b)
```

读取文件的注意事项:
- 工作节点相同路径要可用
- 支持通配符
- 也可配置分区 (这句话什么意思: Note that you cannot have fewer partitions than blocks)

## RDD operation - transformation

RDD --> RDD'

转换操作 lazy 直到 action; 使用`persist/cache`方法保持RDD到内存, 否则每次跑action都会执行一次 (**看起来是个需要常用的重点内容了**)

常见转换:   //TODO 还有很多没整理的transformation
- map: 进行等量转换
- filter: 返回return true的
- flatMap:  //TODO 仍然存疑, 文档解释看不懂
- mapPartitions
- mapPartitionsWithIndex
- groupByKey
- reduceByKey
- aggregateByKey

## RDD operation - action

RDD --> aggregated result: reduce

常见动作:
- reduce(func)
- collect() //TODO 没用过collect(), 要用一用
- count()
- first()
- take(n)
- takeSameple 略
- takeOrdered
- saveAs... 好几个,略
- countByKey()
- foreach(func)

## 函数传递

- 匿名函数, 用的多了不说了
- 全局单例对象中的静态方法
    ```scala
    object MyFunctions {
        def func1(s: String): String = { ... }
    }
    myRdd.map(MyFunctions.func1)
    ```
- 类实例中的方法(比较麻烦?) //TODO 学习好Scala语法后回来补充

## 闭包 

这里的闭包不是函数闭包, 是指spark集群中每个task执行是一个closure (传递给每个executor的数据是pass-by-value)

示例: RDD-action 修改外部变量时的出错, 最终counter仍为0
```scala
var counter = 0
var rdd = sc.parallelize(data)

// Wrong: Don't do this!!
rdd.foreach(x => counter += x)

println("Counter value: " + counter)
```
- 解决方法: 使用**Accumulator** //TODO 补充累加器部分

另外, 在控制台输出也可能遇到这样的问题: 实际上是executor在执行`println`
- 解决办法: 使用`rdd.collect()`, 但这会造成很大的内存使用! 打印部分元素更安全的做法是 `rdd.take(100).foreach(println)`

## 键值对

示例: 用map创建键值对, 计算每行出现的次数
```scala
val lines = sc.textFile("data.txt")
val pairs = lines.map(s => (s, 1))
val counts = pairs.reduceByKey((a, b) => a + b)
```

**示例2: WordCount**
```scala
val lines = sc.textFile("data.txt")
val words = lines.flatmap(line => line.split(" "))
val pairs = words.map(word => (word, 1))
val counts = pairs.reduceByKey((a, b) => a + b)
```

//TODO 这里说自定义 equals() 需要有对应的 hashCode()是什么意思

## Shuffle

Shuffle操作出现的例子: reduceByKey操作需要两两合一, 但同key的pair很有可能不在同一个partition, 甚至不在同一个machine, 他们执行运算的时候必须在 "一起", 这就是 `shuffle`

另外, 如果需要使shuffle以后数据有序, 可以使用
- `mapPartitions`
- repartitionAndSortWithinPartitions
- sortBy 

//TODO 这一部分(结合Map-Reduce)肯定是关于效率的很重要的内容 - 一定要反复去看; 而且还涉及到了中间文件的GC; 这就是区分入门和专家的地方了; 注意这是可以通过配置参数来调整随机行为的

## RDD持久性

//TODO 持久性也是提升运行效率的重要内容之一了

`persist()`, `cache()`; 

Spark的持久化缓存具有容错性, 如果丢失了某部分, 它会用最初的transformation自动重新计算

另外, 持久化RDD有不同的存储级别 (这里略过, 既有内存, 也有磁盘, 还有冗余备份); 选择哪种存储级别文档有介绍:
>Spark的存储级别旨在提供内存使用和CPU效率之间的不同折衷。我们建议您通过以下流程选择一个:
> - 如果您的RDD与默认存储级别（MEMORY_ONLY）很适应，请保持这种状态，此状态CPU效率最高。
> - 若不适应，尝试使用MEMORY_ONLY_SER并选择快速序列化库，以使对象更节省空间，但访问速度仍然相当快。（Java和Scala）
> - 不要轻易用磁盘, 除非数据极其重要或者有大量的过滤操作 (??) 
> - 如果要快速故障恢复，请使用复制的存储级别（例如，如果使用Spark来处理来自Web应用程序的请求）。所有存储级别通过重新计算丢失的数据提供完全容错，但复制的存储级别允许您继续在RDD上运行任务，而无需等待重新计算丢失的分区。

另外, Spark会以 `LRU` (最近最少使用) 的原则删除旧数据, 主动删除可以使用 RDD.unpersist() (显然又是一个性能提升点了)

## 共享变量

正如前面闭包所说, 每一个executor获得的是一个变量副本, 需要使用共享变量来突破这一瓶颈

**Broadcast Variables**:

广播变量, 只读
```scala
val broadcastVar = sc.broadcast(Array(1, 2, 3))
broadcastVar.value
```
//TODO 广播变量文档也有不少看不懂的内容, 和shuffle的stages有关

**Accumulators**

累加器, 只写(用于统计汇总)
```scala
val accum = sc.longAccumulator("My Accumulator")
sc.parallelize(Array(1, 2, 3, 4)).foreach(x => accum.add(x))
```

当然除了`Long` 和 `Double`, 可以通过继承 `AccumulatorV2`来创建自己的类型 (具体需要覆盖的方法就不提了, 查查文档就有)

**注意:** 累加器仍然遵循Spark的lazy, 因此下段代码accumulator依旧是0
```scala
val accum = sc.longAccumulator
data.map { x => accum.add(x); x }
// Here, accum is still 0 because no actions have caused the map operation to be computed.
```
//TODO 可是自己测试的时候发现并不是这样啊! accum还是累加了. 莫非是因为我是单机操作

## 单元测试

//TODO 单元测试框架我都没怎么接触过, 我得先自己去了解一波