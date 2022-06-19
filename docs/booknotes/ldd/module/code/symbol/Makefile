ifneq (${KERNELRELEASE},)
obj-m += export.o
obj-m += import.o
else
KDIR ?= /lib/modules/`uname -r`/build

default:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

endif

install:
	sudo insmod export.ko
	sudo insmod import.ko
	tail -n 5 /var/log/kern.log

uninstall:
	sudo rmmod import.ko
	sudo rmmod export.ko
	tail -n 5 /var/log/kern.log

show:
	modinfo export.ko
	modinfo import.ko
	cat /proc/kallsyms | grep hello_export


