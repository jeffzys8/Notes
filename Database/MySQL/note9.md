# 存储过程和函数

存储过程顾名思义，其调用实质上是一段SQL语句，而函数调用只是返回了一个值，还需要配套SQL语句才能显示出具体的查询结果。

## 两个的基本使用

**创建存储过程**

```sql
delimiter &&
CREATE PROCEDURE select_type_count(IN a INT, OUT b INT)
BEGIN
	SELECT num FROM t_type WHERE t_type.id = a;
END;
&&
delimiter ;
```

注意一下，存储过程中 `IN` 和 `OUT` 是放到一起的 （虽然我也不太懂OUT在这里有什么意义）；另外，对于存储过程的类型也相应进行了忽略，就是简单实现了而已（需要的时候再去查怎么用）

**调用存储过程**

调用存储过程就相当于调用了其所代表的SQL语句

```sql
CALL select_type_count(3,@varb);
```

这个 `@varb`是一个全局变量，但还不清楚是做什么用的

**创建函数**

```sql
delimiter &&
CREATE FUNCTION func_get_type_count(a INT)
RETURNS INT
BEGIN
	RETURN(SELECT num FROM t_type WHERE t_type.id = a);
END;
&&
delimiter ;
```

不要漏了 `RETURN()`, 否则会提示说 不允许返回集合；看来函数只能返回单一数据类型

**调用函数**

和存储过程有区别：函数并没有执行一段SQL语句，所以要输出结果必须配合SQL语句

```sql
SELECT func_get_type_count(3);
```

## 使用变量

```sql
DECLARE m,n VARCHAR(20) DEFAULT 'sdf';
SET m = 'jeff',n = 'amber';
-- 还有利用表给变量赋值, 当然列在这里可以重复
SELECT col1, col2 INTO a,b WHERE id=1;
```

## 游标

在存储过程和函数中，查询语句可能查询出多条记录，所以可以用游标来逐条读取结果。包括 声明、打开、使用、关闭

```sql
DECLARE my_cursor CURSOR FOR
    SELECT 语句;

OPEN my_cursor;

FETCH my_cursor INTO 变量列表;

CLOSE my_cursor;
```

## 流程控制

在存储过程和函数中用流程控制来写（其实前面触发器的时候已经尝试过了）

**IF**

```sql
IF ...
    THEN ...
ELSE IF ...
    THEN ...
ELSE 
    ...
END IF
```

**CASE**

```sql
CASE ...
    WHEN ... THEN ...
    WHEN ... THEN ...
    ELSE ...
END CASE
```

**循环**

```sql
-- LOOP 无限循环
[begin_label: ]LOOP
    xxx;
    xxx;
END LOOP [end_label]

-- leave 相当于Goto
LEAVE label

-- ITERATE 相当于continue
ITERATE label

-- REPEAT UNTIL 相当于DO WHILE
[begin_label: ]REPEAT
    xxx;
    xxx;
    UNTIL xxx;
END REPEAT [end_label]

-- WHILE 就是相当于WHILE
[begin_label: ]WHILE xxx DO
    xxx;
    xxx;
END WHILE [end_label]
```

> 查看,更新,删除存储过程和函数和前面的类似,就不记下来了