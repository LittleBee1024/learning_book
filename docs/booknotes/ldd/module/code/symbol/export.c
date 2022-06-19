#include <linux/init.h>
#include <linux/module.h>

static int export_init(void)
{
   printk(KERN_INFO "Export module enter\n");
   return 0;
}
module_init(export_init);

static void export_exit(void)
{
   printk(KERN_INFO "Export module exit\n");
}
module_exit(export_exit);

void hello_export(void)
{
   printk(KERN_INFO "Hello from another module");
}
EXPORT_SYMBOL(hello_export);

MODULE_LICENSE("GPL v2");
