MODULE_NAME := gmem
MODULE_MAJOR := 111

ifneq (${KERNELRELEASE},)
obj-m := $(MODULE_NAME).o
else

KDIR ?= /lib/modules/`uname -r`/build

default:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

endif

install:
	sudo insmod $(MODULE_NAME).ko
	sudo rm -rf /dev/gmem*
	sudo mknod /dev/gmem0 c $(MODULE_MAJOR) 0
	sudo mknod /dev/gmem1 c $(MODULE_MAJOR) 1
	sudo chown `users` /dev/gmem*

uninstall:
	sudo rm -rf /dev/gmem*
	sudo rmmod $(MODULE_NAME).ko
	tail -n 5 /var/log/kern.log

test:
	echo "hello gmem0" > /dev/gmem0
	echo "hello gmem1" > /dev/gmem1
	cat /dev/gmem0 && cat /dev/gmem1

