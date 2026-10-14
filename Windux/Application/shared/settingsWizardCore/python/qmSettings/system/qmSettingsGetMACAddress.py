######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script reports the macAddress of the scale.
##  REQS    : None
##  RETURNS : String as WiredMACADDRESS|WirelessMACADDRESS
######################################################

# Sample command -> macAddress.py

# Preprocessor directives
##########################
import sys, os, subprocess

ethernetMac = ""
wirelessMac = ""

if os.name == "posix":
    # LINUX OS
    try:
        ethernetMac = subprocess.check_output("ifconfig eth0 | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'", shell=True, stderr=None)
    except:
        ethernetMac = "No Device"

    try:
        wirelessMac = subprocess.check_output("ifconfig wlan0 | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'", shell=True, stderr=None)
    except:
        wirelessMac = "No Device"
    
    print(str(ethernetMac.rstrip('\n')) + "|" + str(wirelessMac.rstrip('\n'))) 

else:
    # WINDOWS OS
    print("Qmt")
