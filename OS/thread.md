# 线程

> focus on Linux

## **线程模型**

内核线程(轻量级进程)，运行在内核空间，内核调度
用户线程，用户空间，由线程库调度

内核线程相当于用户线程的“运行容器”。线程实现有三种模式：完全用户空间实现、完全由内核调度、双层调度。
- 完全用户空间实现，对于内核来说最小调度单位为进程，因为内核不知道有线程存在。M个用户空间线程对应1个内核线程(即进程本身)。
不占用额外内核资源，也无需内核干预(很快)，但是一个进程的多个线程无法运行在不同CPU上。(早期伯克利UNIX线程)
- 完全由内核调度，内核控制线程的创建和调度，线程库(用户空间)无需管理任务，在线程管理和同步上需要占用较多资源。用户线程数=内核线程数。（LinuxThreads和NPTL都是这样实现，现代Linux默认使用线程库是NPTL)
- 双层调度模式，前两者的结合

Linux线程库，采用1:1模型（不同进程的线程能共享互斥锁这么厉害~）

pthread_create; pthread_exit; pthread_join; pthread_cancel;
pthread_attr_t

> 插一句：需要复习一下有限状态机，提了好多次了

线程的TCB内容(不确定是不是叫TCB)：
- 程序计数器
- 寄存器
- 堆栈(每个线程自己会调用不同的函数，所以要记录)
- 状态
- **没有** 地址空间、全局变量、fd、子进程、定时器、信号、账户信息，**因为这些是进程的**

线程的“难题”
- fork时是否要复制线程
- 如果复制了，父进程某个线程的read是否要让子进程同样的线程一起阻塞
- 地址空间增大时发生了线程切换，然后新运行线程也请求了扩大空间?

## 线程同步

竞争条件; 互斥; 临界区(共享内存访问的程序片段)

### 忙等待互斥方案(自旋锁)

- 屏蔽中断，避免进程切换; 有饿死风险; 多CPU系统无效; 对内核很有效(how)
- 严格轮转法，两个线程轮流转；一个在非临界区运行得慢会阻塞另一个进入临界区
```c
// thread A
while(true) {
    while(turn != 0);
    critical_region();
    turn = 1;
}

// thread B
while(true) {
    while(turn != 1);
    critical_region();
    turn = 0;
}
```
- Peterson解法
```c
#define N 2     //进程数为2
int turn;       //现在轮到哪个进程？
int interested[N];  //初始化置为false，即没有在临界区等待读写共享数据的
 
void enter_region(int process) //进入临界区
{
     turn = process;
     int other = 1 - turn; //另一个进程
     interested[turn] = true;
     while(turn == process && interested[other] == true)
                ; //一直循环，直到other进程退出临界区
}
 
void leave_region(int process)
{
     interested[process] = false;
}
```
- TSL; 测试并加锁(锁总线，和屏蔽中断相比对多cpu有效) //TODO 
- XCSG; // TODO


### 睡眠与唤醒

- sleep: 阻塞某个进程的系统调用 (UNIX中为`pause(void)`信号)
- wakeup(pid): 唤醒某个进程 (UNIX中可为`alarm(int seconds)`信号)

生产者消费者(有界缓冲区)问题
> 感觉逻辑不完备啊 《现代操作系统》 page73


### 信号量、互斥量

PV操作

互斥量 = 二元信号量
```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex); // 非阻塞
int pthread_mutex_unlock(pthread_mutex_t *mutex);
```

TSL很好实现互斥量。在获取锁失败时直接阻塞并交给其他进程调度。在下次轮到它时再次检测锁可不可用。

```c
int pthread_mutex_timedlock()
```

### 读写锁

读模式加锁、写模式加锁、不加锁
```c
int pthread_rwlock_init(); // 必须初始化
int pthread_rwlock_destroy(); // 使用完必须销毁

int pthread_rwlock_rdlock();
int pthread_rwlock_wrlock();
int pthread_rwlock_unlock();
```

### 条件变量

> to be continued;

### 屏障
> to be continued;

## 死锁
> to be continued;

哲学家问题

死锁四个条件

银行家算法