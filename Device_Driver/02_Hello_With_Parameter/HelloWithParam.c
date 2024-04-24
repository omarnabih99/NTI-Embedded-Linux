#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Omar Nabih");
MODULE_DESCRIPTION("A trivial Psuedo device driver");

// define the counter to be passed from user space
int counter = 0;

//call the function which uses this parameter
/**
 * module_param - typesafe helper for a module/cmdline parameter
 * @name: the variable to alter, and exposed parameter name.
 * @type: the type of the parameter
 * @perm: visibility in sysfs.
 * @name becomes the module parameter
 */
//module_param (counter, int, 0664);
module_param (counter, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
MODULE_PARM_DESC(counter, "An integer parameter");

static int __init Init_Hello_Driver(void)
{
    for(int i = 1; i <= counter; i++)
    {
        printk("Hello, World!\n");
    }
        return 0;
}

static void __exit Exit_Hello_Driver(void)
{
    for(int i = 1; i <= counter; i++)
    {
        printk("Good Bye!\n");
    }
    
}

module_init(Init_Hello_Driver);
module_exit(Exit_Hello_Driver);