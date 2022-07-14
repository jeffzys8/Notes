- ### 引言
    - 高级语言的优势
        - 类型检查: 规避直接写汇编操作数据时的不一致行为
        - 高级语言写的程序能在多个平台被编译，而汇编代码就非常 machine specific
    - 本章学习两个汇编语言，并学习C语言是怎么编译成对应汇编
        - IA32 (同时也叫 i386)
        - x86-64
    - **深入细节和例子，不要浮于表面的概念**
        - 编译器集成了很多向后兼容的功能，要结合历史去看
        - 编译器会做很多很多优化，不关掉甚至看不出原来的程序是啥样
    - 本章内容
        - C、汇编、机器语言的关系
        - 深入学习 IA32
            - 数据表示
            - 控制流的实现(if, while, switch等)
            - 运行时(procedures, stack)
            - 复合数据类型(array, struct, union)
        - 内存地址错误引用、系统漏洞、GDB
    - (web aside) 汇编和C结合
        - 在linking stage将汇编和C函数结合
        - gcc支持C函数中嵌入汇编
- ### 历史
    - x86: Intel processor line
        - 8086, 8088 IBM 个人电脑 (16-bit)
        - 8087 floating-point coprocessor (x87)
        - i386 32bit, first to support Unix OS, 1985
        - PentiumPro, P6 micoarchitecture, "conditional move" instructions
        - Pentium III, SSE(操作整数向量、浮点数的指令集)
        - Pentinum 4E, 超线程(一核跑两程序)，支持64位(在amd_64之后)
        - core2: 多核
        - corei7: 超线程+多核, 2008
    - 最初的8086和80286的内存模型已经过时，Linux使用平面寻址(进程空间是一块连续的内存数组)
    - GCC默认为i386编译(很老了), 只有给定编译参数或编译64位的操作，才能使GCC使用更多新拓展
        - > 但自己尝试了一下并非如此
- ### 汇编代码初窥
    - C编译过程
        - preprocessor 处理 `#include`, `#define`
        - compiler --> .s汇编程序
        - assember --> .o 二进制(不可重定向?), global values地址未填入
        - linker --> 最终可执行文件
    - 两个重要抽象
        - 指令集架构(ISA)
            - 对处理器的抽象
            - IA32, X86-64都是ISA，都把指令抽象为顺序执行
        - 虚拟内存
            - 对物理内存的抽象
            - 二进制程序使用的内存是虚拟内存(?)
    - 相比较C语言，IA32机器码显示更多C看不到的 **处理器状态**
        - 程序计数器 (PC, `%eip`寄存器)，指示下一个执行指令的内存地址
        - 整数寄存器
            - IA32-CPU有8个32位寄存器，存储整数数据、指针(内存地址)、程序状态、栈变量、函数返回值；
        - 状态码寄存器
            - 最近执行的算数或逻辑运算信息
        - 浮点寄存器
    - 代码示例
        - 简单的`sum`函数
            - ```c
int accum = 0;
int sum(int x, int y)
{
  int t = x + y;
  accum += t;
  return t;
}```
```shell
> gcc -O1 -S code.c```
            - > 汇编代码和书上的示例不一样，即使加上-m32参数也是，如下，后面再研究
                - ```plain text
	.file	"code.c"
	.text
	.globl	sum
	.type	sum, @function
sum:
.LFB0:
	.cfi_startproc
	endbr32
	call	__x86.get_pc_thunk.dx
	addl	$_GLOBAL_OFFSET_TABLE_, %edx
	movl	8(%esp), %eax
	addl	4(%esp), %eax
	addl	%eax, accum@GOTOFF(%edx)
	ret
	.cfi_endproc
.LFE0:
	.size	sum, .-sum
	.globl	accum
	.bss
	.align 4
	.type	accum, @object
	.size	accum, 4
accum:
	.zero	4
	.section	.text.__x86.get_pc_thunk.dx,"axG",@progbits,__x86.get_pc_thunk.dx,comdat
	.globl	__x86.get_pc_thunk.dx
	.hidden	__x86.get_pc_thunk.dx
	.type	__x86.get_pc_thunk.dx, @function
__x86.get_pc_thunk.dx:
.LFB1:
	.cfi_startproc
	movl	(%esp), %edx
	ret
	.cfi_endproc
.LFE1:
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 4
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 4
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 4
4:```
        - disassemblers，由机器码反编译为汇编码
            - ```shell
> objdump -d code.o

code.o：     文件格式 elf64-x86-64


Disassembly of section .text:

0000000000000000 <sum>:
   0:	f3 0f 1e fa          	endbr64 
   4:	8d 04 37             	lea    (%rdi,%rsi,1),%eax
   7:	01 05 00 00 00 00    	add    %eax,0x0(%rip)        # d <sum+0xd>
   d:	c3                   	retq ```
            - 看看32位的(加 `-m32` 参数进行 `gcc` 编译)
```shell
> objdump -d code.o

code.o：     文件格式 elf32-i386


Disassembly of section .text:

00000000 <sum>:
   0:	f3 0f 1e fb          	endbr32 
   4:	e8 fc ff ff ff       	call   5 <sum+0x5>
   9:	81 c2 02 00 00 00    	add    $0x2,%edx
   f:	8b 44 24 08          	mov    0x8(%esp),%eax
  13:	03 44 24 04          	add    0x4(%esp),%eax
  17:	01 82 00 00 00 00    	add    %eax,0x0(%edx)
  1d:	c3                   	ret    

Disassembly of section .text.__x86.get_pc_thunk.dx:

00000000 <__x86.get_pc_thunk.dx>:
   0:	8b 14 24             	mov    (%esp),%edx
   3:	c3                   	ret    ```
            - 可观察到IA32 是变长指令集 $$Len(instruction)\in [1,15] bytes$$
            - 注意到 `objdump -d` 和 `gcc -S` 生成的指令有不同 (比如 `addl` 变成了 `add`)，这是命名规范的差别 (比如这里的`l`是大小标记，大部分时候可以被忽略)
            - 另外 `.` 开头的是给assembler的注解
        - 再新建一个`main.c`来调用`sum`函数
            - ```c
int main()
{
  retur sum(1, 3);
}```
```shell
> gcc -O1 -o prog code.o main.c
```
            - 这里如果`code.o`是使用`-m32`编译的，那这时候即使使用 `-m32`也还是有不明的错误发生；所以暂时都用默认(64位)来走示例
            - 此时再使用`objdump`来反汇编，文件会大得多，我们只关注原`sum`函数部分
