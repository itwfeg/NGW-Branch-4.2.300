#!/usr/bin/env python2
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 16, 2014
##  OVERVIEW: This script deletes a cert from the active certs' location Linux.
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


p.add_option("-S", "--serialNumber",
             dest="serialNumber",
             help="Serial Number of Scale")
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

    # Read the wpa_supplicant.conf file and delete server and client files it refers to.

    confFile = open("/etc/wpa_supplicant.conf", "r")
    for line in confFile:
        line =line.strip()
        keyValuePair = line.split('=')
        ####################################################################
        #Server Cert Filename
        #csrFilename=
        #csrKeyFilename=
        keyValuePair[0] = keyValuePair[0].lstrip()
        if keyValuePair[0] == "ca_cert":
            # we have a server cert
            try:
                caFile = keyValuePair[1].replace('\"','')
                os.remove(caFile)
                cmd = []
                cmd.append("/bin/sync")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
            except:
                pass
        #Client Filename
        elif keyValuePair[0] == "client_cert":
            # we have a client cert
            try:
                clientFile = keyValuePair[1].replace('\"','')
                os.remove(clientFile)
                cmd = []
                cmd.append("/bin/sync")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
            except:
                pass
        #Client key Filename
        elif keyValuePair[0] == "private_key":
            # we have a client cert
            try:
                clientFile = keyValuePair[1].replace('\"','')
                os.remove(clientFile)
                cmd = []
                cmd.append("/bin/sync")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
            except:
                pass

else:
    # WINDOWS BLOCK GOES HERE
    fileToEmpty = open("C:\Program Files\Hobart\Db\SettingsWizard\CURRENTCERTS.txt", "q")
    fileToEmpty.write("")
    fileToEmpty.close()
    pass
sys.exit(exitCode)
