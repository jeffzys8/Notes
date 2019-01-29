# 触发器

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