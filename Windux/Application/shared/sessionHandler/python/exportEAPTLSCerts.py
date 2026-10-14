#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script exports certificates to a flash drive
######################################################


# Preprocessor directives
##########################
import sys
import os
import shellCommand
import file_io
from optparse import OptionParser
import shutil

######################################

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-S", "--trustedRootCertFilename",
             dest="trustedRoot",
             help="Just the filename of the trusted root cetificate.")
p.add_option("-C", "--clientp12CertFilename",
             dest="client",
             help="Just the filename of the client cetificate.")
p.add_option("-D", "--desination",
             dest="destination",
             help="Full path of destination where both files will reside on a Flash Drive")
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

trustedRootFilename = os.path.join("/","usr","local","hobart","db","settingswizard","TRUSTEDROOTSTORE", options.trustedRoot)
clientFilename = os.path.join("/","usr","local","hobart","db","settingswizard","CLIENTCERTSTORE", options.client)

try:
    shutil.copy2(trustedRootFilename, options.destination)
    shutil.copy2(clientFilename,      options.destination)
except:
    exitCode = 1
    pass

cmd = []
cmd.append("/bin/sync")
exitCode, stdout, stderr = shellCommand.execute(cmd)

sys.exit(exitCode)

