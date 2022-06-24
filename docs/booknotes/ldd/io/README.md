# 驱动中的I/O

> [《Linux设备驱动程序》 - 第三版 ](https://1drv.ms/b/s!AkcJSyT7tq80d6mS7pO12K6Qb30)的第6章，[《Linux设备驱动开发详解》 - 宋宝华 ](https://1drv.ms/b/s!AkcJSyT7tq80eFABEg8fSOajqHk)的第8，9章，的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/io/code)中找到

## 休眠和唤醒

对Linux设备驱动程序，进入休眠状态需要注意：

* 不要在原子上下文(拥有锁)中进入休眠
* 被唤醒后需要重新检查等待条件

唤醒时需要知道哪些事件序列会结束休眠，因此需要维护一个等待队列：队列头`wait_queue_head_t`和队列元素`wait_queue`。进程可通过`wait_event`宏进入休眠，通过`wake_up`唤醒等待队列中的进程：
```cpp
// 动态初始化等待队列头
wait_queue_head_t my_queue;
init_waitqueue_head(&my_queue);

// 在等待队列中，添加/移除等待队列元素
void add_wait_queue(wait_queue_head_t *q, wait_queue_t *wait);
void remove_wait_queue(wait_queue_head_t *q, wait_queue_t *wait);

// 等待队列的宏操作
//  定义并初始化名为`my_queue`的等待队列头
DECLARE_WAIT_QUEUE_HEAD(my_queue);
//  定义等待队列元素`wait`, 并用任务task初始化此等待队列元素
DECLARE_WAITQUEUE(wait, task);

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

下图显示了，`wait_queue_head_t`，`wait_queue`和`task_struct`之间的关系：

![wait_queue](./images/wait_queue.png)

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

## 阻塞I/O

驱动程序在某些情况下，可能无法立即完成用户的请求。例如，

* 当数据不可用时，用户调用`read`
* 当输出缓冲区已满，或设备还未准备好接受数据时，用户调用`write`

在这种情况下，驱动程序默认会阻塞该进程，将其置入休眠状态直到请求可继续。

### 驱动实例

[例子"gfifo"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/io/code/gfifo)，创建了一个支持阻塞访问的FIFO设备。当FIFO中没有数据时，`read`请求阻塞，此时向FIFO中写入数据能重新唤醒`read`请求进程。当FIFO满数据时，`write`请求阻塞，此时从FIFO中读出数据能重新唤醒`write`请求进程。

```cpp title="gfifo.c" hl_lines="7 8 15 18 22 24 36 39 40 49 52 56 58 69 72 73"
struct gfifo_dev
{
    struct cdev cdev;
    unsigned int current_len;
    unsigned char mem[GFIFO_SIZE];
    struct mutex mutex;
    wait_queue_head_t r_wait;
    wait_queue_head_t w_wait;
};

static ssize_t gfifo_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
    int ret;
    struct gfifo_dev *dev = filp->private_data;
    DECLARE_WAITQUEUE(wait, current);

    mutex_lock(&dev->mutex);
    add_wait_queue(&dev->r_wait, &wait);

    while (dev->current_len == 0)
    {
        set_current_state(TASK_INTERRUPTIBLE);
        mutex_unlock(&dev->mutex);
        schedule();
        mutex_lock(&dev->mutex);
    }

    if (count > dev->current_len)
        count = dev->current_len;

    copy_to_user(buf, dev->mem, count);
    memcpy(dev->mem, dev->mem + count, dev->current_len - count);
    dev->current_len -= count;
    printk(KERN_INFO "read %lu bytes(s),current_len:%u\n", count, dev->current_len);

    wake_up_interruptible(&dev->w_wait);

    ret = count;
    remove_wait_queue(&dev->r_wait, &wait);
    set_current_state(TASK_RUNNING);
    mutex_unlock(&dev->mutex);
    return ret;
}

static ssize_t gfifo_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
    struct gfifo_dev *dev = filp->private_data;
    int ret;
    DECLARE_WAITQUEUE(wait, current);

    mutex_lock(&dev->mutex);
    add_wait_queue(&dev->w_wait, &wait);

    while (dev->current_len == GFIFO_SIZE)
    {
        set_current_state(TASK_INTERRUPTIBLE);
        mutex_unlock(&dev->mutex);
        schedule();
        mutex_lock(&dev->mutex);
    }

    if (count > GFIFO_SIZE - dev->current_len)
        count = GFIFO_SIZE - dev->current_len;

    copy_from_user(dev->mem + dev->current_len, buf, count);
    dev->current_len += count;
    printk(KERN_INFO "written %lu bytes(s),current_len:%u\n", count, dev->current_len);

    wake_up_interruptible(&dev->r_wait);

    ret = count;
    remove_wait_queue(&dev->w_wait, &wait);
    set_current_state(TASK_RUNNING);
    mutex_unlock(&dev->mutex);
    return ret;
}
```

### 用户空间测试

[例子"gfifo_user_block"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/ldd/io/code/gfifo_user_block)是对上述"gfifo"驱动阻塞I/O行为的在用户空间的测试。其中，一个进程对"/dev/gfifo"设备进行读操作，另一个进程在一定时间之后对"/dev/gfifo"设备进行写操作。读进程会一直阻塞，直到写进程完成写操作。

```cpp title="GFIFO Block I/O Test"
#define GFIFO_DEV "/dev/gfifo"
const char data[] = "Hello, global gfifo\n";

void sleep_write()
{
   printf("[Write Process] Start\n");

   int fd = open(GFIFO_DEV, O_RDWR);
   assert(fd > 0);

   sleep(1);
   printf("[Write Process] Start to write after sleep\n");
   int n = write(fd, data, sizeof(data));
   printf("[Write Process] Written %d bytes to the device\n", n);

   close(fd);

   printf("[Write Process] End\n");
}

void block_read()
{
   printf("[Read Process] Start\n");

   int fd = open(GFIFO_DEV, O_RDWR);
   assert(fd > 0);

   char buf[1024];
   int n = read(fd, buf, sizeof(data));
   printf("[Read Process] Read %d bytes from the device: %s\n", n, buf);

   close(fd);

   printf("[Read Process] End\n");
}

int main(int argc, char **argv)
{
   pid_t pid = fork();
   if (pid == 0)
   {
      // child process
      sleep_write();
      return 0;
   }
   // parent process
   block_read();
   wait(NULL);
   return 0;
}
```
```bash
>./main
[Read Process] Start
[Write Process] Start
[Write Process] Start to write after sleep
[Write Process] Written 21 bytes to the device
[Write Process] End
[Read Process] Read 21 bytes from the device: Hello, global gfifo

[Read Process] End
```

## 非阻塞I/O
