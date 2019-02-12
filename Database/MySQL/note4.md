# 插入数据

首先是必须按表结构顺序的插入法
```sql
INSERT INTO 表名 VALUES(值1, 值2, ...);
```

然后是任意插入
```sql
INSERT INTO 表名(属性名1, 属性名2) VALUES(值1, 值2);
```

还可以同时插入多条记录
```sql
INSERT INTO 表名(属性名1, 属性名2) VALUES
    (值1, 值2), 
    (值1, 值2),
    ...
    (值1, 值2);
```

# 更新数据

```sql
UPDATE 表名 
SET 属性名1 = 值1,
    属性名2 = 值2
    ...
WHERE 条件;
```

# 删除数据

```sql
DELETE FROM 表名 WHERE 条件
```