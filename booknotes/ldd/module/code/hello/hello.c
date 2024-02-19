#include <linux/init.h>
#include <linux/module.h>

static int hello_init(void)
{
   printk(KERN_INFO "Hello World enter\n");
   return 0;
}
module_init(hello_init);

static void hello_exit(void)
{
   printk(KERN_INFO "Hello World exit\n");
}
module_exit(hello_exit);

MODULE_AUTHOR("Little Bee");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("A simple Hello World Module");
MODULE_ALIAS("a simplest module");
