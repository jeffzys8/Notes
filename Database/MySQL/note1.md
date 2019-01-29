# 1-17

- NoSQL: 非关系型数据库，比如Redis, MongoDB都是NoSQL


三种数据库的比较

- MySQL: 关系型数据库，索引和数据都在硬盘中。适合超大型的数据库，但效率较低
- Redis: Key-Value 等数据结构存储，完全在内存中保存数据的数据库，定期写入磁盘。适用放缓存数据
- MongoDB: 擅长查询海量JSON数据 (空间换时间) ，文档型。适用于存储网站数据 (mmap映射) 但是没有事务机制

mmap: 是一个系统调用。映射同一个普通文件实现内存共享。进程无需再调用 `read`, `write` 操作；mmap不分配空间，只是将文件映射到调用进程的地址空间中。（文件 ==> 内存）。可以用 `memcpy`, `memsync`写和同步文件。不过文件长度由`mmap`函数决定了，无法增长。

## MongoDB

MySQL由 **数据库、表、记录** 三个层次组成，而MongoDB由 **数据库、集合、文档对象** 三个，其中集合没有行、列、关系等概念，体现模式自由的特点。

MongoDB擅长查询 JSON 数据，对海量数据查询的效率较高，但是不支持事务。但问题是，MongoDB所占空间随时间会很显著增长。

## 数据类型

- CHAR: 固定长度字符串
- VARCHAR: 可变长度字符串
- DATETIME: 一般使用的日期类型
- ENUM: ?
- SET: ?
- BLOB: 可变长二进制(最多2^16-1字节)，图片

# 1-18

mysql-client 和 mysql-server 的区别相当于 西瓜刀和西瓜。当然除了mysql-client, 很多图形化界面也是“西瓜刀”。navicat; 以前用过mysql-workbench很不错呀

# 1-22

## 无法登录的问题

在Ubuntu(Elementary OS)安装完MySQL以后，没有设置过密码，无法登录并显示：
```
ERROR 1698 (28000): Access denied for user 'root'@'localhost'
```

[StackOverflow](https://stackoverflow.com/questions/39281594/error-1698-28000-access-denied-for-user-rootlocalhost)

Some systems like Ubuntu, mysql is using by default the `UNIX auth_socket` plugin.

- Option1: You can set the root user to use the `mysql_native_password` plugin
```
$ sudo mysql -u root # I had to use "sudo" since is new installation

mysql> USE mysql;
mysql> UPDATE user SET plugin='mysql_native_password' WHERE User='root';
mysql> FLUSH PRIVILEGES;
mysql> exit;

$ service mysql restart
```
- Option2: You can create a new db_user with you system_user (recommended)
```
$ sudo mysql -u root # I had to use "sudo" since is new installation

mysql> USE mysql;
mysql> CREATE USER 'YOUR_SYSTEM_USER'@'localhost' IDENTIFIED BY '';
mysql> GRANT ALL PRIVILEGES ON *.* TO 'YOUR_SYSTEM_USER'@'localhost';
mysql> UPDATE user SET plugin='auth_socket' WHERE User='YOUR_SYSTEM_USER';
mysql> FLUSH PRIVILEGES;
mysql> exit;

$ service mysql restart
```
> 关键点就在于知道Ubuntu的用户密码与mysql密码的一个交互机制；workbench没有办法以ubuntu的用户密码进行登录，所以一定要将某个用户设置为 mysql_native_password

## 基本操作

**创建表**

```sql
CREATE TABLE 表名(属性名 数据类型 [完整性约束条件],
                属性名 数据类型 [完整性约束条件],
                属性名 数据类型 [完整性约束条件],
                ...
);
```

**约束条件**

- PRIMARY KEY
- FOREIGN KEY
- NOT NULL
- UNIQUE 唯一(和主键的区别?)
- AUTO_INCREMENT 自动增加
- DEFAULT 设置默认值

> 留下疑问：AUTO_INCREMENT 的 PK 如何插入

**外键的例子**

这个例子是创建了一个外键约束，约束名为fk；它定义了本表内的bookTypeId属性指向表t_bookType的主键id (也就是说, bookTypeId唯一标识表t_bookType中的一条记录)

```sql
CONSTRAINT `fk` FOREIGN KEY (`bookTypeId`) REFERENCES `t_bookType`(`id`);
```

> 研究一下如何使用workbench创建外键

**查看表**

```sql
DESCRIBE 表名;
DESC 表名;

SHOW CREATE TABLE 表名;
```

**修改表**

- 修改表名 `ALTER TABLE 表名 RENAME 新名`
- 修改属性 `ALTER TABLE 表名 CHANGE 旧属性名 新属性名 新数据类型` (不能改约束吗) 
- 增加属性 `ALTER TABLE 表名 ADD 新属性名 数据类型 [完整性约束] [FIRST|AFTER 某旧属性]`
- 删除属性 `ALTER TABLE 表名 DROP 属性名`
