#include <linux/init.h>
#include <linux/module.h>

extern void hello_export(void);

static int import_init(void)
{
   hello_export();
   printk(KERN_INFO "Import module enter\n");
   return 0;
}
module_init(import_init);

static void import_exit(void)
{
   printk(KERN_INFO "Import module exit\n");
}
module_exit(import_exit);

MODULE_LICENSE("GPL v2");
