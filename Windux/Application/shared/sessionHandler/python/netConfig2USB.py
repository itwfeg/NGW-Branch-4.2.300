#!/usr/bin/env python2
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
import getFullAPInfo
import dumpState

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
passphrase = "woozal"
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
p.add_option("-S", "--trustedRootCertLocation",
             dest="trustedRoot",
             help="Full path of trusted root cetificate.")
p.add_option("-C", "--clientp12CertLocation",
             dest="client",
             help="Full path of client cetificate.")
p.add_option("-F", "--fileToSave",
             dest="fileToSave",
             help="User's choice of tar gzipeed file to save everything as.")
p.add_option("-I", "--image",
             dest="networkScreenshot",
             help="Screenshot of network screen.")
p.add_option("-A", "--accessPoints",
             dest="accessPoints",
             help="Complete info for surrounding detectable access points.")
p.add_option("-c", "--config",
             dest="config",
             help="Complete info for surrounding detectable access points.")
p.add_option("-s", "--stationDump",
             dest="stationDump",
             help="Complete info for current AP connection.")
p.add_option("-w", "--iwConfig",
             dest="iwConfig",
             help="Complete iwcofnig settings")
p.add_option("-f", "--ifconfig",
             dest="ifconfig",
             help="Complete iwconfig settings")
p.add_option("-L", "--iwLink",
             dest="iwLink",
             help="Complete iw dev wlan0 link settings")
p.add_option("-M", "--miiTool",
             dest="miiTool",
             help="Complete mii-tool -v info")
p.add_option("-E", "--ethtool",
             dest="ethtool",
             help="Complete ethtool info")
p.add_option("-Z", "--memoryDumpStates",
             dest="dumpStates",
             help="Include the memory dump states in a tar ball only.")
p.add_option("-Y", "--setNetworkCommands",
             dest="netCommands",
             help="The commands that were sent to setNetwork.py during last connection attempt.")
p.add_option("-T", "--netCommands",
             dest="tabCaptures",
             help="Include the content of the UI-tabs output in a tar ball only.")
p.add_option("-P", "--passphrase",
             dest="passphrase",
             help="The passphrase for encrypting certain file contents.")
