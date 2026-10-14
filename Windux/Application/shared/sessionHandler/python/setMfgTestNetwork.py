#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# not tested in python3 but probably OK

import sys
import os
import shellCommand
import argparse
import configInfo

pythonInstallPath = os.path.join("/","usr","bin")
if os.name == 'nt':
    pythonInstallPath = configInfo.getPythonInstallPath()
pythonScriptPath = os.path.join(configInfo.getRootFolder(),"python")

# 3/28/23 as of now, WWST1 does not exist in Troy anymore :(
# so we need a separate network configuration for either location
if os.path.exists('/etc/hobart/techcenter.conf'):
    location = "techcenter"
else:
    location = "hillsboro"

def configNetwork(interface, location):

    if location == 'techcenter':

        cmd = []
        cmd.append("/usr/bin/python")
        cmd.append("/usr/local/hobart/python/setNetwork.py") 
        cmd.append("-c") 
        cmd.append("0")
        cmd.append("-I")
        cmd.append("64641964") 
        cmd.append("-s")
        cmd.append("0")
        cmd.append( "-D") 
        cmd.append("1")
        cmd.append("-B")
        cmd.append("0")
        cmd.append("-G")
        cmd.append("0")
        cmd.append("-1")
        cmd.append("0")
        cmd.append("-2") 
        cmd.append("0")
        cmd.append("-3")
        cmd.append("0")
        cmd.append("-W")
        cmd.append("1")
        cmd.append("-A")
        cmd.append("wrap@Hobart123")
        cmd.append("-S")
        cmd.append("weighwrap") 
        cmd.append("-M")
        cmd.append("WPA2-Personal")
        cmd.append("-E")
        cmd.append("AES")
        cmd.append("-V")
        cmd.append("0")
        cmd.append("-P") 
        cmd.append("0")
        cmd.append("-v")
        cmd.append( "0" )
        cmd.append("-w")
        cmd.append("0")
        cmd.append("-x")
        cmd.append("0")
        cmd.append("-y") 
        cmd.append("0")
        cmd.append("-z")
        cmd.append("0")
        cmd.append("-N")
        cmd.append("0")
        cmd.append("-n")
        cmd.append("0")
        cmd.append("-K")
        cmd.append("1")
        cmd.append("-X")
        cmd.append( "0")
        cmd.append("-a")
        cmd.append("auto")
        cmd.append("-b")
        cmd.append("1")
        cmd.append("-d")
        cmd.append("auto")
        cmd.append("-5")
        cmd.append("")
        cmd.append("-6")
        cmd.append("")
        cmd.append("-7")
        cmd.append("")
        cmd.append("-8")
        cmd.append("")
        cmd.append("-9")
        cmd.append("")
        cmd.append("-4")
        cmd.append("")
        """
 
  if interface == 'wireles':
        # TODO:
    # Needs configured for ifi connection in Hillsboro
            cmd = []
        cmd.append(os.path.join(pythonInstallPath,"python"))
            cmd.append(os.path.join(pythonScriptPath,"setNetwork.py"))
            cmd.append("-s")
            cmd.append("0")
            cmd.append("-D")
            cmd.append("1")
            cmd.append("-W")
            cmd.append("1")
            cmd.append("-S")
            # SSID
            cmd.append("wwST1")
            cmd.append("-M")
            # Security Method
            cmd.append("WPA2-Personal")
            cmd.append("-E")
            # Encryption
            cmd.append("AES")
            cmd.append("-v")
            cmd.append("0")
            cmd.append("-A")
            # Key
            cmd.append("51Zebra69")
            cmd.append("-X")
            cmd.append("0")
            cmd.append("-F")
            cmd.append("64641964")
            cmd.append("-d")
            cmd.append("auto")
        """
        if interface == 'wired':

            cmd = []
            cmd.append(os.path.join(pythonInstallPath,"python"))
            cmd.append(os.path.join(pythonScriptPath,"setNetwork.py"))
            cmd.append("-s")
            cmd.append("0")
            cmd.append("-W")
            cmd.append("0")
            cmd.append("-D")
            cmd.append("1")
            cmd.append("-F")
            cmd.append("64641964")

    elif location == 'hillsboro':
        if interface == 'wireless':
            # TODO:
            # Needs configured for Wifi connection in Hillsboro
            cmd = []
            cmd.append(os.path.join(pythonInstallPath,"python"))
            cmd.append(os.path.join(pythonScriptPath,"setNetwork.py"))
            cmd.append("-c")
            cmd.append("0")
            cmd.append("-I")
            cmd.append("0")
            cmd.append("-s")
            cmd.append("0")
            cmd.append("-D")
            cmd.append("1")
            cmd.append("-B")
            cmd.append("0")
            cmd.append("-G")
            cmd.append("0")
            cmd.append("-1")
            cmd.append("0")
            cmd.append("-2")
            cmd.append("0")
            cmd.append("-3")
            cmd.append("0")
            cmd.append("-W")
            cmd.append("1")
            cmd.append("-A")
            # WPA-Passphrase
            cmd.append("51Zebra69")  # put Hillsboro key back in here, fas1track2
            cmd.append("-S")
            # SSID
            cmd.append("wwST1")
            cmd.append("-M")
            # Security Method (if it doesn't work beacuse existing network is not both wpa and wpa2 as stated, then change line below to "WPA2-Personal" instead)
            cmd.append("WPA2-Personal")
            cmd.append("-E")
            # Encryption
            cmd.append("AES")
            cmd.append("-V")
            cmd.append("0")
            cmd.append("-P")
            cmd.append("0")
            cmd.append("-v")
            cmd.append("0")
            cmd.append("-w")
            # Key
            cmd.append("0")
            cmd.append("-x")
            cmd.append("0")
            cmd.append("-y")
            cmd.append("0")
            cmd.append("-z")
            cmd.append("0")
            cmd.append("-N")
            cmd.append("0")
            cmd.append("-n")
            cmd.append("0")
            cmd.append("-K")
            cmd.append("1")
            cmd.append("-X")
            cmd.append("0")
            cmd.append("-F")
            cmd.append("64641964")
            cmd.append("-d")
            cmd.append("auto")

        elif interface == 'wired':

            cmd = []
            cmd.append(os.path.join(pythonInstallPath,"python"))
            cmd.append(os.path.join(pythonScriptPath,"setNetwork.py"))
            cmd.append("-s")
            cmd.append("0")
            cmd.append("-W")
            cmd.append("0")
            cmd.append("-D")
            cmd.append("1")
            cmd.append("-F")
            cmd.append("64641964")

    exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)

    if stdout.find("YES") > -1:
        exitCode = 0
    else:

        cmd = []
        cmd.append(os.path.join(pythonInstallPath,"python"))
        cmd.append(os.path.join(pythonScriptPath,"setNetwork.py"))
        cmd.append("-s")
        cmd.append("0")
        cmd.append("-W")
        cmd.append("0")
        cmd.append("-D")
        cmd.append("1")
        cmd.append("-F")
        cmd.append("64641964")
        exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)

        exitCode = 1

    return exitCode

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="configure manufacturing test network environment")
    parser.add_argument("interface",
                        action="store",
                        choices=['wireless','wired'],
                        help="specify interface device (wireless, wired)")

    args = parser.parse_args()

    exitCode = configNetwork(args.interface, location)

    sys.exit(exitCode)
