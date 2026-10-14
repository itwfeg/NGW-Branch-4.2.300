######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script reports the macAddress of the scale.
##  REQS    : None
##  RETURNS : Qmx or Qmt based on os.name
######################################################

# Sample command -> getHostname.py

# Preprocessor directives
##########################
import sys, os, string

if os.name == "posix":
    # LINUX OS
    hostnameFileHandle = open("/etc/hostname", 'r')   
    line = hostnameFileHandle.readline()
    print(str(line.rstrip(' ')))
else:
    # WINDOWS OS
    print("Qmt")
