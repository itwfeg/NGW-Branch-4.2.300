#!/usr/bin/env python3
# -*- coding: utf-8 -*-

'''
Created on Jan 8, 2013

@author: Alex Zavala
'''
from optparse import OptionParser
import encryptDecrypt

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-F", "--FILE",
             dest="file",
             help="The source's file path")
p.add_option("-Z", "--passphrase",
             dest="passphrase",
             help="The passphrase used to decrypt the imported file")

# Parse User's CLI args
#######################
options, args = p.parse_args()

networkConfigurations = encryptDecrypt.importConfigs(options.file, options.passphrase)
print(networkConfigurations)
