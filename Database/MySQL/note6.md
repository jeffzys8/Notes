# 视图

视图可以理解为 将`SELECT`语句形式化存储起来的 当做一个 “虚表”，这个表实际上是不存数据的，引用的数据还是原数据库的数据（在视图被调用时）。视图为多表连接的再次查询提供了便利。

在实践操作中发现，如果是单表创建的视图是可以进行 **实际数据的更改操作**，但多表创建的视图就是只读的了（这样当然可以理解：安全嘛）

> Algorithm 和 CheckOption是做什么的?

视图的作用:
1. 简化操作
2. 增强数据的安全性
3. 增加表的数据独立性

## 创建视图

基本语法

```sql
CREATE VIEW(别名s) AS 
    SELECT 语句
```

## 查看视图

```sql
DESCRIBE 视图名;
DESC 视图名;
SHOW CREATE VIEW 视图名;
SHOW TABLE STATUS LIKE 视图名; 
```

## 修改视图定义

```sql
CREATE OR REPLACE VIEW 视图名 AS SELECT 语句
ALTER VIEW 视图名 AS SELECT 语句
```

## 更新视图的数据

和表的操作是一样的，但会设计权限的设置；前面说单表可以多表不行的说法是不全面的，这很有可能是默认设置如此，实际上可以更改？

## 删除视图

只会删除视图的定义，并不会删除数据

```sql
DROP VIEW [IF EXISTS] 视图名
```