#ifndef BASIC_CHAR_DEV
#define BASIC_CHAR_DEV

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/export.h>
#include <linux/cdev.h>
#include <linux/stat.h>
#include <linux/gpio.h>

#define BASE_MINOR 0
#define NUMBER_OF_CORRESPONDING_DEVICES 1
#define NUMBER_OF_DEVICES 3

#define LED0_BUFFER_SIZE 2
#define LED1_BUFFER_SIZE 2
#define LED2_BUFFER_SIZE 2

#define RDONLY 0x01
#define WRONLY 0x10
#define RDWR   0x11

#define IN 0
#define OUT 1

#define HIGH 1
#define LOW 0

#define LED0_pin 2
#define LED1_pin 3
#define LED2_pin 4

typedef enum EN_Devices_t
{
    LED_0,
    LED_1,
    LED_2
}EN_Devices_t;

typedef struct ST_charDevicePrivateData_t
{
    const char* name;
    unsigned int pin;
    char* buffer;
    unsigned int g_offset;
    size_t size;
    int permissions;
    struct cdev ST_cDev;
}ST_charDevicePrivateData_t;

typedef struct ST_charDriverPrivateData_t
{
    int deviceCount;
    dev_t deviceNumber;
    struct class* ST_My_Class_Ptr;
    struct device* ST_My_Device_Ptr;
    ST_charDevicePrivateData_t ST_characterDevices [NUMBER_OF_DEVICES];
}ST_charDriverPrivateData_t;

#endif