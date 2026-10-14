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

# Sample command -> macAddress.py

# Preprocessor directives
##########################
import sys, os

if os.name == "posix":
    # LINUX OS
    print("33:44:55:66:77")
else:
    # WINDOWS OS
    print("Qmt")
