# 文件权限

User, Group, Others (还有root)

/etc/passwd 存储账号信息, /etc/shadow 存储个人密码, /etc/group 存储群组信息

## 使用指令```ls -al```:

![](filer_ls.png)


1. 档案类型权限：

    - 第一个符号：-表示文件, d表示目录 (l, b, c 暂不记)
    - r w x 分别代表 read, write, execute
    - 2~4 Owner, 5~7 Group, 8~10 Others

2. 连结数：i-node，作用尚不明确（不同的文件名连接到相同的i-node数?）

3. 拥有者
4. 所属群组
5. 档案容量 (byte为单位)
6. 最后修改日期

    - 太久了需要用 ```ls -l --full-time```
    - 乱码需转码 ```export LC_ALL=en_US.utf8``` (ubuntu似乎不太一样)

7. 文件名
    - . 开头为隐藏文件