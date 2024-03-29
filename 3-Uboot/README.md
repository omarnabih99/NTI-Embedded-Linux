# U-boot

U-Boot is **the most popular boot loader in** **linux** **based embedded devices**. It is released as opensource under the GNU GPLv2 license. It supports a wide range of microprocessors like MIPS, ARM, PPC, Blackfin, AVR32 and x86.

## Setup U-boot

We need to setup the U-boot bootloader in order to use it on our target machine

### Download U-Boot

```bash
# at the home directory
git clone git@github.com:u-boot/u-boot.git

# Now, you will find the cloned u-boot directory on your home directory
cd u-boot/

# We need to work on a certain version of the u-boot
git checkout v2022.07
```

### **Firstly:** Configure the u-boot using the default configurations 

In this section we will **configure** the u-boot bootloader to run on our target machine.

```bash
# In the u-boot directory, there is a directory called configs/. This directory contains most of target machines default configurations.
ls configs/ | grep <your_machine_name>
```

#### Vexpress Cortex A9 (QEMU)

**Note:** when we build a tool to be loaded to the target machine, we must compile it using the cross compiler.

```bash
# Set the Cross Compiler into the environment to be used by the Makefile of the u-boot
export CROSS_COMPILE=path/to/your/cross/compiler/arm-cortexa9_neon-linux-musleabihf-
export ARCH=arm

# load the target default configuration of ARM Vexpress Cortex A9
make vexpress_ca9x4_defconfig
```

### **Then:** Configure U-boot using menuconfig

In this part we configure the u-boot according to specific requirements in addition to the default configurations

```bash
# open the menuconfig GUI
make menuconfig
```

**The specific requirements are as the following:**

- [ ] Support **editenv**.
- [ ] Support **bootd**.
- [ ] Store the u-boot environment variables inside a file called **uboot.env**.
- [ ] Unset support of **Flash**
- [ ] Support **FAT file system**
  - [ ] Configure the FAT interface to **mmc**
  - [ ] Configure the partition where the fat is stored to **0:1**

### Build the U-boot 

```bash
# go to the u-boot directory
cd ~/u-boot

#build the u-boot files
make
```

## SD Card

Since we use QEMU Emulator, we have to Emulate a SD Card to store the required data. 

In this section it's required to have SD card with first partition to be FAT as configured in the u-boot menuconfig and the second partition to be EXT4.

```bash
# Go to the home directory and create a directory called EmulateSD
cd ~
mkdir EmulateSD

# Go to the EmulateSD directory
cd EmulateSD

#create a file with 1GB size filled with zero data
dd if=/dev/zero of-sd.img bs=1M count=1024
```

Now, we have a virtual image of the SD card, but we want to configure it's partitions.

```bash
# In EmulateSD directory we have sd.img
cfdisk sd.img
```

Configure the SD card partitions as follows

| Partition Size | Partition Format | Bootable  |
| :------------: | :--------------: | :-------: |
|    `512 MB`    |     `FAT 16`     | ***Yes*** |
|    `511 MB`    |     `Linux`      | ***No***  |

Choose **Write** to apply the changes.

Now, we have a virtual SD card image called sd.img divided into two partitions. Fat partition to store the kernel image and dtb file, and the EXT4 partition to store the root file system. We will know more about these files in next explainations.

### Loop the storage driver for virtual use

1. To emulate the sd.img SD card virtual image as a real SD card, we need to attach it to **loop driver** to be as a **block storage**.

2. We need also have to format Partition table for the two partitions in the virtual SD card

To apply these two steps, we will create a script as everytime we restart our host machine we will need to repeat these two steps.

### A script to loop and format partition the virtual SD card

```bash
# Go to the EmulateSD directory
cd ~/EmulateSD

# Create two directories to mount the partitions on them
mkdir sd
mkdir sd2

#create a text file to write the script in it
touch sd_script

#Edit the script
vim sd_script
```

The script to be copied in your sd_script text file:

```bash
#!/bin/bash

# Find the next available loop device
loop_device=`losetup -f`

# Associate the loop device with the image file
sudo losetup --show --partscan $loop_device ~/EmulateSD/sd.img

# Create filesystems on partitions
sudo mkfs.vfat -F 16 -n boot ${loop_device}p1
sudo mkfs.ext4 -L rootfs ${loop_device}p2

# Create mount points and mount partitions
sudo mount ${loop_device}p1 ~/EmulateSD/sd
sudo mount ${loop_device}p2 ~/EmulateSD/sd2

# Set environment variables
export ARCH=arm
export CROSS_COMPILE=~/x-tools/arm-cortexa9_neon-linux-musleabihf/bin/arm-cortexa9_neon-linux-musleabihf-

# start a child shell to inherit the environment variables
bash
```

Then, we must change the text file into **executable file**:

```bash
chmod +x sd_script
```
Then, we run this script:

```bash
./sd_script
```

Now, we have ready to use virtual SD card

We can check the mounted loop:

```bash
lsblk
```
#### Notes regarding the script

1. We use this script as everytime we restart our host machine 'laptop/pc' we must repeat the previous two steps.

2. !!!Be careful!!!, the two mkfs lines will format the SD card. So, we use them as initialization only then cooment them in the script to not to delete your files stored in the SD card every time you run this script after restarting your host machine unless you don't want to format them and delete this data.

3. During working on QEMU you may face EXT4 errors due to data corruption, error in checksum, ...,etc. In this case you have to re-format your partitions using the two mkfs lines in the script and recopy your files into them again.

4. Modify all the paths in the script to match your own paths.

## Test U-boot

In this section, we will test the **u-boot** and **the virtual SD card**

### Prepare the EmulateSD directory

```bash
# Go to your home directory and move the u-boot directory into the EmulateSD directory
cd ~
mv -r u-boot EmulateSD

# Go to the EmulateSD directory then sd directory
cd EmulateSD/sd

# Touch a text file to act as a kernel image just to test the SD card and u-boot
touch zImage

# Touch another text file to act as a dtb file 
touch vexpress.dtb

# Edit these 2 files
echo "Hi, This is the kernel using SD card">>zImage
echo "Hi, this is the dtb using SDcard">>vexpress.dtb

# Go back to the EmulateSD directory
cd ../
```

Start the Qemu with the **Emulated SD card**

```bash
qemu-system-arm -M vexpress-a9 -m 128M -nographic \
-kernel u-boot/u-boot \
-sd sd.img
```















































































