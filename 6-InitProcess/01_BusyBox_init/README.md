# BusyBox init

- **BusyBox** has a minimal init program that uses a configuration file, /etc/inittab, to define rules to start programs at bootup and to stop them at shutdown.

- The actual work is done by shell scripts found in `/etc/init.d` directory.

- `init` begins by reading `/etc/inittab`. This file contains a list of programs to run, one per line, in this format:

    ```bash
    <id>::<action>:<program>
    ```
    - **id**: This is the controlling terminal for the command.
    
    - **action**: This includes the conditions to run this command.

    - **program**: This is the program to run.

- The **actions**:

1. `sysinit`: Runs the program when init starts before any of the other types of actions.

2. `respawn`: Runs the program and restarts it if it terminates. It is typically used to run a program as a daemon.

3. `askfirst`: This is the same as respawn, but it prints the message `Please press Enter to activate this console` to the console, and it runs the program after Enter has been pressed. It is used to start an interactive shell on a terminal without prompting for a username or password.

4. `once`: Runs the program once but does not attempt to restart it if it terminates.

5. `wait`: Runs the program and waits for it to complete.

6. `ctrlaltdel`: Runs the program when init receives the SIGINT signal, usually as a result of pressing Ctrl + Alt + Del on the console.

## Writing simple BusyBox init scripts

- Developing the BusyBox init is done on the developing machine in the created rootfs and then this rootfs is loaded on the target machine.

1. we need the `inittab` file to direct the `init` program

    ```bash
    # go to /etc in the rootfs you created
    cd /etc

    # create the inittab text file
    touch inittab
    ```
    Copy the following lines in the inittab file :

    ```bash
    #execute rcS when the system start
    ::sysinit:/etc/init.d/rcS

    #ask to enter the shell
    console::askfirst:-/bin/sh

    #Execute when the system will go down
    ::shutdown:/etc/init.d/rcK
    ```
2. we need to implement the `rcS` shell script

    ```bash
    # while you are in the /etc directory
    pwd

    # create a directory called init.d
    mkdir init.d

    # go to init.d directory
    cd init.d

    # create rcS text file
    touch rcS
    ```

    Copy these lines in the rcS text file

    ```bash
    #!/bin/sh

    #mount proc and sys
    mount -t proc proc /proc
    mount -t sysfs sysfs /sys

    #print the welcome message
    echo "rcS is on, Hi everyone, it is Omar Nabih"
    ```

3. We need to implement rcK

    ```bash
    # while you are in the /etc/init.d directory
    pwd

    # create rcK text file
    touch rcK
    ```
    Copy these lines in the rcK text file

    ```bash
    #!/bin/sh

    echo "Goodbye!"
    ```

- Now, we have a simple BusyBox init files. you can now load the rootfs into the target machine and notice the output on booting.

## Typical init scripts: Buildroot init scripts

- Buildroot has two scripts in `/etc/init.d/` named `rcS` and `rcK`.

- `rcS`: The first one runs at bootup and iterates over all the scripts in /etc/init.d/ with names that begin with a capital S followed by two digits, and runs them in numerical order. These are the start scripts. 

- `rcK`: The rcK script is run at shutdown and iterates over all the scripts beginning with capital K followed by two digits and runs them in numerical order. These are the kill scripts.

- If you are using Buildroot, this will be clear. If not, you could use it as a model for writing your own BusyBox init scripts.








