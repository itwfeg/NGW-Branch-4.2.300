#!/usr/bin/env python2
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : June 2, 2015
##  OVERVIEW: This script disconenct windows wlan.
##  REQS    : Must have permissions to execute the
##            OS's commands without password
##            prompt.
######################################################

# Will dicsonnect the wlan

# Preprocessor directives
##########################
import sys, os, string
import ntNetworkUtils
#wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n\n}"
try:
    wirelessAdapterType = "wlan"
    wirelessAdapterName = "Wireless Network Connection"

    ntNetworkUtils.disconnectWlan()

    # setup filters for only the specified SSID
    ntNetworkUtils.setWlanDenyallFilter(False)
    ntNetworkUtils.deleteAllDnsServers(wirelessAdapterName)
    exitCode, stdout, stderr = ntNetworkUtils.deleteAllProfilesForAdapter(wirelessAdapterType,  wirelessAdapterName)
except:
    pass
sys.exit(0)

