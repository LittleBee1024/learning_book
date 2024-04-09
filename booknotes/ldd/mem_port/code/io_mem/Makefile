MODULE_NAME := short
MODULE_MAJOR := 109

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
	sudo rm -rf /dev/short*
	sudo mknod /dev/short0 c $(MODULE_MAJOR) 0
	sudo mknod /dev/short1 c $(MODULE_MAJOR) 1
	sudo chown `users` /dev/short*

uninstall:
	sudo rm -rf /dev/short*
	sudo rmmod $(MODULE_NAME).ko
	tail -n 5 /var/log/kern.log

# https://www.linuxquestions.org/questions/programming-9/doubt-about-physical-dir-and-virtual-dir-691234/
# When reading an IO address which has no associated decoding logic, the usual behavior will be a reading of 'FF' as the bus floats high to a logic '1' on all bits
test:
	echo "a" > /dev/short0
	echo "b" > /dev/short1
	dd if=/dev/short0 bs=1 count=1 && dd if=/dev/short1 bs=1 count=1

