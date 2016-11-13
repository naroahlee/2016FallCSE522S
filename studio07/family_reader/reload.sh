#!/bin/bash
# Make sure only root can run our script

NAROAH_DBG_MOD=./family_reader.ko
NAROAH_DBG_DEV=/sys/kernel/kobject_example/fam_reader

if [ "$(id -u)" != "0" ]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi


echo Unloading Device...
rmmod $NAROAH_DBG_MOD

echo Clearing Kernel Log...
dmesg --clear
echo Reloading Device...
insmod $NAROAH_DBG_MOD
echo chmod 666 @ $NAROAH_DBG_DEV
chmod 666 $NAROAH_DBG_DEV

echo Done.

