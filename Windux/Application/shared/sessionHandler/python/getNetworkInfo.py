#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : April 22, 2014
##  OVERVIEW: This script reports data for the Network Icon Popup.
##  REQS    : None
##  RETURNS : comma delimited string for Backend to return to the Qt GUI
######################################################

# Preprocessor directives
##########################
import sys
import os
import string
import shellCommand
import socket, struct

from getHostname import getHostname

from getMacAddress import getMacAddress

def getGateway():
    gateway = ''
    if os.name == "posix":
        with open("/proc/net/route") as fh:
            for line in fh:
                fields = line.strip().split()
                if fields[1] != '00000000' or not int(fields[3], 16) & 2:
                    continue
                return socket.inet_ntoa(struct.pack("<L", int(fields[2], 16)))
    elif os.name == 'nt':
        import ntNetworkUtils
        gateway = ntNetworkUtils.getGateway()
        
    return gateway

def getConnectionType():
    answer = "wire"
    if os.name == "posix":
        scalesettings = "/usr/local/hobart/db/settingswizard/scalesettings"
        if os.path.exists(scalesettings):
            dbFile = open(scalesettings, "r")
            for line in dbFile:
                keyValuePair = line.split('=')
                if keyValuePair[0] == "connectionType":
                    # 0 = wire      1 = wifi
                    answer = str(keyValuePair[1].rstrip())
            dbFile.close()
    elif os.name == "nt":
        import scaleSettingsInterface
        answer =  scaleSettingsInterface.getValue('connectionType')
    if answer == "0":
        return "wire"
    elif answer == "1":
        return "wireless"
    else:
        return "wire"

def getIPAddress(ifname):
    ipAddress = ''
    
    if os.name == 'posix':
       import fcntl
       s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
       ipAddress = socket.inet_ntoa(fcntl.ioctl(s.fileno(), 0x8915, struct.pack('256s',ifname[:15].encode('utf-8')))[20:24])
    else:
        import ntNetworkUtils
        ipAddress = ntNetworkUtils.getIpAddress()

    return ipAddress

def getDHCPServerIPAddress():
    ipAddress = ''
    device = ''
    if os.name == 'posix':
        if getConnectionType() == 'wire':
            device = 'eth0'      
        else:
            device = 'wlan0'
        cmd = []
        cmd.append('dhcpcd')
        cmd.append('-U')
        cmd.append(str(device))
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        for line in stdout.splitlines():
            if line.find("dhcp_server_identifier=\'") > -1:
                ipAddress = line.split("=\'")
                ipAddress = ipAddress[1].split("\'")
                ipAddress = ipAddress[0]
    elif os.name == 'nt':
        import ntNetworkUtils
        if getConnectionType() == 'wire':
            device = 'Local Area Connection'
        else:
            device = 'Wireless Network Connection'
        ipAddress = ntNetworkUtils.getDhcpServer()
        if not ipAddress:
            ipAddress = ''
    return ipAddress

def getConnectionWinSpeed(lanType='wireless'):
    speed = ''
    import ntNetworkUtils
    if lanType == 'wireless':
        speed = ntNetworkUtils.getWlanAdapterWinSpeed('*')
    elif lanTpe == 'wire':

        speed = ntNetworkUtils.getWireWinSpeed('*')

    if speed == None:
        speed = ''
    try:
        if int(speed) < 1000:
            speed = speed + " Mb/s"
        elif int(speed) > 999:
            speed = speed + " Gb/s"
    except:
        speed = speed + " Mb/s"
        pass

    return speed


def getConnectionSpeed(lanType='wire'):
    linkSpeed = ''
    if os.name == 'posix':
        if lanType == 'wire':
            cmd = []
            cmd.append('mii-tool')
            exitcode, stdout, stderr = shellCommand.execute(cmd)
            for line in stdout.splitlines():
                if line.find("eth0:") > -1:
                    linkSpeed = lien.split(": ")
                    linkSpeed = linkSpeed[1].split(",")
                    linkSpeed = linkSpeed[0]

        elif lanType == 'wireless':
            cmd = []
            cmd.append('iwlist')
            cmd.append('wlan0')
            cmd.append('rate')
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            for line in stdout.splitlines():
                if line.find("Current Bit Rate") > -1:
                    linkSpeed = line.split(":")
                    linkSpeed = linkSpeed[1]
    elif os.name == 'nt':
        import wmi

        wmi_obj = wmi.WMI()
        wmi_sql = "select * from Win32_NetworkAdapter where NetEnabled=true"

        for item in wmi_obj.query( wmi_sql ):
            linkSpeed = str(int(item.Speed) / 1000000)
        linkSpeed = linkSpeed + " Mb/s"
    return linkSpeed

def getChannel():
    channel = ''
    if os.name == 'posix':
        try:
            cmd = []
            cmd.append('iwlist')
            cmd.append('wlan0')
            cmd.append('channel')
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            for line in stdout.splitlines():
                if line.find("Current Frequency:") > -1:
                    channel = line.split(":")
                    channel = channel[1]
        except:
            pass
    elif os.name == 'nt':
        import ntNetworkUtils
        channel = ntNetworkUtils.getWlanAdapterChannel('*')
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
    return channel

def getSignalToNoiseRatio():
    ratio = ''
    if os.name == 'posix':
        try:
            cmd = []
            cmd.append('iwconfig')
            cmd.append('wlan0')
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            for line in stdout.splitlines():
                if line.find("Link Quality") > -1:
                    ratio = line.lstrip()
                    ratio = ratio.rstrip()
        except:
            pass
    elif os.name == 'nt':
        import ntNetworkUtils
        ratio = ntNetworkUtils.getWlanAdapterSignalStrength('Wireless Network Connection')
        try:
            converted = int(ratio) / 100.0
            converted = converted * 70.0
            ratio = "Link Quality=" + str(int(converted)) + "/70"
        except:
            pass
    return ratio

