#ifndef BASIC_CHAR_DEV
#define BASIC_CHAR_DEV

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/export.h>
#include <linux/cdev.h>
#include <linux/stat.h>

#define BASE_MINOR 0
#define NUMBER_OF_CORRESPONDING_DEVICES 1
#define NUMBER_OF_DEVICES 4

#define DEV0_BUFFER_SIZE 1024
#define DEV1_BUFFER_SIZE 512
#define DEV2_BUFFER_SIZE 1024
#define DEV3_BUFFER_SIZE 512

#define RDONLY 0x01
#define WRONLY 0x10
#define RDWR   0x11

typedef enum EN_Devices_t
{
    Device0,
    Device1,
    Device2,
    Device3
}EN_Devices_t;

typedef struct ST_charDevicePrivateData_t
{
    const char* name;
    unsigned int id;
    char* buffer;
    int permissions;
    size_t size;
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