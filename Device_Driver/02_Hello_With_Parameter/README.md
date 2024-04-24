# Hello driver with Parameter

It is the method in which we can pass an argument to the module when we insert it into the kernel

## Implementation

- The passed parameter will be used as a counter to print a welcome message and a goodbye message.

- We can use this method by include <linux/moduleparam.h>.

- We can check the inserted module through `/sys/module`

- We use `module_param(name, type, perm)` to make the kernel understand that this module has an argument to be passed.

- We use `MODULE_PARM_DESC(name, "desc")` to give a description to this parameter

- Parameter:

    1. We will use the passed argument as a parameter controls a for loop.

    2. the for loop will print a welcome and a goodbye messages.

- Permissions:

    We can find the permissions definitions in `linux/stat.h`. and `uapi/linux/stat.h`

    1. Permission controls the visibilty of parameters in /sys/module/<module_name>.

    2. `0` -> the parameters are not visible at all.

    3. You can check the assigned permission by:

    ```bash
    cd /sys/module/<module_name>

    # if the permission is not assigned to zero, you will find parameters directory
    ls

    cd parameters
    
    # you can check your assigned permissions by
    ls -lh
    ```

    4. We can change the parameter during run time according to the assigned permissions without re-compiling the module, in our example we assigned perm to be 0664 to be rw-:rw-:r--. So, we can read it by our user but we must change user to root to modify it:

    ```bash
    # insert the module
    sudo insmod <module_name>.ko <parameter_name>=5

    # check the module output -> you will find that the welcome message will be printed 5 times
    sudo dmesg

    cd /sys/module/<module_name>/parameters

    you will find that the parameter text file contains the value we passed (5)
    cat <parameter_name>

    # switch to root user
    sudo su

    # change the value of the parameter
    echo <desired_value> > <parameter_name>

    # notice the changes -> the goodbye message will be printed according to the desired value entered int the <parameter_name> text file
    sudo rmmod <module_name>
    ```


