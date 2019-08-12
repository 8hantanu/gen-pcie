obj-m := hqm.o
hqm-objs := gpd_main.o gpd_dev.o gpd_fops.o gpd_sys.o 

# export KSRC=/tools/AGRreleases/yocto/rocko/snr_delivery15.1/axxia-snr-frio-4.12/tools/sysroots/core2-64-intelaxxia-linux/usr/src/kernel
# export KSRC=/tools/AGRtools/fedora24/4.8.6-201.frio.fc24.x86_64

KSRC ?= /lib/modules/$(shell uname -r)

default:
	make -C $(KSRC)  M=$(shell pwd) modules

install:
	make -C $(KSRC)  M=$(shell pwd) modules_install

clean:
	make -C $(KSRC)  M=$(shell pwd) clean
