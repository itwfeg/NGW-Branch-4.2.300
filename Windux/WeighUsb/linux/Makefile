##############################################################
#
# File : makefile
#
# Make file used for building MessageCenter.
#
##############################################################

###################################
#defines
###################################

##################################
#make vars
##################################
#
#Vars for Source and object files
#
obj-m := Hobart_usb.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

##################################
#Target defs
##################################
all: build install

build:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
	

remove: clean
	sudo rmmod Hobart_usb
	lsmod | grep Hobart_usb

install:
	sudo insmod Hobart_usb.ko
	lsmod | grep Hobart_usb

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

test:
	echo -n XabC>/dev/Hobart_usb
	cat /dev/Hobart_usb

present:
	lsmod | grep Hobart_usb
