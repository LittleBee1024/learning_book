# 驱动中的I/O

> [《Linux设备驱动程序》 - 第三版 ](https://1drv.ms/b/s!AkcJSyT7tq80d6mS7pO12K6Qb30)的第6章，[《Linux设备驱动开发详解》 - 宋宝华 ](https://1drv.ms/b/s!AkcJSyT7tq80eFABEg8fSOajqHk)的第8，9章，的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/io/code)中找到

## 阻塞与非阻塞I/O

### 休眠和唤醒

对Linux设备驱动程序，进入休眠状态需要注意：

* 不要再原子上下文(拥有锁)中进入休眠
* 被唤醒后需要重新检查等待条件

唤醒时需要知道哪些事件序列会结束休眠，因此需要维护一个等待队列`wait_queue_head_t`。进程可通过`wait_event`宏进入休眠，通过`wake_up`唤醒等待队列中的进程：
```cpp
// 动态初始化等待队列头
wait_queue_head_t my_queue;
init_waitqueue_head(&my_queue);

// 在等待队列中，添加/移除任务`wait_queue_t`
void add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait);
void remove_wait_queue(wait_queue_head_t *q, wait_queue_t *wait);

// 等待队列的宏操作
// 使用宏初始化名为name的等待队列头
DECLARE_WAIT_QUEUE_HEAD(name);
// 初始化名为name的等待队列头，同时添加任务tsk
DECLARE_WAITQUEUE(name, tsk);

// 屏蔽中断休眠
//  queue - 等待队列头
//  condition - 休眠条件，休眠/唤醒时都需要检查
wait_event(queue, condition)
// 不屏蔽中断休眠，返回非零值表示休眠被某个信号中断
wait_event_interruptible(queue, condition)

// 唤醒等待队列中的所有任务
void wake_up(wait_queue_head_t *queue);
// 唤醒等待队列中处于`TASK_INTERRUPTIBLE`的任务
void wake_up_interruptible(wait_queue_head_t *queue);
```

[例子"sleepy"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/io/code/sleepy)，创建了一个"/dev/sleepy"设备。如果某一进程在读取此设备时，会进入休眠状态。通过在另一个进程写此设备，可唤醒休眠的读设备进程。一次只能唤醒一个进程，如果存在多个休眠的读进程，需要多次写入设备。

```cpp title="sleepy.c" hl_lines="1 7 17"
static DECLARE_WAIT_QUEUE_HEAD(wq);
static int flag = 0;

static ssize_t sleepy_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
   printk(KERN_DEBUG "process %i (%s) going to sleep\n", current->pid, current->comm);
   wait_event_interruptible(wq, flag != 0);
   flag = 0;
   printk(KERN_DEBUG "awoken %i (%s)\n", current->pid, current->comm);
   return 0;
}

static ssize_t sleepy_write(struct file *filp, const char __user *buf, size_t count, loff_t *pos)
{
   printk(KERN_DEBUG "process %i (%s) awakening the readers...\n", current->pid, current->comm);
   flag = 1;
   wake_up_interruptible(&wq);
   return count;
}
```
