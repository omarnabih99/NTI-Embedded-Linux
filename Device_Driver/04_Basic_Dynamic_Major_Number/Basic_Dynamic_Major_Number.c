#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/export.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Omar Nabih");
MODULE_DESCRIPTION("A trivial Psuedo device driver");


// Implement my_open() which will be called when an open syscall is invoked
static int my_open(struct inode* deviceFile, struct file* instance)
{
    printk("%s dev_nr - open was called \n", __FUNCTION__);
    return 0;
}


// Implement my_release() which will be called when a close syscall is invoked
static int my_release(struct inode* deviceFile, struct file* instance)
{
    printk("%s dev_nr - close was called \n", __FUNCTION__);
    return 0;
}

// create an instace of file_operations struct
struct file_operations myFops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release
};
// create a pointer to this struct
struct file_operations* myFopsPtr = &myFops;

dev_t deviceNumber;
static int __init Init_Hello_Driver(void)
{
    int returnValue = 0;
    
    printk("Hello, World!\n");

    alloc_chrdev_region(&deviceNumber, 0, 1, "Basic_Dynamic_Major_Number");
    if (0 == returnValue)
    {
        printk("Device is registered sucessfully using: %s with Major = %d and Minor = %d \n", __FUNCTION__, MAJOR(deviceNumber), MINOR(deviceNumber));
    }
    else 
    {
        printk("Device is not registered sucessfully\n");
        return -1;
    }
        return 0;
}

static void __exit Exit_Hello_Driver(void)
{
    unregister_chrdev_region (deviceNumber, 1);
    printk("Good Bye!\n");
}

module_init(Init_Hello_Driver);
module_exit(Exit_Hello_Driver);