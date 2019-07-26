obj-m := gpd.o
gpd-objs := gpd_main.o gpd_sys.o gpd_dev.o

KSRC ?= /lib/modules/$(shell uname -r)

default:
	make -C $(KSRC)  M=$(shell pwd) modules

install:
	make -C $(KSRC)  M=$(shell pwd) modules_install

clean:
	make -C $(KSRC)  M=$(shell pwd) clean
