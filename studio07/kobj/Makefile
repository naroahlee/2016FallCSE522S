TARGET_MODULE:=kobject-example
BUILDSYSTEM_DIR:= /home/pi/Kernel/linux_src

PWD:=$(shell pwd)

obj-m := $(TARGET_MODULE).o
$(TARGET_MODULE)-objs := main.o

all : 
# run kernel build system to make module
	$(MAKE) -C $(BUILDSYSTEM_DIR) SUBDIRS=$(PWD) modules

clean:
# run kernel build system to cleanup in current directory
	$(MAKE) -C $(BUILDSYSTEM_DIR) SUBDIRS=$(PWD) clean

load:
	insmod ./$(TARGET_MODULE).ko

unload:
	rmmod ./$(TARGET_MODULE).ko

