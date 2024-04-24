#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <linux/export.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Omar Nabih");
MODULE_DESCRIPTION("A trivial Psuedo device driver");

// define the parameter to be passed from user space
int number = 0;
int majorNumber = 0;


//call the function which uses this parameter
//module_param (counter, int, 0664);
module_param (number, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
MODULE_PARM_DESC(number, "Major Number");


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

static int __init Init_Hello_Driver(void)
{
    int returnValue = 0;
    majorNumber = number;
    printk("Hello, World!\n");

    returnValue = register_chrdev(majorNumber, "Basic_Major_Number", myFopsPtr);
    if (0 == returnValue)
    {
        printk("Device is registered sucessfully\n");
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
    unregister_chrdev (majorNumber, "Basic_Major_Number");
    printk("Good Bye!\n");
}

module_init(Init_Hello_Driver);
module_exit(Exit_Hello_Driver);