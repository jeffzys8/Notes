# Shell脚本语法

搭配正则表达式、管道命令与文件重定向等功能

`\` 换行
`#` 注释
遇到 `[Enter]` 即执行指令

第一行的 `#!/bin/bash` 表示执行用`bash` 来执行，挺重要

`read`交互式输入
```bash
read -p "Please input your first name: " firstname       #提示使用者输入 
read -p "Please input your last name: " lastname        #提示使用者输入 
echo -e "\nYour full name is: ${firstname} ${lastname}" #结果由萤幕输出
```
`${var}` `$(command)`

`var=${str:-expr}` 是用`expr`填充空或未设定的`str`，属于Bash特性的内容

数值运算 
`var=$((运算内容))` 仅支持整数
`declare -i total=${firstnu}*${secnu}` 

使用`bc`计算π, `4*a(1)`是bc提供的计算pi的工具
```sh
read -p "The scale number (10~10000) ? " checking
num=${checking:-"10"}            #开始判断有否有输入数值
echo -e "Starting calculate pi value. Be patient."
time echo "scale=${num}; 4*a(1)" | bc -lq
```

执行方式差异
`bash` `sh` `/../.sh` `./.sh` 直接执行，子进程bash执行
`source` 在原进程中执行

条件判断
`$?` `||` `&&` `ls`配合来完成条件判断，重定向部分讲
`test`
- 文件类型
  - `-e` 存在
  - `-f` 是否存在并为file (普通和二进制都算)
  - `-d` 是否存在并为directory
- 文件权限
- 文件比较
- 整数比较
  - `-eq` 两数值相等(equal)
  - `-ne` 两数值不等(not equal)
  - `-gt` n1大于n2 (greater than)
  - `-lt` n1小于n2 (less than)
  - `-ge` n1大于等于n2 (greater than or equal)
  - `-le` n1小于等于n2 (less than or equal)
- 字符串判断
  - `-z` 空字符串
  - `-n` 非空字符串
  - `stra == strb`, `stra != strb`
- 逻辑词
  - `-a`, `-o`, `!`

```sh
test -e /dmtsai && echo "exist" || echo "Not exist" 
```

`[ ]` 也可做判断符，需要空格
```sh
[□"$HOME"□==□"$MAIL"□]
```
- 在中括号内的变数，最好都以双引号括号起来；
- 在中括号内的常数，最好都以单或双引号括号起来。
  
否则导致的错误示范：
```sh
[dmtsai@study ~]$ name="VBird Tsai" 
[dmtsai@study ~]$ [ ${name} == "VBird" ]
bash: [: too many arguments
```

我发现shell也是可以用括号来表达优先级的，配套`-o`,`-a`,`!`使用

$相关:
- `$0` `$1` `$2`...传参，其中`$0` 是脚本文件名
- `$#` 表示参数个数
- `$@` ：代表`"$1" "$2" "$3" "$4"`，每个参数独立的(用双引号括起来)
- `$*` ：代表`"$1 c $2 c $3 c $4"`，其中`c`为分隔字元，预设为空白键，所以本例中代表`"$1 $2 $3 $4"`之意。
- 上述两个一般用`$@`就好了，这是为了**防止传入的某个参数内本身就有空格**
- `shift`可以偏移参数，不知道有啥用

if
```sh
if [ 条件判断式一 ]; then
  ...
elif [ 条件判断式二 ]; then
  ...
else
  ...
fi
```

利用`netstat -tuln`来取得目前计算机有启动的服务


case
```sh
case ${var} in
  "xxx" )
    ...
    ;;
  "xxx" )
    ...
    ;;
  * )
    ...
    ;;
esac
```

function
- 放在最前面
- 可以传参，也是用`$0`,`$1`...来拿 
```sh
#!/bin/bash
function printit(){
	echo "Your choice is ${1}" # 取第一个参数
}
echo "This program will print your selection !"
case ${1} in
  "one")
	printit 1  # 函数调用传参
	;;
  "two")
	printit 2
	;;
  *)
	echo "Usage ${0} {one|two}"
	;;
esac
```

循环

```sh
while [ condition ] 
do
	...
done
```
> 感觉多此一举的`until`
```sh 
until [ condition ]
do
	...
done
```

```sh
for var in con1 con2 con3 ...
do
	...
done
```
- 用 `{startn, endn}` 来生成一个数列
- **发现个问题：`$@` 和 `$*` 在这没区别了**，那传入参数有空格怎么办
- `$(seq startn step endn)` 更好

```sh
for ((set;condition;step))
do
	...
done
```

数组，随机数在bash章节学，32767

debug，`sh`的参数：
- `-n` 查语法，不运行
- `-v` 执行前输出脚本内容
- `-x` 列出执行过程

shell script 用在系统管理上面是很好的一项工具，但是用在处理大量数值运算上， 就不够好了，因为Shell scripts 的速度较慢，且使用的CPU 资源较多，造成主机资源的分配不良。