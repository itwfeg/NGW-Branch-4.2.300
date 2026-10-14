######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script reports the Timezone of the scale.
##  REQS    : None
##  RETURNS : String as three characters to be mapped in Qt app
######################################################

# Sample command -> qmSettingGetTimezone.py

# Preprocessor directives
##########################
import sys, os, subprocess

timezone = ""

if os.name == "posix":
    # LINUX OS
    try:
        timezone = subprocess.check_output("date +%Z", shell=True, stderr=None)
    except:
        timezone = "None"

    print(str(timezone.rstrip('\n'))) 

else:
    # WINDOWS OS
    print("Qmt")
