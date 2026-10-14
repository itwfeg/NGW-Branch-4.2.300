#!/bin/bash

# The /proc/buddyinfo contains information regarding free pages available in
# each memory zone, for example:
# Node 0, zone   Normal    159    146     84     48      8      4      4      2      1      3      2      1      2      8 
# Node 0, zone      DMA    125    149     92     30     11     13     39     35     17     17     10      5      4     11 
#
# Here we grab number of high order pages in the DMA zone. If below a
# watermark we clear out all cached pages and compact memory in order to
# generate more high order pages. If we run out of high order pages there is
# a high possibility of processes getting terminated due to OOM.

# watermark level
WATERMARK_HIGH=2
WATERMARK_LOW=2048

# determine which zone to check
if cat /proc/buddyinfo | grep -q 'DMA'; then
    ZONE=DMA
elif cat /proc/buddyinfo | grep -q 'Normal'; then
    ZONE=Normal
else
    echo "Could not locate proper memory zone."
    exit
fi

# retrieve high order DMA pages free
PAGES_HIGH=`cat /proc/buddyinfo | grep ${ZONE} | grep -Eo "[0-9]+[[:space:]]*$"`
PAGES_LOW=`cat /proc/buddyinfo | grep -Eo "${ZONE}[[:space:]]+[0-9]+" | grep -Eo "[0-9]+"`

echo "Pages Free, zone ${ZONE}: ${PAGES_LOW} ${PAGES_HIGH}"

# check for force flag
#if [ $# -eq 1 ]; then
#    if [ "$1" == "-f" ]; then
#        PAGES_HIGH=0
#        PAGES_LOW=10000
#    elif [ "$1" == "-F" ]; then
#        PAGES_HIGH=0
#        PAGES_LOW=10000
#    fi
#fi

# drop caches and compact memory
if [ "${PAGES_HIGH}" -gt "${WATERMARK_HIGH}" ]; then
    echo "No need to drop caches... system above watermark level."
elif [ "${PAGES_LOW}" -lt "${WATERMARK_LOW}" ]; then
    echo "No need to drop caches... system below watermark level."
else
    echo -n "Dropping caches and compacting memory... "

    sync
    echo 3 > /proc/sys/vm/drop_caches
    echo 1 > /proc/sys/vm/compact_memory

    echo "done!"
fi
