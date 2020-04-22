# 硬盘分区、开机流程初探


## 参考资料

- [鸟哥的Linux私房菜](http://linux.vbird.org/linux_basic/0130designlinux.php)
- [Linux就该这么学](https://www.linuxprobe.com/chapter-06.html#62)

## 设备命名规则
在Linux系统中，每个设备都被当做一个文件来对待，存储在`/dev`中

|硬件设备|文件名称|
|--|--|
|IDE设备|`/dev/hd[a-d]`|
|SCSI/SATA/U盘|	`/dev/sd[a-p]`|
|虚拟I/O界面(用于虚拟机)|`/dev/vd[a-p]`|
|软驱|`/dev/fd[0-1]`|
|打印机|`/dev/lp[0-15]`|
|光驱|`/dev/cdrom`|
|鼠标|`/dev/mouse`|

由于现在的IDE设备已经很少见了，所以一般的硬盘设备都会是以“/dev/sd”开头的。近年来为了统一处理，大部分Linux distribution已经将IDE接口的硬盘也模拟成SATA。

> 在Android中的虚拟机也被当做设备`/dev/kvm`，在后面会了解到kvm不仅仅是用作Android虚拟机这么简单

## 虚拟机

Linux系统内置 **KVM** 进行虚拟机操作，但实际上现在VirtualBox已有Linux版本，所以无需再使用命令行配置启动虚拟机，使用VirtualBox是个不错的选择。我也也尝试了用虚拟机制作WindowsXP+CentOS双系统，具体可以看[这里](virtual_double_sys.md)

> 其实主要是kvm用不会啦，第一次安装ubuntu就失败了，以后再尝试


## 硬盘分区

在解释硬盘前先要理解一下硬盘的一些基本原理。如果对硬盘构造不熟悉的话，可以参考[这篇博客](https://www.cnblogs.com/jswang/p/9071847.html)巩固一下硬盘的基础知识，包括扇区、磁道、柱面等概念。

硬盘的扇区一般有两种规格，分别是`512bytes`与`4Kbytes`；硬盘的第一扇区存储了该磁盘的分区的启动引导程序(boot loader，比如 grub)，以及硬盘的分区信息。

硬盘的信息记录和分区方式有两种，分别是 MBR 和 GPT。

### MBR

MBR 即 Master Boot Record(主引导记录)，是早期兼容Windows硬盘的一种硬盘信息管理方式。**最主要就是只使用硬盘的第一扇区来记录记录 boot loader 和 分区信息的方式**。这种分区方式支持的最小单位是柱面（一个“圈”，对于单盘来说就是磁道）

在早期MBR诞生时，硬盘的扇区只有 `512bytes` 这种规格，因此第一扇区同样也就是这么大。其中：
- `446 bytes`是启动引导程序(boot loader)
- `64 bytes` 即为分区信息，最多仅能有4组记录，每组记录区记录了该区段的起止柱面/磁道，构成**主分区**
- 对于物理结构上有n个面(磁头)的硬盘，其分区空间的最小值为: `n × 扇区/磁道 × 512Bytes`。

这么看起来一个MBR格式的硬盘就只能分四个分区了？其实不然。更多的分区是通过**扩展分区**来实现的。简单来说就是利用扩展分区中的额外扇区来记录更多的分区信息构成**逻辑分区**。看起来是这个样子：
![MBR分区示例](disk_partition/p_e_partition.png)

其中 `/dev/sda[1-4]` 是分配给主分区和扩展分区（即硬盘第一个扇区所记录的分区）的，而逻辑分区就得从`/dev/sda5`开始了

另外，扩展分区的分区表也不是像图中那么简单，毕竟要可以支持几十个分区，仅用一个扇区肯定是不够的，[维基](http://en.wikipedia.org/wiki/Extended_boot_record)有讲实际的逻辑分区是怎么记录的。

> 当日后还有分区需要的时候，比较好的做法就是分3个主分区，然后后面的剩余空间都留给逻辑分区

**个人实践环节**

下面是博主使用虚拟机安装双系统时对40G模拟硬盘的MBR分区（具体过程见于[这篇笔记](virtual_double_sys.md))

- 1M ~ 15G : 主要分区1，用于安装CentOS (`sda1`)
- 15G ~ 30G : 主要分区2，用于安装WindowsXP (`sda2`)
- 30G ~ 40G : 扩展分区 (`sda3`)
    - 30G ~ 38G ：逻辑分区1，用于windows和cent共享数据 (`sda5`) 
        > 注意到这里从5开始，因为如前所述1~4分给了primary和extended
    - 38G ~ 40G : 逻辑分区2，用于换页，增强内存功能 (`sda6`)

**MBR的问题**

- 一个分区最多2T
  - 单个分区表`64B/4 = 16bB`，其中扇区数占`4B=32bits`，一个扇区`512B`，则分区最大值为`2^32*512B=2TB`)。
- MBR 仅有一个区块，损坏很难恢复。
- 给 boot loader 就分配了 `446B`，太小了。

> 疑问：分区表里记录的不是起止柱面吗，为什么大小要用扇区数，不用柱面数来记呢？

### GPT

新的硬盘单扇区被设计为`4KB`，为了向下兼容会使用到逻辑区块地址(Logical Block Address, LBA)来作为磁盘最小单位（预设 `1LBA=512B`，就和旧的兼容了)，LBA从0开始编号。


GPT即 GUID partition table，是新一代的分区方式。以下是简要的GPT介绍：
- 首34个LBA区块记录分区内容
  - `LBA0` - MBR相容区块，存储了boot loader + GPT标识
  - `LBA1` - GPT表头记录，分区表及备份分区表的位置和大小，以及GPT校验码
  - `LBA2-33` - 分区表，32个LBA，即`32*4=128`个分区(每个都类似MBR的分区表因此是*4)；每个分区记录分配了`64bits`记录起止扇区(*注意这里是扇区不是柱面，分区更细*)，最大可达`8ZB`的分区
- 末33个LBA区块做备份
- GPT没有了主、延伸、逻辑分区的概念
> 疑问：GPT的boot loader被扩充了吗？how？也许后面会讲到吧。

## 分区指令

`fdisk` 不适用于GPT
`gdisk`, `parted`

> 后面会再更细致地进行分区学习，指令再补充

## 开机流程

这里要讨论的就是开机流程，包括BIOS、UEFI以及Boot Loader。首先简单介绍一下前部分的开机流程。

1. 开机主动执行BIOS、UEFI进行上电自检，同时从硬盘中读取boot loader
2. 执行Boot Loader，载入内核，启动操作系统
3. ...

> 开机流程可以专门开一个记录做深入补充，这部分可以在做了MIT6.828的相关实验以后再搞。

### BIOS

- BIOS是写入到主板的固件(firmware)，是计算机开机时执行的第一个程序。
- BIOS可识别MBR格式的硬盘
- 对于GPT硬盘，BIOS也可通过`LBA0`(MBR相容区块)的boot loader来开机。但其实它无法识别GPT，要交给boot loader去认。如果boot loader也无法识别GPT就无法开机(例如Windows XP)
- *(此段感觉怪怪的)* 由于LBA0 仅提供第一阶段的启动引导程序，因此如果使用如 grub 的启动引导程序的话，就要额外分配一个`BIOS boot`的分区，用于放置后阶段的启动引导程序。在CentOS 当中，这个分区通常占用2MB 左右而已。

### 启动引导程序

BIOS是“刻”在主板上的，MBR是硬盘本身就支持的功能，而Boot Loader则是在装系统时系统写上去的一个**软件**。由于MBR只分配了`446B`给Boot Loader，因此这个软件肯定是**简洁而美**的。它提供以下功能：
- 选择系统的菜单（多系统的前提）
- 载入系统内核
- 将控制权转给其他的Boot Loader
  - 不仅有MBR可以装Boot Loader，每个硬盘分区其实都有一个boot sector来支持多系统。


**“如果要安装多重开机，最好先安装Windows再安装Linux” 的原因：**
- Linux安装时，其Boot Loader
  - 可以选择安装在MBR，也可以是分区的boot sector
  - 可以设置开机菜单，转移boot loader的控制权
- Windows安装时，其Boot Loader
  - 直接粗暴覆盖MBR
  - 不提供开机菜单

当然上述情况也不是绝对，可以采取救援MBR的方式达成先装Linux再装Windows，这博主是折腾过了，可以看看这篇[在VBox安装双系统](virtual_double_sys.md)的博客。

**Example 一个实践的栗子：**

在还不懂开机流程的某一天，我将Ubuntu装到了一整个U盘上，然后发现开机进入Grub2（当时还不认识Grub2这个boot loader），而且也无法启动Ubuntu，更无法进入Windows，然后惊慌的我赶紧将U盘内容格式化，又能直接进入Windows了。

当时重启后进入Grub2而不是直接进入Ubuntu说明**Ubuntu并没有安装成功**，因为虽然U盘的MBR的确安装上了Grub2作为Boot Loader，但是在检测到只有一个系统的时候，Grub2应该会直接引导开启操作系统。现在看来安装失败有可能是硬盘格式问题。

然后格式化U盘以后又能重新启动Windows，是因为装Windows的硬盘从头到尾没有动过(包括其MBR), 因此虽然U盘启动的优先级高于Windows(自己的设置)，但由于系统没能在U盘的MBR找到boot loader，因此转而向装有Windows的硬盘寻求启动了。

> 有时间可以再尝试一次

### UEFI

UEFI (Unified Extensible Firmware Interface)，更高级的BIOS。

- 区别于BIOS使用汇编开发，使用C语言开发，更像OS，可图形化
- 可识别GPT硬盘
- 轮训方式管理硬件资源，因此不够优
- 增加安全启动机制，比BIOS更安全

> 没看懂的内容：
> - 与BIOS模式相比，虽然UEFI可以直接取得GPT的分割表，不过最好依旧拥有BIOS boot的分割槽支援（是怕grub把GPT当MBR来处理了吗）
> - 为了与windows相容，并且提供其他第三方厂商所使用的UEFI应用程式储存的空间，你必须要格式化一个`vfat`的档案系统，大约提供512MB到1G左右的容量，以让其他UEFI执行较为方便。
> - 由于UEFI 已经克服了BIOS 的1024 磁柱的问题，因此你的开机管理程式与核心放置在磁碟开始的前2TB 位置内即可(???)。
> - 加上之前提到的`BIOS boot` 以及`vfat`，基本上你的`/boot` 目录几乎都是`/dev/sda3` 之后的号码了（`boot`又是干嘛的来着）
> - 与以前熟悉的分割状况已经不同， /boot 不再是/dev/sda1 啰！很有趣吧！(有趣个头啊，以前哪里讲过了)


## 分区挂载规划（重要）

像 C盘, D盘 等是典型的 Windows 分区方式，但就 Linux 的哲学 “一切都是文件” 而言，硬盘所分的分区也应该是以文件的形式存在。

### 目录树与挂载原理
一切从`/`开始
TODO：从fs.md引回来

### 规划
TODO：关于不要分配`/boot`的，以及`/efi`的作用