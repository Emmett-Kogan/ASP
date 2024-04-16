
obj-m := Assignment6Code.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	@$(MAKE) -C $(KERNEL_DIR) M=$(shell pwd) modules
	
app: 
	@gcc -o test test.c

clean:
	@rm -rf *.o *.ko *.mod.* *.symvers *.order test Assignment6Code.mod .Assignment6Code.* .Module.* .modules.*
