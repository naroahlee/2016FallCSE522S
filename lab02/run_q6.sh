#!/bin/bash

rm -rf ./res
mkdir ./res

PerCore=2
Prio0=2
Prio1=1
for((i=0;i<5;i++))
{
	rm -rf ./exp.cfg
	echo ${PerCore} ${Prio0} ${Prio1} > exp.cfg
	sudo trace-cmd record -e sched_switch ./lab02_main SCHED_FIFO 1000 1000 2 3 5 7 11
	rm -rf FIFO_${PerCore}_${Prio0}_${Prio1}_${i}.dat
	mv trace.dat ./res/FIFO_${PerCore}_${Prio0}_${Prio1}_${i}.dat

	rm -rf ./exp.cfg
	echo ${PerCore} ${Prio0} ${Prio1} > exp.cfg
	sudo trace-cmd record -e sched_switch ./lab02_main SCHED_RR 1000 1000 2 3 5 7 11
	rm -rf RR_${PerCore}_${Prio0}_${Prio1}_${i}.dat
	mv trace.dat ./res/RR_${PerCore}_${Prio0}_${Prio1}_${i}.dat

}

