obj-m += gpd_main.o

KSRC ?= /lib/modules/$(shell uname -r)/build

all:
	make -C $(KSRC)  M=$(shell pwd) modules

clean:
	make -C $(KSRC)  M=$(shell pwd) clean
