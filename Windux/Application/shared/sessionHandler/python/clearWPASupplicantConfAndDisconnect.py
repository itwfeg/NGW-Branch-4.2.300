#!/usr/bin/env python2
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script sets Linux/WES7 system to clear the wpa_supplicant contents and take system offline (Clearing EAP-TLS certs).
##  REQS    : Must have permissions to execute the
##            OS's commands without password
##            prompt. All options must be provided, except for -v -h -q
######################################################

#Sample command -> qmSettingsNetwork.pyc -TBD

# Will set network to provided values

# Preprocessor directives
##########################
import sys, os, string
from optparse import OptionParser
import shlex
import time, subprocess
import shellCommand
import socket, struct
if os.name == "posix":
    import fcntl
import shutil
import re
import createWirelessProfile
import math

wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n\n}"
try:
    wpaSupplicantFileHandle = open("/etc/wpa_supplicant.conf", 'w')
    wpaSupplicantFileHandle.write(wpa_supplicantFileContents)
    wpaSupplicantFileHandle.close()
except:
    pass

try:
    cmd = []
    cmd.append("/etc/init.d/wpa_supplicant")
    cmd.append("stop")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    cmd = []
    cmd.append("/etc/init.d/wpa_supplicantForSetupUtility")
    cmd.append("stop")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
except:
    pass

try:
    cmd = []
    cmd.append("killall")
    cmd.append("dhcpcd")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
except:
    pass

try:
    cmd = []
    cmd.append("rm")
    cmd.append("/var/db/dhcpcd-wlan0.lease")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
except:
    pass
