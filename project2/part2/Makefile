obj-m := moduleSmiteUnsmite.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
all:	module smite unsmite
smite: smite.o
	gcc smite.o -o smite
smite.o:
	gcc smite.c -c
clean_smite:
	rm smite.o smite
unsmite: unsmite.o
	gcc unsmite.o -o unsmite
unsmite.o:
	gcc unsmite.c -c
clean_unsmite:
	rm unsmite.o unsmite
module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean_module:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
clean: clean_module clean_smite clean_smite
