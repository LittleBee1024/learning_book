#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL v2");

#define DEVICE_NUM 3
#define GMEM_MAJOR 111
#define GLOBALMEM_SIZE 0x1000
// ioctl CMD
#define MEM_CLEAR 0x1

struct gmem_dev
{
   struct cdev cdev;
   unsigned char mem[GLOBALMEM_SIZE];
};
struct gmem_dev *gmem_devp;

static int gmem_open(struct inode *inode, struct file *filp)
{
   struct gmem_dev *dev = container_of(inode->i_cdev, struct gmem_dev, cdev);
   filp->private_data = dev;
   return 0;
}

static int gmem_release(struct inode *inode, struct file *filp)
{
   return 0;
}

static long gmem_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
   struct gmem_dev *dev = filp->private_data;

   switch (cmd)
   {
   case MEM_CLEAR:
      memset(dev->mem, 0, GLOBALMEM_SIZE);
      printk(KERN_INFO "gmem is set to zero\n");
      break;

   default:
      return -EINVAL;
   }

   return 0;
}

static ssize_t gmem_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
   unsigned long p = *ppos;
   unsigned int count = size;
   struct gmem_dev *dev = filp->private_data;

   if (p >= GLOBALMEM_SIZE)
   {
      printk(KERN_INFO "reach the end of the device whose size is %u bytes(s)\n", GLOBALMEM_SIZE);
      return 0;
   }

   if (count > GLOBALMEM_SIZE - p)
      count = GLOBALMEM_SIZE - p;

   if (copy_to_user(buf, dev->mem + p, count))
      return -EFAULT;

   *ppos += count;
   printk(KERN_INFO "read %u bytes(s) from %lu\n", count, p);

   return count;
}

static ssize_t gmem_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
   unsigned long p = *ppos;
   unsigned int count = size;
   struct gmem_dev *dev = filp->private_data;

   if (p >= GLOBALMEM_SIZE)
   {
      printk(KERN_INFO "reach the end of the device whose size is %u bytes(s)\n", GLOBALMEM_SIZE);
      return 0;
   }

   if (count > GLOBALMEM_SIZE - p)
      count = GLOBALMEM_SIZE - p;

   if (copy_from_user(dev->mem + p, buf, count))
      return -EFAULT;

   *ppos += count;
   printk(KERN_INFO "written %u bytes(s) from %lu\n", count, p);

   return count;
}

static loff_t gmem_llseek(struct file *filp, loff_t offset, int whence)
{
   loff_t newpos = 0;
   switch (whence)
   {
   case 0: /* SEEK_SET */
      newpos = offset;
      break;
   case 1: /* SEEK_CUR */
      newpos = filp->f_pos + offset;
      break;
   case 2: /* SEEK_END */
      newpos = GLOBALMEM_SIZE + offset;
      break;
   default:
      return -EINVAL;
   }
   if (newpos < 0 || newpos >= GLOBALMEM_SIZE)
      return -EINVAL;

   filp->f_pos = newpos;
   return newpos;
}

static const struct file_operations gmem_fops = {
    .owner = THIS_MODULE,
    .llseek = gmem_llseek,
    .read = gmem_read,
    .write = gmem_write,
    .unlocked_ioctl = gmem_ioctl,
    .open = gmem_open,
    .release = gmem_release,
};

static void gmem_setup_cdev(struct gmem_dev *dev, int index)
{
   int err, devno = MKDEV(GMEM_MAJOR, index);

   cdev_init(&dev->cdev, &gmem_fops);
   dev->cdev.owner = THIS_MODULE;
   err = cdev_add(&dev->cdev, devno, 1);
   if (err)
      printk(KERN_NOTICE "Error %d adding gmem%d", err, index);
}

static int __init gmem_init(void)
{
   int ret;
   dev_t devno;
   int i;
   printk(KERN_INFO "Gmem enter\n");

   devno = MKDEV(GMEM_MAJOR, 0);
   ret = register_chrdev_region(devno, DEVICE_NUM, "gmem");
   if (ret < 0)
   {
      printk(KERN_ERR "Failed to register major number %d for gmem module\n", GMEM_MAJOR);
      return ret;
   }
   printk(KERN_INFO "Register major number %d for gmem module", GMEM_MAJOR);

   gmem_devp = kzalloc(sizeof(struct gmem_dev) * DEVICE_NUM, GFP_KERNEL);
   if (!gmem_devp)
   {
      printk(KERN_ERR "Failed to malloc memory for gmem module\n");
      ret = -ENOMEM;
      goto fail_malloc;
   }

   for (i = 0; i < DEVICE_NUM; i++)
      gmem_setup_cdev(gmem_devp + i, i);

   return 0;

fail_malloc:
   unregister_chrdev_region(devno, DEVICE_NUM);
   return ret;
}
module_init(gmem_init);

static void __exit gmem_exit(void)
{
   int i;
   printk(KERN_INFO "Gmem exit\n");

   for (i = 0; i < DEVICE_NUM; i++)
      cdev_del(&((gmem_devp + i)->cdev));
   kfree(gmem_devp);
   unregister_chrdev_region(MKDEV(GMEM_MAJOR, 0), DEVICE_NUM);
}
module_exit(gmem_exit);
