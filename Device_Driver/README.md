# Device Driver

A device driver is a special kind of `software program` that controls a `specific hardware device` attached to a computer.

## How does it work 

**User Space** (file under /dev for example) -> **Sys Call** (open, close, read, write) -> **Kernel Space** (driver) -> **Hardware** (device).

## Types of devices

1. **Character**: These devices transmit data character by character like a mouse or a keyboard. ex) Sensors, motors, LCDs, ..., etc.

2. **Block**: These devices are used to store data. ex) eeprom, hard disk, ..., etc.

3. **Network**: These devices used in networking. ex) WiFi, Bluetooth, ..., etc.

**Note**: On-board communication protocols as UART we can deal with it as a `Character device`.

## Types of driver

### Static in tree 

Loaded in `booting time`

### Module Dynamic

Loaded in in `User Space`.

## Loading Steps

1. ROM bootloader.

2. First Stage bootloader. (as MLO)

3. Second Stage Bootloade. (as U-boot)

4. Kernel Booting.

    The Kernel starts, and uses it's built-in drivers (`Static in tree`) and those are loaded by the loader to find the system disk(s), mount the root file system, and start /sbin/init.

5. Init process (`User Space`)

6. Kernel starts loading modules (`Module Dynamic`).

## Archeticture

- Device Driver codes are compiled by the kernel `Makefile` not the `User Space` compiler.

- Kernel by default doesn't see the GNU C Library

- That means we can't -for example- write `printf()` in `kernel space drivers`.

- But linux as a distro covers everything in GNU C Library by creating a version of it for the `Kernel Space`.

- So, instead of using -for example- `printf()`, we will use `printk()`.

- Therefore, we won't use anything from GNU C Library.

    ![Arch](image.png)

## Pseudo devices

- They are files usually located in `/dev`. They are like a device files, but instead of acting as a bridge between the `Operating System` and `Hardware`.

- They don't really exist as real devices.

## Kernel Headers

- Linux kernel headers are the essential files needed for compiling and linking programs that interface directly with the Linux kernel. They contain declarations of structures, constants, and functions that user-space programs can use to interact with the kernel. These headers define the interfaces that applications, device drivers, and other kernel modules use to communicate with the kernel.

- Having the kernel headers installed is especially important when compiling certain software packages, like device drivers or system-level utilities, as they often need to interact with the kernel directly. Without the appropriate headers, compiling such software would not be possible, or it might result in errors due to missing function declarations or constant definitions.

### Installing Kernel Headers

```bash
sudo apt get-update
sudo apt install linux-headers-$(uname -r)
```
### Locating installed Kernel Headers

- You can know your linux version through: 

```bash
uname -r
```

- You will find the installed headers according to your linux version:

```bash
# depends on your linux version
cd /usr/src/linux-headers-6.5.0-27-generic/include
```

- You will notice that the directories in this path are symlinked to directories in another path which is `linux-hwe-6.5-headers-6.5.0-27/include/`

```bash
# In /usr/src/linux-headers-6.5.0-27-generic/include
ls -lh
```
for example: `linux -> ../../linux-hwe-6.5-headers-6.5.0-27/include/linux`

- `include/linux`: This directory contains general-purpose kernel headers, including those related to core kernel functionality.

- `include/uapi/linux`: This directory contains headers defining user-space APIs, including those related to device drivers that user-space programs may interact with.

- Subdirectories corresponding to specific subsystems: For example, headers related to networking would be found in `include/net`, while headers related to block devices might be found in `include/block`.


## Character Devices

- Character devices are identified in the user space by a special file called a `device node`.

- This filename is mapped to a device driver using the `major` and `minor` numbers associated with it.

- Broadly speaking, the `major number` maps the `device node` to a particular `device driver`, while the `minor number` tells the `driver` which `interface is being accessed`.

- Example:

```bash
ls -lh /dev/tty*
```

- This will show you device nodes starting with tty.

- You will find for example:

    crw--w---- 1 root      tty     `4`,  `0` Apr 22 21:07 /dev/tty0

    crw--w---- 1 root      tty     `4`,  `1` Apr 22 21:07 /dev/tty1

    crw--w---- 1 root      tty     `4`, `10` Apr 22 21:07 /dev/tty10

    crw--w---- 1 root      tty     `4`, `11` Apr 22 21:07 /dev/tty11

