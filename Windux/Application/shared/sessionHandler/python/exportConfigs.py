#!/usr/bin/env python2
# -*- coding: utf-8 -*-

'''
Created on Jan 8, 2013

@author: Alex Zavala
'''
from optparse import OptionParser
import encryptDecrypt
import sys

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-F", "--FILE",
             dest="file",
             help="The destination's file path")

# Parse User's CLI args
#######################
options, args = p.parse_args()

encryptDecrypt.exportConfigs(options.file)