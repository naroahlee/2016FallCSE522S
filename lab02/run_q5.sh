#!/bin/bash

rm -rf ./res
mkdir ./res

PerCore=2
for((i=-15;i<=15;i=i+5))
{
	rm -rf ./exp.cfg
	Prio0=${i}
	Prio1=${i}
	echo ${PerCore} ${Prio0} ${Prio1} > exp.cfg
	sudo trace-cmd record -e sched_switch ./lab02_main SCHED_OTHER 1000 1000 2 3 5 7 11
	rm -rf normal_${PerCore}_${Prio0}_${Prio1}.dat
	mv trace.dat ./res/normal_${PerCore}_${Prio0}_${Prio1}.dat
}
