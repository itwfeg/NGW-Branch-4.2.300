#!/bin/bash

#while true; do

echo
echo ==================================================
date
echo ==================================================
cat /var/log/dmesg
echo ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

dmesg

lsmod
cat /proc/modules

lsusb
lspci

echo --------------------------------------------------

cat /proc/meminfo
cat /proc/buddyinfo
echo -------------XXXXXX----XXXXXXX
echo ------TODD BEGIN
ps -e -orss=,pid=,args= | sort -b -k1,1n
echo ------TODD END
echo --------------------------------------------------

slabtop -o -s c
cat /proc/slabinfo

echo --------------------------------------------------

netstat -an
top -n1 b

echo --------------------------------------------------

lsof
lsof |grep chobart_d |wc
lsof |grep scaleui |wc
lsof |grep gcd |wc

#sleep 4s
#done


