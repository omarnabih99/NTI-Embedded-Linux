# Linux Kernel

The Linux kernel is the core component of the Linux operating system, responsible for managing system resources such as memory, processes, input/output (I/O), and device drivers. It is an open-source software project that was initially developed by Linus Torvalds in 1991 and is now maintained by a large community of developers around the world.

## Download Linux Kernel

### Vexpress (Qemu)

```bash
git clone --depth=1 git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
cd linux

#configure the kernel to vexpress
make ARCH=arm vexpress_defconfig

# To indentify your kernel version 
make ARCH=arm kernelversion
```

## Configure Kernel

For all the next board this configuration must be checked

- [ ] Enable **devtmpfs**
- [ ] Change kernel compression to **XZ**
- [ ] Change your kernel local version to your name and append on it <youName>-v1.0

### Vexpress (Qemu)

```bash
#export the compiler
export CROSS_COMPILE=PathToCompiler/arm-linux-cortexa9Compiler
#export the architecture used
export ARCH=arm

#configure the kernel with the configuration discuss above
make menuconfig

#build the kernel
make -j4 zImage modules dtbs
```

## Load the Kernel into the virtual board memory

### Initialize the virtual SD Card

```bash
# Go to EmulateSD directory
cd ~/EmulateSD

# Run the SD card script
./sd_script
```
### Copy the zImage file and dtb files into the virtual SD Card

```bash
# Go to this path to get the zImage
cd ~/linux/arch/arm/boot

# Copy the zImage into the boot partition of the virtual SD Card
cp zImage ~/EmulateSD/sd

# Go to this path to get the dtb file
cd ~/linux/arch/arm/boot/dts/arm

# Copy the dtb file corresponding to our virtual board which is vexpress-a9
cp vexpress-v2p-ca9.dtb ~/EmulateSD/sd
```

### Run Qemu
```bash
sudo qemu-system-arm -M vexpress-a9 -m 128M -nographic \
-kernel u-boot/u-boot \
-sd sd.img
```
### Load the zImage and dtb files through the virtual SD Card

Now, we want to load these files-zImage and dtb-into the virtual board memory:

```bash
fatload mmc 0:1 $kernel_addr_r zImage
fatload mmc 0:1 $fdt_addr_r vexpress.dtb
```
Then, we want to check that the files are loaded to the memory sucessfulyy:

```bash
md $kernel_addr_r
md $fdt_addr_r
```
These two commands will dispaly the memory starting from the given address to a certain size showing the data written in these addresses.

Now, we have the files loaded into the memory. We want to boot the kernel.

```bash
bootz $kernel_addr_r - $fdt_addr_r
```
########## THE KERNEL WONT BOOT IT WILL **PANIC** ###########

### Load the zImage and dtb files through TFTP

#### Initialize TFTP protocl

**!!! Skip this step if you have done it before !!!**

```bash
#Switch to root
sudo su
#Make sure you are connected to internet
ping google.com
#Download tftp protocol
sudo apt-get install tftpd-hpa
#Check the tftp ip address
ip addr `will be needed`
#Change the configuration of tftp
nano /etc/default/tftpd-hpa
	#write inside the file
    tftf_option = “--secure –-create”
#Restart the protocal
Systemctl restart tftpd-hpa
#Make sure the tftp protocol is running
Systemctl status tftpd-hpa
#Change the file owner
cd /srv
chown tftp:tftp tftp 
```
#### Copy the zImage and dtb files

```bash
# Go to this path to get the zImage
cd ~/linux/arch/arm/boot

# Copy the zImage into the boot partition of the virtual SD Card
cp zImage ~/srv/tftp

# Go to this path to get the dtb file
cd ~/linux/arch/arm/boot/dts/arm

# Copy the dtb file corresponding to our virtual board which is vexpress-a9
cp vexpress-v2p-ca9.dtb /srv/tftp
```
#### Create a virtual Ethernet for QEMU

**!! This section for `QEMU` Emulator using only !!**

```bash
# Go to EmulateSD directory
cd ~/EmulateSD
# Create the qemu-ifup text file
touch qemu-ifup
```
Copy this script into the text file
```bash
#!/bin/sh
ip a add 192.168.0.1/24 dev $1
ip link set $1 up
```
Make the text file executable
```bash
sudo chmod +x qemu-ifup
```

**!! You can assign any ip address you want without modyfing the net mask, but make sure that the ip address of the `QEMU` will have the same network ID (first three fields in this ip address). ex: 192.168.0.2/24 !!**

#### Start Qemu

In order to start `QEMU` with the new virtual Ethernet.

```bash
# Go to EmulateSD directory
cd ~/EmulateSD
# Start Qemu
sudo qemu-system-arm -M vexpress-a9 -m 128M -nographic \
-kernel u-boot/u-boot \
-sd sd.img \
-net tap,script=./qemu-ifup -net nic
```

Now, `QEMU` has started. So, we need to configure two environment variables: ipaddr and serverip.

`ipaddr` stores the ip address of the u-boot.

`serverip` stores the ip address of the server where the files are stored.

```bash
#Apply ip address for embedded device
setenv ipaddr [chose]
#Set the server ip address that we get from previous slide
setenv serverip [host ip address]
```
#### WARNING ####
#the ip address should has the same net mask

#### Load the files through TFTP

Now we have the embedded device connected to the `QEMU` virtual Ethernet.

```bash
# addressRam is a variable known from bdinfo commend
tftp $kernel_addr_r zImage
tftp $fdt_addr_r vexpress-v2p-ca9.dtb
```
The two file are supposed to be loaded in the memory. We can check that:

```bash
md $kernel_addr_r
md $fdt_addr_r
```

These two commands will dispaly the memory starting from the given address to a certain size showing the data written in these addresses.

Now, we have the files loaded into the memory. We want to boot the kernel.

```bash
bootz $kernel_addr_r - $fdt_addr_r
```
########## THE KERNEL WONT BOOT IT WILL **PANIC** ###########
