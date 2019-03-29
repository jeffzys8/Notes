# 杂记

## rdd v.s. dataset

```scala
sc.textFile("textname") //RDD
spark.read.textFile("textname") //dataset
```

## Dataset --> cache
 
## 本地跑scala-jar

```shell
spark-submit --class app.SimpleApp --master local spark-test-1.0.0-jar-with-dependencies.jar
```

> 最好配置用那种 "控制台接收" 的形式来输入, 不要硬编码

## TDW-spark的API文档

[API文档](https://git.code.oa.com/tdw/tdw-spark-common/wikis/api)

# questions

- RDD 的 `map` 和 `flatMap` 有什么区别
    ```scala
    val lines = sc.textFile("filename")
    val lineCount = sc.map(line => line.split(" "))
    val lineCount2 = sc.flatMap(line => line.split(" "))
    ```
    对于`map`, 只是将RDD的每一行拆分成单词数组, 可以将`lineCount`理解为数组的数组; 而`flatMap` 则将所有数组整合成为一个数组了
- `groupByKey`, `reduceByKey`, `aggregateByKey`
- 如何对性能进行测试