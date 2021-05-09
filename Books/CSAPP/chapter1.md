"K&R" *The C Programming Language*


# Chapter 1 - Intro

## 编译

追踪 C - Helloworld 的生命周期

```c
/* $begin hello */
#include <stdio.h>

int main() 
{
    printf("hello, world\n");
    return 0;
}
/* $end hello */
```

机器对数字的表示是**有限的近似**，会有非预期的行为发生(**Chapter2** 详解)

编译流程:
- Preprocessing
  - 比如头文件加载，变成`.i`文件
- Compilation
  - 编译为汇编文本 `.s`
- Assembly
  - 从汇编生成可重定向二进制文件 `hello.o`
- Linking
  - `hello.o` 和 `printf.o` 组装起来

了解编译的行为可以解释这些问题
- `switch`一定比`if`高效吗
- 一个函数调用的开销是多少
- `while`比`for`高效吗
- 引用指针比引用数组更高效吗
- 为什么在循环中使用局部变量求和会比使用按引用传值的参数运行更快
- 如何通过更改运算括号来提升速度

了解编译行为的原因:
- 优化程序性能
  - **Chapter3**介绍 IA32, x86-64, **Chapter5**介绍C程序优化, **Chapter6**介绍内存层级架构以及C程序数据是怎么存在内存的
- 理解链接时错误
  - **Chapter7**: 同名global var?静态库动态库?run-time link error?
- 规避安全漏洞
  - **Chapter3**: **buffer overflow vulnerabilities**

> 形成闭环:) 

## 硬件结构

- Buses
  - 总线负责在各种组件传数据
  - 单位通常是 word，一个word通常是4/8 bytes(对应32/64位)
- I/O Devices
  - 通过 Controllers/Adapter 与 I/O bus相连
  - **Chapter6** 介绍I/O设备的工作方式
  - **Chapter10** 介绍UNIX的I/O接口
- Main Memoery
  - **Chapter6**
- Processor
  - PC 程序计数器 word-sized register: 指向当前指令在内存中的位置
  - Register file(怎么翻译): 具名寄存器集合
  - ALU 算数逻辑单元
  - 执行*指令集架构*中的指令(Load, Store, Operate, Jump...)

**Chapter3**介绍机器码以及指令集架构提供的抽象, **Chapter4**会进一步介绍处理器是怎么实现的

> 什么时候能再次看懂自己当时写的多周期CPU

## 存储层级

**Caching**

- 程序的执行包含着大量的数据拷贝: 比如`hello,world\n`一开始在硬盘，然后被DMA方式复制到内存，再在执行时被复制到CPU中的寄存器，这都是开销，怎么让这些复制更快一些?
- 存储空间越大，访问速度越慢; CPU<->寄存器 和 CPU<->内存 存取速度相差巨大 --> 
- 引入Cache memories (L1, L2, L3 cache)：缓存背后利用的是程序的 **局部性** 原理，即程序在短时间内重复从某个局部中访问代码和数据的趋势

```
# 存储层级
L0: CPU原生registers
L1~L3: SRAM
L4: 内存(DRAM)
L5: 硬盘等二级存储
L6: 远程二级存储(分布式FS, Web Servers...)
```

## 操作系统

1. 保护硬件
2. 为软件提供底层硬件的统一抽象: 进程、虚拟内存、文件

**进程**
- “独占系统”的抽象，实则多个程序并发执行
- context swtiching
  - context: PC, 寄存器内容, 内存内容(?)
- **Chapter8**

**线程**: **Chapter12**

**虚拟内存**:
- “独占内存”的抽象, 虚拟地址空间
- Linux虚拟地址空间 (也可见类似的笔记[OS/process](../../OS/process.md)):
```
---------           高地址
Kernel virtual memory --> 命令行参数、环境变量之类, 用户代码只能通过syscall读取
---------
User Stack --> 函数栈空间
    |
    ↓


    ↑
    |
Memory mapped region 
for shared libraries --> 比如 printf 函数(dynamic linking)


    ↑
    |
User Stack --> malloc & free 运行时动态分配
---------
Read/write data --> bss 未初始化段?(还没有讲)
---------
Read-only code and data
---------           低地址
```

**文件**
- *一切皆文件*