- The device node `tty0` has major number of `4` and minor number of `0`. The device node for `tty1` has major number of `4` and minor number of `1` and so on.

- This means that these device nodes which have the same `major number` are handleld by the same device driver.

- `Major numbers` are **12-bits** long.

- `Minor Numbers` are **20-bits** long.

### Finding out about drivers at run time

- Once you have a running Linux system, it is useful to know which device drivers have been loaded and what state they are in. You can find out a lot by reading the files in `/proc` and `/sys`.

#### Getting information from procfs

- you can list the character and block device drivers that are currently loaded and active by reading /proc/devices:

```bash 
cat /proc/devices
```
- For each driver, you can see the major number and the base name. 

- This does not tell you how many devices each driver is attached to.

#### Getting information from sysfs

- You can define `sysfs` in a pedantic way as a representation of kernel objects, attributes, and relationships.

    1. A kernel object is a `directory`.

    2. An attribute is a `file`.

    3. A relationship is a `symbolic link from one object to another`.

- From a more practical point of view, since the Linux device driver model represents all devices and drivers as kernel objects, you can see the kernel's view of the system laid out before you by looking in /sys:

```bash
ls /sys
```

- In the context of discovering information about devices and drivers, I will look at these directories: `devices`, and `class`.

##### The devices: /sys/devices

- This is the kernel's view of the devices that have been discovered since boot and how they are connected to each other.

```bash
ls /sys/devices
```

- There are three directories that are present on all systems:

    1. **system/**: This contains devices at the heart of the system, including CPUs and clocks.

    2. **virtual/**: This contains devices that are memory-based. You will find the memory devices that appear as `/dev/null`, `/dev/random`, and `/dev/zero` in `virtual/mem`. You will find the `loopback` device, lo, in `virtual/net`.

    3. **platform/**: This is a catch-all for devices that are not connected via a conventional hardware bus. This may be almost everything on an embedded device.

- The other devices appear in directories that correspond to actual system buses. For example, the PCI root bus, if there is one, appears as pci0000:00.

- Navigating this hierarchy is quite hard as the pathnames become quite long and hard to remember.

- ex) to examine tty0:

```bash
cd /sys/devices/virtual/tty/tty0
```

- To make life easier, /sys/class offers different view of the devices.

##### The drivers: /sys/class

- This is a view of the device drivers presented by their type.

- It is a software view rather than a hardware view which is found at /sys/devices.

- Each of the subdirectories represents a class of driver.

- For example, UART devices are managed by the tty layer, and you will find them in `/sys/class/tty`.

```bash 
cd /sys/class/tty
ls
```

- You will find input devices such as the keyboard, the touchscreen, and the mouse in `/sys/class/input`, and so on.

```bash
cd /sys/class/input
ls
```

- You can notice that there is a symlink in each subdirectory for each instance of that type of device pointing to its representation in `/sys/device`.

- To take a concrete example,  let's look at the serial ports on our machine.

```bash
ls -d /sys/class/tty/tty*
```

- You will find many of them.

- Each directory is a representation of the kernel object that is associated with an instance of a device interface.

- Looking within one of these directories:

```bash 
cd /sys/class/tty
ls
```
- Each directory is a representation of the kernel object that is associated with an instance of a device interface.

- Looking within one of these directories:

```bash 
cd /sys/class/tty/tty0
ls
```

- You will find `files`, and `links`.

- We can see the attributes of the object, represented as files.

- The relationships with other objects, represented by links.

    1. The link called `device` points to the hardware object for the device.

    2. The link called subsystem points back to the parent subsystem, /sys/class/tty.

    3. Some attribute files are writable and allow you to tune parameters in the driver at runtime.

- You can find `dev` attribute. This attribute is particularly interesting. If you look at its value, you will find the `Major` and `Minor` numbers of this device.

```bash
cat /sys/class/tty/tty0/dev
```

- This attribute is created when the driver registers this interface. It is from this file that udev and mdev find the `major` and `minor` numbers of the device driver.





















