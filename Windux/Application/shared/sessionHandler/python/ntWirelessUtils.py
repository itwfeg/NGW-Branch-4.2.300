import threading
import shellCommand
import configInfo
import osUtils
import re
import string
import os
import subprocess

def getWirelessNetworkInfo():
    value = None

    # run netsh command
    exitCode, stdout, stderr = shellCommand.execute(['netsh', 'wlan', 'show', 'interface'])

    # OUTPUT is:
    # hostname,MAC,IP,dhcp,gateway,typeOfDevice,speed,duration,ssid,channel,linkQuality
    hostname = "" #
    MAC = "" #
    IP = "" #
    dhcp = ""
    gateway = "" #
    typeOfDevice = "wireless" #
    speed = "" #
    duration = ""
    SSID = "" #
    channel = "" #
    linkQuality = "" #

    #print stdout
    #print valueType
    for line in stdout.splitlines():
        #print line
        if line.find("Physical address") > -1:
            #print line
            valueMatch = re.search("\s+Physical address\s*:\s*(([0-9A-F]{2}([-:][0-9A-F]{2}){1,5}))",  line,  re.I)
            if valueMatch:
                MAC = valueMatch.group(1).strip()
                MAC = MAC.replace('-',':').upper()

            #print value
        elif line.find(" SSID") > -1:
            #print line
            s = string.split(line,":")
            SSID = s[1].strip()
        elif line.find("Receive") > -1:
            s = string.split(line,":")
            speed = s[1].strip()
            try:
                if int(speed) < 1000.0:
                    speed = speed + " Mb/s"
                elif int(speed) > 999.0:
                    speed = speed + " Gb/s"
            except:
                speed = speed + " Mb/s"
                pass
        elif line.find("Channel") > -1:
            s = string.split(line,":")
            channel = s[1].strip()
            if channel == '1':
                channel = "2.412 GHz (Channel 1)"
            elif channel == '2':
                channel = "2.417 GHz (Channel 2)"
            elif channel == '3':
                channel = "2.422 GHz (Channel 3)"
            elif channel == '4':
                channel = "2.427 GHz (Channel 4)"
            elif channel == '5':
                channel = "2.432 GHz (Channel 5)"
            elif channel == '6':
                channel = "2.437 GHz (Channel 6)"
            elif channel == '7':
                channel = "2.442 GHz (Channel 7)"
            elif channel == '8':
                channel = "2.447 GHz (Channel 8)"
            elif channel == '9':
                channel = "2.452 GHz (Channel 9)"
            elif channel == '10':
                channel = "2.457 GHz (Channel 10)"
            elif channel == '11':
                channel = "2.462 GHz (Channel 11)"
            elif channel == '12':
                channel = "2.467 GHz (Channel 12)"
            elif channel == '13':
                channel = "2.472 GHz (Channel 13)"
            elif channel == '14':
                channel = "2.484 GHz (Channel 14)"
            elif channel == '131':
                channel = "3.6575 GHz (Channel 131)"
            elif channel == '132':
                channel = "3.660 GHz (Channel 132)"
            elif channel == '133':
                channel = "3.665 GHz (Channel 133)"
            elif channel == '134':
                channel = "3.670 GHz (Channel 134)"
            elif channel == '135':
                channel = "3.675 GHz (Channel 135)"
            elif channel == '136':
                channel = "3.680 GHz (Channel 136)"
            elif channel == '137':
                channel = "3.685 GHz (Channel 137)"
            elif channel == '138':
                channel = "3.690 GHz (Channel 138)"
            elif channel == '149':
                channel = "5.745 GHz (Channel 149)"
            elif channel == '153':
                channel = "5.765 GHz (Channel 153)"
            elif channel == '157':
                channel = "5.785 GHz (Channel 157)"
            elif channel == '161':
                channel = "5.805 GHz (Channel 161)"
            elif channel == '165':
                channel = "5.825 GHz (Channel 165)"

            #print channel
        elif line.find("Signal") > -1:
            s = string.split(line,":")
            linkQuality = s[1].strip()
            ratio = linkQuality.replace("%","")
            try:
                converted = int(ratio) / 100.0
                converted = converted * 70.0
                linkQuality = "Link Quality=" + str(int(converted)) + "/70"
            except:
                pass

    # now need hostname IP DHCP GATEWAY
    cmd = []
    cmd.append("hostname")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    hostname = string.strip(stdout)

    cmd = 'netsh interface ipv4 show address name="Wireless Network Connection"'
    netshcmd = subprocess.Popen(cmd,universal_newlines=True,shell=True,stderr=subprocess.STDOUT,stdout=subprocess.PIPE)
    #stdout, errors = netshcmd.communicate()
    text = netshcmd.stdout.read()
    retCode = netshcmd.wait()
    #print text
    for line in text.splitlines():
        if line.find("IP Address") > -1:
            #print line
            s = string.split(line,":")
            IP = s[1].strip()
        elif line.find("Default Gateway") > -1:
            s = string.split(line,":")
            gateway = s[1].strip()

    exitCode, stdout, stderr = shellCommand.execute(['ipconfig', '/all'])

    dhcpServerMatch = re.search("\s+DHCP Server . . . . . . . . . . . :\s*((?:[0-9]{1,3}\.){3}[0-9]{1,3})",  stdout,  re.I)

    if dhcpServerMatch:
        dhcp = dhcpServerMatch.group(1)
    answer = hostname + "," + MAC + "," + IP + "," + dhcp + "," + gateway + "," + typeOfDevice + "," + speed + "," + duration + "," + SSID + "," + channel + "," + linkQuality
    return answer

if __name__=='__main__':

    # initializers
    exitCode = 0
    print((getWirelessNetworkInfo()))
