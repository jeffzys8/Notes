# 文件系统

## 原理

> 大致过一下文件系统的原理

superblock: 整个文件系统的inode, block的总量、使用量、剩余量
inode: 文件的权限和属性
data block: 具体的文件信息

每个文件占用一个inode，同时记录其所在block #
大文件会占用多个block

索引式文件系统: inode记录全部block#
FAT格式: 没有inode，链表连接block; 硬盘重组?

标准Linux的文件系统 Ext2
区分多个block group，每个group有自己的一套 inode/block/superblock; 文件系统的最开头有个boot sector，而不用占用MBR，使得制作多重开机成为可能

inode记录block#的区域定义为12个直接索引，一个2级索引，一个3级，还有一个4级索引; 多级索引在block中进一步记录。

目录树? 对于每个目录，文件系统会分配1个inode和至少1个block，然后block中就能记录该目录下文件的inode号码(索引)了。

文件名不记录在inode中，而是在data中（准确来说只是记录在它的目录之中，而对于下面会讲到的硬链接，一个文件可在多个目录下有不同的文件名）。

将文件系统挂载到目录上。

VFS，Virtual Filesystem Switch. 用户无需知道每个partition的fs是什么，由内核提供的VFS帮我们去读写数据。

## 操作

`df`
列出文件系统整体的使用量；读取superblock内的信息，所以速度很快
- `-a` 列出包括`/proc`等系统特有的fs在内的所有fs
- `-h` 带单位
- `-i` 以inode数量显示

`du`
常用于显示某个目录下文件所占用量
- `-s` 列出总量，而不列出各个别的目录占用容量(??)
- `-a`
- `-h`
- `--depth n` 递归深度

`ln`
软连接和硬连接
引用计数
硬链接不能跨fs
强烈建议不要link目录（容易出现死循环）
软链接 = 符号链接，就是类似Windows的快捷方式