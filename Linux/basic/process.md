# 进程管理

程序以文件形式存在硬盘等存储设备中。执行程序的时候，系统会根据执行的用户分配 用户/组 的权限，分配 PID，然后将程序载入内存成为 进程。

PPID 进程的父进程
父进程通过fork复制一个一模一样的子进程，使用新的PID。而后通过exec执行新的指令

daemon, 服务, 守护进程
crontab，由crontd守护进程扫描`/etc/crontab`执行

某个终端因为程序卡死时，可以切换其他终端`[Alt]+[F1~F7]`来杀掉进程恢复正常

## bash job control - 背景进程管理（自己bash运行的）

- `&` 直接将指令丢到背景去运行
  - 会显示job number 和 PID：`[1] 14432   <== [job number] PID`
  - 指令完成退出后输出：`[1]+ Done tar -zpcf /tmp/etc.tar.gz /etc`
- `[Ctrl+Z]` 暂停当前的工作并回到前景
  - 同样会输出job number
- `jobs` 观察目前的背景工作状态
  - `l` 列出PID
  - `r` 仅列出背景中running的
  - `s` 仅列出背景中stopped的
  - 显示 `+-` 号相当于栈顶的头两个
- `fg %jobnumber` 从栈顶取任务来前台跑
  - 不加jobn从栈顶取(`+`)
- `bg` 栈里拉去背景跑
- `kill -signal %jobnumber` 传信号（比如杀掉）
  - `-9` 强制杀掉（最常用）
  - `-l` 列出可用信号
  - `-15` 尝试正常杀掉
  - `-1` reload?
  - `-2` Ctrl+Z
  - kill还有很多其他的用处, `man -7 kill`
- **背景程序 != 系统背景，就是终端背景，如果想退出还运行的话要用`nohup`**