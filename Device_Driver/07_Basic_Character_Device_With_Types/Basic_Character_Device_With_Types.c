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
    .release = my_release,
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

        cdev_addReturnValue = cdev_add(&ST_myDriver.ST_characterDevices[i].ST_cDev, ST_myDriver.deviceNumber + i, NUMBER_OF_DRIVERS);
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
    
    // 3- generate device files under /sys/class
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
