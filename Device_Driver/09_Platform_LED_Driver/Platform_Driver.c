#include "Platform_Driver.h"



/***************************************************************************************************************************************************
*   1- Meta Information
***************************************************************************************************************************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Omar Nabih");
MODULE_DESCRIPTION("Platform Driver to register Platform LEDs");



/***************************************************************************************************************************************************
*   2- File Operations
***************************************************************************************************************************************************/
// Implement check_permissions() function to check the matching between the access mode and the device permissions
int Check_Permissions (int devicePermissions, fmode_t fileMode)
{
    // 1- if the device permissions is read/write -> you don't need to check the access mode
    if (devicePermissions == RDWR)
    {
        return 0;
    }

    // 2- if the device permissions is read only
    if ( (devicePermissions == RDONLY) && (fileMode & FMODE_READ) && !(fileMode & FMODE_WRITE))
    {
        return 0;
    }

    // 3- if the device permissions is write only
    if ( (devicePermissions == WRONLY) && (fileMode & FMODE_WRITE) && !(fileMode & FMODE_READ))
    {
        return 0;
    }

    return -1;
}

// Implement my_open() which will be called when an open syscall is invoked
static int my_open(struct inode* deviceInode, struct file* deviceFile)
{
    int minorNumber;

    ST_charDevicePrivateData_t *characterDevicePrivateDataPtr;

    int checkPermissionsReturn;

    minorNumber = MINOR(deviceInode -> i_rdev);
    printk("%s was called on device %d \n", __FUNCTION__, minorNumber);

    characterDevicePrivateDataPtr = container_of(deviceInode -> i_cdev, ST_charDevicePrivateData_t, ST_cDev);
    deviceFile -> private_data = characterDevicePrivateDataPtr;

    checkPermissionsReturn = Check_Permissions(characterDevicePrivateDataPtr ->ST_Device_Platform_Data.permissions, deviceFile -> f_mode);
    if(checkPermissionsReturn == 0)
    {
        printk("Device is opened sucessfully \n");
    }
    else
    {
        printk("Device is not opened sucessfully \n");
        return -1;
    }

    return 0;
}

// Implement my_release() which will be called when a close syscall is invoked
static int my_release(struct inode* deviceFile, struct file* instance)
{
    printk("%s: close was called \n", __FUNCTION__);
    return 0;
}

// Implement my_read() function which will be called when a read syscall is invoked
ssize_t my_read(struct file* deviceFile, char __user* userBuffer, size_t count, loff_t* offset)
{
    int notCopied;

    ST_charDevicePrivateData_t *characterDevicePrivateDataPtr;

    int maxSize;

    characterDevicePrivateDataPtr = (ST_charDevicePrivateData_t*) deviceFile ->private_data;

    maxSize = characterDevicePrivateDataPtr ->ST_Device_Platform_Data.size;

    printk("my_read is called \n");

    printk("The count to read: %ld \n", count);

    printk("The offset to read: %lld \n", *offset);

    // a logic to handle the count in read function
    if ((count + *offset) > maxSize)
    {
        count = maxSize - *offset;
    }

    notCopied = copy_to_user(userBuffer, characterDevicePrivateDataPtr -> buffer + (*offset), count);
    if (notCopied)
    {
        printk("not copied is: %d \n", notCopied);
        return -1;
    }

    // if the kernel buffer size is larger than the read count, this function will be called more than one time
    // so, we will assign the current count to the offset to ensure that it will start at the end of the previous call
    *offset += count;

    printk("already copied is: %ld \n", count);

    return count;
}

// Implement my_write() function which will be called when a write syscall is invoked
ssize_t my_write(struct file* deviceFile, const char __user* userBuffer, size_t count, loff_t* offset)
{
    int notWritten;

    ST_charDevicePrivateData_t *characterDevicePrivateDataPtr;

    int maxSize;

    characterDevicePrivateDataPtr = (ST_charDevicePrivateData_t*) deviceFile ->private_data;

    maxSize = characterDevicePrivateDataPtr ->ST_Device_Platform_Data.size;

    printk("my_write is called \n");

    printk("The count to write: %ld \n", count);

    printk("The offset to write: %lld \n", *offset);

    // a logic to handle the count in write function
    if ((count + *offset) > maxSize)
    {
        count = maxSize - *offset;
    }
    if (!count)
    {
        printk("No space is left in the buffer \n");
        return -1;
    }

    if (deviceFile -> f_flags & O_APPEND)
    {
        *offset = characterDevicePrivateDataPtr ->g_offset;
    }
    else
    {
        memset(characterDevicePrivateDataPtr -> buffer, 0, maxSize);
    }

    notWritten = copy_from_user(characterDevicePrivateDataPtr -> buffer + (*offset), userBuffer, count);
    if (notWritten)
    {
        printk("not written is: %d \n", notWritten);
        return -1;
    }

    switch(characterDevicePrivateDataPtr ->buffer[0])
    {
        case '0':
                gpio_set_value(characterDevicePrivateDataPtr ->pin, LOW);
                break;
        
        case '1':
                gpio_set_value(characterDevicePrivateDataPtr ->pin, HIGH);
                break;
    }

    // if the kernel buffer size is larger than the read count, this function will be called more than one time
    // so, we will assign the current count to the offset to ensure that it will start at the end of the previous call
    *offset += count;

    characterDevicePrivateDataPtr ->g_offset = *offset;

    printk("already written is: %ld \n", count);

    printk("The message is: %s \n", characterDevicePrivateDataPtr->buffer);

    return count;
}

