# 管道和管道命令

> 这个笔记是Bash的延伸，因为管道实际也是Bash的功能，只不过涉及一些比较重要的命令，内容也不少，就单独开笔记了。

管道：将前一个STDOUT发送给下一个STDIN，因此管道命令就是能接受STDIN的命令（会忽略STDOUT，不过可以通过`2&>1`来重定向）。

```
 stdout → → stdin   stdout → → stdin
   ↑            ↓     ↑         ↓
   ↑            ↓     ↑         ↓
COMMAND1    |   COMMAND2    |   COMMAND3
```
	
## `cut`

切割每行的输入（对格式化的文档来说没有`awk`好用）

- `-d` 和 `-f` 配合使用，使用给定分隔符`-d`分割文本并取出序号`-f`的字段（从1开始）（可以列出多个`,`分隔）
- `-c` 输出给定范围文本 `start-end`，可以是单字符，也可以省略`start`或`end`


## `grep`

筛选所需要的行（正则表达式，后面详细学正则还会补充）

- `-a` ：将binary文件以text文件的方式查找
- `-c` ：计算找到'搜寻字符串'的次数
- `-i` ：忽略大小写
- `-n` ：顺便输出行号
- `-v` ：反向选择
- `--color=auto` ：可以将找到的关键字部分加上颜色的显示喔！

## `sort`

排序

sort [-fbMnrtuk] [file or stdin] 
选项与参数：
- `-f` ：忽略大小写；
- `-b` ：忽略最前面的空白；
- `-M` ：以月份的名字来排序，例如JAN, DEC等；
- **`-n` ：以纯数字形式排序，默认是字典序**
- `-r` ：反向排序；
- `-u` ：类似`uniq`指令，相同资料仅出现一行；
- `-t` ：分隔符号，预设是用[tab]键来分隔；
- `-k` ：排序区间，查看`man sort`有更多选项，但最常用就是单个序号表示的列

## `uniq`

相同资料只显示一行
- `-c` 进行计数
- `-i` 忽略大小写

## `wc`

字数、行数统计

- `-l` ：仅列出行；
- `-w` ：仅列出多少字(英文单字)；
- `-m` ：多少字符；

## `tee`

重定向到多个目标 tee

- 屏幕+文件都输出
  ```sh
  ls | tee filename
  ```
- `-a` 追加(>>)

## `tr`

替换或删除字符，结果输出到标STDOUT

- `-d` 删除字符
- `-s` 重复去除

示例：
- `tr 'abc' 'xyz'` 按字符替换
  - `tr 'abcde' 'xyz'` de按z替换
  - `tr 'abc' 'xyzmn'` 后面mn没用了
- 大小写替换： `last | tr '[az]' '[AZ]'`
- **不能这样写，会变空** `tr 'A-Z' 'a-z' > /app/test < /app/test`
- 将windows的\r\n 转为 \n：`tr -d '\r' < fn > fn2`

## `split` 

按行数拆分大文件 `split [-bl] file PREFIX`

选项与参数：
- `-b` ：分割成的文件大小，可加单位，例如b, k, m 等；
- `-l` ：以行数来进行分割。
- `PREFIX` ：代表分隔文件的前缀名

例子，其中`-`表示STDIN:
```sh
ls -al / | split -l 10 - lsroot 
```

## `xargs`

参数代换：产生某个指令的参数

- xargs可以读入stdin的资料，并且以空白字元或断行字元作为分辨，将stdin的资料分隔成为arguments 
- 没有接指令时预设 `echo` 输出
- `-n` 设置每次传几个参数给指令
- `-p` 每次执行命令前询问
- `-e` 终止字符串，检测到它就停止

示例，显示每个用户(`/etc/passwd`)的`id`内容
- 这样行不通，因为`id`只能接一个参数：
  ```sh
  id $(cut -d ':' -f 1 /etc/passwd | head -n 3) 
  ```
- 用管道也不行，因为`id`不是管道命令：
  ```sh
  cut -d ':' -f 1 /etc/passwd | head -n 3 | id
  ```
- 错误的xargs使用，这样会一股脑把所得参数全给命令：
  ```sh
  cut -d ':' -f 1 /etc/passwd | head -n 3 | xargs id
  ```
- **正确做法**
  ```sh
  cut -d ':' -f 1 /etc/passwd | head -n 3 | xargs -n 1 id
  ```

会使用xargs的原因是， **很多指令其实并不支援管线命令**，因此我们可以透过xargs来提供该指令引用standard input之用

## 不常用的命令

- `join` 合并有相同元素的行（略）
- `paste` 直接合并，`-` 表示 STDIN（略）
- `expand` 将[空格]换成[Tab]

## 关于 `-`

在管道中，可能STDOUT --> STDIN 这个过程隐藏在两个命令中的文件名中，这时可以用`-`来代替，例子：
```sh
tar -cvf - /home | tar -xvf - -C /tmp/homeback
```
例如上面的第一个 `-` 表示 STDOUT, 下一个表示 STDIN，这样就**省略了中间冗余文件名**

## 有意思的题目

有意思的题目：1+2+3+…+100
```sh
echo {1..100} | tr ' ' '+' | bc
```

有意思的题目2：文件中只保留英文词，每个一行
```sh
cat filename | tr -sc 'a-zA-Z' '\n'
```