```shell
0000000000001129 <sum>:
    1129:       f3 0f 1e fa             endbr64 
    112d:       8d 04 37                lea    (%rdi,%rsi,1),%eax
    1130:       01 05 de 2e 00 00       add    %eax,0x2ede(%rip)        # 4014 <accum>
    1136:       c3                      retq   ```
                - 指令地址改变 (shifted by linker)
                - 确定了全局变量 `accum` 的存储位置 (`0x2ede`)
                - 可以注意到是小端模式，所以字节顺序 `de 2e` 和地址表示 `0x2ede` 相反 #[[字节序 Byte Ordering]]
        - {{[[TODO]]}} 这一部分看的迷迷糊糊地，关于编译还有很多要补
    - ATT v.s. Intel 汇编格式
        - **在本书中用的是ATT**
            - named after AT&T
            - 是`GCC`, `OBJDUMP`, 以及其他常用工具的默认格式
        - `gcc` 也可以生成 Intel format:
            - ```shell
gcc -O1 -S -masm=intel code.c```
        - Intel 和 ATT 的一些区别
            - Intel code 忽略了size，比如只有`mov`而没有`movl`
            - Intel code 在寄存器前忽略 `%`
            - Intel code 在表示memory地址时有区别，比如 `DWORD PTR [ebp+8]`，这在ATT是 `8(%ebp)`
            - 两者一些指令的操作数是相反的 （比如 3.6 中的 `CMP` 指令）
- ### 数据表示
    - 由于历史16-bit架构原因，Intel使用"word"来表示16-bit数据类型，而对应32和64是"double words", "quad words".
    - C内置类型在 IA32 的表示
        - {{table}}
            - **C declaration**
                - **Intel data type**
                    - **Assembly code suffix**
                        - **Size (bytes)**
            - `char`
                - Byte
                    - `b` 
                        - 1
            - `short`
                - Word
                    - `w` 
                        - 2
            - `int`
                - Double Word
                    - `l` 
                        - 4
            - `long int`
                - Double Word
                    - `l` 
                        - 4
            - `long long int`
                - -
                    - -
                        - 8
            - any pointer
                - Double Word
                    - `l` 
                        - 4
            - 
            - `float`
                - Single Precision
                    - `s`
                        - 4
            - `double`
                - Double Precision
                    - `l`
                        - 8
            - `long double`
                - Extended Precision
                    - `t`
                        - 10/12
        - 大部分都是"double words"，不管有符号还是无符号
        - 所有指针都是 doubler words
        - **C语言中的 long long int 并不被IA32硬件支持，所以编译器要转成一系列对 32-bit (double words)的操作**
        - 浮点数用的是另外一套表示体系，具体看表，gcc 使用 `long double`表示拓展精度
        - gcc编译的汇编代码有一个后缀，表示操作数的大小
            - 比如数据移动指令 就有几个变形 `movb`, `movw`, `movl`
            - 尽管 Double Word 的汇编指令后缀 和 Single Precision 相同 (都是l)，但这没有关系，因为**浮点数用的是完全不同的汇编指令和寄存器**
        - {{[[TODO]]}} 不知道64位会是怎么样的，之后可以留意一下
