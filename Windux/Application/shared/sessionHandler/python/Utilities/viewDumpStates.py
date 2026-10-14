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
import subprocess
import encryptDecrypt
import codecs

#################################
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
p.add_option("-F", "--fileToView",
             dest="fileToView",
             help="Full path, with filename, of an encrypted file to view.")
p.add_option("-P", "--passphrase",
             dest="passphrase",
             help="passphrase required to decrypt the file.")
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

    if len(options.fileToView) > 2:
        try:
            # Decrypt the content
            if len(options.passphrase) > 4:

                try:
                    cipheredTxt = codecs.open(options.fileToView, 'r', 'base64')
                    encryptedTxt = cipheredTxt.read()
                    print((encryptDecrypt.decryptAny(encryptedTxt, options.passphrase)))
                    cipheredTxt.close()
                except:
                    pass
        except:
            pass

else:
    TBD = "windows"

sys.exit(0)
