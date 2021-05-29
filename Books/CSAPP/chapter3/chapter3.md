# Chapter 3 程序的机器表示(汇编)

## 引言

高级语言的优势
- 类型检查: 规避直接写汇编操作数据时的不一致行为
- 高级语言写的程序能在多个平台被编译，而汇编代码就非常 machine specific

本章学习两个汇编语言，并学习C语言是怎么编译成对应汇编
- IA32
- x86-64

**深入细节和例子，不要浮于表面的概念**

- 编译器集成了很多向后兼容的功能，要结合历史去看
- 编译器会做很多很多优化，不关掉甚至看不出原来的程序是啥样

本章内容 
- C、汇编、机器语言的关系 
- 深入学习 IA32
  - 数据表示
  - 控制流的实现(if, while, switch等)
  - 运行时(procedures, stack)
  - 复合数据类型(array, struct, union)
- 内存地址错误引用、系统漏洞、GDB

(web aside) 汇编和C结合
- 在linking stage将汇编和C函数结合
- gcc支持C函数中嵌入汇编

## 历史

x86: Intel processor line
- 8086, 8088 IBM 个人电脑 (16-bit)
- 8087 floating-point coprocessor (x87)
- i386 32bit, first to support Unix OS, 1985
- PentiumPro, P6 micoarchitecture, "conditional move" instructions
- Pentium III, SSE(操作整数向量、浮点数的指令集)
- Pentinum 4E, 超线程(一核跑两程序)，支持64位(在amd_64之后)
- core2: 多核
- corei7: 超线程+多核, 2008

最初的8086和80286的内存模型已经过时，Linux使用平面寻址(进程空间是一块连续的内存数组)

GCC默认为i386编译(很老了), 只有给定编译参数或编译64位的操作，才能使GCC使用更多新拓展
> 比较好奇，现在还是这样吗

## 汇编代码初窥

C编译过程
- preprocessor #include, #define
- compiler --> .s汇编
- assember --> .o 二进制(不可重定向?), global values地址未填入
- linker --> 最终可执行文件

两个重要抽象
- 指令集架构(ISA)
  - 对处理器的抽象
  - IA32, X86-64都是ISA，都把指令抽象为顺序执行
- 虚拟内存
  - 对物理内存的抽象
  - 二进制程序使用的内存是虚拟内存(?)

相比较C语言，IA32机器码显示更多C看不到的 **处理器状态**
- 程序计数器 (PC, `%eip`)，指示下一个执行指令的内存地址
- integer register file * 8
  - 存 32-bit 数据 
  - 地址(C指针)、数据、程序状态、栈变量、函数返回值
- condition code register
  - 最近执行的算数或逻辑运算信息
- floating-point registers

### 代码示例

```c
int accum = 0;
int sum(int x, int y)
{
  int t = x + y;
  accum += t;
  return t;
}
```

```sh
> gcc -O1 -S code.c
```
> 汇编代码和书上的示例不一样，即使加上`-m32`参数也是，如下，后面再研究
```asm
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
4:
```

disassemblers: 由机器码到汇编码
```
> objdump -d code.o

code.o：     文件格式 elf64-x86-64


Disassembly of section .text:

0000000000000000 <sum>:
   0:	f3 0f 1e fa          	endbr64 
   4:	8d 04 37             	lea    (%rdi,%rsi,1),%eax
   7:	01 05 00 00 00 00    	add    %eax,0x0(%rip)        # d <sum+0xd>
   d:	c3                   	retq 
```

看看32位(加`-m32`参数)的：
```sh
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
   3:	c3                   	ret    
```

> 那么问题来了，IA32 和 i386的区别？ -- IA-32 (short for "Intel Architecture, 32-bit", sometimes also called i386) from wikipedia; 所以就是一个东西了

- IA32 为变长指令集 $\in [1,15]$ bytes
- 注意到 `objdump -d` 和 `gcc -S` 生成的指令有不同 (比如 `addl` 变成了 `add`)，这是命名规范的差别 (比如这里的`l`是大小标记，大部分时候可以被忽略)
- > 另外我查到 `.` 开头的是给assembler的注解

