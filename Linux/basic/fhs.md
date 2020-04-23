# FHS 

FHS 全称Filesystem Hierarchy Standard 文件系统层级结构标准。Linux内核在初始化时会生成根目录`/`为起点的文件目录树，而 FHS 作为Linux的工业标准规范了目录树的基本结构，当然各个发行版在这个基础上还有额外增加的内容。

[FHS官方文档](http://www.pathname.com/fhs/)

总分类：
- `/` (root) 与开机系统有关；
- `/usr` (unix software resource) 与软件安装/执行有关；
- `/var` (variable) 与系统运作过程有关

接下来介绍各个目录具体的作用和意义。

## `/`

FHS建议根目录越小越好，且安装内容最好不要与根目录放在同一个分区内，减少根目录的空间占用。原因是根目录最重要，和开机/还原/系统修复等动作密切相关。

- FHS规定`/`下必须存在的次级目录: `/boot`, `/bin`, `/dev`, `/etc`, `/lib`, `/media`, `/mnt`, `/opt`, `/run`, `/sbin`, `/srv`, `/tmp`, **`/usr`, `/var`**
- FHS规定`/`中建议存在的次级目录: `/home`, `/lib<?>`, `/root`
- 不在FHS标准中的次级目录: `/lost+found`, `/proc`, `/sys`

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
### `/media`
### `/mnt`
### `/opt`
### `/run`
### `/sbin`
### `/srv`
### `/tmp`


## `/usr`

放置的资料属于**可分享的**与**不可变动的**(shareable, static)，软件安装相关

must: `./bin`, `./lib`, `./local`, `./sbin`, `./share`, 

recommended: `./games`, `./include`, `./libexec`, `./lib<?>`,  `./src`

## `/var`

经常变动的文件目录

must: `./cache`, `./lib`, `./lock`, `./log`, `./mail`, `./run`, `./spool`

## 其他