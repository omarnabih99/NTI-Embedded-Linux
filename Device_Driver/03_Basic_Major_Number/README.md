# Basic Major Number Driver

Broadly speaking, the `major number` maps the `device node` to a particular `device driver`.

## Implementation

- We can check the devices `major numbers` on our machine through:

    1. cat /proc/devices

    2. ls -lh /dev

    3. through `dev` file in /sys/class/<device_name>

- We need to include `<linux/fs.h>

- We need to register a major number for the driver, we have 2 methods:

    1. automatically through `allocate` function.

    2. manually through `register_chrdevdev` function.

- Here, we will use the manual method.

- Manual method depends on searching for an available `major number` which is not used by another device.

```bash
cat /proc/devices
```

- `register_chrdev()` arguments:

    1. The major number of type unsigned int.

    2. Device name as a string literal.

    3. address to file_operations struct.

- `register_chrdev()` returns:

    1. If major == 0 this functions will dynamically allocate a major and return its number.

    2. If major > 0 this function will attempt to reserve a device with the given major number and will return zero on success.

    3. Returns a -ve errno on failure.

- To free the registered `major` number, we will use `unregister_chrdev()`

- `unregister_chrdev()` arguments:

    1. The major number of type unsigned int.

    2. Device name as a string literal.

- `register_chrdev()` returns:

    - void.

- After inserting the module with the available major number as a parameter, you will find the registered major number in:

    ```bash
    cat /proc/devices
    ```
    but we will not find it in /dev

- Manual method also depends on manually creating a file under /dev with a major number equals to the major number in our module.

    ```bash 
    # this command takes: name of the device, type of the device, maj number, minor number
    sudo mknod /dev/manual_test c 50 0
    ```
- Now, you can find the manually created device file under /dev with major number 50.

- try to read or write to this file

    ```bash
    cat /dev/manual_test
    ```

    - It will give an error **Invalid argument** as we didn't implement the `read` function.

    - But in the same time, open and close syscalls are invoked. you can check it through the dmesg:

    ```bash 
    sudo dmesg
    ```
    you will notice that the print messages in the implemented my_open() and my_release() functions are printed.

    - Same will happen when trying to write on this file as the write() is not implemented.

- When we remove this module, the `unregister_chrdev` will be called which removes the major number from the memory. we can check this through:

    ```bash
    cat /proc/devices
    ```
    - but we can still find it under /dev.

    ```bash
    ls -lh /dev/manual_test
    ```
    - This can be solved when we use the automatic method.

- Try to insert the module with a already taken major number
    
    1. pick a major number for any device:

    ```bash
    cat /proc/devices
    ```

    2. insert the module with this number.

    3. You will notice an error on the terminal: Operation not perimitted.

    4. You can notice the error in the kernel through:

    ```bash
    sudo dmesg
    ```
    you will see that the failure if condition in the code is satisfied printing device is not successfully registered.

