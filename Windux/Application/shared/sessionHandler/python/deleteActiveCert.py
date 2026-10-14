#!/usr/bin/env python2
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 16, 2014
##  OVERVIEW: This script deletes a cert from the active certs' location Linux/WES7 system time.
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

certToDelete = ""
if os.name == "posix":
    certToDelete = os.path.join("/","usr","local","hobart","db","settingswizard",options.certName)

    try:
        if options.certName != "" and options.certName != None:
            os.remove(certToDelete)
    except:
        exitCode = 1
        pass


    cmd = []
    cmd.append("/bin/sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    sys.exit(exitCode)

else:
    # WINDOWS BLOCK GOES HERE
    certToDelete = os.path.join("C:/","Program Files","Hobart","db","settingswizard",options.certName)

    try:
        if options.certName != "" and options.certName != None:
            os.remove(certToDelete)
    except:
        exitCode = 1
        pass

    sys.exit(exitCode)

