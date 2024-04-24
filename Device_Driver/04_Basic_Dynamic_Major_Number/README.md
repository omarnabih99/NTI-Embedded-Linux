# Basic Dynamic Major Number

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

- To remove the allocated device, we use `unregister_chrdev_region()`

- `unregister_chrdev_region()` parameters:

    1. `dev_t from`: the first in the range of numbers to unregister.

    2. `unsigned count`: the number of device numbers to unregister.