p.add_option("-R", "--route",
             dest="route",
             help="Include the contents of route.")
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
    #try:
    #    f = open("/usr/local/hobart/db/settingswizard/netCFGCommands.txt", "w")
    #    f.writelines("options : %r" % (options))
    #    #for line in sys.argv
    #    f.close()
    #except:
    #    pass

    try:
        os.remove("/usr/local/hobart/db/settingswizard/networkConfigScreenshot.png")
        os.remove("/usr/local/hobart/db/settingswizard/apScan.txt")
        os.remove("/usr/local/hobart/db/settingswizard/ifconfig.txt")
        os.remove("/usr/local/hobart/db/settingswizard/ethtool.txt")
        os.remove("/usr/local/hobart/db/settingswizard/iwLink.txt")
        os.remove("/usr/local/hobart/db/settingswizard/miiTool.txt")
        os.remove("/usr/local/hobart/db/settingswizard/iwconfig.txt")
        os.remove("/usr/local/hobart/db/settingswizard/stationDump.txt")
    except:
        pass
    cmdb = []
    cmdb.append("/bin/sync")
    exitCode, stdout, stderr = shellCommand.execute(cmdb)

    cmd = []
    cmd.append("/usr/bin/tar")
    cmd.append("-c")
    cmd.append("-z")
    cmd.append("-f")
    cmd.append(options.fileToSave)
    if options.trustedRoot != None:
        cmd.append("-C")
        cmd.append("/usr/local/hobart/db/settingswizard/TRUSTEDROOTSTORE")
        cmd.append(options.trustedRoot)
    if options.client != None:
        cmd.append("-C")
        cmd.append("/usr/local/hobart/db/settingswizard/CLIENTCERTSTORE")
        cmd.append(options.client)
    if options.config == "1":
        cmd.append("-C")
        cmd.append("/usr/local/hobart/db/settingswizard")
        cmd.append("config.txt")

    if options.networkScreenshot == "1":
        # do screenshot
        try:
            cmdb = []
            cmdb.append("import")
            cmdb.append("-window")
            cmdb.append("root")
            cmdb.append("/usr/local/hobart/db/settingswizard/networkConfigScreenshot.png")
            exitCode, stdout, stderr = shellCommand.execute(cmdb) 
            if exitCode == 0:
                cmdb = []
                cmdb.append("/bin/sync")
                exitCode, stdout, stderr = shellCommand.execute(cmdb)
                cmd.append("-C")
                cmd.append("/usr/local/hobart/db/settingswizard")
                cmd.append("networkConfigScreenshot.png")
        except:
            pass

    if options.accessPoints == "1":
        # do ap scan
        try:
            APresults = getFullAPInfo.getFullAPInfoText()
            # Open a file in write mode
            fo = open("/usr/local/hobart/db/settingswizard/apScan.txt", "w")
            fo.writelines(APresults)
            fo.close()
            cmda = []
            cmda.append("/bin/sync")
            exitCode, stdout, stderr = shellCommand.execute(cmda)
            cmd.append("-C")
            cmd.append("/usr/local/hobart/db/settingswizard")
            cmd.append("apScan.txt")

        except:
            pass
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    if options.ifconfig == "1":
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/sbin/ifconfig")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            # Open a file in write mode
            fo = open("/usr/local/hobart/db/settingswizard/ifconfig.txt", "w")
            fo.writelines(stdout)
            fo.close()
            cmda = []
            cmda.append("/bin/sync")
            exitCode, stdout, stderr = shellCommand.execute(cmda)
            cmd.append("-C")
            cmd.append("/usr/local/hobart/db/settingswizard")
            cmd.append("ifconfig.txt")

        except:
            pass

    if options.ethtool == "1":
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/usr/sbin/ethtool")
            cmdd.append("eth0")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            # Open a file in write mode
            fo = open("/usr/local/hobart/db/settingswizard/ethtool.txt", "w")
            fo.writelines(stdout)
            fo.close()
            cmda = []
            cmda.append("/bin/sync")
            exitCode, stdout, stderr = shellCommand.execute(cmda)
            cmd.append("-C")
            cmd.append("/usr/local/hobart/db/settingswizard")
            cmd.append("ethtool.txt")
        except:
            pass

    if options.iwLink == "1":
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/usr/sbin/iw")
            cmdd.append("dev")
            cmdd.append("wlan0")
            cmdd.append("link")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            # Open a file in write mode
            fo = open("/usr/local/hobart/db/settingswizard/iwLink.txt", "w")
            fo.writelines(stdout)
            fo.close()
            cmda = []
            cmda.append("/bin/sync")
            exitCode, stdout, stderr = shellCommand.execute(cmda)
            cmd.append("-C")
            cmd.append("/usr/local/hobart/db/settingswizard")
            cmd.append("iwLink.txt")
        except:
            pass

    if options.miiTool == "1":
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/sbin/mii-tool")
            cmdd.append("-v")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            # Open a file in write mode
            fo = open("/usr/local/hobart/db/settingswizard/miiTool.txt", "w")
            fo.writelines(stdout)
            fo.close()
            cmda = []
            cmda.append("/bin/sync")
            exitCode, stdout, stderr = shellCommand.execute(cmda)
            cmd.append("-C")
            cmd.append("/usr/local/hobart/db/settingswizard")
            cmd.append("miiTool.txt")
        except:
            pass

    if options.iwConfig == "1":
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/usr/sbin/iwconfig")
            cmdd.append("wlan0")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            # Open a file in write mode
            fo = open("/usr/local/hobart/db/settingswizard/iwconfig.txt", "w")
            fo.writelines(stdout)
            fo.close()
            cmda = []
            cmda.append("/bin/sync")
            exitCode, stdout, stderr = shellCommand.execute(cmda)
            cmd.append("-C")
            cmd.append("/usr/local/hobart/db/settingswizard")
            cmd.append("iwconfig.txt")
        except:
            pass

    if options.stationDump == "1":
        #
        try:
            cmdd = []
            cmdd.append("/usr/sbin/iw")
            cmdd.append("dev")
            cmdd.append("wlan0")
            cmdd.append("station")
            cmdd.append("dump")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            # Open a file in write mode
            fo = open("/usr/local/hobart/db/settingswizard/stationDump.txt", "w")
            fo.writelines(stdout)
            fo.close()
            cmda = []
            cmda.append("/bin/sync")
            exitCode, stdout, stderr = shellCommand.execute(cmda)
            cmd.append("-C")
            cmd.append("/usr/local/hobart/db/settingswizard")
            cmd.append("stationDump.txt")
        except:
            pass

    if options.dumpStates == "1":
        # write the memory dump states to the tar ball
        try:
            dumpState.dumpStates("0", "1", options.passphrase)
            cmd.append("-C")
            cmd.append("/usr/local/hobart/db/settingswizard")
            cmd.append("dumpStates.txt")
        except:
            pass

    if options.netCommands == "1":
        # write the lsat setNetwork.py commands to the tar ball
        try:
            cmd.append("-C")
            cmd.append("/usr/local/hobart/db/settingswizard")
            cmd.append("setNetworkCommands.txt")
        except:
            pass
    #f = open("/usr/local/hobart/db/settingswizard/tarCommands.txt", "w")
    #for item in cmd:
    #    f.write("%s\n" % item)
    #f.write(cmd[])
    #for line in sys.argv
    #f.close()

    if options.tabCaptures == "1":
        cmd.append("-C")
        cmd.append("/usr/local/hobart/db/settingswizard")
        cmd.append("toolsTabCapture.txt")
        cmd.append("-C")
        cmd.append("/usr/local/hobart/db/settingswizard")
        cmd.append("statusTabCapture.txt")

    if options.route == "1":
        # do ap scan
        try:
            cmdd = []
            cmdd.append("/sbin/route")
            exitCode, stdout, stderr = shellCommand.execute(cmdd)
            # Open a file in write mode
            fo = open("/usr/local/hobart/db/settingswizard/route.txt", "w")
            fo.writelines(stdout)
            fo.close()
            cmda = []
            cmda.append("/bin/sync")
            exitCode, stdout, stderr = shellCommand.execute(cmda)
            cmd.append("-C")
            cmd.append("/usr/local/hobart/db/settingswizard")
            cmd.append("route.txt")
        except:
            pass

    cmd.append("-C")
    cmd.append("/usr/local/hobart/db/settingswizard")
    cmd.append("scalesettings")

    exitCode, stdout, stderr = shellCommand.execute(cmd)

    cmd = []
    cmd.append("/bin/sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)


else:
    TBD = "windows"

sys.exit(0)