- ### 数据访问 
    - 整数寄存器
        - IA32-CPU有8个32位寄存器，存储整数数据、指针(内存地址)、程序状态、栈变量、函数返回值；
        - 这些寄存器都以 `%e` 开头
        - 在8086时，每个寄存器有独特的用途，不过flat addressing(线性地址空间)使得寄存器的专用性消失了
        - 32位寄存器分类
            - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FdU-KLOz4k8.png?alt=media&token=db7d4500-944f-4065-a1a7-395b1813888e)
            - 通用寄存器(前6个)
                - 前3(`%eax`,`%ecx`,`%edx`)和后3(`%ebx`,`%esi`,`%edi`)在存取数据又有一定差异
                    - {{[[TODO]]}} 后面会详细讲
                - 前4(`%eax`,`%ecx`,`%edx`,`%ebx`)为了向前兼容, 分为 1byte-1byte-1word(刚好32bit)三个区域，可以独立存取;
                    - 比如`%eax`分为`%al`,`%ah`,`%ax` 三个部分，剩余三个寄存器命名类似
                    - 刚好可以用来处理`char`和`short`类型数据
            - 特殊用途寄存器(后2个)
                - `%esp`: stack pointer
                - `%ebp`: frame pointer
        - 64位有些是以`%r`开头
            - 例如对应32位后4个: `%rdi`,`%rsi`, `%rsp`,`%rbp`
    - 操作数
        - 三种类型的操作数：**立即数、寄存器值、内存引用**
        - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FolU725Rh9z.png?alt=media&token=46409e6d-92aa-4427-a336-210bab72e34a)
        - 其他两种都比较直观，内存引用需要解释一下
            - 内存引用通过计算所得的**有效地址**访问内存数据
                - {{[[TODO]]}} 有效地址是什么 
            - 通用形式 $$Imm(E_b,E_i,s)$$ 对应的有效地址是 $$Imm+R[E_b]+R[E_i]\cdot s$$
                - $$Imm$$ 是一个代表偏移量的立即数
                - $$E_b$$ 是基地址寄存器 Base register
                - $$E_i$$ 是索引寄存器 Index register
                - $$s$$ 是比例印子 scale factor，**且只能是1,2,4或8**
                - 引用数组元素时很常见这种形式; 而其他内存表示形式也更像是这种形式的特例
    - 数据移动(拷贝)
        - 其实是拷贝操作，基本上是最常用的指令。共5个类别，每个类别内的所有指令执行相同操作，区别在于操作数大小不同
        - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FUJMOm5-pIP.png?alt=media&token=19c3b960-f045-4fd9-9c46-8b4ae3fdfaa7)
            -  `S` 和 `D` 都代表操作数，其中`S`(source)只可以是立即数、寄存器值或内存值，而`D`(destination)只能是某个寄存器或内存地址。并且**两个操作数不能都指向内存**
                - {{[[DONE]]}} 为什么不支持两个操作数都指向内存 
            - `mov S,D`: $$S\rightarrow D$$
                - `movb`: 移动`byte`(8bit); 寄存器(`%ah`~`%bh`, `%al`~`%bl`)
                - `movw`: 移动`word`(16bit); 寄存器(`%ax`~`%bp`)
                - `movl`: 移动`double word`(32bit); 寄存器(`%eax`~`ebp`)
            - `movs S,D`: $$SignExtend(S)\rightarrow D$$
                - 补符号位移动
                - `movsbw`, 移动`byte`到`word`
                - `movsbl`, 移动`byte`到`double word`
                - `movswl`, 移动`word`到`double word`
            - `movz S,D`: $$ZeroExtend(S)\rightarrow D$$
                - `movzbw`, `movzbl`, `movzwl`, 操作数大小意义同 `mov`
                - 区别在于 `movs`**是高位补符号位，**`movz`**是高位补0**
            - 栈操作(两类)
                - 和函数调用密切相关
                - 栈空间从内存高位往低位拓展
                - `pushl S`: Push double word
                    - 运算步骤
                        - 1. $$(R[\%esp]-4)\rightarrow R[\%esp]$$
                        - 2. $$S\rightarrow M[R[\%esp]]$$
                    - `pushl %ebp`等价于:
                        - 1. `subl $4,%esp`
                        - 2. `movl %ebp,(%esp)`
                - `popl D`: Pop double word; 
                    - 运算步骤
                        - 1. $$M[R[\%esp]]\rightarrow D$$
                        - 2. $$R[\%esp]+4\rightarrow R[\%esp]$$
                    - `popl %eax` 等价于
                        - 1. `movl (%esp),%eax`
                        - 2. `addl $4,%esp`
            - {{[[TODO]]}} 如果立即数长度超过指令规定，会截掉高位吗，还是报警
                - 应该是会截掉高位，见Problem 3.4，TBC
        - 从机器码用`objdump`转汇编没有操作数大小后缀, 而`gcc -S` 编译为汇编有操作数大小后缀
            - 需从指令的操作数上看具体的执行操作数大小
                - IA32内存引用地址都是用的32位寄存器，而且只标明地址，没标明数据大小; 所以其中一个操作数为内存引用的指令，要看另一个操作数的大小 (可见Problem3.2) 这也解释了为什么[不能两个操作数都是内存引用](((kSvEBpRwQ))) 
            - 操作数大小和指令不匹配会报错(见Problem3.3)
        - 数据移动示例
            - 暂时忽略了运行时栈空间的分配和释放，这一部分在后面 procedure linkage 
            - {{[[DONE]]}} frame pointer `%ebp` 是什么，好像3.7会讲
            - 代码
                - ```c
int exchange(int *xp, int y)
{
	int x = *xp;
  	*xp = y;
  	return x;
}```
                - 转换为Assembly
```shell
# xp at %ebp+8, y at %ebp+12
movl	8(%ebp),	%edx	# get xp(pointer value) to a register
movl	(%edx),		%eax	# %eax serves as return value, returning x = *xp
movl	12(%ebp),	%ecx	# get y(int value) to a register
movl 	%ecx,		(%edx)	# store y at *xp```
            - local variables 通常是存在寄存器里，而不是存在内存中 (不过这应该只能说针对C语言)
