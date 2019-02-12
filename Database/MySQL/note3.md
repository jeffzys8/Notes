# 连接查询

## 内连接

笛卡尔积，通过`WHERE`的限制实现自然连接。

```sql
SELECT t_person.Name, t_type.Name FROM t_person, t_type WHERE t_person.type = t_type.id; 
```

当然上面这样子写有两个问题：
1. SQL语句过长（由于表名较长）
2. 输出的两列都是`Name`

可以通过别名的方式进行修改，注意这里的`AS`都被省略了（可加可不加）

```sql
SELECT p.Name person, t.Name type FROM t_person p, t_type t  WHERE p.type = t.id; 
```

## 外连接

内连接用`WHERE`可以形成自然连接，但是无法解决这样的场景：我希望某个表的所有内容都列出来，然后再去匹配另一个表，在另一个表不存在的话相应条目就设为`NULL`; 而自然连接只能显示双方匹配的项目，不满足我们的要求 —— 这是我们可以用**外连接**解决

**左连接** 是先把左边表的项全部列出来，依次去匹配。当然，如果左表某项匹配右边的多个项，结果会有多个项。如果无匹配，结果中右表的字段为`NULL`:

```sql
SELECT p.Name person, t.Name ptype FROM t_person p LEFT JOIN t_type t ON p.type = t.id;
```

**右连接** 则是反过来，先列右表，然后依次匹配。


# 子查询

将查询结果作为一个集合进行判断操作; 以下这些都是在`WHERE`字句中用的（暂时这么理解吧）

## IN, NOT IN

```sql
SELECT * FROM t_person p WHERE p.type IN(
	SELECT id FROM t_type
);
```
可以回忆起之前`IN`是用过的, 当时是`IN(1,2,3)`相当于上面`SELECT id FROM t_type`的搜索结果是一个`(1,2,3)`的列

## 带比较的子查询

```sql
SELECT * FROM t_person p WHERE p.type > (
	SELECT id FROM t_type t WHERE t.name = 'boy'
);
```

## EXISTS, NOT EXISTS

条件语句，如果真则执行外层查询语句，否则不执行。当然也是属于`WHERE`字句的一部分

```sql
SELECT * FROM t_person p WHERE EXISTS(
	SELECT * FROM t_type t WHERE t.name='boy'
);
```

这条SQL语句会把`t_person`都输出出来

## ANY

条件语句，满足任意一个即可。常与比较语句嵌套。如果用=的话就和`IN`没有区别了

```sql
SELECT * FROM t_person p WHERE p.type > ANY (
	SELECT id FROM t_type t WHERE t.name='boy' OR t.name='girl'
);
```

这条语句不会输出 boy 类型的人，但还是会输出 girl 类型的人

## ALL

条件语句，满足所有

```sql
SELECT * FROM t_person p WHERE p.type > ALL (
	SELECT id FROM t_type t WHERE t.name='boy' OR t.name='girl'
);
```

这条语句不会输出 boy 和 girl 类型的人

# 合并查询

合并查询结果, 字段需匹配，会删除重复字段

```sql
SELECT type id, Name name FROM t_person 
UNION 
SELECT id, name FROM t_type 
ORDER BY id;
```

值得注意的是，如果需要排序，直接这样写就OK了，因为`ORDER BY`的优先级低于`UNION`

如果要不删除重复字段，使用`UNION ALL`

# 取别名

前面已经用了很多了，这里具体讲一下

## 给表取别名

```sql
表名 别名
```

## 给属性取别名

```sql
属性名 [AS] 别名
```