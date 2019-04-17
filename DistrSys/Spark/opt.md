# Spark 优化

*关键字*: 
- 开发调优
- 资源调优
- 数据倾斜调优
- shuffle调优

*参考链接*:
1. [基础](https://tech.meituan.com/2016/04/29/spark-tuning-basic.html)
2. // TODO: [高阶优化](https://tech.meituan.com/2016/05/12/spark-tuning-pro.html)

<hr>

## 开发调优

### 原则一：避免创建重复的RDD

### 原则二：尽可能复用同一个RDD

### 原则三：对多次使用的RDD进行持久化

(持久化规则)

### 原则四：尽量避免使用shuffle类算子

- shuffle算子执行之前的代码会被划分为一个stage，shuffle算子执行以及之后的代码会被划分为下一个stage
- 例如reduceByKey, join
- 优化方法示例: Broadcast与map进行join

### 原则五：使用map-side预聚合的shuffle操作

> 通常来说，在可能的情况下，建议使用reduceByKey或者aggregateByKey算子来替代掉groupByKey算子。因为reduceByKey和aggregateByKey算子都会使用用户自定义的函数对每个节点本地的相同key进行预聚合; (用户自定义的函数???)

### 原则六：使用高性能的算子

- 使用reduceByKey/aggregateByKey替代groupByKey
- 使用foreachPartition, mapPartition
  - 但要小心OOM
- 使用filter之后进行coalesce操作
  - 数据减少后压缩partition
- 使用repartitionAndSortWithinPartitions替代repartition与sort类操作
  - //TODO: 没接触过

### 原则七：广播大变量

- 大变量不广播, 每个task都会有一个副本, 会导致频繁GC和网络传输负载大
- 广播大变量, 每个executor只有一个副本, 多个tasks共享这个大变量

### 原则八：使用Kryo优化序列化性能

- 三个序列化场景
  - 算子使用外部变量(广播大变量)
  - RDD泛型使用自定义类
  - MEMORY_ONLY_SER 持久化
- Spark默认用Java的序列化接口, 用Kryo快十倍
- //TODO: 没接触过, 以后再来弄

### 原则九：优化数据结构

- 集合类型 --> 数组
- 对象 --> 字符串
- 字符串 --> 基本类型

<hr>

## 资源调优

### Spark程序运行的基本原理

> 一个stage的所有task都执行完毕之后，会在各个节点本地的磁盘文件中写入计算中间结果，然后Driver就会调度运行下一个stage。下一个stage的task的输入数据就是上一个stage输出的中间结果。

### 参数详解

- num-executors: 
  - *注意executors不等于工作节点数量, 类似于进程*
  - 50 ~ 100? 有待商榷
- executor-memory
  - 4G ~ 8G
  - 不要超过队列最大总内存 1/3 ~ 1/2
- executor-cores
  - 2 ~ 4
  - 同样也是上面的规则
- driver-memory
  - 如果不用collect就不用考虑
- spark.default.parallelism
  - 设置每个stage默认tasks数量
  - 极其重要! 不好好设置分配的资源就浪费了
  - num-executors * executor-cores的2~3倍
  - **从kafka读出来的数据是按分片数分配task的**
- spark.storage.memoryFraction
  - RDD持久化数据在Executor内存中能占的比例, 默认0.6
  - 频繁gc需要调低, 执行内存不够了
- spark.shuffle.memoryFraction
  - shuffle过程中一个task拉取到上个stage的task的输出后，进行聚合操作时能够使用的Executor内存的比例，默认是0.2
  - 持久化少, shuffle高, 建议提升
  - 同样, 频繁gc需要调低
  - // TODO: 怎么看shuffle是否溢写到了磁盘

<hr>

## 数据倾斜

> 数据倾斜的原理很简单：在进行shuffle的时候，必须将各个节点上相同的key拉取到某个节点上的一个task来进行处理，比如按照key进行聚合或join等操作。此时如果某个key对应的数据量特别大的话，就会发生数据倾斜。比如大部分key对应10条数据，但是个别key却对应了100万条数据，那么大部分task可能就只会分配到10条数据，然后1秒钟就运行完了；但是个别task可能分配到了100万数据，要运行一两个小时。因此，整个Spark作业的运行进度是由运行时间最长的那个task决定的。