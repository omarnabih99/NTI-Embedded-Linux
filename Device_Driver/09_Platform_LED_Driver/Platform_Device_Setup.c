#include "Platform_Driver.h"



/***************************************************************************************************************************************************
*   1- Meta Information
***************************************************************************************************************************************************/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Omar Nabih");
MODULE_DESCRIPTION("Platform Devices Setup");



/***************************************************************************************************************************************************
*   2- Platform Devices Methods
***************************************************************************************************************************************************/
void Platform_my_release(struct device *dev)
{
    printk("Device is released \n");
}



/***************************************************************************************************************************************************
*   3- LED Devices Registeration Data
***************************************************************************************************************************************************/
ST_PlatformDeviceData_t ST_Platform_Device_Data[NUMBER_OF_DEVICES] = 
{
    [LED_0] = 
    {
        .name = "LED_0",
        .size = LED0_BUFFER_SIZE,
        .permissions = RDWR,
        .pin = LED0_pin
    },

    [LED_1] =
    {
        .name = "LED_1",
        .size = LED1_BUFFER_SIZE,
        .permissions = RDWR,
        .pin = LED1_pin
    },

    [LED_2] = 
    {
        .name = "LED_2",
        .size = LED2_BUFFER_SIZE,
        .permissions = RDWR,
        .pin = LED2_pin
    }
};

struct platform_device ST_Platform_Device_LEDs [NUMBER_OF_DEVICES] =
{
    [LED_0] = 
    {
        .name = "Platform_LEDs",
        .id   = LED_0,
        .dev = 
                {
                    .platform_data = &ST_Platform_Device_Data[LED_0],
                    .release = Platform_my_release
                }
    },

    [LED_1] =
    {
        .name = "Platform_LEDs",
        .id   = LED_1,
        .dev = 
                {
                    .platform_data = &ST_Platform_Device_Data[LED_1],
                    .release = Platform_my_release
                }
    },

    [LED_2] = 
    {
        .name = "Platform_LEDs",
        .id   = LED_2,
        .dev = 
                {
                    .platform_data = &ST_Platform_Device_Data[LED_2],
                    .release = Platform_my_release
                }
    }
};





/***************************************************************************************************************************************************
*   5- Module Init
***************************************************************************************************************************************************/
static int __init Init_Platform_LEDs(void)
{
    int i;

    printk("LEDs Devices Insertion is started...\n");

    // 1- register a platform device
    for(i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        platform_device_register (&ST_Platform_Device_LEDs[i]);
        printk("%s is registered \n", ST_Platform_Device_Data[i].name);
    }

    return 0;
}



/***************************************************************************************************************************************************
*   6- Module Exit
***************************************************************************************************************************************************/
static void __exit Exit_Platform_LEDs(void)
{
    int i;
    
    printk("Char Devices Clean Up is started...\n");

    for(i = 0; i < NUMBER_OF_DEVICES; i++)
    {
        platform_device_unregister (&ST_Platform_Device_LEDs[i]);
        printk("%s is unregistered \n", ST_Platform_Device_Data[i].name);
    }

    printk("Char Devices Clean Up is done!\n");
}



/***************************************************************************************************************************************************
*   7- Module Init and Exit Invoking
***************************************************************************************************************************************************/
module_init(Init_Platform_LEDs);
module_exit(Exit_Platform_LEDs);
