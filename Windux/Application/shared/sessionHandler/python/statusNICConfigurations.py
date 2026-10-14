#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script copies the current network config to a USB drive
##  REQS    :
######################################################

# Preprocessor directives
##########################
import sys
import os
import shellCommand
import file_io
from optparse import OptionParser
import shutil
import string
import getFullAPInfo

##################################
#
#     G L O B A L S
#
# some easy to change vars below
#
#
#################################
trustedRootStoreDir = os.path.join("/","usr","local","hobart","db","settingswizard","TRUSTEDROOTSTORE")
clientCertStoreDir = os.path.join("/","usr","local","hobart","db","settingswizard","CLIENTCERTSTORE")

##################################
#
#     E N D   G L O B A L S
#
##################################

######################################

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-f", "--ifconfig",
             dest="ifconfig",
             help="Complete iwconfig settings")
p.add_option("-v", "--verbose",
             dest="verbose", default=None, action="store_true",
             help="Print detailed status")
p.add_option("-q", "--quiet",
             dest="verbose", default=None, action="store_false",
             help="No status printing (default)")

# Parse User's CLI args
#######################
options, args = p.parse_args()
exitCode = 0

if os.name == "posix":

    if options.ifconfig == "1":
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/sbin/ifconfig")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            print(stdout)
        except:
            pass

else:
    TBD = "windows"

sys.exit(0)


