# 文件系统

## FHS 

Linux目录配置的依据--FHS; Filesystem Hierarchy Standard (FHS)标准

> 这里先把文件夹名字记录下来，以后在实践中逐渐积累经验，如果需要查阅信息可以

总分类：
- `/` (root) 与开机系统有关；
- `/usr` (unix software resource) 与软件安装/执行有关；
- `/var` (variable) 与系统运作过程有关

### /

FHS建议根目录越小越好

must: `/boot`, `/bin`, `/dev`, `/etc`, `/lib`, `/media`, `/mnt`, `/opt`, `/run`, `/sbin`, `/srv`, `/tmp`, **`/usr`, `/var`**

recommended: `/home`, `/lib<?>`, `/root`

not in FHS: `/lost+found`, `/proc`, `/sys`

### /usr

放置的资料属于**可分享的**与**不可变动的**(shareable, static)，软件安装相关

must: `./bin`, `./lib`, `./local`, `./sbin`, `./share`, 

recommended: `./games`, `./include`, `./libexec`, `./lib<?>`,  `./src`

### /var

经常变动的文件目录

must: `./cache`, `./lib`, `./lock`, `./log`, `./mail`, `./run`, `./spool`

## 文件系统的分区规划

**目录树的文件系统(挂载)**

Linux文件系统呈“目录树”的逻辑形式（见下图），所有的文件和文件夹都是由根目录 (`/`)衍生而来

![目录树](img/partition_tree.png)

逻辑形式如此，但前面的学习中我们知道文件都是存放在划分好的磁盘的，那目录树和硬盘空间又是怎么对应的呢？答案是 **挂载mount**

所谓的挂载就是**利用一个目录当成分区的进入点**，将磁盘分区的资料放置在该目录下；也就是说，进入该目录就可以读取该分区

> 实际上Windows也是采用的挂载的做法；但Windows的挂载常常就是 `C:` `D:` 这样的盘符形式，显得就好像是直接进入磁盘当中；而Linux的挂载更灵活，`/`一个挂载， `/home`又一个挂载, `/home/space`又一个挂载

**硬盘分区规划**

建议使用自定义分区，这样更有助于了解系统分区方式,并在日后熟练以后构造适合自己的分区

初次自定义分区，只需要分割成两个分区: `/` 和 `swap` 即可；当然建议预留一些分区作备份和日后再分区的使用。

进阶分区，给下列所需空间大且读写频繁的目录单独挂载磁盘分区，避免在这些目录Error时影响到根目录的资料：`/boot` `/home` `/var`；这些目录的具体含义和作用会在之后的笔记中讨论