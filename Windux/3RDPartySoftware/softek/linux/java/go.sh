#!/bin/sh
set -x
LD_LIBRARY_PATH=../lib export LD_LIBRARY_PATH
CLASSPATH=$CLASSPATH:. export CLASSPATH
java sample

