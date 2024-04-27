#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/export.h>
#include <linux/cdev.h>
#include <linux/device/class.h>

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
struct file_operations ST_MyFops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release
};
// create a pointer to this struct
struct file_operations* ST_MyFopsPtr = &ST_MyFops;


// create an instance of dev_t type to hold the output of the allocation function
dev_t deviceNumber;
dev_t* deviceNumberPtr = &deviceNumber;


// create an instance of cdev struct to hold the output initialization data of cdev_init and then passed to cdev_add 
struct cdev ST_Cdev;
struct cdev* ST_Cdev_Ptr = &ST_Cdev;


// create a pointer to class struct to hold the output of class_create() and then passed to device_create()
struct class* ST_My_Class_Ptr;


// create a pointer to device struct
struct device* ST_My_Device_Ptr;


static int __init Init_Basic_Character_Device(void)
{
    int allocateReturnValue = 0;
    int cdev_addReturnValue = 0;

    printk("Hello, World!\n");

    // 1- allocate a memory region for our character devices
    allocateReturnValue = alloc_chrdev_region(deviceNumberPtr, 0, 1, "Basic_Character_Device");
    if (0 == allocateReturnValue)
    {
        printk("Device is allocated sucessfully using: %s with Major = %d and Minor = %d \n", __FUNCTION__, MAJOR(deviceNumber), MINOR(deviceNumber));
    }
    else 
    {
        printk("Device is not allocated sucessfully\n");
        return -1;
    }


    // 2- define the allocated region to be character devices
    cdev_init(ST_Cdev_Ptr, ST_MyFopsPtr);

    cdev_addReturnValue = cdev_add(ST_Cdev_Ptr, deviceNumber, 1);
    if (cdev_addReturnValue != 0)
    {
        unregister_chrdev_region (deviceNumber, 1);
        printk("Device is not added sucessfully\n");
        return -1;        
    }


    // 3- generate device files under /sys/class
    ST_My_Class_Ptr = class_create("Basic_Character_Class");
    if (ST_My_Class_Ptr == NULL)
    {
        printk("Device class can not be created successfully\n");
        cdev_del(ST_Cdev_Ptr);
        unregister_chrdev_region (deviceNumber, 1);
        return -1;
    }
    else
    {
        printk("Device class is created successfully\n");
    }


    // 4- generate the device node
    ST_My_Device_Ptr = device_create(ST_My_Class_Ptr, NULL, deviceNumber, NULL, "Basic_Device0");
    if (ST_My_Device_Ptr == NULL)
    {

        printk("Device node can not be created successfully\n");
        class_destroy(ST_My_Class_Ptr);
        cdev_del(ST_Cdev_Ptr);
        unregister_chrdev_region (deviceNumber, 1);
        return -1;
    }
    else
    {
        printk("Device node is created successfully\n");
    }

    return 0;
}

static void __exit Exit_Basic_Character_Device(void)
{
    device_destroy(ST_My_Class_Ptr, deviceNumber);
    class_destroy(ST_My_Class_Ptr);
    cdev_del(ST_Cdev_Ptr);
    unregister_chrdev_region (deviceNumber, 1);
    printk("Good Bye!\n");
}

module_init(Init_Basic_Character_Device);
module_exit(Exit_Basic_Character_Device);