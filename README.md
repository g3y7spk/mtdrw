## About

mtdrw is a Linux loadable kernel module to make the read-only MTD device writable while the module is loaded.

This module is for Atmark Techno's Armadillo-4x0 series CPU board running Linux v2.6.26.

* http://armadillo.atmark-techno.com/armadillo-420
* http://armadillo.atmark-techno.com/armadillo-440

You don't need this module when you are using newer Linux v3.14 for Armadillo-4x0 because the MTD driver of Linux v3.14 offers the sysfs interface to do the same like:

``` r
# echo 0 > /sys/class/mtd/mtd0/ro
```

Using this module, you can update the bootloader of Armadillo-4x0 using netflash command from the userland. The bootloader of Armadillo-4x0 is written in the first partition of the NOR flash, but the first partition of the NOR flash is marked as read-only and usually cannot be written from the userland.

## Build

The build of mtdrw should be done on ATDE5. ATDE5 is a cross compilation environment for Armadillo-4x0 running on the VMware virtual machine. Please set up ATDE5 before doing the build. For detail please see <http://armadillo.atmark-techno.com/atde>.

First build the kernel for Armadillo-4x0.
``` r
$ cd
$ wget http://armadillo.atmark-techno.com/files/downloads/armadillo-420/source/kernel/linux-2.6.26-at27.tar.gz
$ tar -zxvf linux-2.6.26-at27.tar.gz
$ cd linux-2.6.26-at27
$ make ARCH=arm armadillo400_defconfig
$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi-
```

Then build mtdrw.
``` r
$ cd
$ git clone https://github.com/g3y7spk/mtdrw.git
$ cd mtdrw
$ make
```
After build, you can find the loadable kernel module (mtdrw.ko) in the current directory.

NOTE: If you are using the different version of kernel then change KERNEL_SRC in mtdrw/Makefile.

``` r
KERNEL_SRC = /home/atmark/linux-2.6.26-at27

ARCH = arm
CROSS_COMPILE = arm-linux-gnueabi-

obj-m += mtdrw.o
clean-files += modules.order

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_SRC) M=$(PWD) clean
```

## Usage
mtdrw has one parameter called *name* and you should specify the name of MTD device with it. For Armadillo-4x0, the following names are assigned to MTD devices.

* nor.bootloader
* nor.kernel
* nor.userland
* nor.config

The command sequence for updating the bootloader of Armadillo-4x0 is illustrated below as an example. You need to execute commands on Armadillo-4x0. Assume that mtdrw.ko and loader-armadillo4x0-v2.2.0.bin are both located in /tmp directory on Armadillo-4x0. mtdrw.ko is the loadable kernel module you just built and loader-armadillo4x0-v2.0.0.bin is the bootloader image of Armadillo-4x0 available at <http://armadillo.atmark-techno.com/armadillo-420/downloads-linux2-6-26>.

``` r
# cd /tmp
# insmod ./mtdrw.ko name=nor.bootloader
Using ./mtdrw.ko
mtd0 is writeable now
# netflash -b -k -n -u -s -r /dev/flash/bootloader ./loader-armadillo4x0-v2.2.0.bin
# rmmod mtdrw
mtd0 is read-only again
```
