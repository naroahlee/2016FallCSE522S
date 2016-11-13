.PHONY: all clean

all: work spinlock sleeplock

# Original Code
work: workload.c
	gcc -fopenmp workload.c -o work

# Task1 Spinlock
spinlock: spinlock.c
	gcc -fopenmp spinlock.c -o spinlock

# Task2 Sleeplock
sleeplock: sleeplock.c
	gcc -fopenmp sleeplock.c -o sleeplock

clean:
	rm -rf work spinlock sleeplock
