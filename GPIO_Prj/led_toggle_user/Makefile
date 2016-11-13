# Makefile of CSE522 Lab02
# Haoran Li
# lihaoran@email.wustl.edu

INCLUDE_DIR := ./inc
SRC_PATH := ./src
CC = /usr/bin/gcc
#LDLIBS = -lpthread -lrt -lm
DBG_LVL = -1
OPT_LVL = 2
# Add -DDISABLE_PIN_CORE to remove the cpu affinities 
# Add -DUSE_SPINLOCK to USE SPINLOCK instead of MUTEX for variable protection
# Choose -DCUSTOM_SPIN_BARRIER or -DCUSTOM_SPLEEP_BARRIER or default(pthread_barrier)
CFLAG = -I${INCLUDE_DIR} -Wall -O${OPT_LVL} -DDBG_LVL=${DBG_LVL} 

# Src Directories
vpath %.c ${SRC_PATH}

.PHONY: all clean install update_csdb

ALL = led_toggle_user

all: ${ALL}

clean:
	rm -rf ${ALL} *.o
	clear

install:
	cp ${ALL} ./bin


update_csdb:
	cscope -Rb

# Policy for building
%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

led_toggle_user: main.o bcm2837_gpio.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@
