#!/bin/bash
echo Load Module 
sudo make load
echo Change Sysfs Permission 
sudo chmod 666 /sys/class/CustomLed/Led0/period
echo Change Frequency 1Hz
echo 1000 > /sys/class/CustomLed/Led0/period
sleep 8
echo Change Frequency 5Hz
echo 200 > /sys/class/CustomLed/Led0/period
sleep 5
echo Change Frequency 20Hz
echo 50 > /sys/class/CustomLed/Led0/period
sleep 5

echo Unload Module 
sudo make unload
