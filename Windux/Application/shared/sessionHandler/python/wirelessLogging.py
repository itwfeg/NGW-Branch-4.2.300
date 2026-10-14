#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import file_io
from optparse import OptionParser
import shutil
import subprocess
import encryptDecrypt
import codecs


def startWirelessLogging(startLogging):
    if os.name == "posix":

        if startLogging == "1":
            print("A")
            cmd = []
            cmd.append("pkill")
            cmd.append("-9")
            cmd.append("wpa_supplicant")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            print("B")
            cmd = []
            cmd.append("pkill")
            cmd.append("-9")
            cmd.append("dhcpcd")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            print("C")

            command = "wpa_supplicant -Dwext -iwlan0 -c/etc/wpa_supplicant.conf -dd"
            with open('/usr/local/hobart/db/settingswizard/wirelessLogging.txt', "w") as outfile:
                subprocess.call(command, stdout=outfile, shell=True)
            print("D")
        elif startLogging == "0":
            #stop logging
            #print("A")
            cmd = []
            cmd.append("pkill")
            cmd.append("-9")
            cmd.append("wpa_supplicant")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #print("B")
            cmd = []
            cmd.append("pkill")
            cmd.append("-9")
            cmd.append("dhcpcd")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #print "C"

            cmd = []
            cmd.append("ip")
            cmd.append("addr")
            cmd.append("flush")
            cmd.append("dev")
            cmd.append("wlan0")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            cmd = []
            cmd.append("wpa_supplicant")
            cmd.append("-Dwext")
            cmd.append("-iwlan0")
            cmd.append("-c/etc/wpa_supplicant.conf")
            cmd.append("-B")
            exitCode, stdout, stderr = shellCommand.execute(cmd)


        isStatic = ""
        autoDNS = ""
        ipAddress = ""
        netmask = ""
        gateway = ""
        dns1 = ""
        dns2 = ""
        dns3 = ""
        dbFile = open("/usr/local/hobart/db/settingswizard/scalesettings", "r")
        for line in dbFile:
            line = line.strip()
            keyValuePair = line.split('=')
            ####################################################################
            if keyValuePair[0] == "isStatic":
                isStatic = keyValuePair[1]
            elif keyValuePair[0] == "autoDNS":
                autoDNS = keyValuePair[1]
            elif keyValuePair[0] == "ipAddress":
                ipAddress = keyValuePair[1]
            elif keyValuePair[0] == "netmask":
                netmask = keyValuePair[1]
            elif keyValuePair[0] == "gateway":
                gateway = keyValuePair[1]
            elif keyValuePair[0] == "dns1":
                dns1 = keyValuePair[1]
            elif keyValuePair[0] == "dns2":
                dns2 = keyValuePair[1]
            elif keyValuePair[0] == "dns3":
                dns3 = keyValuePair[1]
        dbFile.close()

        if isStatic == "0":
            cmd = []
            cmd.append("dhcpcd")
            cmd.append("wlan0")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
        elif isStatic == "1":
            cmd = []
            cmd.append("ifconfig")
            cmd.append("wlan0")
            cmd.append("ipAddress")
            cmd.append("netmask")
            cmd.append(netmask)
            sys.stdout.flush()
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            cmd = []
            cmd.append("ip")
            cmd.append("route")
            cmd.append("add")
            cmd.append("default")
            cmd.append("via")
            cmd.append(gateway)
            sys.stdout.flush()
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        if autoDNS == "0":
            try:
                realResolvConf = open("/etc/resolv.conf", "r")
                newResolvConfArray = []
                for line in realResolvConf:
                    if "nameserver" not in line:
                        newResolvConfArray.append(line)
                realResolvConf.close()
                if len(str(dns1)) > 0:
                    newResolvConfArray.append("nameserver " + str(dns1))
                if len(str(secondary)) > 0:
                    newResolvConfArray.append("nameserver " + str(dns2))
                if len(str(tertiary)) > 0:
                    newResolvConfArray.append("nameserver " + str(dns3))
                newResolvConfFile = open("/etc/resolvNew.conf", "w")
                for item in newResolvConfArray:
                    newResolvConfFile.write("%s\n" % item)
                newResolvConfFile.close()

                cmd = []
                cmd.append("mv")
                cmd.append("/etc/resolvNew.conf")
                cmd.append("/etc/resolv.conf")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
            except:
                pass
           
            cmd = []
            cmd.append("sync")
            exitCode, stdout, stderr = shellCommand.execute(cmd)


           ####################################################################


            #command = "wpa_supplicant -Dwext -iwlan0 -c/etc/wpa_supplicant.conf -dd"
            #with open('/usr/local/hobart/db/settingswizard/wirelessLogging.txt', "w") as outfile:
            #    subprocess.call(command, stdout=outfile)
            #print "D"
    else:
        TBD = "windows"


if __name__ == "__main__":

    # Let's get a parser to use
    ###########################
    p = OptionParser()

    # Now add some CLI options
    ##########################
    p.add_option("-S", "--startLogging",
                 dest="startLogging",
                 help="Whether to start or stop logging, 1= start, 0 = stop")
    p.add_option("-v", "--verbose",
                 dest="verbose", default=None, action="store_true",
                 help="Print detailed status")
    p.add_option("-q", "--quiet",
                 dest="verbose", default=None, action="store_false",
                 help="No status printing (default)")

    options, args = p.parse_args()
    startWirelessLogging(options.startLogging)
    exitCode = 0


    sys.exit(0)
