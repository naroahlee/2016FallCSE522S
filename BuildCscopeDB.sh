#!/bin/bash

LNX=/home/pi/Kernel/linux_src
CSCOPE_DB_DIR=/home/pi/CscopeDB

cd /
find $LNX \
     -path "$LNX/arch/*" ! -path "$LNX/arch/arm*" -prune -o \
     -path "$LNX/tmp*" -prune -o \
     -path "$LNX/Documentation*" -prune -o \
     -path "$LNX/scripts*" -prune -o \
     -path "$LNX/drivers*" -prune -o \
     -name "*.[chxsS]" -print > $CSCOPE_DB_DIR/cscope.files

cd $CSCOPE_DB_DIR
cscope -b -q -k

echo ResolveOK!

