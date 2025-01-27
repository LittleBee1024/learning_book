MODULE_NAME = gmemp

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
	sudo mknod /dev/$(MODULE_NAME) c $(MODULE_MAJOR) 0
	sudo chown `users` /dev/$(MODULE_NAME)

uninstall:
	sudo rm -rf /dev/$(MODULE_NAME)
	sudo rmmod $(MODULE_NAME).ko