- ### 算术逻辑操作
    - 基本运算符
        - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2F9dez5c9Q6V.png?alt=media&token=770280b4-3ee9-4609-8061-5ab39da7405c)
        - 表中除了`leal`以外，其他都是表示**指令集合**，包含不同大小的操作数（例如`ADD`包括了`addb`, `addw`, `addl`)
        - 表中指令集合被分为四组：取地址的`leal`、单目运算、双目运算、移位
        - `leal` 
            - 在形式上类似 `movl` —— 指明来源memory地址和目标register。但区别在于`leal` **并不读取这个memory地址里具体的数据，而是取计算所得的地址本身**。
            - 用途：生成指针数据、用于简单的整数运算（编译器会利用这点来优化）
        - 单目运算操作(unary)
            - 单一的操作数既作为 src 也作为 dest，**该操作数既可以是register也可以是memory**
            - `INC` 和 `DEC` 类似C中的 `++` 和 `--` 操作符
            - `NEG` 是取补码，`NOT`取反码
        - 双目运算操作(binary)
            - 两个操作数，第二个操作数既作为 src 也作为 dest。第一个操作数可以是 immediate/register/memory，第二个操作数同单目只能为 register/memory (值得一提的是，**数据移动指令中两个操作数不能同时为memory**，比如`movl`)
            - 类似C中的 `+=`, `-=`, `*=`，**但要注意和C中操作数顺序相反，dest在后面**（比如 `SUB S,D` 用C表示是 `D -= S`)
        - 位移操作
            - 第一个操作数`k` 只用一个byte去表示，因为移位数只有 0~31 有效(实际上甚至是取低5位)。因此 `k` **只能为一个立即数或者一个单byte register** (比如`%cl`)；第二个操作数同样也是即作为 src 也作为dest，可以为register或memory
            - 左移不分算数和逻辑左移（因为都是低位补0），因此`SAL`和`SHL`效果相同
            - `SAR` 为算数右移，高位补符号位；`SHR`为逻辑右移，高位补零
        - {{[[TODO]]}} Problem 3.10 ，使用 `xor` 去完成赋值0的操作，竟然汇编比使用`movl`更省空间(2bytes v.s. 5bytes)。自己汇编验证一下
    - 特殊运算符
        - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FqB-ICegzDy.png?alt=media&token=5cb88e62-48df-4f24-8819-ba62ca6c92ce)
        - 提供full-64bit的运算而不是截断
        - 都用到了`%eax` 和 `%edx`
        - `imull`和`mull`
            - 在 双目运算操作(binary) 里描述的 `IMUL` 集合有个`imull`指令，负责32bit * 32bit -> 32bit
                - $$*_{32}^{u}$$ $$*_{32}^{t}$$ Section 2.3.4 & 2.3.5
                - {{[[TODO]]}} 对于这个截取后32位的乘法，对无符号和有符号效果一样。为什么呢？这个可被严谨证明，可以回2.3.4和2.3.5看。 
            - 但此处 `imull S`和 `mull S` 单目指令，是用来获取32bit * 32bit -> 64bit的完整输出
                - 32bit * 32bit 最多只会到64bit，可以简单演算证明
                - `mull` 用于无符号乘法，`imull` 用于有符号乘法
            - $$S\times R[\%eax]$$，高32bit存 %edx, 低32bit 存 %eax
        - `idivl` 和 `divl`
            - 前面32-bit的指令(Figure3.7)里面没有的除法和求余（取模），这两条指令同时做
            - $$R[\%edx]:R[\%eax]\div S $$ 结果存 %eax，余数存%edx
            - `divl`用于无符号除法，`idivl` 用于有符号除法
            - {{[[TODO]]}} 如果除法运算的结果比 %eax 长，那怎么截断？C语言又怎么做正确的计算 -- 可以自己跑汇编试一下
        - `cltd`
            - 在做上述**有符号除法运算**时，如果被除数只用 %eax 就能存，通常需要先做 SignExtend；
```javascript
movl %eax, %edx // copy value in %eax to %edx
sarl $31, %edx // sign extend %eax in %edx```
            - `cltd` 直接等价于上述操作
        - {{[[TODO]]}} Problem 3.12 实现 `unsigned long long * unsigned` 很值得回看，并证明正确性
- ### 控制语句
    - 底层机器码提供两种机制来实现**条件式行为**：检测数值，然后修改**数据流或控制流**
        - {{[[DONE]]}} 什么是数据流/控制流
            - https://stackoverflow.com/questions/4452770/ssis-control-flow-vs-data-flow
            - 个人理解
                - 数据流 = 数据拷贝的方向；控制流 = 指令执行的顺序
                - 比如 `int a = a_bool ? b : c;` 是操纵数据流，for loop 是操纵控制流 （不确定对不对）
    - 状态码
        - 单bit寄存器，保存最近的算术、逻辑操作结果
        - 最常用的状态码
            - CF: Carry Flag, 最高位的进位输出，**用于检测无符号数运算的溢出**
            - ZF: Zero Flag, 运算结果是否为0
            - SF: Sign Flag, 运算结果是否为负数
            - OF: Overflow Flag, **用于检测是否有有符号（补码）运算的溢出**
        - 假设有C语句 `t=a+b`, 其中三个变量都是 `int`，上述状态码的设置等价于如下C语句
            - CF: `(unsigned) t < (unsigned) a` (无符号溢出肯定小于两个数中的任何一个)
            - ZF: `(t == 0)`
                - {{[[TODO]]}} 但如果溢出而且刚好结果是0呢
            - SF: `(t < 0)`
            - OF: `(a < 0 == b < 0) && (t < 0 != a < 0)` (有符号溢出意味着`a` `b`同号而结果`t`异号)
        - 状态码设置对于各个运算符的规则
            - 除`leal`外图3.7(基本运算符)的运算符都会设置这些flag
                - `leal`不会改变状态码，因为本身就是为了内存地址计算而存在的;
            - 对于逻辑操作(比如`XOR`), CF 和 OF 都被设为0 (不可能有溢出)
            - 对于shift, CF被设为****最后被移出去****的那bit, 而OF被设为0
            - `INC`和`DEC`(自增自减)会设置 OF ZF, 但不改变 CF (此处没展开讲原因)
        - 不改变其他寄存器，**只改变状态码**的指令
            - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2F3e_efkPyCF.png?alt=media&token=35ea2236-9da9-43a4-9bbe-938b543f3c9e)
            - 这两类指令都具有 8, 16, 32 位的版本
            - `CMP` 效果和 `SUB` 一致，但并不指定目标位置
                - 要注意在书中展示的ATT格式中，两个操作数是相反的（估计是为了对齐`SUB S, D = D <- D-S`）
                - 判断相等使用 ZF 就行
                - {{[[TODO]]}} 判断谁大谁小用什么状态码？在有符号无符号情况下分别怎么做？CF能判断无符号减法的溢出吗
            - `TEST` 效果和 `AND` 一致，但同样页并不指定目标位置
                - test 两个相同的操作数(例如`testl %eax, %eax`)可以判断%eax是否为负、零、正数
        - 访问状态码
            - 访问状态码有三种方式
                1. SET - 根据状态码的某种组合去设置某个byte
                2. JUMP - 根据不同状态跳转到程序的不同位置
                3. 条件式地移动（拷贝）数据
    - `SET`指令
        - 根据状态码的某种组合去设置某个byte
        - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FUg5WhhoAsF.png?alt=media&token=6456c35f-1483-4ab0-8073-dddd02dc0bc5)
        - 注意SET指令后缀代表使用的不同状态码，而不是操作数大小
        - SET指令的Destination必须是 **1byte寄存器 / 1byte内存地址**
        - 示例：C语言中的 `a<b`
            - `a` `b` 都是 `int`, 返回结果放在 `%eax`
            - ```shell
# a is in %edx, b is in %eax
cmpl  %eax, %edx  # compare a with b, setting condition codes
setl  %al  # set low-order byte of %eax
movzbl %al, %eax # zero-extend %eax```
        - {{[[TODO]]}} 逐个SET指令去推理 (像解数学题一样)
            - {{[[TODO]]}} `sete`
            - {{[[TODO]]}} `setl`
            - {{[[TODO]]}} ...
            - {{[[TODO]]}} Problem 3.14 对 `TEST` 的使用（很巧刚好`TEST`结果也和`SET`的指令名对的上）
    - 跳转(Jump)指令
        - 顾名思义，将原本顺序执行的指令跳转到新的位置；跳转位置使用 **label** 来指定；assembler在汇编时将 labels 转成目标指令地址
        - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2F9x5LYqIVTZ.png?alt=media&token=0774cc35-88cc-47b6-83df-1e8ec3996dff)
            - `jmp`: 可以是跳转到 label, 也可以是用 `*` 读取register/memory地址进行跳转
            - 其余jump指令都是根据 **状态码** 来决定是跳转 Label 还是顺序执行（命名规则同`SET`)；但这些指令只能跳转Label不能从 register/mem 读取地址
        - Jump跳转位置在机器码的编码方式
            - 对 Chapter7 的 Linking 很重要
            - 汇编码中跳转位置是用 labels 定义的；assembler 和 linker 对跳转位置的编码有多种方式
                - 其中最常用的是 **PC relative**：PC即cpu中的program counter，PC relative指 **跳转位置和Jump下一条指令的指令差数**；可以用1/2/4 bytes 来编码这个差值
                    - 可见使用PC-relative计算跳转位置时，指令计数器处于 jump指令的下一位而非Jump本身，这是一个历史实现惯例：处理器会在执行当前指令之前先把PC更新到下一位
                        - > 好像之前计组实验就是这样做的（真希望什么时候能再实现一个CPU）
                - 另一种编码是用 **绝对地址**，使用 4bytes编码
                - assembler 和 linker 会根据情况选取适当的编码方式
                - PC-relative的机器码例子
                    - assemble 后的 `.o` 代码，再 disassemble:
