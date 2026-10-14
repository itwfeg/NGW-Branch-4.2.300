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
import sys, os, string
from optparse import OptionParser
import subprocess

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################

p.add_option("-H", "--hostname",
             dest="hostname",
             help="The desired Hostname (ex. HobartHost)")
p.add_option("-v", "--verbose",
             dest="verbose", default=None, action="store_true",
             help="Print detailed status")
p.add_option("-q", "--quiet",
             dest="verbose", default=None, action="store_false",
             help="No status printing (default)")

# Parse User's CLI args
#######################
options, args = p.parse_args()

if os.name == "posix":
    # LINUX OS
    hostnameFileHandle = open("/etc/hostname", 'w')   
    hostnameFileHandle.write(str(options.hostname)+str("\n"))
else:
    # WINDOWS OS
    print("Qmt")
