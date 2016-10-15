#!/bin/sh

obj-m += Button_Interrupt.o

KDIR := ../../ITOP4412_linux_Kernel_3.0/iTop4412_Kernel_3.0
LOCAL_CC := arm-none-linux-gnueabi-gcc

PWD ?= $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules
#	${LOCAL_CC} -o leds_invoke leds_invoke.c
	echo ${LOACL_CC}
	

clean:
	rm -rf *.o *.ko *.order *.symvers *.mod.*
