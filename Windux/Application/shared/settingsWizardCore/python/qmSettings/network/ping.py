######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script pings a remote IP address.
##  REQS    : Must have permissions to execute the
##            ping command without password
##            prompt. All options must be provided, except for -v -h -q
######################################################

# Sample command -># ping.pyc -c 3 10.0.0.0

# Will ping address 10.0.0.0 3 times

# Preprocessor directives
##########################
import sys, os
from optparse import OptionParser

##################################
#
#     G L O B A L S
#
# some easy to change vars below
#
# 
#################################

# Enter relative path & file for save 
mainCommand = "ping"
dbName = ""

##################################
#
#     E N D   G L O B A L S
#  
##################################
# Default maximum number of pings per Darryl needler on 01/07/2013 - change at will
pingCount = 30

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-c", "--count",
             dest="pingTrials",
             help="The number of times to test a ping.")

p.add_option("-d", "--destination",
             dest="destination",
             help="The address to ping.")

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
    # Test Incoming
    #######################
    #print("i.MX6")
    #print options
    #print args

    os.system(mainCommand + " -c " + str(pingCount) + " " + str(options.destination))

else:
    # WINDOWS BLOCK GOES HERE
    print("Windows")
