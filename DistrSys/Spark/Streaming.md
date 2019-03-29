# Streaming

## Simple Ex.


```scala
val conf = new SparkConf().setMaster("local[2]").setAppName("NetworkWordCount")

// ssc 可以理解为spark stream的 "驱动程序", 每秒一个DStream
val ssc = new StreamingContext(conf, Seconds(1))

val lines = ssc.socketTextStream("localhost", 8888)
val words = lines.flatMap(_.split(" "))
var pairs = words.map(word => (word, 1))
val wordCounts: DStream[(String, Int)] = pairs.reduceByKey(_ + _) //这写法太帅了
wordCounts.print()

ssc.start()
ssc.awaitTermination()
```

## StreamingContext

`StreamingContext`是流的入口;
```scala
val ssc = new StreamingContext(conf, Seconds(1)) 
```
这种创建方法`ssc.sparkContxt`在内部创建了; 
```scala
val sc = ...
val ssc = new StreamingContext(sc, Seconds(1))
```
这种方法基于原有SparkContext创建

创建时设定了DStream的时间间隔, 这个间隔是需要注意调整的 //TODO 后面补一下StreamingContext的设定时间间隔

一个StreamingContext定义以后需要做以下的事情:

1. 设定一下数据源 (文本? socket? 并行数据集?)
2. 定义transformation 和 action
3. `ssc.start()`
4. `ssc.awaitTermination()`

注意在这以后, 就不能再添加新的操作了; 可以使用 `ssc.stop()` 停止接受数据, 如果想要复用`sc`, 记得要设置一下`stop()`的参数 (为什么要复用? 可以重复创建多个StreamingContexts); 一个JVM只能有一个StreamingContext

## DStreams

//TODO

将逻辑扩展到在集群上运行时，分配给Spark Streaming应用程序的核心数必须大于接收器数。否则系统将接收数据，但无法处理数据。