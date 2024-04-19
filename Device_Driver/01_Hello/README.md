# Hello World

This is `pseudo` device driver i.e. a driver for a virtual device.

## Implementation

- Kernel has a database in order to deal with device drivers and recognize them.

- To insert into this database we use:

    1. `module_init()`: takes a pointer to function as an argument. The passed function will be the first task the kernel will run when the module is inserted into the system.

    2. `module_exit()`: takes a pointer to function as an argument. The passed function will be the last task the kernel will run when the module is exited from the system.

- When we implement a function to be passed to `module_init()` or `module_exit()`, we can tell the kernel to remove the function section from the memory after the function has finished it's task through **__init** and **__exit**

- We add `Meta Information` for the module: it's strings added to the database to clarify the module.

    1. `MODULE_LICENSE`: allows the kernel differentiates whether the module uses a license or not.

    2. `MODULE_AUTHOR`: Stores the the module author name/e-mail.

    3. `MODULE_DESCRIPTION`: Stores the module functionality.

## Compilation

- We can't use the `User Space` compiler to compile the module.

- We will use the `Kernel Space` Makefile.

- Our created Makefile will direct to the `Kernel Makefile` found in /usr/src/linux-hwe-6.5-headers-6.5.0-27

- In Makfile:

    - `obj-m += <module_name.o>`: states that <module_name> is a module which runs during the **run time**.

- To compile the module:
    ```bash
    # go to the module directory
    cd ~/path/to/the/module

    # compile the module
    make
    ```
- After compilation has finished, we will get some files. One of these files has extension `.ko`. This is kernel module object file.

## Commands to be used with the Kernel Modules

1. `lsmod`: lists all the running modules.

2. `insmod`: to insert a module.

3. `modprobe`: same as `insmod`.

4. `rmmod`: deletes a module.

5. `modinfo`: lists a module information.

## Insert a compiled module

- To insert a kernel module we use `insmod <module_name> while we are in the module directory

```bash
# go to the module directory
cd ~/path/to/the/module

# insert the module .ko file
# sudo is required to insert module to the kernel
sudo insmod <module_name.ko>

# to see the module output
sudo dmesg
```



