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

![Arch]()


