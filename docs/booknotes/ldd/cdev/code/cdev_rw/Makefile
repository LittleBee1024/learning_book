MODULE_NAME := gmem

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
	cat /proc/devices | grep gmem

uninstall:
	sudo rmmod $(MODULE_NAME).ko
	tail -n 3 /var/log/kern.log

# lsmod : show "/proc/modules"
show:
	modinfo ./$(MODULE_NAME).ko
	cat /proc/modules | grep $(MODULE_NAME)
	ls /sys/module/$(MODULE_NAME)
