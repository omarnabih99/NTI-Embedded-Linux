# Basic Character Device

Allocating major number statically can lead to errors, system crash, ..., etc. as the static major number maybe used by the kernel to allocate another device.

The solution will be allocating this device dynamically

## Implementation

- We use `alloc_chrdev_region()` to allocate a range of char device numbers. The major number will be chosen dynamically.

- `alloc_chrdev_region()` parameters:

    1. `dev_t *dev`: output parameter for first assigned number

    2. `unsigned baseminor`: first of the requested range of minor numbers

    3. `unsigned count`: the number of minor numbers required

    4. `const char *name`: the name of the associated device or driver

- `alloc_chrdev_region()` returns:

    1. `zero`: success

    2. `-ve value`: fail

- `dev_t` is of type **uint32**. it is used to store the major and minor numbers of the first allocated device. You can extract these number using `MAJOR()` and `MINOR()` macros defined in <linux/kdev_t.h>.

- After inserting the module , you will find the registered major number in:

    ```bash
    cat /proc/devices
    ```
    but we will not find it in /dev. So, we need a method to create the device node under `/dev` automatically wihtout using `mknod` command.

- After allocating a region in the memory we need to define our character device through:

    1. cdev_init() -> Initializes cdev, remembering fops, making it ready to add to the system with cdev_add().

        - Parameters: 
            1. struct cdev *cdev -> the structure to initialize.

            2. const struct file_operations *fops -> the file_operations for this device.
    
        - Returns: 

            - void.

    2. cdev_add() -> adds the device represented by p to the system, making it live immediately. A negative error code is returned on failure.

        - Parameters:

            1. struct cdev *p -> the cdev structure for the device.

            2. dev_t dev -> the first device number for which this device is responsible.

            3. unsigned count -> the number of consecutive minor numbers corresponding to this device.

        - Returns:

            - Negative Error code on failure.

    - These functions are declared in <linux/cdev.h>

- Then, we need to generate a device class in `/sys/class`:

    - class_create() -> This is used to create a struct class pointer that can then be used in calls to device_create().

        - Parameters:

            - const char *name -> pointer to a string for the name of this class.

        - Returns:

            - Returns struct class pointer on success, or ERR_PTR() on error.

- Then, we need to generate a device file (device node):

    - device_create() -> This function can be used by char device classes. A struct device will be created in sysfs, registered to the specified class. A `dev` file will be created, showing the dev_t for the device, if the dev_t is not `0,0`. If a pointer to a parent struct device is passed in, the newly created struct device will be a child of that device in sysfs. The pointer to the struct device will be returned from the call. Any further sysfs files that might be required can be created using this pointer.

        - Parameters: 
            1. const struct class *class -> pointer to the struct class that this device should be registered to.

            2. struct device *parent -> pointer to the parent struct device of this new device, if any.

            3. dev_t devt -> the dev_t for the char device to be added.

            4. void *drvdata -> the data to be added to the device for callbacks.

            5. const char *fmt -> string for the device’s name.

            6. ... -> variable arguments

        - Returns:

            - struct device pointer on success, or ERR_PTR() on error.



- To remove the registered device, we use these function in **the same order**:

    1. `device_destroy()` -> This call unregisters and cleans up a device that was created with a call to `device_create()`.

        - Parameters:

            1. `const struct class *class` -> pointer to the struct class that this device was registered with.

            2. `dev_t devt` -> the dev_t of the device that was previously registered.

    2. `class_destroy()` -> destroys a struct class structure.

        - Parameters:

            1. `const struct class *cls` -> pointer to the struct class that is to be destroyed.

    3. `cdev_del()` -> removes p from the system, possibly freeing the structure itself.

        - Parameters:

            1. `struct cdev *p` -> the cdev structure to be removed

    4. `unregister_chrdev_region`() -> This function will unregister a range of `count` device numbers, starting with `from`.

        - Parameters:

            1. `dev_t from`: the first in the range of numbers to unregister.

            2. `unsigned count`: the number of device numbers to unregister.