// create an instace of file_operations struct
const struct file_operations ST_MyFops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write
};

// create a pointer to this struct
const struct file_operations* ST_MyFopsPtr = &ST_MyFops;



/***************************************************************************************************************************************************
*   3- Driver Private Data Structure
***************************************************************************************************************************************************/
ST_charDriverPrivateData_t ST_myDriver =
{
    .deviceCount = NUMBER_OF_DEVICES,
};



/***************************************************************************************************************************************************
*   4- Platform Driver Methods
***************************************************************************************************************************************************/
// Implement probe function which is called when the platform device is detected
int Platform_my_probe(struct platform_device* ST_Platform_Device_LEDs)
{
    ST_charDevicePrivateData_t* ST_LEDsPrivateDataPtr;

    ST_PlatformDeviceData_t* ST_LEDsPlatformDataPtr;

    int cdev_addReturnValue;


    // 1- get the platform data
    ST_LEDsPlatformDataPtr = (ST_PlatformDeviceData_t*)dev_get_platdata(&ST_Platform_Device_LEDs->dev);
    if(!ST_LEDsPlatformDataPtr)
    {
        printk("No LEDs platform data available \n");
        return -1;
    }

    // 2- dynamically allocate memory for the device private data and extract the passed platform data into the device private data
    ST_LEDsPrivateDataPtr = (ST_charDevicePrivateData_t*)kzalloc(sizeof(ST_charDevicePrivateData_t), GFP_KERNEL);
    if(!ST_LEDsPrivateDataPtr)
    {
        printk("Cannot allocate memory for LED private data \n");
        return -1;
    }

    ST_LEDsPrivateDataPtr->ST_Device_Platform_Data.name = ST_LEDsPlatformDataPtr->name;
    printk("Device: %s is detected \n", ST_LEDsPrivateDataPtr->ST_Device_Platform_Data.name);

    ST_LEDsPrivateDataPtr->ST_Device_Platform_Data.size = ST_LEDsPlatformDataPtr->size;
    printk("Device size is: %ld \n", ST_LEDsPrivateDataPtr->ST_Device_Platform_Data.size);

    ST_LEDsPrivateDataPtr->ST_Device_Platform_Data.permissions = ST_LEDsPlatformDataPtr->permissions;
    printk("Device permissions is: %d \n", ST_LEDsPrivateDataPtr->ST_Device_Platform_Data.permissions);



    // 3- dynamically allocate memory for the device buffer using size information from
    //    from the platform data
    ST_LEDsPrivateDataPtr->buffer = kzalloc(ST_LEDsPrivateDataPtr->ST_Device_Platform_Data.size, GFP_KERNEL);
    if(!ST_LEDsPrivateDataPtr->buffer)
    {
        kfree(ST_LEDsPrivateDataPtr);
        printk("Cannot allocate buffer for the LED device\n");
        return -1;
    }
    

    // 4- get the device number
    ST_LEDsPrivateDataPtr->deviceNumber = ST_myDriver.baseDeviceNumber + ST_Platform_Device_LEDs->id;


    // 5- do cdev init and cdev add
    cdev_init(&ST_LEDsPrivateDataPtr->ST_cDev, ST_MyFopsPtr);

    cdev_addReturnValue = cdev_add(&ST_LEDsPrivateDataPtr->ST_cDev, ST_LEDsPrivateDataPtr->deviceNumber, NUMBER_OF_CORRESPONDING_DEVICES);
    if (cdev_addReturnValue != 0)
    {
        kfree(ST_LEDsPrivateDataPtr->buffer);
        kfree(ST_LEDsPrivateDataPtr);
        printk("LED device is not added sucessfully\n");
        return -1;        
    }


    // 6- create device file for the detected platform device
    ST_myDriver.ST_My_Device_Ptr= device_create(ST_myDriver.ST_My_Class_Ptr, NULL, ST_LEDsPrivateDataPtr->deviceNumber, NULL, ST_LEDsPrivateDataPtr->ST_Device_Platform_Data.name);
        if (ST_myDriver.ST_My_Device_Ptr == NULL)
        {
            printk("LED Device node can not be created successfully\n");

            cdev_del(&ST_LEDsPrivateDataPtr->ST_cDev); 
            kfree(ST_LEDsPrivateDataPtr->buffer);
            kfree(ST_LEDsPrivateDataPtr);

            return -1;
        }
        else
        {
            printk("%s Device node is created successfully\n", ST_LEDsPrivateDataPtr->ST_Device_Platform_Data.name);
        }    


    // 7- pass the pointer to device private data struct to remove() function
    //ST_Platform_Device_LEDs->dev.driver_data = ST_LEDsPrivateDataPtr;
    dev_set_drvdata(&ST_Platform_Device_LEDs->dev, ST_LEDsPrivateDataPtr);

    // 8- increment the number of devices
    ST_myDriver.deviceCount++;

    return 0;
}

