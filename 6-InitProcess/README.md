# Init Process

- It is the First program run by the kernel after booting up.

- There are many possible implementations of `init`.

- We will describe the three main ones:

    1. BusyBox Init.

    2. System V init.

    3. systemd.

## After The Kernel has booted

- The kernel bootstrap code seeks to find a root file system which is either initramfs or a filesystem specified by `root=`.

- Then it executes a program that is by default is `/init` for initramfs and `/sbin/init` for a regular file system.

- The `init` program has **root** privilege.

- It is the first process to run. So, it has a **PID**: 1.

- If -for some reason- `init` cannot be started, the kernel will panic.

- The `init` program is the ancestor of all other processes:

    ```bash
    pstree -gn
    ```

- The job of the init program is to take control of the boot process in user space and set it running.

- It may be as simple as a shell command running a shell script.

-  In the majority of cases, you will be using a dedicated init daemon to perform the following tasks:

    1.  `During boot`, after the kernel transfers control, the init program starts other daemon programs and configures system parameters and other things needed to get the system into a working state.

    2. Optionally, it launches a login daemon, such as getty, on terminals that allow a login shell.

    3. It adopts processes that become orphaned as a result of their immediate parent terminating and there being no other processes in the thread group.

    4. It responds to any of the `init's immediate children` terminating by catching the `SIGCHLD` signal and collecting the return value to prevent them from becoming zombie processes.

    5. `Optionally`, it restarts those daemons that have terminated.

    6. It handles the system shutdown.

- `init` manages the life cycle of the system from boot up to shutdown.

