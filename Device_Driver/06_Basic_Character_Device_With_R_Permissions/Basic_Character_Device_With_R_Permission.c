#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/export.h>
#include <linux/cdev.h>
#include <linux/stat.h>

#define KERNEL_BUFFER_SIZE 15

/***************************************************************************************************************************************************
*   1- Meta Information
***************************************************************************************************************************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Omar Nabih");
MODULE_DESCRIPTION("A trivial Psuedo device driver");



/***************************************************************************************************************************************************
*   2- File Operations
***************************************************************************************************************************************************/
// Create the buffer used in my_read() and my_write()
static unsigned char kernelBuffer[KERNEL_BUFFER_SIZE] = "";

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

// Implement my_read() function which will be called when a read syscall is invoked
ssize_t my_read(struct file* file, char __user* userBuffer, size_t count, loff_t* offset)
{
    int notCopied;

    printk("my_read is called \n");

    printk("The count to read: %ld \n", count);

    printk("The offset to read: %lld \n", *offset);

    // a logic to handle the count in read function
    if ((count + *offset) > KERNEL_BUFFER_SIZE)
    {
        count = KERNEL_BUFFER_SIZE - *offset;
    }

    notCopied = copy_to_user(userBuffer, &kernelBuffer[*offset], count);
    if (notCopied)
    {
        printk("not copied is: %d \n", notCopied);
        return -1;
    }

    // if the kernel buffer size is larger than the read count, this function will be called more than one time
    // so, we will assign the current count to the offset to ensure that it will start at the end of the previous call
    *offset = count;

    printk("already copied is: %ld \n", count);

    return count;
}

// Implement my_write() function which will be called when a write syscall is invoked
ssize_t my_write(struct file* file, const char __user* userBuffer, size_t count, loff_t* offset)
{
    int notWritten;

    printk("my_write is called \n");

    printk("The count to write: %ld \n", count);

    printk("The offset to write: %lld \n", *offset);

    // a logic to handle the count in write function
    if ((count + *offset) > KERNEL_BUFFER_SIZE)
    {
        count = KERNEL_BUFFER_SIZE - *offset;
    }
    if (!count)
    {
        printk("No space is left in the buffer \n");
        return -1;
    }

    memset(kernelBuffer, 0, KERNEL_BUFFER_SIZE);

    notWritten = copy_from_user(&kernelBuffer[*offset], userBuffer, count);
    if (notWritten)
    {
        printk("not written is: %d \n", notWritten);
        return -1;
    }

    // if the kernel buffer size is larger than the read count, this function will be called more than one time
    // so, we will assign the current count to the offset to ensure that it will start at the end of the previous call
    *offset = count;

    printk("already written is: %ld \n", count);

    printk("The message is: %s \n", kernelBuffer);

    return count;
}

// create an instace of file_operations struct
struct file_operations ST_MyFops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write
};

// create a pointer to this struct
struct file_operations* ST_MyFopsPtr = &ST_MyFops;



/***************************************************************************************************************************************************
*   3- Character Device Registeration Data
***************************************************************************************************************************************************/
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




/***************************************************************************************************************************************************
*   4- Character Device Permission
***************************************************************************************************************************************************/
char* My_Char_Dev_Devnode(const struct device *dev, umode_t *mode)
{
    if (!mode)
    {
        return NULL;
    }

    *mode = (S_IRUGO);

    return NULL;
}



/***************************************************************************************************************************************************
*   5- Module Init
***************************************************************************************************************************************************/
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

    // 4- Set the permissions
    ST_My_Class_Ptr -> devnode = My_Char_Dev_Devnode;

    // 5- generate the device node
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



/***************************************************************************************************************************************************
*   6- Module Exit
***************************************************************************************************************************************************/
static void __exit Exit_Basic_Character_Device(void)
{
    device_destroy(ST_My_Class_Ptr, deviceNumber);
    class_destroy(ST_My_Class_Ptr);
    cdev_del(ST_Cdev_Ptr);
    unregister_chrdev_region (deviceNumber, 1);
    printk("Good Bye!\n");
}



/***************************************************************************************************************************************************
*   7- Module Init and Exit Invoking
***************************************************************************************************************************************************/
module_init(Init_Basic_Character_Device);
module_exit(Exit_Basic_Character_Device);