'''
Not supported by Module driver
def getConnectionDurationTime(lanType='wire'):
    time = ''
    if lanType == 'wire':
        #get difference in lease time
    elif lanType == 'wireless':

    return time
'''

def getSSID():
    SSID = ''
    if os.name == 'posix':
        try:
            cmd = []
            cmd.append('iwconfig')
            cmd.append('wlan0')
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            if stdout.find("Access Point: Not-Associated") > -1:
                SSID = ''
            else:
                for line in stdout.splitlines():
                    if line.find("ESSID:") > -1:
                        SSID = line.split("ESSID:\"")
                        SSID = SSID[1].split("\"")            
                        SSID = SSID[0]
        except:
            pass
    elif os.name == 'nt':
        import ntNetworkUtils
        SSID = ntNetworkUtils.getWlanAdapterSsid()
    return SSID

if __name__=='__main__':
    
    # initializers
    exitCode = 0
    Hostname = ''
    MACAddress = ''
    IPAddress = ''
    DHCPServerIPAddress = ''
    GatewayIPAddress = ''
    ConnectionType = ''
    ConnectionSpeed = ''
    ConnectionDurationTime = 'N/A'
    SSID = 'N/A'
    WirelessChannelUsed = 'N/A'
    SignalToNoiseRatio = 'N/A'

    connectionType = getConnectionType()
    if os.name == "nt":
        if connectionType == "wireless":
            import ntWirelessUtils
            print((ntWirelessUtils.getWirelessNetworkInfo()))
        elif connectionType == "wire":
            import ntWiredUtils
            print((ntWiredUtils.getWiredNetworkInfo()))
    else:
        try:
            Hostname = getHostname()[1]
            if Hostname == '':
                Hostanme = 'N/A'
        except:
            Hostname = 'N/A'
            pass
        try:
            MACAddress = getMacAddress(getConnectionType())[1]
            MACAddress = MACAddress.upper()
            if MACAddress == '':
                MACAddress = 'N/A'
        except:
            MACAddress = 'N/A'
            pass
        try:
            GatewayIPAddress = getGateway()
            if GatewayIPAddress == '':
                GatewayIPAddress = 'N/A'
        except:
            GatewayIPAddress = 'N/A'
            pass
        try:
            ConnectionType = getConnectionType()
            if ConnectionType == '':
                 ConnectionType = 'N/A'
        except:
            ConnectionType = 'N/A'
            pass
        try:
            if ConnectionType == 'wire':
                IPAddress = getIPAddress('eth0')
            elif ConnectionType == 'wireless':
                IPAddress = getIPAddress('wlan0')
            #IPAddress = getIPAddress()
            if IPAddress == '':
                IPAddress = 'N/A'
        except:
            IPAddress = 'N/A'
            pass
        try:
            DHCPServerIPAddress = getDHCPServerIPAddress()
            if DHCPServerIPAddress == '':
                DHCPServerIPAddress = 'N/A'
        except:
            DHCPServerIPAddress = 'N/A'
            pass
        try:
            ConnectionType = getConnectionType()
            if ConnectionType == '':
                ConnectionType = 'N/A'
        except:
            ConnectionType = 'N/A'
            pass
        try:
            if os.name == 'posix':
                ConnectionSpeed = getConnectionSpeed(ConnectionType)
            else:
                if ConnectionType == "wireless":
                    ConnectionSpeed = getConnectionWinSpeed(ConnectionType)
                else:
                    ConnectionSpeed = getConnectionSpeed(ConnectionType)
            if ConnectionSpeed == '':
                ConnectionSpeed = 'N/A'
        except:
            ConnectionSpeed = 'N/A'
            pass
    
        # Not supported by logging or driver or module
        # ConnectionDurationTime = getConnectionDurationTime(ConnectionType)


        if ConnectionType == "wireless":
            try:
                SSID = getSSID()
                if SSID == '':
                    SSID = 'N/A'
            except:
                SSID = 'N/A'
                pass
            try:
                WirelessChannelUsed = getChannel()
                if WirelessChannelUsed == '':
                    WirelessChannelUsed = 'N/A'
            except:
                WirelessChannelUsed = 'N/A'
                pass
            try:
                SignalToNoiseRatio = getSignalToNoiseRatio()
                if SignalToNoiseRatio == '':
                    SignalToNoiseRatio = 'N/A'
            except:
                SignalToNoiseRatio = 'N/A'
                pass

        print((Hostname + ',' + MACAddress + ',' + IPAddress + ',' + DHCPServerIPAddress + ',' + GatewayIPAddress + ',' + ConnectionType + ',' + ConnectionSpeed + ',' + ConnectionDurationTime + ',' + SSID + ',' + WirelessChannelUsed + ',' + SignalToNoiseRatio))

        # final string in the format: hostname,MACAddress,IPAddress,DHCPServerIPAddress,GatewayIPAddress,ConnectionType,ConnectionSpeed,ConnectedDurationTime,SSID,WirelessChannelUsed,SignalToNoiseRatio
        # e.g. "Walmart-Deli2,12:23:34:45:56,192.168.1.2,192.168.1.34,192.168.1.1,Wifi,54 Mb/sec,03:30:02,wwST1,6,51/70
    
    sys.exit(exitCode)


