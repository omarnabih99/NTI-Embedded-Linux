#include "Basic_Character_Device_With_Types.h"



/***************************************************************************************************************************************************
*   1- Meta Information
***************************************************************************************************************************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Omar Nabih");
MODULE_DESCRIPTION("A trivial Psuedo device driver");



/***************************************************************************************************************************************************
*   2- File Operations
***************************************************************************************************************************************************/
char Device_Buffer_Dev0[DEV0_BUFFER_SIZE]= "";
char Device_Buffer_Dev1[DEV0_BUFFER_SIZE]= "";
char Device_Buffer_Dev2[DEV0_BUFFER_SIZE]= "";
char Device_Buffer_Dev3[DEV0_BUFFER_SIZE]= "";

// Implement Check_Permissions function
int Check_Permissions (void)
{
    return 0;
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

    checkPermissionsReturn = Check_Permissions();
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

    maxSize = characterDevicePrivateDataPtr -> size;

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

    maxSize = characterDevicePrivateDataPtr -> size;

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

    memset(characterDevicePrivateDataPtr -> buffer, 0, maxSize);

    notWritten = copy_from_user(characterDevicePrivateDataPtr -> buffer + (*offset), userBuffer, count);
    if (notWritten)
    {
        printk("not written is: %d \n", notWritten);
        return -1;
    }

    // if the kernel buffer size is larger than the read count, this function will be called more than one time
    // so, we will assign the current count to the offset to ensure that it will start at the end of the previous call
    *offset += count;

    printk("already written is: %ld \n", count);

    printk("The message is: %s \n", characterDevicePrivateDataPtr->buffer);

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
ST_charDriverPrivateData_t ST_myDriver =
{
    .deviceCount = NUMBER_OF_DEVICES,
    .ST_characterDevices = 
    {
        [Device0] = 
        {
            .name = "Basic_Device0",
            .buffer = Device_Buffer_Dev0,
            .size = DEV0_BUFFER_SIZE,
            //.permissions = RDONLY
        },
        [Device1] = 
        {
            .name = "Basic_Device1",
            .buffer = Device_Buffer_Dev1,
            .size = DEV1_BUFFER_SIZE,
            //.permissions = WRONLY
        },
        [Device2] = 
        {
            .name = "Basic_Device2",
            .buffer = Device_Buffer_Dev2,
            .size = DEV2_BUFFER_SIZE,
            //.permissions = RDWR
        },
        [Device3] = 
        {
            .name = "Basic_Device3",
            .buffer = Device_Buffer_Dev3,
            .size = DEV3_BUFFER_SIZE,
            //.permissions = RDWR
        },

    }
};



/***************************************************************************************************************************************************
*   4- Module Init
***************************************************************************************************************************************************/
static int __init Init_Basic_Character_Device(void)
{
    int allocateReturnValue = 0;
    int cdev_addReturnValue = 0;

    int i;
    int j;

    printk("Char Devices Insertion is started...\n");

    // 1- allocate a memory region for our character devices
    allocateReturnValue = alloc_chrdev_region(&ST_myDriver.deviceNumber, BASE_MINOR, NUMBER_OF_DEVICES, "Basic_Character_Devices");
    if (0 == allocateReturnValue)
    {
        for (i = 0; i < NUMBER_OF_DEVICES; i++)
        {
            printk("Device is allocated sucessfully using: %s with Major = %d and Minor = %d \n", __FUNCTION__, MAJOR(ST_myDriver.deviceNumber + i), MINOR(ST_myDriver.deviceNumber + i));
        }
    }
    else 
    {
        printk("Device is not allocated sucessfully\n");
        return -1;
    }

    // 2- define the allocated region to be character devices
    for (i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        cdev_init(&ST_myDriver.ST_characterDevices[i].ST_cDev, ST_MyFopsPtr);

        ST_myDriver.ST_characterDevices[i].ST_cDev.owner = THIS_MODULE;

        cdev_addReturnValue = cdev_add(&ST_myDriver.ST_characterDevices[i].ST_cDev, ST_myDriver.deviceNumber + i, NUMBER_OF_CORRESPONDING_DEVICES);
        if (cdev_addReturnValue != 0)
        {
            for(j = 0; j < i; j++)
            {
                cdev_del(&ST_myDriver.ST_characterDevices[j].ST_cDev);
            }
            unregister_chrdev_region (ST_myDriver.deviceNumber, NUMBER_OF_DEVICES);
            printk("Device is not added sucessfully\n");
            return -1;        
        }
    }
    
    // 3- generate driver class under /sys/class
    ST_myDriver.ST_My_Class_Ptr = class_create("Basic_Character_Devices_Class");
    if (ST_myDriver.ST_My_Class_Ptr == NULL)
    {
        printk("Device class can not be created successfully\n");
        for (i = 0; i < NUMBER_OF_DEVICES; i++)
        {
            cdev_del(&ST_myDriver.ST_characterDevices[i].ST_cDev);
        }
        unregister_chrdev_region (ST_myDriver.deviceNumber, NUMBER_OF_DEVICES);
        return -1;
    }
    else
    {
        printk("Device class is created successfully\n");
    }

    // 4- generate the device node
    for (i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        ST_myDriver.ST_My_Device_Ptr= device_create(ST_myDriver.ST_My_Class_Ptr, NULL, ST_myDriver.deviceNumber + i, NULL, ST_myDriver.ST_characterDevices[i].name);
        if (ST_myDriver.ST_My_Device_Ptr == NULL)
        {
            printk("Device node can not be created successfully\n");
            
            for(j = 0; j < i; j++)
            {
                device_destroy(ST_myDriver.ST_My_Class_Ptr, ST_myDriver.deviceNumber + j);
            }

            class_destroy(ST_myDriver.ST_My_Class_Ptr);

            for (j = 0; j < NUMBER_OF_DEVICES; j++)
            {
                cdev_del(&ST_myDriver.ST_characterDevices[j].ST_cDev);
            }    

            unregister_chrdev_region (ST_myDriver.deviceNumber, NUMBER_OF_DEVICES);

            return -1;
        }
        else
        {
            printk("Device node is created successfully\n");
        }
    }

    printk("Char Devices Insertion is successful!\n");

    return 0;
}



/***************************************************************************************************************************************************
*   5- Module Exit
***************************************************************************************************************************************************/
static void __exit Exit_Basic_Character_Device(void)
{
    int i;
    
    printk("Char Devices Clean Up is started...\n");

    for (i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        device_destroy(ST_myDriver.ST_My_Class_Ptr, ST_myDriver.deviceNumber + i);
        cdev_del(&ST_myDriver.ST_characterDevices[i].ST_cDev);
    }
    
    class_destroy(ST_myDriver.ST_My_Class_Ptr);

    unregister_chrdev_region (ST_myDriver.deviceNumber, NUMBER_OF_DEVICES);

    printk("Char Devices Clean Up is done!\n");
}



/***************************************************************************************************************************************************
*   7- Module Init and Exit Invoking
***************************************************************************************************************************************************/
module_init(Init_Basic_Character_Device);
module_exit(Exit_Basic_Character_Device);
