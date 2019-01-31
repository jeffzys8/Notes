# MySQL 常用函数

## 日期时间

```sql
SELECT CURDATE(), CURTIME(), MONTH(某个Date型字段) FROM 表名
```

## 字符串

- 计算长度 `CHAR_LENGTH()`
- 大写, 小写 `UPPER() LOWER()`

> 当然还有不少操作，比如截取之类的，需要的时候自己查手册

## 数学

```
ABS(x)
SQRT(x)
MOD(x,y)
```

## 加密

不可逆:
```
PASSWORD(str)
MD5(str)
```
可逆, 结果是二进制数，必须用 `BLOB` 存储:
```
ENCODE(str,pswd_str)
DECODE(str,pswd_str)
```