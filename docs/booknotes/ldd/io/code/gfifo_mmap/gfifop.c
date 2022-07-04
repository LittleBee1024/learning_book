#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL v2");

// GFIFOP_ORDER define the total size: PAGE_SIZE * (2 ^ GFIFOP_ORDER)
// This driver only supports zero order
#define GFIFOP_ORDER 0
#define FIFO_CLEAR 0x1
#define GFIFOP_MAJOR 230

static int gfifop_major = GFIFOP_MAJOR;
module_param(gfifop_major, int, S_IRUGO);

struct gfifop_dev
{
   struct cdev cdev;
   unsigned int current_len;
   size_t total_size;
   unsigned char *mem;
   struct mutex mutex;
   wait_queue_head_t r_wait;
   wait_queue_head_t w_wait;
   struct fasync_struct *async_queue;
};

struct gfifop_dev *gfifop_devp;

static int gfifop_fasync(int fd, struct file *filp, int mode)
{
   struct gfifop_dev *dev = filp->private_data;
   return fasync_helper(fd, filp, mode, &dev->async_queue);
}

static int gfifop_open(struct inode *inode, struct file *filp)
{
   filp->private_data = gfifop_devp;
   return 0;
}

static int gfifop_release(struct inode *inode, struct file *filp)
{
   gfifop_fasync(-1, filp, 0);
   return 0;
}

static long gfifop_ioctl(struct file *filp, unsigned int cmd,
                         unsigned long arg)
{
   struct gfifop_dev *dev = filp->private_data;

   switch (cmd)
   {
   case FIFO_CLEAR:
      mutex_lock(&dev->mutex);
      dev->current_len = 0;
      memset(dev->mem, 0, dev->total_size);
      mutex_unlock(&dev->mutex);

      printk(KERN_INFO "gfifop is set to zero\n");
      break;

   default:
      return -EINVAL;
   }
   return 0;
}

static unsigned int gfifop_poll(struct file *filp, poll_table *wait)
{
   unsigned int mask = 0;
   struct gfifop_dev *dev = filp->private_data;

   mutex_lock(&dev->mutex);

   poll_wait(filp, &dev->r_wait, wait);
   poll_wait(filp, &dev->w_wait, wait);

   if (dev->current_len != 0)
   {
      mask |= POLLIN | POLLRDNORM;
   }

   if (dev->current_len != dev->total_size)
   {
      mask |= POLLOUT | POLLWRNORM;
   }

   mutex_unlock(&dev->mutex);
   return mask;
}

static ssize_t gfifop_read(struct file *filp, char __user *buf,
                           size_t count, loff_t *ppos)
{
   int ret;
   struct gfifop_dev *dev = filp->private_data;
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
      set_current_state(TASK_INTERRUPTIBLE);
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

static ssize_t gfifop_write(struct file *filp, const char __user *buf,
                            size_t count, loff_t *ppos)
{
   struct gfifop_dev *dev = filp->private_data;
   int ret;
   DECLARE_WAITQUEUE(wait, current);

   mutex_lock(&dev->mutex);
   add_wait_queue(&dev->w_wait, &wait);

   while (dev->current_len == dev->total_size)
   {
      if (filp->f_flags & O_NONBLOCK)
      {
         ret = -EAGAIN;
         goto out;
      }
      set_current_state(TASK_INTERRUPTIBLE);

      mutex_unlock(&dev->mutex);

      schedule();
      if (signal_pending(current))
      {
         ret = -ERESTARTSYS;
         goto out2;
      }

      mutex_lock(&dev->mutex);
   }

   if (count > dev->total_size - dev->current_len)
      count = dev->total_size - dev->current_len;

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

      if (dev->async_queue)
      {
         kill_fasync(&dev->async_queue, SIGIO, POLL_IN);
         printk(KERN_DEBUG "%s kill SIGIO\n", __func__);
      }

      ret = count;
   }

out:
   mutex_unlock(&dev->mutex);
out2:
   remove_wait_queue(&dev->w_wait, &wait);
   set_current_state(TASK_RUNNING);
   return ret;
}

