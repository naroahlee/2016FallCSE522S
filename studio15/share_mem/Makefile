INCLUDE_DIR := ./inc
SRC_PATH := ./src
# Default Condition
# CFLAG = -I${INCLUDE_DIR} -Wall -O2
# Condition for Q6
CFLAG = -I${INCLUDE_DIR} -Wall -O2 -DSTUDIO15_Q6 -DENABLE_STAGE_OUTPUT -DSIZE_SHARE_MEM=10000000
CC = /usr/bin/gcc
LDLIBS = -lrt

vpath %.c ${SRC_PATH}

.PHONY: all clean install
ALL = leader follower

all: ${ALL}
	cscope -Rb

clean:
	rm -rf ${ALL} *.o 
	clear

install:
	cp ${ALL} ./bin

%.o: %.c
	${CC} ${CFLAG} -c $^ -o $@

leader: leader.o shm_ds.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@

follower: follower.o shm_ds.o
	${CC} ${CFLAG} $^ ${LDLIBS} -o $@
