#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script checks a cert for active use in wireless connections on Linux/WES7 systems.
##  REQS    : All options must be provided, except for -v -h -q
######################################################


# Preprocessor directives
##########################
import sys
import os
import shellCommand
import file_io
from optparse import OptionParser

##################################
#
#     G L O B A L S
#
# some easy to change vars below
#
#
#################################

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
p.add_option("-C", "--certSourceName",
             dest="certName",
             help="Name of source file.")
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

certToCheck = ""
if os.name == "posix":
    certToCheck = os.path.join("/","usr","local","hobart","db","settingswizard",options.certName)

    if options.certName != "" and options.certName != None:
        if os.path.exists(certToCheck):
            exit(50)
        else:
            exit(0)
else:
    # WINDOWS BLOCK GOES HERE
    certToCheck = os.path.join("C:/","Program Files","Hobart","Db","settingswizard",options.certName)

    if options.certName != "" and options.certName != None:
        if os.path.exists(certToCheck):
            exit(50)
        else:
            exit(0)

    sys.exit(exitCode)
