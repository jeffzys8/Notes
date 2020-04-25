# Linux目录树标准 - FHS 

FHS 全称Filesystem Hierarchy Standard 文件系统层级结构标准。Linux内核在初始化时会生成根目录`/`为起点的文件目录树，而 FHS 作为Linux的工业标准规范了目录树的基本结构，当然各个发行版在这个基础上还有额外增加的内容。

[FHS官方文档](http://www.pathname.com/fhs/)

总分类：
- `/` (root) 与开机系统有关；
- `/usr` (unix software resource) 与软件安装/执行有关；
- `/var` (variable) 与系统运作过程有关

接下来介绍各个目录具体的作用和意义。

## `/`

FHS建议根目录越小越好，且安装内容最好不要与根目录放在同一个分区内，减少根目录的空间占用。原因是根目录最重要，和开机/还原/系统修复等动作密切相关。

- must: `/boot`, `/bin`, `/dev`, `/etc`, `/lib`, `/media`, `/mnt`, `/opt`, `/run`, `/sbin`, `/srv`, `/tmp`, **`/usr`, `/var`**
- recommended: `/home`, `/lib<?>`, `/root`
- not included: `/lost+found`, `/proc`, `/sys`

### `/boot`

主要存放：
- boot loader
  - grub2则在 `/boot/grub2` 中
- 内核相关(vmlinuz, initramfs) (之后开机流程会再碰到)
### `/bin`
放一些特殊的可执行文件 - 在**单人维护模式**下还能够被操作的指令。 

在/bin底下的指令可以被root与一般帐号所使用，主要有：cat, chmod, chown, date, mv, mkdir, cp, bash等等常用的指令。

### `/dev`

存放各种设备(文件形式)

设备分类：
- 字符设备（线性：键盘鼠标）
- 块设备（随机：磁盘）
  
比要重要的设备：`/dev/null`, `/dev/zero`, `/dev/tty` , `/dev/loop*`, `/dev/sd*`

### `/etc`

系统最主要的配置文件基本都在这里，不能放二进制文件。通常用户都可`read`，但一般只有`root`可以`write`。

### `/lib`

- 开机时会用到的函数库
- 为/bin 和 /sin 提供函数库
- `/lib/modules` 存放可更换的驱动程序

### `/media`

挂载点：便携性设备（光盘软盘）

### `/mnt`

临时挂载点

> 但其实本质来说怎么挂载没有严格限制，哪里都可以挂，预设的文件夹只是作为参考。

### `/opt`

附加的软件包，一般用来装一些第三方软件包，但其实现在很少软件包会往opt放了，主要都放到`/usr/local`

### `/run`

存放开机后产生的各种数据，包括PID。早期存在 `/var/run`，现在软连接到`/run`了。所存数据使用内存，不占用硬盘容量且性能较好。

- 锁 `/run/lock`

### `/sbin`

系统管理程序，`root`才能调用的开机、修复、还原指令。


### `/srv`

网络服务资料存放点，比如`/srv/www`

### `/tmp`

临时文件目录，程序两次启动间不能认为这里的数据有保存（断电可能被擦除）

### * `/home`

普通用户的家目录

### * `/lib<?>`

存放其他格式的函数库，通常是 `lib64`，支持64位的函数

### * `/root`

`root`用户的家目录

### !`/lost+found`

ext2/ext3/ext4才有，/xfs无。文件系统发生错误时会存储一些资料。

### !`/proc`

同样也是个虚拟文件系统（存在内存）。记录内核、进程、各种设备的状态、网络状态等。

例如 `net.ipv4.ip_forward --> /proc/net/ipv4/ip_forward`

系统调优的关键入口

### !`/sys`

同样也是个虚拟文件系统（存在内存）。也是记录内核、硬件相关信息。linux 2.6 才出现。

提供比proc更为理想的访问内存数据的接口。/sys 导出内核数据比 /proc 更合理、设计更好。主要是为管理Linux设备提供统一模型的接口

同样也是系统调优的关键入口

[IBM参考](https://www.ibm.com/developerworks/cn/linux/l-cn-sysfs/)

## `/usr`

放置的资料属于**可分享的**与**不可变动的**(shareable, static)，软件安装相关。

所有distribution所预置的软件都会放这里，因此有点像Windows的`C:\Windows\(当中的一部份) + C:\Program files\`

- must: `./bin`, `./lib`, `./local`, `./sbin`, `./share`
- recommended: `./games`, `./include`, `./libexec`, `./lib<?>`,  `./src`

> 这个文件夹内容和根目录中重名的即类似的，不再赘述

### 软链接

- `/lib` --> `/usr/lib`
- `/lib64` --> `/usr/lib64`
- `/bin` --> `/usr/bin`
- `/sbin` --> `/usr/sbin`

### !`/usr/local`

又是一个类似 `/usr` 的层级目录，但这里放置的是非distribution预置的软件（也就是用户自己下载的）。

### !`/usr/share`

只读的文字资料

### *`/usr/src`

存放源码，比如Linux源码在 `/usr/src/linux`
> CentOS8怎么找不到这个文件夹

### *`/usr/libexec`

不常用的可执行文件/脚本，比如XWindow, gnome的操作脚本

### *`/usr/include`

一些C/C++头文件，安装某些应用时会用到
 
## `/var`

经常变动的文件目录，存放经常被修改的数据文件

must: `./cache`, `./lib`, `./lock`, `./log`, `./mail`, `./run`, `./spool`

### `/var/cache`

应用程序运行过程中的缓存数据

### `/var/lib`

各个程序运行过程中需要时用到的数据。在此目录下各自软件应该要有独立的目录。举例来说，MySQL的数据放置到`/var/lib/mysql/`

### `/var/lock`

设备锁，已软链接到 `/run/lock`

### `/var/log`

重要的日志文件目录

### `/var/mail`

邮件，软链接到 `/var/spool/mail`

### `/var/run`

运行中程序的PID，软链接到 `/run`

### `/var/spool`

储存等待被使用的**队列**数据，这些数据被使用完以后通常会被删除掉。比如mail和crontab