新建一个main.c来调用sum:
```c
int main()
{
  retur sum(1, 3);
}
```
```sh
> gcc -O1 -o prog code.o main.c
```
> 这个时候code.o还是-m32编译的，所以会报错:
```
/usr/bin/ld: i386 architecture of input file `code.o' is incompatible with i386:x86-64 output
/usr/bin/ld: i386 architecture of input file `main.o' is incompatible with i386:x86-64 output
``` 
> 但此时即使用-m32参数来链接也会出错，还搞不懂
```
/usr/bin/ld: 找不到 Scrt1.o: 没有那个文件或目录
/usr/bin/ld: 找不到 crti.o: 没有那个文件或目录
/usr/bin/ld: 当搜索用于 /usr/lib/gcc/x86_64-linux-gnu/9/libgcc.a 时跳过不兼容的 -lgcc
/usr/bin/ld: 找不到 -lgcc
/usr/bin/ld: 当搜索用于 /usr/lib/gcc/x86_64-linux-gnu/9/libgcc.a 时跳过不兼容的 -lgcc
/usr/bin/ld: 找不到 -lgcc
collect2: error: ld returned 1 exit status
```
> 所以先回归使用64位(default)，后面学的时候再去研究

使用`objdump`分析新生成的`prog`可执行文件，这时候文件大多了，就只看`sum`函数部分：

```asm
0000000000001129 <sum>:
    1129:       f3 0f 1e fa             endbr64 
    112d:       8d 04 37                lea    (%rdi,%rsi,1),%eax
    1130:       01 05 de 2e 00 00       add    %eax,0x2ede(%rip)        # 4014 <accum>
    1136:       c3                      retq   
```
可以看到和 `code.o`的反汇编相比，首先是指令的地址改变了(shifted by linker)；其次是确定了全局变量`accum`的存储位置 (`0x2ede`)，见下面：
```
7:	01 05 00 00 00 00    	add    %eax,0x0(%rip)        # d <sum+0xd>
1130:   01 05 de 2e 00 00       add    %eax,0x2ede(%rip)        # 4014 <accum>
```
> 可以注意到这里是小端模式，所以字节顺序 `de 2e` 和地址 `0x2ede` 反了

## 数据表示

由于历史16-bit架构原因，Intel使用"word"来表示16-bit数据类型，而对应32和64是"double words", "quad words".

以下表格是C内置类型在 IA32 的表示：
C declaration| Intel data type | Assembly code suffix |Size (bytes)
--|--|--|--|
char|Byte |`b` | 1| 
short|Word|`w`|2|
int|Double word|`l`|4
long int|Double word|`l`|4|
long long int|—|—|4
any pointer|Double word|`l`|4
float|Single precision|`s`|4
double|Doub1le precision|`l`|8
long double|Extended precision|`t`|10/12

- 大部分都是"double words"，不管有符号还是无符号
- 所有指针都是 doubler words
- **C语言中的 `long long int` 并不被IA32硬件支持，所以编译器要转成一系列对 32-bit (double words)的操作**
- 浮点数用的是另外一套表示体系，具体看表，gcc 使用 `long double`表示拓展精度
- gcc编译的汇编代码有一个后缀，表示操作数的大小
  - 比如数据移动指令 就有几个变形 `movb`, `movw`, `movl`
  - 尽管 Double Word 的汇编指令后缀 和 Single Precision 相同 (都是`l`)，但这没有关系，因为**浮点数用的是完全不同的汇编指令和寄存器**
- > 不知道64位会是怎么样的，之后可以留意一下



## TODO

RIP, RBP, RSP, RDI, etc.
These are the same as EIP, EBP, ESP, EDI, etc. but are used in 64-bit systems. Essentially, they are the same names except with an R instead of an E.

x86寄存器列表 https://www.eecg.utoronto.ca/~amza/www.mindsec.com/files/x86regs.html