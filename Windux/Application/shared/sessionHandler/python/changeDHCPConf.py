#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shutil
import stat
from optparse import OptionParser
import shellCommand

if os.name == 'posix':
    # Change below to where HTi looks for it
    DhcpConfFileLocation = '/etc/dhcpcd.conf'
elif os.name == 'nt':
    # Change below to where HTx looks for it (soon it will be C:\Program Files\Hobart\dhcpConf)
    DhcpConfFileLocation = 'n/a'
    if os.path.exists(DhcpConfFileLocation):
        os.chmod(DhcpConfFileLocation, stat.S_IWRITE)

def getDhcpConfFileLocation():
    return DhcpConfFileLocation

def DhcpConf_file_exists():
    try:
        with open(DhcpConfFileLocation) as f:
            return True
    except IOError:
        return False

def setHostnameFromDHCP(value):
    #if value='0' then no; else get hostname

    success = False
    if DhcpConf_file_exists():
        try:
            #Read the contents and replace inline
            import fileinput
            foundIt = False
            for line in fileinput.FileInput(DhcpConfFileLocation, inplace=1):
                if "env force_hostname" in line:
                    foundIt = True
                    if value == '0':
                        line = "#env force_hostname=YES"
                    else:
                        line = "env force_hostname=YES"
                    success = True
                print((line.rstrip()))

            #secure against it no being there
            if not foundIt:
                with open(DhcpConfFileLocation, "a") as myfile:
                    entry = "env force_hostname=YES"
                    if value == "0":
                        entry = "#" + entry
                    myfile.write("\n" + entry + "\n")

            cmd = []
            cmd.append("/bin/sync")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        except:
            success = False
    return success

if __name__ == "__main__":

    p = OptionParser()

    # Now add some CLI options
    ##########################
    p.add_option("-H", "--hostname",
                 dest="hostname",
                 help="0=ignore hostname from dhcp server, 1=set hostname from dhcpcd server")
    p.add_option("-v", "--verbose",
                 dest="verbose", default=None, action="store_true",
                 help="Print detailed status")
    p.add_option("-q", "--quiet",
                 dest="verbose", default=None, action="store_false",
                 help="No status printing (default)")

    # Parse User's CLI args
    #######################
    options, args = p.parse_args()

    result = setHostnameFromDHCP(options.hostname)
