#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <linux/ioport.h>

MODULE_LICENSE("GPL v2");

#define DEVICE_NUM 2
#define SHORT_MAJOR 109
#define SHORT_MEM_BASE 0xfe800000  // pnp
unsigned long io_short_base = 0;

static int short_open(struct inode *inode, struct file *filp)
{
   return 0;
}

static int short_release(struct inode *inode, struct file *filp)
{
   return 0;
}

static ssize_t short_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
   int count = size;
   struct inode *inode = file_dentry(filp)->d_inode;
   void *address = (void *)io_short_base + (iminor(inode) & 0x0f);

   unsigned char *kbuf = kmalloc(size, GFP_KERNEL);
   unsigned char *ptr = kbuf;
   if (!kbuf)
      return -ENOMEM;

   while (size--)
   {
      *(ptr++) = ioread8(address);
      rmb();
   }

   if (copy_to_user(buf, kbuf, count))
      count = -EFAULT;

   printk(KERN_INFO "[short_read] address=0x%lx, kbuf[0]=%d, count=%d\n", (unsigned long) address, kbuf[0], count);

   kfree(kbuf);
   return count;
}

static ssize_t short_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
   int count = size;
   struct inode *inode = file_dentry(filp)->d_inode;
   void *address = (void *)io_short_base + (iminor(inode) & 0x0f);

   unsigned char *kbuf = kmalloc(size, GFP_KERNEL);
   unsigned char *ptr = kbuf;
   if (!kbuf)
      return -ENOMEM;

   if (copy_from_user(kbuf, buf, size))
      count = -EFAULT;

   while (count > 0 && size--)
   {
      iowrite8(*(ptr++), address);
      wmb();
   }

   printk(KERN_INFO "[short_write] address=0x%lx, kbuf[0]=%d, count=%d\n", (unsigned long) address, kbuf[0], count);

   kfree(kbuf);
   return count;
}

struct file_operations short_fops = {
    .owner = THIS_MODULE,
    .read = short_read,
    .write = short_write,
    .open = short_open,
    .release = short_release,
};

static int __init short_init(void)
{
   int result;

   if (!request_mem_region(SHORT_MEM_BASE, DEVICE_NUM, "short"))
   {
      printk(KERN_INFO "[short_init] can't get I/O mem address 0x%x\n", SHORT_MEM_BASE);
      return -ENODEV;
   }
   io_short_base = (unsigned long) ioremap(SHORT_MEM_BASE, DEVICE_NUM);
   printk(KERN_INFO "[short_init] ioremap returns 0x%lx\n", io_short_base);

   result = register_chrdev(SHORT_MAJOR, "short", &short_fops);
   if (result < 0)
   {
      printk(KERN_INFO "[short_init] can't get major number\n");
      release_mem_region(SHORT_MEM_BASE, DEVICE_NUM);
      return result;
   }
   printk(KERN_INFO "[short_init] done\n");

   return 0;
}
module_init(short_init);

static void __exit short_exit(void)
{
   unregister_chrdev(SHORT_MAJOR, "short");
   iounmap((void __iomem *)io_short_base);
   release_mem_region(SHORT_MEM_BASE, DEVICE_NUM);
   printk(KERN_INFO "[short_exit] done\n");
}
module_exit(short_exit);
