# 触发器

> 触发器这一章学的乱了些，主要就是被一些细节（比如分隔符）和一些没学的概念（比如`IF`）搞得有点虚，之后回来重看一下；但基本上应用是没有问题的。但个人认为触发器在使用上可能会导致和应用代码的逻辑产生冲突：数据库方已经执行了关联操作，而应用方却以为没有执行，于是写多了几行代码来做这件事，结果就导致操作重复--逻辑错误了。这一部分的内容要好好思考一下怎么解决。

由某个事件（插入、删除...)来触发某个操作

**创建只有一个执行语句的触发器**

```sql
CREATE TRIGGER 触发器名 BEFORE|AFTER 触发事件
    ON 表名 FOR EACH ROW 执行语句
```

**创建由多个执行语句的触发器**

```sql
CREATE TRIGGER 触发器名 BEFORE|AFTER 触发事件
    ON 表名 FOR EACH ROW
    BEGIN
        执行语句列表
    END
```

> 过渡变量的概念，似乎会牵扯出 MySQL的各种变量的概念（这似乎得好好学！）

**过渡变量 new/old 使用**

在执行了某个触发操作的时候，需要将该语句操作的行暂时保存起来作为过渡变量，`INSERT`对应 `new`, `DELETE`对应`old`
```sql
CREATE TRIGGER tri_test AFTER INSERT
    ON t_book FOR EACH ROW
        UPDATE t_booktype SET bookNum=bookNum+1 WHERE new.bookType=t_booktype.id
```

通过阅读书籍，还查到了如何为过渡变量重命名，遗憾的是好像MySQL并不支持这种用法，那就只好按照原来的`new`和`old`，但还是把代码贴出来。

```sql
CREATE TRIGGER 触发器名 AFTER UPDATE OF 字段名 ON 表名
    REFERENCING NEW ROW AS nrow
    REFERENCING OLD ROW AS orow
    FOR EACH ROW
    WHEN 判断语句
    BEGIN [ATOMIC]
        执行语句;
        执行语句;
    END;
```

下面是一段自己测试过可用的触发器，问题是在`MySQL-workbench`上还是遇到了分隔符`DELIMITER`的问题，在[StackOverflow](https://stackoverflow.com/questions/39307880/mysql-trigger-syntax-missing-semicolon)解释了解决办法，其实就是不要在query里直接新建触发器

```sql
CREATE TRIGGER type_count AFTER INSERT ON t_person
FOR EACH ROW
    BEGIN
	    IF new.type <> 4
	    THEN
			UPDATE t_type SET num=num+1 WHERE t_type.id = 3;
		END IF;
    END;

```

还有一种办法

```sql
DELIMITER &&
---
要执行的东西
---
&&
DELIMITER ;
```