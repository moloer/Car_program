#KERN=/usr/src/kernels/`uname -r`
KERN=/root/linux-3.0.8
SRC=`pwd`
obj-m+=mydriver.o
all:
	make -C $(KERN) M=$(SRC) modules
clean:
	make -C $(KERN) M=$(SRC) clean


