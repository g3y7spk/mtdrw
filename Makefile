KERNEL_SRC = /home/atmark/linux-2.6.26-at27

ARCH = arm
CROSS_COMPILE = arm-linux-gnueabi-

obj-m += mtdrw.o
clean-files += modules.order

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KERNEL_SRC) M=$(PWD) clean
