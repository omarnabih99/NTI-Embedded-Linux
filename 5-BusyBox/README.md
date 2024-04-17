# BusyBox

**BusyBox** is a software suite that provides a single binary executable file that contains many common Unix utilities. It is designed to be small and efficient, and is often used in embedded systems or other systems with limited resources.

## Download BusyBox

```bash
# Go to Home directory
cd

# Clone busy box repo
git clone https://github.com/mirror/busybox.git

# Go to the busy box directory
cd busybox
```

## Configure BusyBox

```bash
make menuconfig
```

## Compile BusyBox 

```bash
# export the compiler on the system
# chose the right compiler that corespond to your board
export CROSS_COMPILE=arm-cortexa9_neon-linux-musleabihf-
export ARCH=arm

# Configure busybox to be static build from menuconfig
make menuconfig

#build the busybox to get the minimal command
make

# generate the rootfs
make install
# this will create folder name _install has all binaries
```
## Create the ROOTFS

```bash
# Go to home directory
cd 

# create directory rootfs
mkdir rootfs

# go to rootfs directory
cd rootfs

# copy the content inside the _install into rootfs
cp -r ~/busybox/_install/* ~/rootfs

# copy the sysroot contents in the rootfs directory
cp -r ~/x-tools/arm-cortexa9_neon-linux-musleabihf/arm-cortexa9_neon-linux-musleabihf/sysroot/* ~/rootfs

# create the rest folders for rootfs
mkdir etc dev mnt home proc sys
```

## Configure inittab
- BusyBox operates on BusyBox init which is a simple systemd init.

- We need to configure the BusyBox init.

```bash
# Go to rootfs directory
cd ~/rootfs

# go to etc directory
cd etc

# create the inittab text file
touch inittab

# edit the inittab file
vim inittab
```

- Copy these lines into the inittab file: 

```bash
#execute rcS1 when the system start
::sysinit:/etc/init.d/rcS

#ask to enter the shell
console::askfirst:-/bin/sh

#Execute when the system will go down
::shutdown:/etc/init.d/rcK
```
- Create init.d directory: 

```bash
# create the init.d directory
mkdir init.d
```

- Create rcS and rcK scripts

```bash
# go to init.d directory
cd init.d

# create rcS text file
touch rcS

# create rcK text file
touch rcK
```

- Copy these lines in rcS script

```bash
#!/bin/sh

#mount proc and sys
mount -t proc proc /proc
mount -t sysfs sysfs /sys

#print the welcome message
echo "rcs1 is on, Hi everyone, it is Omar Nabih"
```

- Copy these lines in rcK script

```bash
#!/bin/sh

echo "shutdown deamon app"
```

- Make these scripts excuteable

```bash 
sudo chmod +x rcS

sudo chmod +x rcK
```
## Change the ownership of the rootfs directory

- As there is no user on the target device `QEMU`. We must change the ownership to be root instead of `your username`.

- You can check the ownership of the rootfs directory:

```bash
cd ~
ls -lh | grep "rootfs"
```

- You will notice that the ownership belongs to your user.

- You can change the ownership of the rootfs directory through:

```bash
sudo chown -R root:root rootfs
```

## Mount rootfs through virtual SD Card

```bash
# go to EmulateSD directory
cd ~/EmulateSD

# run the SD script
./sd_script

# copy the rootfs directory contents into sd2
cp -r ~/rootfs/* ~/EmulateSD/sd2

# Unmount the virtual SD card
sudo umount /dev/loop<loop_number>p1
sudo umount /dev/loop<loop_number>p2

# replace <loop_number> with your specified virtual SD card loop number
# you can check the loop number through lsblk command
# this will show you all the attached devices to your machine
lsblk
```
## Run Qemu

```bash
# Go to EmulateSD directory
cd ~/EmulateSD
# run QEMU
qemu-system-arm -M vexpress-a9 -m 128M -nographic \
-kernel u-boot/u-boot \
-sd sd.img
```

## Set Bootargs un U-boot

```bash
# console is set depends on the machine
setenv bootargs 'console=ttyAMA0 root=/dev/mmcblk0p2 rootfstype=ext4 rw rootwait init=/sbin/init'

# save the environment variable
saveenv

# load the zImage and dtb files
fatload mmc 0:1 $kernel_addr_r zImage
fatload mmc 0:1 $fdt_addr_r vexpress-v2p-ca9.dtb

# boot the virtual machine
bootz $kernel_addr_r - $fdt_addr_r
```






