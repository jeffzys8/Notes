# Linux进程管理

程序以文件形式存在硬盘等存储设备中。执行程序的时候，系统会根据执行的用户分配 用户/组 的权限，分配 PID，然后将进程载入内存成为**进程**。

PPID 进程的父进程
父进程通过fork复制一个一模一样的子进程，使用新的PID。而后通过exec执行新的指令

daemon, 服务, 守护进程
crontab，由crontd守护进程扫描`/etc/crontab`执行

某个终端因为进程卡死时，可以切换其他终端`[Alt]+[F1~F7]`来杀掉进程恢复正常

## bash job control - 背景进程管理（自己bash运行的）

- `&` 直接将指令丢到背景去运行
  - 会显示job number 和 PID：`[1] 14432   <== [job number] PID`
  - 指令完成退出后输出：`[1]+ Done tar -zpcf /tmp/etc.tar.gz /etc`
- `[Ctrl+Z]` 暂停当前的工作并回到前景
  - 同样会输出job number
  - > TODO: 输出的是什么信号？
- `jobs` 观察目前的背景工作状态
  - `l` 列出PID
  - `r` 仅列出背景中running的
  - `s` 仅列出背景中stopped的
  - 显示 `+-` 号相当于栈顶的头两个
- `fg %jobnumber` 从栈顶取任务来前台跑
  - 不加jobn从栈顶取(`+`)
- `bg` 栈里拉去背景跑
  - > TODO: 不在背景里怎么执行的bg指令？
- `kill -signal %jobnumber` 传信号（比如杀掉）
  - `-9` 强制杀掉（最常用）
  - `-l` 列出可用信号
  - `-15` 尝试正常杀掉
  - `-1` reload?
  - `-2` Ctrl+Z
  - kill还有很多其他的用处, `man -7 kill`
  - > TODO: 需要好好归纳
- **背景进程 != 系统背景，就是终端背景，如果想退出还运行的话要用`nohup`**
  - `nohup [指令与参数] &` 放到背景去
  - 输出到 `nohup.out`


## ps - 当前进程列表

> ps命令实在是难查，就记一些常用的就好了啊

- `ps -l` 查自己终端的进程
- `ps aux` 查看系统所有的进程
- 上面两个都是比较详细地输出
- `ps -e` = `ps -A`，都会输出系统地所有process

### `ps -l`
```sh
[root@study ~]# ps -l 
F S UID PID PPID C PRI NI ADDR SZ WCHAN TTY TIME CMD
4 S 0 14830 13970 0 80 0 - 52686 poll_s pts/0 00:00:00 sudo
4 S 0 14835 14830 0 80 0 - 50511 wait pts/0 00:00:00 su
4 S 0 14836 14835 0 80 0 - 29035 wait pts/0 00:00:00 bash
0 R 0 15011 14836 0 80 0 - 30319 - pts/0 00:00:00 ps
```
- 仅列出与你的操作环境(bash) 有关的进程，即最上层的父进程会是你自己的bash
- `-f`也能列出更多信息，但`-l`字段还多一些

参数信息：
- `F` process flags，说明这个进程的总结权限，常见号码有：
  - 4 表示此进程的权限为root ；
  - 1 表示此子进程仅进行复制(fork)而没有实际执行(exec)。
- `S` **代表这个进程的状态(STAT)**，主要的状态有：
  - R (Running)：该进程处于就绪(等待调度)或运行状态
  - S (Sleep)：该进程目前正在睡眠状态(idle)，但可以被唤醒(signal)。
  - D：不可被唤醒的睡眠状态
    - 进程不响应系统异步信号
    - 通常这支进程可能在等待I/O 的情况(比如等待打印机)
    - > 使用`kill`也不行？
  - T：停止状态(stop)，进程收到停止信号后停止运行。可能是在工作控制(背景暂停)或跟踪(traced) 状态；
    - > 跟踪状态应该是gdb，后面用到可以查证一下
  - Z (Zombie)：僵尸状态，进程已经终止但却无法被移除至内存外。（异常？）
    -  直到父进程调用`wait4()`系统函数后将进程释放
    -  > TODO: Zombie进程和野进程各自到底怎么出现
- `UID/PID/PPID`
- `C` CPU 使用率，单位为百分比；
- `PRI/NI` Priority/Nice的缩写，代表此进程被CPU所执行的优先顺序，数值越小代表该进程越快被CPU执行。
- `ADDR/SZ/WCHAN` 都与内存有关，
  - ADDR 是kernel function，指出该进程在内存的哪个部分，如果是个running 的进程，一般就会显示『 - 』 
  - SZ 代表此进程用掉多少内存
  - WCHAN 表示目前进程是否运作中，同样的， 若为-表示正在运作中。
  - > 这部分是什么鬼
