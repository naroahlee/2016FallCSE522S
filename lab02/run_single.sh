#!/bin/bash
PerCore=2
Prio0=2
Prio1=1
Sched=SCHED_RR
Round=1000
Iter=1000
echo ${PerCore} ${Prio0} ${Prio1} > exp.cfg
sudo trace-cmd record -e sched_switch ./lab02_main ${Sched} ${Round} ${Iter} 2 3 5 7 11
