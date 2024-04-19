#include <linux/module.h>
#include <linux/init.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Omar Nabih");
MODULE_DESCRIPTION("A trivial Psuedo device driver");

static int __init Init_Hello_Driver(void)
{
    printk("Hello, World!\n");
    return 0;
}

static void __exit Exit_Hello_Driver(void)
{
    printk("Good Bye!\n");
}

module_init(Init_Hello_Driver);
module_exit(Exit_Hello_Driver);