# 虚拟机搭建CentOS

使用 Ubuntu 18.04 LTS + QEMU 进行搭建

首先新建了一个30G的虚拟磁盘

直接运行

panic : 没分配内存

分配内存运行

开启kvm加速

<hr>

创建img文件

1. qemu-img create -f raw /opt/ol6.img 40G



安装系统到img文件上

2. qemu-system-x86_64 -enable-kvm -m 2048 -hda /opt/ol6.img -cdrom /opt/ubuntu-Server-x86_64-dvd.iso -boot d -vnc 10.10.1.1:1



启动qemu

3. qemu-system-x86_64 -enable-kvm -m 4096 -drive file=/opt/ol6.img -vnc 10.10.1.1:1
--------------------- 
作者：mounter625 
来源：CSDN 
原文：https://blog.csdn.net/mounter625/article/details/75027914 
版权声明：本文为博主原创文章，转载请附上博文链接！

<hr>

无法 kvm , 抛弃 qemu, 使用Virtual-Box

磁盘分割

- 标准分区：/dev/vda1之类的分区
- LVM：这是一种可以弹性增加/削减文件系统容量的设备
- LVM紧张供应：LVM的进阶版！与传统LVM直接分配固定的容量不同，这个『LVM紧张供应』的项目，可以让你在使用多少容量才分配多少磁盘容量给你


- BIOS Boot：就是GPT分割表可能会使用到的项目，若你使用MBR分割，那就不需要这个项目了！