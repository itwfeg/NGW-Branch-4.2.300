#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# is this ever used anywhere?

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
p.add_option("-M", "--miiTool",
             dest="miiTool",
             help="Complete mii-tool -v info")
p.add_option("-s", "--stationDump",
             dest="stationDump",
             help="Complete info for current AP connection.")
p.add_option("-E", "--ethtool",
             dest="ethtool",
             help="Complete ethtool info")
p.add_option("-v", "--verbose",
             dest="verbose", default=None, action="store_true",
             help="Print detailed status")
p.add_option("-L", "--iwLink",
             dest="iwLink",
             help="Complete iw dev wlan0 link settings")
p.add_option("-q", "--quiet",
             dest="verbose", default=None, action="store_false",
             help="No status printing (default)")

# Parse User's CLI args
#######################
options, args = p.parse_args()
exitCode = 0

if os.name == "posix":
    if options.iwLink is not None:
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/usr/sbin/iw")
            cmdd.append("dev")
            cmdd.append("wlan0")
            cmdd.append("link")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            #print "<TABLE BGCOLOR=#000000><TR><TD>&nbsp;</TD></TR></TABLE>"
            print(stdout)
        except:
            pass

    if options.miiTool is not None:
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/sbin/mii-tool")
            cmdd.append("-v")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            print(stdout)
        except:
            pass

    if options.stationDump is not None:
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/usr/sbin/iw")
            cmdd.append("dev")
            cmdd.append("wlan0")
            cmdd.append("station")
            cmdd.append("dump")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            print(stdout)
        except:
            pass

    if options.ethtool is not None:
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/usr/sbin/ethtool")
            cmdd.append("eth0")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            print(stdout)
        except:
            pass

else:
    TBD = "windows"

sys.exit(0)



