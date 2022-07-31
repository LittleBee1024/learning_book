#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/poll.h>
#include <linux/mm.h>

MODULE_LICENSE("GPL v2");

// This driver only supports one page
#define GMEMP_SIZE PAGE_SIZE
// zero order means one page
#define GMEMP_ORDER 0
#define GMEMP_CLEAR 0x1
#define GMEMP_MAJOR 229

static int gmemp_major = GMEMP_MAJOR;
module_param(gmemp_major, int, S_IRUGO);

struct gmemp_dev
{
   struct cdev cdev;
   size_t total_size;
   unsigned char *mem;
};
struct gmemp_dev *gmemp_devp;

static int gmemp_open(struct inode *inode, struct file *filp)
{
   struct gmemp_dev *dev = container_of(inode->i_cdev, struct gmemp_dev, cdev);
   filp->private_data = dev;
   return 0;
}

static int gmemp_release(struct inode *inode, struct file *filp)
{
   return 0;
}

static long gmemp_ioctl(struct file *filp, unsigned int cmd,
                         unsigned long arg)
{
   struct gmemp_dev *dev = filp->private_data;

   switch (cmd)
   {
   case GMEMP_CLEAR:
      memset(dev->mem, 0, dev->total_size);
      printk(KERN_INFO "gmemp is set to zero\n");
      break;

   default:
      return -EINVAL;
   }
   return 0;
}

static ssize_t gmemp_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
   unsigned long p = *ppos;
   unsigned int count = size;
   struct gmemp_dev *dev = filp->private_data;

   if (p >= dev->total_size)
      return 0;

   if (count > dev->total_size - p)
      count = dev->total_size - p;

   if (copy_to_user(buf, dev->mem + p, count))
      return -EFAULT;

   *ppos += count;
   printk(KERN_INFO "read %u bytes(s) from %lu\n", count, p);

   return count;
}

static ssize_t gmemp_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
   unsigned long p = *ppos;
   unsigned int count = size;
   struct gmemp_dev *dev = filp->private_data;

   if (p >= dev->total_size)
      return 0;

   if (count > dev->total_size - p)
      count = dev->total_size - p;

   if (copy_from_user(dev->mem + p, buf, count))
      return -EFAULT;

   *ppos += count;
   printk(KERN_INFO "written %u bytes(s) from %lu\n", count, p);

   return count;
}

void gmemp_vma_open(struct vm_area_struct *vma)
{
   printk(KERN_INFO "GMEMP VMA open, vm_start %lx, vm_end %lx, vm_pgoff %lx\n",
          vma->vm_start, vma->vm_end, vma->vm_pgoff);
}

void gmemp_vma_close(struct vm_area_struct *vma)
{
   printk(KERN_INFO "GMEMP VMA close.\n");
}

static vm_fault_t gmemp_vma_nopage(struct vm_fault *vmf)
{
   unsigned long offset;
   struct vm_area_struct *vma = vmf->vma;
   struct gmemp_dev *dev = vma->vm_private_data;
   struct page *page = NULL;
   void *pageptr = NULL;

   offset = (unsigned long)(vmf->address - vma->vm_start) + (vma->vm_pgoff << PAGE_SHIFT);
   if (offset >= dev->total_size)
      return VM_FAULT_NOPAGE;

   pageptr = (void *)&dev->mem[offset];
   if (!pageptr)
      return VM_FAULT_NOPAGE;

   page = virt_to_page(pageptr);
   get_page(page);
   vmf->page = page;
   return 0;
}

struct vm_operations_struct gmemp_vm_ops = {
    .open = gmemp_vma_open,
    .close = gmemp_vma_close,
    .fault = gmemp_vma_nopage,
};

int gmemp_mmap(struct file *filp, struct vm_area_struct *vma)
{
   /* don't do anything here: "nopage" will set up page table entries */
   vma->vm_ops = &gmemp_vm_ops;
   vma->vm_private_data = filp->private_data;
   gmemp_vma_open(vma);
   return 0;
}

static const struct file_operations gmemp_fops = {
    .owner = THIS_MODULE,
    .read = gmemp_read,
    .write = gmemp_write,
    .unlocked_ioctl = gmemp_ioctl,
    .mmap = gmemp_mmap,
    .open = gmemp_open,
    .release = gmemp_release,
};

static void gmemp_setup_cdev(struct gmemp_dev *dev, int index)
{
   int err, devno = MKDEV(gmemp_major, index);

   cdev_init(&dev->cdev, &gmemp_fops);
   dev->cdev.owner = THIS_MODULE;
   err = cdev_add(&dev->cdev, devno, 1);
   if (err)
      printk(KERN_NOTICE "Error %d adding gmemp%d", err, index);
}

static int __init gmemp_init(void)
{
   int ret;
   dev_t devno = MKDEV(gmemp_major, 0);

   if (gmemp_major)
      ret = register_chrdev_region(devno, 1, "gmemp");
   else
   {
      ret = alloc_chrdev_region(&devno, 0, 1, "gmemp");
      gmemp_major = MAJOR(devno);
   }
   if (ret < 0)
      return ret;

   gmemp_devp = kzalloc(sizeof(struct gmemp_dev), GFP_KERNEL);
   if (!gmemp_devp)
   {
      ret = -ENOMEM;
      goto fail_malloc;
   }
   gmemp_devp->total_size = GMEMP_SIZE;
   // Has to use __get_free_pages to allocate memory, kmalloc will crash
   // page order is zero, which means only one page
   gmemp_devp->mem = (unsigned char *)__get_free_pages(GFP_KERNEL, GMEMP_ORDER);

   gmemp_setup_cdev(gmemp_devp, 0);

   return 0;

fail_malloc:
   unregister_chrdev_region(devno, 1);
   return ret;
}
module_init(gmemp_init);

static void __exit gmemp_exit(void)
{
   cdev_del(&gmemp_devp->cdev);
   if(gmemp_devp->mem)
      free_pages((unsigned long)(gmemp_devp->mem), GMEMP_ORDER);
   kfree(gmemp_devp);
   unregister_chrdev_region(MKDEV(gmemp_major, 0), 1);
}
module_exit(gmemp_exit);
