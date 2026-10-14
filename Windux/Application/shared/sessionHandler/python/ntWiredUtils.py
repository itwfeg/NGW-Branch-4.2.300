#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# windows only?

import threading
import shellCommand
import configInfo
import osUtils
import re
import string
import os
import subprocess

def getWiredNetworkInfo():
    # OUTPUT is:
    # hostname,MAC,IP,dhcp,gateway,typeOfDevice,speed,N/A,N/A,N/A,N/A

    foundIt = False
    numLines = 0
    hostname = "" #
    MAC = "" #
    IP = "" #
    dhcp = ""
    gateway = "" #
    typeOfDevice = "wire" #
    speed = "" #
    duration = "N/A"
    SSID = "N/A" #
    channel = "N/A" #
    linkQuality = "N/A" #

    cmd = 'ipconfig /all'
    netshcmd = subprocess.Popen(cmd,universal_newlines=True,shell=True,stderr=subprocess.STDOUT,stdout=subprocess.PIPE)
    text = netshcmd.stdout.read()
    retCode = netshcmd.wait()

    #print stdout
    #print valueType
    for line in text.splitlines():
        #print line
        if foundIt:
            numLines = numLines + 1
        if line.find("Host Name") > -1:
            #print line
            s = string.split(line,":")
            hostname = s[1].strip()
        elif line.find("Ethernet adapter Local Area Connection") > -1:
            foundIt = True
        elif numLines < 20:
            if line.find("Physical Address") > -1:
                s = string.split(line,":")
                MAC = s[1].strip()
                MAC = MAC.replace('-',':').upper()
            elif line.find("IPv4 Address") > -1:
                #print line
                s = string.split(line,":")
                s = s[1].strip()
                #print s
                s = string.split(s,"(")
                IP = s[0].strip()
            elif line.find("DHCP Server") > -1:
                s = string.split(line,":")
                dhcp = s[1].strip()
            elif line.find("Default Gateway") > -1:
                s = string.split(line,":")
                gateway = s[1].strip()
        elif numLines > 19:
            pass
    import wmi
    wmi_obj = wmi.WMI()
    wmi_sql = "select * from Win32_NetworkAdapter where NetEnabled=true"

    for item in wmi_obj.query( wmi_sql ):
        linkSpeed = str(int(item.Speed) / 1000000)
        speed = linkSpeed + " Mb/s"

    answer = hostname + "," + MAC + "," + IP + "," + dhcp + "," + gateway + "," + typeOfDevice + "," + speed + "," + duration + "," + SSID + "," + channel + "," + linkQuality
    return answer

if __name__=='__main__':

    # initializers
    exitCode = 0
    print((getWiredNetworkInfo()))
