MODULE_NAME := book

# 如果已经定义KERNELRELEASE，说明当前任务在内核构造系统中，可利用内建语句
ifneq (${KERNELRELEASE},)

obj-m := $(MODULE_NAME).o

else

# 当前系统的内核源代码目录
KDIR ?= /lib/modules/`uname -r`/build

# -C : 改变目录到指定的位置(即内核源代码目录)，其中保存有内核的顶层makefile文件
# M= : 在构造"modules"目标之前返回到模块源代码目录，并指向obj-m变量中设定的模块.o文件
default:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

endif

install:
	sudo insmod $(MODULE_NAME).ko book_name="Good_Book" book_num=1000
	tail -n 5 /var/log/kern.log
	cat /sys/module/book/parameters/book_name
	cat /sys/module/book/parameters/book_num

uninstall:
	sudo rmmod $(MODULE_NAME).ko
	tail -n 5 /var/log/kern.log

# lsmod : show "/proc/modules"
show:
	modinfo ./$(MODULE_NAME).ko
	cat /proc/modules | grep $(MODULE_NAME)
	ls /sys/module/$(MODULE_NAME)


