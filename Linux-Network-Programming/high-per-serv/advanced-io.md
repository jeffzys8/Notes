# 高级I/O

pipe; 创建管道实现进程间通信;`fd[2]`; 单向, 双工需要两个pipe

dup, dup2; 重定向

readv, writev; fd分散读入内存, 分散内存集中写入fd

sendfile; fd间传递数据，零拷贝(无kernel<->user拷贝);

mmap; 文件直接映射到内存空间; munmap释放

splice; 也是类似sendflie的零拷贝操作

tee; 在两个管道fd间复制数据(零拷贝)，源文件数据还在(有点Linux的tee的感觉)

fcntl; 对fd的控制操作