void gfifop_vma_open(struct vm_area_struct *vma)
{
   printk(KERN_INFO "GFIFOP VMA open, vm_start %lx, vm_end %lx, vm_pgoff %lx\n",
          vma->vm_start, vma->vm_end, vma->vm_pgoff);
}

void gfifop_vma_close(struct vm_area_struct *vma)
{
   printk(KERN_INFO "GFIFOP VMA close.\n");
}

static vm_fault_t gfifop_vma_nopage(struct vm_fault *vmf)
{
   unsigned long offset;
   struct vm_area_struct *vma = vmf->vma;
   struct gfifop_dev *dev = vma->vm_private_data;
   struct page *page = NULL;
   void *pageptr = NULL;
   vm_fault_t retval = VM_FAULT_NOPAGE;

   mutex_lock(&dev->mutex);

   offset = (unsigned long)(vmf->address - vma->vm_start) + (vma->vm_pgoff << PAGE_SHIFT);
   if (offset >= dev->total_size)
      goto out;

   pageptr = (void *)&dev->mem[offset];
   if (!pageptr)
      goto out;

   page = virt_to_page(pageptr);
   get_page(page);
   vmf->page = page;
   retval = 0;

out:
   mutex_unlock(&dev->mutex);
   return retval;
}

struct vm_operations_struct gfifop_vm_ops = {
    .open = gfifop_vma_open,
    .close = gfifop_vma_close,
    .fault = gfifop_vma_nopage,
};

int gfifop_mmap(struct file *filp, struct vm_area_struct *vma)
{
   /* don't do anything here: "nopage" will set up page table entries */
   vma->vm_ops = &gfifop_vm_ops;
   vma->vm_private_data = filp->private_data;
   gfifop_vma_open(vma);
   return 0;
}

static const struct file_operations gfifop_fops = {
    .owner = THIS_MODULE,
    .read = gfifop_read,
    .write = gfifop_write,
    .unlocked_ioctl = gfifop_ioctl,
    .mmap = gfifop_mmap,
    .poll = gfifop_poll,
    .fasync = gfifop_fasync,
    .open = gfifop_open,
    .release = gfifop_release,
};

static void gfifop_setup_cdev(struct gfifop_dev *dev, int index)
{
   int err, devno = MKDEV(gfifop_major, index);

   cdev_init(&dev->cdev, &gfifop_fops);
   dev->cdev.owner = THIS_MODULE;
   err = cdev_add(&dev->cdev, devno, 1);
   if (err)
      printk(KERN_NOTICE "Error %d adding gfifop%d", err, index);
}

static int __init gfifop_init(void)
{
   int ret;
   dev_t devno = MKDEV(gfifop_major, 0);

   if (gfifop_major)
      ret = register_chrdev_region(devno, 1, "gfifop");
   else
   {
      ret = alloc_chrdev_region(&devno, 0, 1, "gfifop");
      gfifop_major = MAJOR(devno);
   }
   if (ret < 0)
      return ret;

   gfifop_devp = kzalloc(sizeof(struct gfifop_dev), GFP_KERNEL);
   if (!gfifop_devp)
   {
      ret = -ENOMEM;
      goto fail_malloc;
   }
   gfifop_devp->total_size = PAGE_SIZE * (1 << GFIFOP_ORDER);
   // Has to use __get_free_pages to allocate memory, kmalloc will crash
   gfifop_devp->mem = (unsigned char *)__get_free_pages(GFP_KERNEL, GFIFOP_ORDER);

   gfifop_setup_cdev(gfifop_devp, 0);

   mutex_init(&gfifop_devp->mutex);
   init_waitqueue_head(&gfifop_devp->r_wait);
   init_waitqueue_head(&gfifop_devp->w_wait);

   return 0;

fail_malloc:
   unregister_chrdev_region(devno, 1);
   return ret;
}
module_init(gfifop_init);

static void __exit gfifop_exit(void)
{
   cdev_del(&gfifop_devp->cdev);
   kfree(gfifop_devp);
   unregister_chrdev_region(MKDEV(gfifop_major, 0), 1);
}
module_exit(gfifop_exit);
