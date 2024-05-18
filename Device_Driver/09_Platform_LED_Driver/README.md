# Platform Drivers

Drivers which are in charge of handling Platform Devices.

## Platform Bus

- In Computer Science, a bus is a collection of electrical wirings which transfer information `data`, `address`, or `control` between devices.

- `Platform Bus` is a term used by `Linux` device model to represent all non-discoverable busses of an embedded platform.

- It is a `psuedo` bus or `Linux's virtual bus`. It doesn't have any physical existence.

- It is a `Linux` Terminology representing bus interfaces that do not have `auto discoverable` and `hot plugging` capability.

### PC Scenario

- In `PC` scenario, There is a central bus system which is always based on `PCI` or `PCIe`.

- Linux on `PC` or any another operating system know this bus.

- The devices connected over `PCI` or `PCIe` are actually auto discoverable.

- This bus supports `hot plugging`.

### Embedded Platform Scenario

- The bus interfaces on the SOC or on a microcontroller is vendor specific.

- each vendor has it's own design for interconnection of devices and peripherals with the processor.

- There is no central bus as PCI to discover all on chip as well as off chip devices.

- But there is a vendor specific central bus system through which the processor talks to the peripherals such as: GPIO, UART, I2C, ..., etc.

- The point is that all these bus interfaces on the embedded platform don't support neither hot plugging nor auto discoverable as almost all the on chip peripherals of a SOC in the embedded platform are fixed on the board. So, there is no need to a bus interface like `PCI`.

- `Linux` gives a common name to all these non-discoverable bus which is `Platform Bus`.

- From hardware point of view, the peripherals talks to the processor through the vendor specific on-board bus. But from Linux point of view, all these devices which are non discoverable are connected to the processor via the `Platform Bus`.

- Linux doesn't care about the peripheral type as GPIO, I2C, SPI, ..., etc.

- For Linux, it is just Platform Bus, and the devices which are connected to the platform bus are called `Platform Devices`.

## Platform Devices

- Platform Devices are the non discoverable devices which are connected to the CPU through the Platform Bus. 

- The platform device analogy makes Linux device model design a pretty much simple and also avoides lots of code duplications. 

- The Platform Devices are connected to the CPU over the Platform Bus.

## Discovery of Devices

- Every device has it's configuration data and resources, which need to be reported to the operating system running on the computer system.

- The operating system such as windows or linux, can auto-discover these data. So, the OS learn about the connected devices automatically (Device Enumeration).

- Enumeration is a process through which the OS can inquire and recieve information, such as: the type of the device, the manufacturer, device configuration, and all the devices connected to a given bus.

- Once the OS gathers the information about a device, it can autload the appropriate driver for the device. In PC scenario, buses like PCI and USB support auto enumeration/hot plugging of devices.

- In embedded systems platforms scenario, this may not be the case since most peripherals are connected to the CPU over buses that don't support auto-discovery or enumeration of devices. We can call them Platform Devices

- Most of time the on-chip peripherals are already permanently embedded inside the SOC, there is no requirement of hot plugging.

- All these platform devices which are non-discoverable by nature must be part of the Linux device model. So, the information about these devices must be fed to the Linux kernel manually either at compile time or at boot time of the kernel.

## Device Information

- The Platform Driver is the driver which extracts the device information and configure the platform device properly.

- The device information can be: 

    - Memory or I/O mapped base address and range information.

    - IRQ number on which the device issues interrupt to the processor.

    - Device identification information.

    - DMA channel information.

    - Device address.

    - Pin configuration.

    - Power and voltage parameters.

    - Other device specific data such as: Serial Number and vendor.

## Adding Platform Devices Information to the Kernel

### During Compilation of the Kernel

- Static method.

- Hardware details are part of kernel files.

- Deprecated and not recommended.

### Loading Dynamically

- As a kernel module.

- Not recommended.

### During Kernel Boot

- Device Tree Blob.

- Latest and recommended.