![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FFfWZW4DKAc.png?alt=media&token=4aa12a7c-d02c-4a15-a5fc-59f5eace192e)
                        - 第一条 `jle` 指令指向汇编码中的 `0x17` 位置；机器码用 1byte 编码了 `0x0d`(13)；`0x17 - 0x0a = 0x0d`
                        - 倒数第二条 `jg` 指令指向汇编码中的 `0xa` 位置；机器码用 1byte 编码了`0xf3`(1111 0011 -->(补码) 0000 1101 = -13)；`0x0a - 0x17 = 0xf3`(补码减法)
                    - linking 后的代码，再disassemble:
![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FV7zvW84aJV.png?alt=media&token=157b04f9-72fa-4eca-a619-996fa39b6bd9)
                        - 指令地址被重定位，但jump指令由于是 PC-relative所以跳转地址不变，而且编码代价也小（绝对地址要4bytes，这里只要1byte
                        - {{[[TODO]]}} 如果是更大的函数，跳转地址用1byte表示不了怎么办 
            - 
    - 条件分支 (`if`)
        - 条件语句通常用 JUMP 来实现（控制流），但3.6.6也会介绍某些情况下可以用数据流来实现
        - 条件语句对应的汇编码，可以用 `goto`语句来类比（`goto`就类似`jmp`)
            - 原条件语句
```c
if (test-expr) {
  // then-statement
}
else {
  // else-statement
}```
            - `goto`语句 （用来类比汇编后的结果）
```c
  t = test-expr; // cmp, test 之类的指令
  if (!t) { goto FALSE; } // 利用条件JUMP语句实现
  // then-statement
  goto DONE;
FALSE:
  // else statement
DONE:```
            - 思考了一下为什么是 `if(!t)`来跳转 而不是按照原代码中的 `if(t)`（Problem 3.17，但我居然是先去想这个问题然后再遇到这道题的:) ）
                - 假设是多级if-elseif-else结构，使用`if(t)`来判断跳转的话if判断和各自的statements会分离；而使用`if(!t)`和原顺序一致，可读性高了很多
                    - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FpMdqvLSKhs.png?alt=media&token=8eeea0ed-e5bc-4735-b54c-56319377a433)
                - 刚好Problem 3.17就是说这个问题，答案还有一个原因：
                    - 如果是一个简单不带`else`的条件语句

                    - 使用 `if(t)`:
```c
  t = test-expr;
  if (t) { goto TRUE; }
  goto END;
TRUE:
  // statements
END:```
                    - 使用 `if(!t)`:
```c
  t = test-expr;
  if (!t) { goto END; }
  // statements
END:```
                    -  显然代码简单了不少
    - 循环
        - 同样也是用 `CMP`, `TEST`, `JUMP` 之类的构造循环结构
        - 大多数编译器使用 `do-while` 作为循环基础形式，其他形式循环只是其变种；同if，将用 goto 语句来类比汇编代码，简化阅读
        - `do-while`
            - 原形式
```c
do {
  // body statements
} while(test-expr);```
            - goto形式
```c
LOOP:
  // body statements
  t = test-expr;
  if (t) { goto LOOP; }```
        - `while`
            - 原形式
```c
while(test-expr) {
  // body statements
}```
            - goto 形式
```c
  t = test-expr;
  if (!t) { goto DONE; }
LOOP:
  // body statements
  t = test-expr;
  if (t) { goto LOOP; }
DONE:```
                - 上述是GCC采用的实现，相当于将`while`转换成`if`+`do-while`。
                - {{[[TODO]]}} 可以想到有另一种“更简洁”的实现，为什么不这样做呢：
