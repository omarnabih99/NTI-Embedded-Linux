#ifndef PLATFORM_DRIVER_H
#define PLATFORM_DRIVER_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/export.h>
#include <linux/cdev.h>
#include <linux/stat.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#define BASE_MINOR 0
#define NUMBER_OF_CORRESPONDING_DEVICES 1
#define NUMBER_OF_DEVICES 3

#define LED0_BUFFER_SIZE 2
#define LED1_BUFFER_SIZE 2
#define LED2_BUFFER_SIZE 2

#define RDONLY 0x01
#define WRONLY 0x10
#define RDWR   0x11

#define IN  0
#define OUT 1

#define HIGH 1
#define LOW  0

#define LED0_pin 2
#define LED1_pin 3
#define LED2_pin 4

typedef enum EN_Devices_t
{
    LED_0,
    LED_1,
    LED_2
}EN_Devices_t;

typedef struct ST_PlatformDeviceData_t
{
    const char*     name;
    size_t          size;
    int             permissions;
}ST_PlatformDeviceData_t;

typedef struct ST_charDevicePrivateData_t
{
    dev_t                           deviceNumber;
    unsigned int                    pin;
    char*                           buffer;
    unsigned int                    g_offset;
    struct cdev                     ST_cDev;
    ST_PlatformDeviceData_t         ST_Device_Platform_Data;
}ST_charDevicePrivateData_t;

typedef struct ST_charDriverPrivateData_t
{
    int                             deviceCount;
    dev_t                           baseDeviceNumber;
    const struct class*             ST_My_Class_Ptr;
    struct device*                  ST_My_Device_Ptr;
}ST_charDriverPrivateData_t;


#endif