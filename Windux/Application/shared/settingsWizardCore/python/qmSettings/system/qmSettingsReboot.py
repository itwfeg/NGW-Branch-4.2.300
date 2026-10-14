######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : January 3, 2013
##  OVERVIEW: This script reboots the scale.
##  REQS    : root authority
##  RETURNS : none
######################################################

# Sample command -> qmSettingsReboot.py

# Preprocessor directives
##########################
import sys, os, subprocess

if os.name == "posix":
    # LINUX OS
    os.system("/sbin/reboot")
else:
    # WINDOWS OS
    print(" ")