```c
LOOP:
  t = test-expr;
  if (!t) { goto DONE; }
  // body statements
  goto LOOP;
DONE:```
                    - 书上的说法：使用这种实现策略，编译器通常可以优化初始测试，例如确定测试条件将始终成立（就完全是do-while了）。
                    - 确实感觉也是这样，通常使用while都预先设置一个n，而且一定符合条件，这样编译器就可以直接不考虑第一个判断了。
            - {{[[TODO]]}} Problem 3.21 的优化有点意思，但是编译器是怎么detect这种优化的呢
        - `for`
            - 原形式
```c
for (init-expr; test-expr; update-expr) {
  // body statements
}```
            - 转化为`while`形式
```c
init-expr;
while(test-expr) {
  // body statements
  update-expr;
}```
            - goto 形式
```c
  init-expr;
  t = test-expr;
  if (!t) { goto DONE; }
LOOP:
  // body statements
  update-expr;
  t = test-expr;
  if (t) { goto LOOP; }
DONE:```
    - 条件拷贝指令
        - 数据流 - 实现条件式操作的另一种方式，区别于控制流（控制流更简单和易懂，但对于现代处理器来说**不够高效**）
        - 三元表达式是最好的例子：两个结果值都求，然后根据`test-expr`选择一个返回
        - `cmovl` 后缀是 less，不是long
            - {{[[DONE]]}} 那可以移动不同大小的数据吗
                - 没有标识操作数大小，但根据Register来定，且只能是 32bit/16bit (这是老奔腾，不知道新的指令怎么样)
        - 相比较 条件分支(Jump) 性能上的优势
            - 处理器流水线和分支预测
                - Chapter4、5会详细讲
                - 流水线：指令由一系列的steps组成；处理器通过**重叠连续指令的steps**来提高性能
                - 处理器需要提前确定指令执行的顺序，从而**让流水线填满指令**，从而达到重叠stage的效果
                - 但遇到 JUMP 指令（通常称为**分支**）时，并不能直接确定分支的走向以及在这之后的指令，所以引入了复杂的**分支预测**，去猜测分支的走向
                - 分支预测猜对了，流水线填满指令，性能好；分支预测猜错了，处理器丢弃前面前向的许多工作，会浪费20~40个时钟周期
            - 以求绝对值函数的两种实现方法为例
                - Figure 3.13 是控制流，Figure 3.16是数据流
                - 对于`x<y`的分支预测，再厉害的分支预测也只能说五五开；另外由于程序中每条指令占时钟数很少，因此分支预测失败的成本很高
                - 最终结果是：条件分支 最好结果13时钟周期，最坏13+44=57时钟周期，平均也有35；条件拷贝都只需14时钟周期，不被数据影响
        - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2F9ctTT7BC7C.png?alt=media&token=3576a6af-237f-4ba7-b38f-5f62efa9b046)
            - 没有标识操作数大小，但根据Register来定，且只能是 32bit/16bit (这是老奔腾，不知道新的指令怎么样)
        - 并非所有三元运算符语句都能用条件拷贝(conditional move)
            - 比如表达式会导致错误：`return (xp ? *xp: 0);`，提前解引用可能会有空指针异常，所以必须用 分支
            - 比如表达式具有副作用：`return x < y ? (lcount++, y-x) : x-y;`
        - 条件拷贝并不总提高效率
            - 比如表达式计算量很大
        - {{[[TODO]]}} Problem 3.26 
            - 关于计算 / 4，负数加 bias = 2^2 -1 = 3
            - Section 2.3.7
    - `switch`
        - 基于 **整数索引值** 的条件分支；不仅可读性高，而且使用高效的**跳转表**进行实现
        - 跳转表
            - 本质就是一个数组，case条件值对应数组index，而数组值对应跳转地址
            - 仅会在cases多(>=4)且数值波动范围小的时候使用跳转表
            - 与使用多级 `if-else` 相比的优势：计算出`switch`的值就直接可以跳转了，不需要逐个去判断，使case数量和计算时间无关
        - 例子 Figure 3.19
            - {{[[TODO]]}} 为什么转成机器码后case的顺序变得这么奇怪，这是某种优化的结果吗
            - 留意到`jmp`使用的是 indirect jump，即跳转到内存内存储的地址
            -  Section 3.8 会介绍数组如何转成机器码
- ### 过程调用 (procedure call)
    - 概述
        - {{[[TODO]]}} 过程调用和函数的关系是什么
        - 过程调用涉及 数据(参数/返回值) 和 控制 的传递
            - 数据传递（local var分配和解除）基于对 **程序栈** 的操作
            - {{[[TODO]]}} 控制传递？是基于jump吗
    - 栈帧(Stack frame)
        - 程序栈作用
            - 传递参数
            - 存储返回信息
            - 保存register（以便恢复现场）
            - local var的存储
        - 栈帧是程序栈中分配给一个procedure的空间，可以类比一个函数的执行环境
        - 栈帧结构
            - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FF_9zHeHc2S.png?alt=media&token=54e898f1-4077-45dd-a0d4-43edf8c55d93)
            - 栈顶的栈帧
                - 栈顶栈帧即当前所执行帧，由 **基址指针寄存器(%ebp)(frame pointer)**所指向地址 和 **栈指针寄存器(%esp)(stack pointer)**所指向地址 分割
                - 栈从高地址往低拓展，所以栈顶是最低地址
                - `%esp` 在procedure执行过程中会变化(栈空间变化)，因此栈上大部分数据都是通过 `%ebp` 去访问
            - 以具体的过程调用 (过程P --> 过程Q)为例
                - P调用Q时的实参先逆序地压栈顶，最后将P的返回地址压栈顶（作为P帧最后一节数据）
                - Q帧第一个数据是复制P帧原来 `%ebp` 的数据（记录P帧起始地址？），同时新的 `%ebp` 指向这个数据地址
                - Q帧其他数据包括
                    - 寄存器值拷贝
                        - {{[[TODO]]}} followed by copies of any other saved register values? 除了本地变量以外为什么会需要拷贝寄存器值
                    - 局部变量（寄存器数不足、数组或struct、&取地址操作）
    - 控制转移
        - 支持过程调用和返回的指令