// Implement the remove function which is called when the platform device is removed
int Platform_my_remove (struct platform_device* ST_Platform_Device_LEDs)
{
    ST_charDevicePrivateData_t* ST_LEDsPrivateDataPtr;


    // 1- get the pointer to device private data struct
    ST_LEDsPrivateDataPtr = (ST_charDevicePrivateData_t*) dev_get_drvdata(&ST_Platform_Device_LEDs->dev);


    // 2- remove a device that was created by device_create()
    device_destroy(ST_myDriver.ST_My_Class_Ptr, ST_LEDsPrivateDataPtr->deviceNumber);


    // 3- remove the cdev struct from the system
    cdev_del(&ST_LEDsPrivateDataPtr->ST_cDev);


    // 4- free the memory held by the device (device private data and device buffer)
    kfree(ST_LEDsPrivateDataPtr->buffer);
    kfree(ST_LEDsPrivateDataPtr);

    // 5- decrement the device counter    
    ST_myDriver.deviceCount--;


    printk("%s is removed \n", ST_LEDsPrivateDataPtr->ST_Device_Platform_Data.name);


    return 0;
}



/***************************************************************************************************************************************************
*   5- Platform Driver Struct
***************************************************************************************************************************************************/
struct platform_driver ST_Platform_Driver_LEDs =
{
    .probe = Platform_my_probe,
    .remove = Platform_my_remove,
    .driver = 
            {
                .name = "Platform_LEDs"
            }
};



/***************************************************************************************************************************************************
*   6- Module Init
***************************************************************************************************************************************************/
static int __init Init_Platform_LEDs_Driver(void)
{
    int allocateReturnValue = 0;
    int registerReturnValue = 0;

    int i;

    printk("LEDs Devices Insertion is started...\n");

    // 1- allocate a memory region for our character devices
    allocateReturnValue = alloc_chrdev_region(&ST_myDriver.baseDeviceNumber, BASE_MINOR, NUMBER_OF_DEVICES, "LEDs");
    if (0 == allocateReturnValue)
    {
        for (i = 0; i < NUMBER_OF_DEVICES; i++)
        {
            printk("Device is allocated sucessfully using: %s with Major = %d and Minor = %d \n", __FUNCTION__, MAJOR(ST_myDriver.baseDeviceNumber + i), MINOR(ST_myDriver.baseDeviceNumber + i));
        }
    }
    else 
    {
        printk("Device is not allocated sucessfully\n");
        return -1;
    }

    // 2- generate driver class under /sys/class
    ST_myDriver.ST_My_Class_Ptr = class_create("LEDs");
    if (ST_myDriver.ST_My_Class_Ptr == NULL)
    {
        printk("LEDs class can not be created successfully\n");
        unregister_chrdev_region (ST_myDriver.baseDeviceNumber, NUMBER_OF_DEVICES);
        return -1;
    }
    else
    {
        printk("Device class is created successfully\n");
    }

    // 3- register the platform driver
    registerReturnValue = platform_driver_register(&ST_Platform_Driver_LEDs);
    if (registerReturnValue == 0)
    {
        printk("platform driver is loaded \n");
    }
    else
    {
        printk("platform driver is not loaded \n");
        class_destroy(ST_myDriver.ST_My_Class_Ptr);
        unregister_chrdev_region (ST_myDriver.baseDeviceNumber, NUMBER_OF_DEVICES);
    }

    return 0;
}



/***************************************************************************************************************************************************
*   7- Module Exit
***************************************************************************************************************************************************/
static void __exit Exit_Platform_LEDs_Driver(void)
{
    platform_driver_unregister(&ST_Platform_Driver_LEDs);

    class_destroy(ST_myDriver.ST_My_Class_Ptr);

    unregister_chrdev_region (ST_myDriver.baseDeviceNumber, NUMBER_OF_DEVICES);

    printk("platform driver is unloaded \n");
}



/***************************************************************************************************************************************************
*   8- Module Init and Exit Invoking
***************************************************************************************************************************************************/
module_init(Init_Platform_LEDs_Driver);
module_exit(Exit_Platform_LEDs_Driver);
