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
