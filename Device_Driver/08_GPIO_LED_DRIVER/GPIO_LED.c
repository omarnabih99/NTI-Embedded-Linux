#include "GPIO_LED.h"



/***************************************************************************************************************************************************
*   1- Meta Information
***************************************************************************************************************************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Omar Nabih");
MODULE_DESCRIPTION("A trivial LED device driver");



/***************************************************************************************************************************************************
*   2- File Operations
***************************************************************************************************************************************************/
char Device_Buffer_LED0[LED0_BUFFER_SIZE]= "";
char Device_Buffer_LED1[LED1_BUFFER_SIZE]= "";
char Device_Buffer_LED2[LED2_BUFFER_SIZE]= "";
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

    checkPermissionsReturn = Check_Permissions(characterDevicePrivateDataPtr -> permissions, deviceFile -> f_mode);
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
        [LED_0] = 
        {
            .name = "LED_0",
            .pin = 2,
            .buffer = Device_Buffer_LED0,
            .size = LED0_BUFFER_SIZE,
            .permissions = RDWR
        },
        [LED_1] = 
        {
            .name = "LED_1",
            .pin = 3,
            .buffer = Device_Buffer_LED1,
            .size = LED1_BUFFER_SIZE,
            .permissions = RDWR
        },
        [LED_2] = 
        {
            .name = "LED_2",
            .pin = 4,
            .buffer = Device_Buffer_LED2,
            .size = LED2_BUFFER_SIZE,
            .permissions = RDWR
        }

    }
};



/***************************************************************************************************************************************************
*   4- Module Init
***************************************************************************************************************************************************/
static int __init Init_LEDs(void)
{
    int allocateReturnValue = 0;
    int cdev_addReturnValue = 0;

    int i;
    int j;

    printk("LEDs Devices Insertion is started...\n");

    // 1- allocate a memory region for our character devices
    allocateReturnValue = alloc_chrdev_region(&ST_myDriver.deviceNumber, BASE_MINOR, NUMBER_OF_DEVICES, "LEDs");
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
    ST_myDriver.ST_My_Class_Ptr = class_create("LEDs");
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

    // 5- Initialize GPIO Pins
    gpio_request(LED0_pin, "LED0_gpio_pin");
    gpio_direction_output(LED0_pin, LOW);

    gpio_request(LED1_pin, "LED1_gpio_pin");
    gpio_direction_output(LED1_pin, LOW);

    gpio_request(LED2_pin, "LED2_gpio_pin");
    gpio_direction_output(LED2_pin, LOW);

    printk("Char Devices Insertion is successful!\n");

    return 0;
}



/***************************************************************************************************************************************************
*   5- Module Exit
***************************************************************************************************************************************************/
static void __exit Exit_LEDs(void)
{
    int i;
    
    printk("Char Devices Clean Up is started...\n");

    gpio_set_value(LED0_pin, LOW);
    gpio_free(LED0_pin);

    gpio_set_value(LED1_pin, LOW);
    gpio_free(LED1_pin);

    gpio_set_value(LED2_pin, LOW);
    gpio_free(LED2_pin);

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
module_init(Init_LEDs);
module_exit(Exit_LEDs);
