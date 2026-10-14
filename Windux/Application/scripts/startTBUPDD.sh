#!/bin/sh
# Script to get the Touch-base touch screen driver and associated processes running
# FHA 3/4/2011
export LD_LIBRARY_PATH=/opt/tbupddlx:$LD_LIBRARY_PATH
export DISPLAY=:0.0
killall aidaemon >/dev/null
#auplay /home/bsquare/opt/tbupddlx/beep.wav
sleep 7
#/home/bsquare/opt/tbupddlx/tblinuxmouse & /home/bsquare/opt/tbupddlx/startaidaemon
/opt/tbupddlx/tblinuxmouse & /opt/tbupddlx/startaidaemon


