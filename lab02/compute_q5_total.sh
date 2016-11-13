#!/bin/bash


PerCore=2

for((i=-15;i<=15;i=i+5))
{
	Prio0=${i}
	Prio1=${i}
	trace-cmd report ./res/normal_${PerCore}_${Prio0}_${Prio1}.dat > temp.txt
	endt=`tail -n 1 temp.txt | awk '{print $3}' | cut -d ":" -f1`
	startt=`head -n 3 temp.txt | tail -n 1 | awk '{print $3}' | cut -d ":" -f1`
	echo $endt - $startt | bc
	rm -rf temp.txt
}