- `TTY` 该process 是在那个终端机上面运作，若与终端机无关则显示?，另外， tty1-tty6 是本机上面的登入者程序，若为pts/0 等等的，则表示为由网路连接进主机的程序。
- `TIME` 使用掉的CPU 时间，注意，是此进程实际花费CPU 运作的时间，而不是系统时间；
- `CMD` 进程command

### `ps aux`

对系统资源看的更全一点：
- `%CPU` 该process 占用的CPU 资源百分比；
- `%MEM` 该process 占用的物理内存百分比；
- `VSZ` 该process 使用的虚拟内存(Kbytes)
- `RSS` 该process 占用的固定的记忆体量(Kbytes)
- `STAT`的**小尾巴**
  - `<` 高优先级
  - `N` 低优先级
  - `L` 有些页被锁进内存 
    - > ??
  - `s` 包含子进程
  - `+` 位于后台的进程组；
  - `l` 多线程，克隆线程 multi-threaded (using CLONE_THREAD, like NPTL pthreads do)


> [内存耗用概念：VSS/RSS/PSS/USS](https://blog.csdn.net/edmond999/article/details/79637433) 我在wsl上查不到相关指令，不知道是不是只和手机相关


## top - 动态观察程序的变化


```sh
[root@study ~]# top [-d数字] | top [-bnp] 
选项与参数：
-d ：后面可以接秒数，就是整个程序画面更新的秒数。预设是5 秒；
-b ：以批次的方式执行top ，还有更多的参数可以使用喔！
      通常会搭配资料流重导向来将批次的结果输出成为档案。
-n ：与-b 搭配，意义是，需要进行几次top 的输出结果。
-p ：指定某些个PID 来进行观察
在top 执行过程当中可以使用的按键指令：
	? ：显示在top 当中可以输入的按键指令；
	P ：以CPU 的使用资源排序显示；
	M ：以Memory 的使用资源排序显示；
	N ：以PID 来排序
	T ：由该Process 使用的CPU 时间累积(TIME+) 排序。
	k ：给予某个PID 一个信号(signal)
	r ：给予某个PID 重新制订一个nice 值。
	q ：离开top 软体的按键。
```

示例查询，每两秒钟更新一次top ，观察整体资讯： 
```sh
[root@study ~]# top -d 2 
top - 00:53:59 up 6:07, 3 users,   load average: 0.00, 0.01, 0.05 
Tasks : 179 total, 2 running, 177 sleeping, 0 stopped,    0 zombie 
%Cpu(s): 0.0 us, 0.0 sy, 0.0 ni,100.0 id,   0.0 wa , 0.0 hi, 0.0 si, 0.0 st
KiB Mem : 2916388 total, 1839140 free, 353712 used, 723536 buff/cache
KiB Swap: 1048572 total, 1048572 free,         0 used . 2318680 avail Mem
     <==如果加入k或r时，就会有相关的字样出现在这里喔！
  PID USER PR NI VIRT RES SHR S %CPU %MEM TIME+ COMMAND    
18804 root 20 0 130028 1872 1276 R 0.5 0.1 0:00.02 top
    1 root 20 0 60636 7948 2656 S 0.0 0.3 0:01.70 systemd
    2 root 20 0 0 0 0 S 0.0 0.0 0:00.01 kthreadd
    3 root 20 0 0 0 0 S 0.0 0.0 0:00.00 ksoftirqd/0
```
> 逐行来介绍一下

第一行(top...)：
- 目前的时间：`00:53:59`
- 开机到目前为止所经过的时间：`up 6:07`
- 已经登入系统的使用者人数：`3 users`
- 系统在1, 5, 15分钟的平均工作负载。代表的是1, 5, 15分钟，系统平均要负责运作几个程序(工作)的意思。若高于1要注意系统进程是否太多。

第二行(Tasks...):
- 显示的是目前进程的总量与各个状态(running, sleeping, stopped, zombie)下的进程。比较需要注意的是最后的zombie 那个数值，如果不是0 ！好好看看到底是那个process变zombie了。

第三行(%Cpus...)：
- 显示的是CPU 的整体负载，每个项目可使用? 查阅
  - 5.9%us — 用户空间占用CPU的百分比。
  - 3.4% sy — 内核空间占用CPU的百分比。
  - 0.0% ni — 改变过优先级的进程占用CPU的百分比
  - 90.4% id — 空闲CPU百分比
  - 0.0% wa — IO等待占用CPU的百分比
  - 0.0% hi — 硬中断（Hardware IRQ）占用CPU的百分比
  - 0.2% si — 软中断（Software Interrupts）占用CPU的百分比
  - > 还有一个st 是什么
- 需要特别注意的是 `wa` 项目，代表的是`I/O wait`， 通常系统会变慢都是I/O 产生的问题比较大。因此这里得要注意这个项目耗用CPU的资源。
- 如果是多核CPU，可以按下数字键『1』来切换成查看不同CPU 的负载。

第四行与第五行：
- 表示目前的物理内存与虚拟内存(`Mem/Swap`) 的使用情况。再次重申，要注意的是swap 的使用量要尽量的少！如果swap 被用的很大量，表示系统的实体记忆体实在不足！

第六行：
- 对top输入指令后的结果状态显示处

后面是每个process具体的情况，以下是关键字段（好像漏了不少）
- PID
- USER：该process 所属的使用者
- PR ：Priority 的简写，程序的优先执行顺序，越小越早被执行；
- NI ：Nice 的简写，与Priority 有关，也是越小越早被执行；
- %CPU：CPU的占用率；
- %MEM：内存的占用率；
- TIME+：CPU 累计使用时间
- ？VIRT, RES, SHR

一些使用技巧

- M 按内存占用排序，P按CPU排序
- 可以输出成文件，不然那屏幕太小看不全 
  ```sh
  top -b -n 2 > /tmp/top.txt 
  ```
- 仅观察某个PID `top -p 14836`

Mem:
191272k total    物理内存总量
173656k used    使用的物理内存总量
17616k free    空闲内存总量
22052k buffers    用作内核缓存的内存量
Swap: 
192772k total    交换区总量
0k used    使用的交换区总量
192772k free    空闲交换区总量
123988k cached    缓冲的交换区总量,内存中的内容被换出到交换区，而后又被换入到内存，但使用过的交换区尚未被覆盖，该数值即为这些内容已存在于内存中的交换区的大小,相应的内存再次被换出时可不必再对交换区写入。

**可用内存数：第四行的free + 第四行的buffers + 第五行的cached**

当你读写文件的时候，Linux内核为了提高读写性能与速度，会将文件在内存中进行缓存，这部分内存就是Cache Memory(缓存内存)。即使你的程序运行结束后，Cache Memory也不会自动释放。这就会导致你在Linux系统中程序频繁读写文件后，你会发现可用物理内存会很少。

https://blog.csdn.net/kaikai_sk/article/details/79177036

total——总物理内存
used——已使用内存，一般情况这个值会比较大，因为这个值包括了cache+应用程序使用的内存
free——完全未被使用的内存
shared——应用程序共享内存
buffers——缓存，主要用于目录方面,inode值等（ls大目录可看到这个值增加）
cached——缓存，用于已打开的文件

## pstree

查询进程间的相关性

- `-p` 并同时列出每个process 的PID；
- `-u` 并同时列出每个process 的所属帐号名称。

## 进程管理 kill

- `kill`可以帮我们将这个signal传送给某个工作(%jobnumber)或者是某个PID (直接输入数字)
- `killall`输入进程名

```
1	SIGHUP	启动被终止的程序，可让该PID 重新读取自己的设定档，类似重新启动
2	SIGINT	相当于用键盘输入[ctrl]-c 来中断一个程序的进行
9	SIGKILL	代表强制中断一个程序的进行，如果该程序进行到一半， 那么尚未完成的部分可能会有『半产品』产生，类似vim会有.filename.swp 保留下来。
15	SIGTERM	以正常的结束程序来终止该程序。由于是正常的终止， 所以后续的动作会将他完成。不过，如果该程序已经发生问题，就是无法使用正常的方法终止时， 输入这个signal 也是没有用的。
19	SIGSTOP	相当于用键盘输入[ctrl]-z 来暂停一个程序的进行
```

## Priority & Nice

> 决定进程的执行顺序、快慢，

两个值越低优先级越高，但PRI是kernel动态调整的，用户只能调NICE [-20, 19]，其中root随便调，但一般用户只能[0, 19]，而且还只能往高了调。
```
PRI(new) = PRI(old) + nice
```

## free

> 显示内存占用，包括物理内存和swap空间

- 有一次面试被问到，系统没什么进程的时候内存占用很高是怎么回事（当时答不上来），这里有一个解释就是一些文件数据被 shared/buffers/cached 了，也就是 **被系统高效暂存起来** 了。当系统再次忙碌起来时，这些内存是可以被释放的。
- 内存被用光挺正常，要更关注 **swap**

## netstat

```
netstat -[atunlp] 
选项与参数：
-a ：将目前系统上所有的连线、监听、Socket 资料都列出来
-t ：列出tcp 网路封包的资料
-u ：列出udp 网路封包的资料
-n ：不以程序的服务名称，以端口号(port number) 来显示；
-l ：列出目前正在网路监听(listen) 的服务；
-p ：列出该网路服务的程序PID 
```

包括了两部分信息：
- Active Internet connections，网络连接
- Active UNIX domain sockets，进程间socket通信

## vmstat

## 其他指令

- uname
- uptime
