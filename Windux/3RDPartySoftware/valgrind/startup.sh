#!/bin/bash

# startup chobart_d (in valgrind)
VALGRIND_LIB=/usr/lib/valgrind valgrind --error-limit=no --undef-value-errors=no --log-file="/root/valgrindlog.txt" chobart_d &

# wait a little bit
sleep 90

# startup scaleui
DISPLAY=:0 scaleui &

# log output
tail -f /root/valgrindlog.txt


