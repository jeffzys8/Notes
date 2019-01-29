# 编码

视频中出现了编码问题，特意去了解了一下如何设置**编码**

- 创建数据库时,设置数据库的编码方式 
    ```sql
    -- CHARACTER SET:指定数据库采用的字符集,utf8不能写成utf-8
    -- COLLATE:指定数据库字符集的排序规则,utf8的默认排序规则为utf8_general_ci（通过show character set查看）
    -- 这里的dbname修改为您要创建的数据库名称即可
    drop database if EXISTS dbname;
    create database dbname CHARACTER SET utf8 COLLATE utf8_general_ci;
    ```

- 修改数据库编码
    ```sql
    alter database dbname CHARACTER SET GBK COLLATE gbk_chinese_ci;
    alter database dbname CHARACTER SET utf8 COLLATE utf8_general_ci;
    ```

# 查询

```sql
SELECT 属性名a, 属性名b(*) FROM 表名
```

**WHERE**

```sql
SELECT FROM 表名 WHERE 表达式 
```
- `IN (a,b,c,..)`, `NOT IN()` 在/不在列出的属性值内
- `BETWEEN a AND b`, `NOT BETWEEN` 在/不在两属性值之间
- `LIKE` 模糊查询:
  - `%` 代表任意字符
  - `_` 代表单一字符
- `IS NULL`, `IS NOT NULL` **判断空值不能直接=NULL** 
- `AND` 和 `OR` 多语句查询

**DISTINCT**

```sql
-- 去除重复查询
SELECT DISTINCT 属性名 FROM 表名
```

**ORDER BY**

```sql
-- 升序
SELECT * FROM 表名 ORDER BY 属性名 ASC
-- 降序
SELECT * FROM 表名 ORDER BY 属性名 DESC
```

**GROUP BY**

分组查询，比较重要的内容了

```sql
SELECT FROM 属性名 FROM 表名 GROUP BY 属性名
```

- `GROUP BY`单独使用，会使每个属性只保留一条记录，意义不大 (自己测试发现MySQL这样做不行)
- 使用`GROUP_CONCAT`作为新属性值，会将group by 同属性值的聚合到一起，逗号隔开，形成一个新的属性; 用语言很方便提取 
- 使用聚合函数 `SUM()`, `COUNT()` 作为新属性值；
> 有个问题: 比如`SELECT COUNT(description) FROM t_test GROUP BY sex;` 这一句，如果有相同`description`字段, COUNT函数会重复计算；如何避免？<br> 答案: 变成 `SELECT COUNT(DISTINCT description) ...` 即可

- `HAVING` , 对聚合函数进行条件判断 (聚合函数的`WHERE`)
- `WITH ROLLUP`, 结果结尾增加总和行

**分页查询**

```sql
SELECT ? FROM ? LIMIT 0,5;
```
表示从第一条记录开始，搜寻五条记录 

# 使用聚合函数

与 `GROUP BY 一起使用`:COUNT, SUM, AVG, MAX, MIN

自己对 `GROUP BY` 过程的一个理解：
1. 将表用`WHERE`去除掉不符合的记录
2. 执行`GROUP BY`按选定属性划分为堆
3. 用聚合函数对每堆操作，结果赋值给堆内的每个属性
4. 执行`SELECT`, 其所能识别的属性是堆内**每条记录的值都相同**的属性

因此，下面语句不能执行，因为`sex`不是堆的属性

```sql
SELECT sex,COUNT(description) FROM t_test GROUP BY description;
```

而下面的语句可以执行，因为在GROUP BY的时候`description`提到了堆层次; 另外如果给`COUNT`加个`DISTINCT`，显然该字段结果都是1

```sql
SELECT description,COUNT(description) AS d FROM t_test GROUP BY description;
```

而没有GROUP BY语句，但同时又使用了聚合函数，最后就会把整体当成一个堆进行聚合，最后返回的结果只有一行; 当然了，如果有`WHERE`字句，那么符合的行组成一个堆

```sql
SELECT COUNT(*) FROM t_test;
SELECT COUNT(DISTINCT sex) FROM t_test;
```

<hr>

> 留下疑问: 下面这一句无法知道获取的分数对应哪个课程；下面这个写法会报错 / 只显示每个堆的第一行而非正确结果。 正确结果该如何获得呢
```sql
SELECT stuName, course, MAX(score) FROM t_grade GROUP BY stuName
```

可行的做法:
```sql
SELECT * 
FROM t_grade g 
WHERE 
    g.grade = ( SELECT MAX(grade) 
                FROM t_grade g2 
                WHERE g.name = g2.name);
```

但有没有更优的, 使用`GROUP BY`的做法呢