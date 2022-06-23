#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/poll.h>

MODULE_LICENSE("GPL v2");

#define GFIFO_SIZE 0x1000
#define FIFO_CLEAR 0x1
#define GFIFO_MAJOR 231

static int gfifo_major = GFIFO_MAJOR;
module_param(gfifo_major, int, S_IRUGO);

struct gfifo_dev
{
   struct cdev cdev;
   unsigned int current_len;
   unsigned char mem[GFIFO_SIZE];
   struct mutex mutex;
   wait_queue_head_t r_wait;
   wait_queue_head_t w_wait;
};

struct gfifo_dev *gfifo_devp;

static int gfifo_open(struct inode *inode, struct file *filp)
{
   filp->private_data = gfifo_devp;
   return 0;
}

static int gfifo_release(struct inode *inode, struct file *filp)
{
   return 0;
}

static long gfifo_ioctl(struct file *filp, unsigned int cmd,
                        unsigned long arg)
{
   struct gfifo_dev *dev = filp->private_data;

   switch (cmd)
   {
   case FIFO_CLEAR:
      mutex_lock(&dev->mutex);
      dev->current_len = 0;
      memset(dev->mem, 0, GFIFO_SIZE);
      mutex_unlock(&dev->mutex);

      printk(KERN_INFO "gfifo is set to zero\n");
      break;

   default:
      return -EINVAL;
   }
   return 0;
}

static unsigned int gfifo_poll(struct file *filp, poll_table *wait)
{
   unsigned int mask = 0;
   struct gfifo_dev *dev = filp->private_data;

   mutex_lock(&dev->mutex);

   poll_wait(filp, &dev->r_wait, wait);
   poll_wait(filp, &dev->w_wait, wait);

   if (dev->current_len != 0)
   {
      mask |= POLLIN | POLLRDNORM;
   }

   if (dev->current_len != GFIFO_SIZE)
   {
      mask |= POLLOUT | POLLWRNORM;
   }

   mutex_unlock(&dev->mutex);
   return mask;
}

static ssize_t gfifo_read(struct file *filp, char __user *buf,
                          size_t count, loff_t *ppos)
{
   int ret;
   struct gfifo_dev *dev = filp->private_data;
   DECLARE_WAITQUEUE(wait, current);

   mutex_lock(&dev->mutex);
   add_wait_queue(&dev->r_wait, &wait);

   while (dev->current_len == 0)
   {
      if (filp->f_flags & O_NONBLOCK)
      {
         ret = -EAGAIN;
         goto out;
      }
      __set_current_state(TASK_INTERRUPTIBLE);
      mutex_unlock(&dev->mutex);

      schedule();
      if (signal_pending(current))
      {
         ret = -ERESTARTSYS;
         goto out2;
      }

      mutex_lock(&dev->mutex);
   }

   if (count > dev->current_len)
      count = dev->current_len;

   if (copy_to_user(buf, dev->mem, count))
   {
      ret = -EFAULT;
      goto out;
   }
   else
   {
      memcpy(dev->mem, dev->mem + count, dev->current_len - count);
      dev->current_len -= count;
      printk(KERN_INFO "read %lu bytes(s),current_len:%u\n", count, dev->current_len);

      wake_up_interruptible(&dev->w_wait);

      ret = count;
   }
out:
   mutex_unlock(&dev->mutex);
out2:
   remove_wait_queue(&dev->r_wait, &wait);
   set_current_state(TASK_RUNNING);
   return ret;
}

static ssize_t gfifo_write(struct file *filp, const char __user *buf,
                           size_t count, loff_t *ppos)
{
   struct gfifo_dev *dev = filp->private_data;
   int ret;
   DECLARE_WAITQUEUE(wait, current);

   mutex_lock(&dev->mutex);
   add_wait_queue(&dev->w_wait, &wait);

   while (dev->current_len == GFIFO_SIZE)
   {
      if (filp->f_flags & O_NONBLOCK)
      {
         ret = -EAGAIN;
         goto out;
      }
      __set_current_state(TASK_INTERRUPTIBLE);

      mutex_unlock(&dev->mutex);

      schedule();
      if (signal_pending(current))
      {
         ret = -ERESTARTSYS;
         goto out2;
      }

      mutex_lock(&dev->mutex);
   }

   if (count > GFIFO_SIZE - dev->current_len)
      count = GFIFO_SIZE - dev->current_len;

   if (copy_from_user(dev->mem + dev->current_len, buf, count))
   {
      ret = -EFAULT;
      goto out;
   }
   else
   {
      dev->current_len += count;
      printk(KERN_INFO "written %lu bytes(s),current_len:%u\n", count, dev->current_len);

      wake_up_interruptible(&dev->r_wait);

      ret = count;
   }

out:
   mutex_unlock(&dev->mutex);
out2:
   remove_wait_queue(&dev->w_wait, &wait);
   set_current_state(TASK_RUNNING);
   return ret;
}

static const struct file_operations gfifo_fops = {
    .owner = THIS_MODULE,
    .read = gfifo_read,
    .write = gfifo_write,
    .unlocked_ioctl = gfifo_ioctl,
    .poll = gfifo_poll,
    .open = gfifo_open,
    .release = gfifo_release,
};

static void gfifo_setup_cdev(struct gfifo_dev *dev, int index)
{
   int err, devno = MKDEV(gfifo_major, index);

   cdev_init(&dev->cdev, &gfifo_fops);
   dev->cdev.owner = THIS_MODULE;
   err = cdev_add(&dev->cdev, devno, 1);
   if (err)
      printk(KERN_NOTICE "Error %d adding gfifo%d", err, index);
}

static int __init gfifo_init(void)
{
   int ret;
   dev_t devno = MKDEV(gfifo_major, 0);

   if (gfifo_major)
      ret = register_chrdev_region(devno, 1, "gfifo");
   else
   {
      ret = alloc_chrdev_region(&devno, 0, 1, "gfifo");
      gfifo_major = MAJOR(devno);
   }
   if (ret < 0)
      return ret;

   gfifo_devp = kzalloc(sizeof(struct gfifo_dev), GFP_KERNEL);
   if (!gfifo_devp)
   {
      ret = -ENOMEM;
      goto fail_malloc;
   }

   gfifo_setup_cdev(gfifo_devp, 0);

   mutex_init(&gfifo_devp->mutex);
   init_waitqueue_head(&gfifo_devp->r_wait);
   init_waitqueue_head(&gfifo_devp->w_wait);

   return 0;

fail_malloc:
   unregister_chrdev_region(devno, 1);
   return ret;
}
module_init(gfifo_init);

static void __exit gfifo_exit(void)
{
   cdev_del(&gfifo_devp->cdev);
   kfree(gfifo_devp);
   unregister_chrdev_region(MKDEV(gfifo_major, 0), 1);
}
module_exit(gfifo_exit);
