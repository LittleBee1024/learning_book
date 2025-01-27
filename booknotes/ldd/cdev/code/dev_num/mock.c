#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/types.h>

#define MOCK_MAJOR 111

static int mock_init(void)
{
   int ret;

   printk(KERN_INFO "Mock enter\n");

   ret = register_chrdev_region(MKDEV(MOCK_MAJOR, 0), 1, "mock");
   if (ret < 0)
   {
      printk(KERN_ERR "Failed to register major number %d for mock module\n", MOCK_MAJOR);
      return ret;
   }

   printk(KERN_INFO "Register major number %d for mock module", MOCK_MAJOR);
   return 0;
}
module_init(mock_init);

static void mock_exit(void)
{
   printk(KERN_INFO "Mock exit\n");
   unregister_chrdev_region(MKDEV(MOCK_MAJOR, 0), 1);
}
module_exit(mock_exit);

MODULE_LICENSE("GPL v2");