![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FG420K69qm3.png?alt=media&token=3b1948d4-f147-43b8-84a3-323c72a1bfe6)
            - `call`: 将返回地址(call指令的下一个地址 )压到栈上，然后执行jump
                - 类似jump, call可以是直接跳label，也可以是间接跳到register/memory存储值所表示的地址
            - `ret`: 将栈顶上保存的地址弹出，然后jump到该地址。
                - ret指令的正确用法是先将栈顶(即`%esp`指向位置)指向`call`指令存储返回地址的栈上位置
            - `leave`：用于准备调用`ret`前的堆栈
                - leave等价于下述指令集
                    - ```javascript
movl  %ebp, %esp
popl  %ebp```
                - 结合栈帧结构来看，就是将%esp(栈顶)和%ebp指回caller栈帧，其中%esp指向要跳转返回的指令地址
                - 准备返回堆栈也可以不调用 `leave`, 显式地调用move和pop，并且 %eax 可被用于存储返回整数/指针地返回值。
            - * Problem 3.30: IA32获得程序计数器(PC)值的唯一办法
                - 先调用call，然后立即pop到%eax，不是一个完整的过程调用（指令执行顺序压根没变）
                - > 是不是用来断点调试用的?
    - 寄存器使用惯例
        - 寄存器被所有 procedure 共有，因此要保证caller不会覆盖callee将会使用到的寄存器值。IA32使用一套惯例来保证。
        - %eax, %edx, %ecx --> caller-save 寄存器
            - callee 可以随意更改这些寄存器
        - %ebx, %esi, %edi --> callee-save 寄存器
            - callee 需要保证返回时这些寄存器值保持不变
        - %ebp, %esp 是只用于上文描述的栈操作
        - 举例
            - ```c
int P(int x) 
{
  int y = x*x;
  int z = Q(y);
  return y+z;
}```
            - 需要保证函数(过程)`Q`返回以后，`y`值仍然可被访问，这有两种办法
                - 在`P`调用时先将`y`保存到`P`的栈帧(mem)中 (`P`负责保存值)
                - `P`将`y`保存在callee-save寄存器，交由`Q`决定是否需要存到自己的栈帧，只要保证返回时恢复寄存器即可 (`Q`负责保存值)
        - > 我一直以为恢复现场会恢复所有的寄存器（即所有都是 callee-safe），但看起来为了性能或者其他什么考虑，分成了恢复/不会恢复两部分
    - **过程调用示例**
        - C代码
            - ```c
// called by caller()
int swap_add(int *xp, int *yp)
{
  int x = *xp;
  int y = *yp;

  *xp = y;
  *yp = x;
  return x + y;
}

int caller()
{
  int arg1 = 534;
  int arg2 = 1057;
  int sum = swap_add(&arg1, &arg2);
  int diff = arg1 - arg2;

  return sum * diff;
}```
        - `caller` 的汇编代码（开头 --> 调用 `swap_add`）
            - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FaANpsms3tl.png?alt=media&token=26a6d28f-a3f2-454b-b9fb-5b48f815e8c4)
            - 先执行函数栈帧常规初始化操作：保存caller的`%ebp`, 并指定新的`%ebp`位置（到新栈帧头，也即当前栈顶`%esp`）
            - `subl $24, %esp` 分配了“6格”(24bytes)的栈空间（32位指令集）
            - 两个`&` 取址运算并赋值到`swap_add`实参时都用了 `%eax`当中介，因为不能两个运算符同时为内存 
            - 有“2格”空间(8bytes)未被使用，因为 **GCC遵从x86的编程规范，函数使用的空间必须是16bytes的倍数 **
                - 每“格”是32bit=4bytes，也就是函数空间必须是4格的倍数
                - 这是包括了4bytes的%ebp存根 + 4bytes的返回地址
                - 这种规范是为了数据访问中的**对齐**，3.9.3会详细讲述
        - `caller` 在准备调用 `swap_add` 时的栈帧
            - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FL8I89gK3vT.png?alt=media&token=1d6bf585-79c2-4b36-92ff-e38940b5555e)
            - 注意在调用 `call` 以后，返回地址还会被压到栈顶，图中没显示
        - `swap_add`的汇编代码
            - 分为三部分：setup, body, finish
            - setup 初始化栈帧
                - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FUPq4ffqrG4.png?alt=media&token=c7d984fb-68cc-4371-8afe-81547f8b0f69)
                - 先执行常规操作 (`pushl %ebp`, `movl %esp, %ebp`)
                - `swap_add` 函数会需要用到 `%ebx`, 而且这个寄存器是 callee-save，所以需要先存到栈上（caller-save的还不够用）
            - body 执行函数的实际流程
                - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2Fd7mgBo9spb.png?alt=media&token=8de2985d-fe53-4210-9fbe-5d30e18e7326)
            - finish 恢复调用者栈并执行返回
                - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FEkfZxMYerg.png?alt=media&token=f1241550-2099-458d-927c-ab33158c7685)
        - `swap_add` 执行完setup以后的栈帧状态
            - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2FIjm-Org-pV.png?alt=media&token=456cd71e-5c15-4141-ab61-af98a4b04e50)
        - `caller` 从 `swap_add` 返回后后续汇编代码
            - ![](https://firebasestorage.googleapis.com/v0/b/firescript-577a2.appspot.com/o/imgs%2Fapp%2Fjeff-zys-8%2F6w01Pt75Db.png?alt=media&token=7ec4230e-2d0e-45f6-881e-25a69e1e6343)
            - 留意到`caller`使用`leave`去重置 `%esp`和`%ebp`
                - 有时候是`leave`，有时候是调用`movl`和`popl`，两者其实都可以
        - 总结
            - compiler用一组简单的规则+栈来实现函数调用
            - 实参(args)通过栈来传递，并通过`%ebp`的相对地址来访问
            - 栈上空间可以通过`push`分配，也可以直接通过直接把`%esp`减去一定值来分配
            - 函数返回前，需要**恢复现场**：
                - 将callee-save寄存器恢复
                - 将栈帧恢复(`%esp`和`%ebp`)
        - {{[[DONE]]}} Problem 3.33 是比较综合的栈帧考察，值得回看
    - 阶乘函数例子
- ### 数组分配和访问
    - 结合前面的知识，这里的数组分配看起来很直观，就是分配一段连续的 size * N 的空间，再分配一个指针空间指向连续空间的开始地址
    - 指针的算数运算
        - `+` `-`: 每个单位 (1) = 指针指向类型size
            - `&E[i]-E` 结果是 `i`，而不是 `size*i`
        - `&` `*`: `A[i] = *(A+i)`
    - 多维数组
        - 用行列的概念组合计算就可以了，没什么复杂的 
        - 若有 `T D[R][C]`，则`&D[i][j] = start_addr + sizeof(T) * (C * i + j)`
        - 编译器优化 3.8.4~3.8.5：比较中规中矩，就是地址运算的一些优化
- ### 结构体和联合体
    - struct
        - 实例化一个struct时，会分配所需的一段连续空间并返回该连串地址的首位
        - 类型信息是compiler管理的，因此编译后的机器码就直接用 "first_addr+offset" 替换成员变量引用，不会保留任何成员变量的名字信息
    - union
        - 联合体内的成员共享同一片内存空间
        - 在联合体内成员访问互斥(即同一实例只会访问其中一个成员)时使用
        - 或用来强转数据类型 (比如用`unsigned`去换`float`，直接可以看二进制数，每一bit都不变)
            - ```c++
unsigned float2bit(float f)
{
  union {
    float f;
    unsigned u;
  } temp;
  temp.f = f;
  return temp.u;
}```
            - 上述代码转成机器码很简单：
```javascript
movl 8(%ebp), %eax```
    - {{[[TODO]]}} 内存对齐
        - > 这小节标记为未完成，因为感觉没有读透：内存对齐到底谁来做。我觉得这里的一个空白是OS和编译器；到此为止现在书里的例子都是单一的“裸procedure”，它是怎么被编译器根据不同OS平台出来，又怎么被OS调用执行，这是空白的。之后学更多了再来回看应该更有领悟。至少现在知道了有内存对齐这么个东西就可以了。
        - 许多计算机系统(??)会限制内置类型的memory地址必须为某K值的倍数(常见的为2, 4, 8)
        - 这样的内存对齐策略简化了作为 processor和memory之间接口的硬件的设计
            - 例如，假如处理器(正如IA32)每次正好从8bytes倍数地址取8Bytes数据，那对于取`double`类型数据而言，如果能保证这类型数据都存在8的倍数的地址，每次读写就只需要一次对memory的操作
        - Linux
            - 2-byte数据类型(`short`)地址需为2的倍数 = 地址值最低有效位为0
            - 更大的数据类型地址需为4的倍数 = 地址值最低两个bit为0
        - {{[[TODO]]}} 对于大多数 IA32 指令，保持数据对齐可以提高效率，但不会影响程序行为。
            -  为什么不会影响程序行为，如果不对齐，同一段C语句不就产生不同的汇编语句了吗
        - {{[[TODO]]}} 所以内存对齐是谁来控制的？编译器吗？还是操作系统？
            - 从书的描述看起来是OS在控制的
            - https://blog.csdn.net/cc_net/article/details/2908600
                - 这里博客说的是OS+编译器共同控制
        - 为保持对齐，会在`struct`中间或结尾填入一些空隙，要考虑`struct`数组的情况，每一个数组元素（包括`struct`指针）都要满足最大的成员的内存对齐
- ### 对指针的综合理解
    - 每个指针都有对应类型
        - 当然类型信息只是编译器知道，不会放到机器码里；类型信息是高级语言的抽象
        - `void*` 为通用指针
    - 每个指针都有一个值。`NULL(0)`表示不指向任何地方
    - 指针和数组关联密切
    - **注意做指针运算(+-)的时候单位是指针对应的类型**
        - {{[[TODO]]}} 那如果对 `void*`做指针运算会怎么样呢
    - 指针可以指向函数
        - 函数指针指向函数对应机器码的第一条
        - ```c++
int fun(int x, int *p);

(int) (*fp)(int, int *);
fp = fun;

int y = 1;
int result = fp(3, &y);```
- ### GDB Debugger
    - > 这一节没有讲太多内容，但我自己在wsl-64bit尝试反编译了示例程序，真的看不懂..
- ### {{[[TODO]]}} 内存越界和缓存溢出
    - 缓存溢出(buffer overflow) 是 内存越界(out-of-bounds mem ref) 的结果
        - https://en.wikipedia.org/wiki/Buffer_overflow
        - 数组访问越界的话，有可能篡改同在栈上的寄存器数据、返回地址（栈是从高地址向低地址增长）
    - **缓存溢出攻击**
        - 正如 Problem 3.43 描述的，将代码植入到超出空间的 string 中，使返回地址直接指向恶意程序所在处
            - 比如使用系统调用调起shell
        - {{[[TODO]]}} 阻止缓存溢出攻击（以gcc in Linux为例）
            - 
    - 
- ### {{[[TODO]]}} 32-bit --> 64bit
- ### {{[[TODO]]}} 浮点数
- [[roam/comments]]
