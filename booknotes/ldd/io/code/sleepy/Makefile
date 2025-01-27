MODULE_NAME = sleepy

ifneq (${KERNELRELEASE},)

obj-m := $(MODULE_NAME).o

else

KDIR ?= /lib/modules/`uname -r`/build

default:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

endif

insmod:
	sudo insmod $(MODULE_NAME).ko

MODULE_MAJOR = $(shell awk '$$2=="${MODULE_NAME}" {print $$1}' /proc/devices)
install: insmod
	sudo mknod /dev/sleepy c $(MODULE_MAJOR) 0
	sudo chown `users` /dev/sleepy

uninstall:
	sudo rm -rf /dev/sleepy
	sudo rmmod $(MODULE_NAME).ko

sleep:
	cat /dev/sleepy

wake:
	echo 'wake up' > /dev/sleepy

