# 索引

由数据表的一列或多列组成，提高查询速度

## 索引的分类

- 普通索引
  - 可以创建在任意的数据类型内
- 唯一性索引：
  - `UNIQUE`
  - 没懂，是说该数据是UNIQUE才可以设置唯一性索引吗
- 全文索引：
  - `FULLTEXT`
  - CHAR, VARCHAR, TEXT; 提高查询较大字符串的速度
  - MySQL默认引擎不支持(默认是啥?InnoDB?弹幕说现在已经支持了?); MyISAM引擎支持
- 单列索引
  - 表的单个字段创建索引 (上面三种都行)
- 多列索引
  - 表的多个字段创建索引
- 空间索引
  - `SPATIAL`
  - 空间数据类型??这是什么
  - 也是需要MyISAM引擎

[关于普通索引和唯一索引的区别, 以及其他索引的简介](https://blog.csdn.net/u014071328/article/details/78780683)

- 普通索引和唯一索引都是用B树，执行速度都是 `O(logN)`
- 普通索引允许被索引的数据列包含重复的值，比如`Name`字段可以有重复的人名
- 但如果字段能保证`UNIQUE`, 则索引也应设为`UNIQUE`，其好处是：
  - 简化了MySQL对这个索引的管理
  - 更快地实现新插入数据的合法性检索 (存在索引即存在值，则拒绝插入)

## 创建索引

可以在创建表的时候就创建索引

```sql
CREATE TABLE t_indexTest1(
	id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(25),
    INDEX(username)
);
```

实践观察到，创建表的时候，主键`id`也被新建了一个索引；另外，普通索引的实现方式是`BTREE`(为什么不是B+树呢)

> 我看到主键的索引实现方式是 `BTREE(CLUSTERED)`，这是什么意思呢

**唯一性索引**

```sql
CREATE TABLE t_indexTest1(
	id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(25),
    UNIQUE INDEX(username)
);
```

前面的拓展文章也说了，唯一性索引的实现方式也是`BTREE`，实践后也的确如此

**为索引取别名**

直接在括号前加就是了

```sql
UNIQUE INDEX index_name(username)
```

**多列索引**

多列索引也是可以普通、唯一的
```sql
CREATE TABLE t_indexTest2(
	id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(25),
    course VARCHAR(25),
    UNIQUE INDEX index_un_co(username, course)
);
```

> 全文索引、空间索引暂时略过了；学到存储引擎再来细究

**给已存在的表增加索引**

```sql
CREATE [UNIQUE|...] INDEX 索引名 ON 表名(字段s);
```

## 更改索引名 + 更改索引字段

```sql
ALTER TABLE 表名 
DROP INDEX 原索引名 ,
ADD [UNIQUE|...] INDEX 新索引名 (字段s);
```

## 删除索引

```sql
ALTER TABLE 表名 
DROP INDEX 索引名;
-- 或者
DROP INDEX 索引名 ON 表名
```