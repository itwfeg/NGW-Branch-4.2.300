######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script sets Linux/WES7 system network interfaces.
##  REQS    : Must have permissions to execute the
##            OS's commands without password
##            prompt. All options must be provided, except for -v -h -q
######################################################


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
else:
    import _winreg
import shutil
import re
import configInfo
import createWirelessProfile
import math
import bootSettings
import osUtils

SIOCGIFNETMASK = 0x891b

debug = 0

def get_network_mask(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    netmask = fcntl.ioctl(s, SIOCGIFNETMASK, struct.pack('256s', ifname))[20:24]
    return socket.inet_ntoa(netmask)

def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

def get_gateway(ifname):
    cmd = "ip route list dev "+ ifname + " | awk ' /^default/ {print $3}'"
    fin,fout = os.popen4(cmd)
    result = fout.read()
    #print result
    return result

def get_DNS():
    nameservers= ""
    count = 0
    try:
        rconf = open("/etc/resolv.conf","r")
        line = rconf.readline()
        while line:
            #ip = re.search(r"\b(?:[0-9]{1,3}\.){3}[0-9]{1,3}\b",line)

            if len(line) > 0:
                words = line.split(' ')
                if words[0] == 'nameserver':
                    nameservers += "|" + words[1].rstrip('\n')
            line= rconf.readline()
        rconf.close()
    except:
        pass
    return nameservers

def printNetworkInfo(deviceName):
    #print("Made it here")
    #sys.stdout.flush()
    output = str(get_ip_address(deviceName)).rstrip('\n')
    output += "|" + str(get_gateway(deviceName)).rstrip('\n')
    output += "|" + str(get_network_mask(deviceName)).rstrip('\n')
    output += str(get_DNS()).rstrip('\n')
    return output

def getCertIdentity(certName):
    answer = ""
    #openssl x509 -noout -in 123456789_cert.pem -subject
    cmd = []
    cmd.append("openssl")
    cmd.append("x509")
    cmd.append("-noout")
    cmd.append("-in")
    cmd.append(certName)
    cmd.append("-subject")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    #print certName
    #print stdout
    #print stderr
    stdout = stdout.strip()
    subject = stdout.split('/')
    for line in subject:
        parts = line.split('=')

        if parts[0] == "CN":
            # set customer brightness
            #scaleSettings['serverCertFilename'] = str(keyValuePair[1])
            print(str(parts[1]))
            answer = str(parts[1])
    return answer

def powerNetworkDevices(states):

    if states[1:2] == "1":
        #turn on wifi
        cmd = []
        cmd.append("ifconfig")
        cmd.append("wlan0")
        cmd.append("up")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
    elif states[1:2] == "0":
        #turn off wifi
        cmd = []
        cmd.append("ifconfig")
        cmd.append("wlan0")
        cmd.append("down")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    if states[0:1] == "1":
        #turn on wire
        cmd = []
        cmd.append("ifconfig")
        cmd.append("eth0")
        cmd.append("up")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
    elif states[0:1] == "0":
        #turn off wire
        cmd = []
        cmd.append("ifconfig")
        cmd.append("eth0")
        cmd.append("down")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

def setDNSSuffixes(first, second, third):
    try:
        realResolvConf = open("/etc/resolv.conf", "r")
        newResolvConfArray = []
        foundFirstNameServer = False
        foundSearch = False
        for line in realResolvConf:
            if "search" in line:
                foundSearch = True
                newResolveConfArray.append("search " + first + " " + second + " " + third)
            elif "nameserver" in line and not foundFirstNameServer and not foundSearch:
                    foundFirstNameServer = True
                    newResolveConfArray.append("search " + first + " " + second + " " + third)
                    newResolvConfArray.append(line)
            else:
                newResolveConfArray.append(line)
        realResolvConf.close()

        newResolvConfFile = open("/etc/resolvSuffix.conf", "w")
        for item in newResolvConfArray:
            newResolvConfFile.write("%s\n" % item)
        newResolvConfFile.close()

        cmd = []
        cmd.append("mv")
        cmd.append("/etc/resolvSuffix.conf")
        cmd.append("/etc/resolv.conf")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
    except:
        pass
    cmd = []
    cmd.append("sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)


def setNameServers(primary, secondary, tertiary, first, second, third):
    try:
        #realResolvConf = open("/etc/resolv.conf", "r")
        newResolvConfArray = []
        #for line in realResolvConf:
        #    if "nameserver" not in line and "search" not in line:
        #        newResolvConfArray.append(line)
        #realResolvConf.close()
        suffixString = ""
        if len(str(first)) > 0 or len(str(second)) > 0 or len(str(third)) > 0:
            if len(str(first)) > 0:
                suffixString = suffixString + str(first)
            if len(str(second)) > 0:
                suffixString = suffixString + " " + str(second)
            if len(str(third)) > 0:
                suffixString = suffixString + " " + str(third)
            newResolvConfArray.append("search " + suffixString)
        if len(str(primary)) > 0 and primary != None:
            newResolvConfArray.append("nameserver " + str(primary))
        if len(str(secondary)) > 0 and secondary != None:
            newResolvConfArray.append("nameserver " + str(secondary))
        if len(str(tertiary)) > 0 and tertiary != None:
            newResolvConfArray.append("nameserver " + str(tertiary))

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

def setEtcNetworkInterfacesFileDHCP(deviceName):
    tbd = ''
    # copy /etc/network/interfaces?-DHCP to /etc/network/interfaces file
    try:
        os.remove("/etc/network/interfaces")
        if deviceName == 'eth0':
            shutil.copy2('/etc/network/interfacesWIRE-DHCP', '/etc/network/interfaces')
        else:
            shutil.copy2('/etc/network/interfacesWIFI-DHCP', '/etc/network/interfaces')
    except:
        pass
    cmd = []
    cmd.append("sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)


def set_reg(name, value):
    name = "SearchList"
    REG_PATH = r"SYSTEM\ControlSet001\services\Tcpip\Parameters"
    if os.name == "nt":
        try:
            _winreg.CreateKey(_winreg.HKEY_LOCAL_MACHINE, REG_PATH)
            registry_key = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, REG_PATH, 0,
                                                        _winreg.KEY_WRITE)
            _winreg.SetValueEx(registry_key, name, 0, _winreg.REG_SZ, value)
            _winreg.CloseKey(registry_key)
            return True
        except WindowsError:
            return False

def get_reg(name):
    if os.name == "nt":
        name = "SearchList"
        REG_PATH = r"SYSTEM\ControlSet001\services\Tcpip\Parameters"
        try:
            registry_key = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, REG_PATH, 0,
                                                        _winreg.KEY_READ)
            value, regtype = _winreg.QueryValueEx(registry_key, name)
            _winreg.CloseKey(registry_key)
            return value
        except WindowsError:
            return None

def showWirelessAPs():
    try:
        # iwlist is not antiquated and so iw must be used instead
        # iw wlan0
        test = 1
    except:
        pass

def setEtcNetworkInterfacesFileSTATIC(deviceName, address, netmask, gateway):
    tbd = ''
    try:
        os.remove("/etc/network/interfaces")
        if deviceName == 'eth0':
            shutil.copy2('/etc/network/interfacesWIRE-STATIC', '/etc/network/interfaces')
        else:
            shutil.copy2('/etc/network/interfacesWIFI-STATIC', '/etc/network/interfaces')
    except:
        pass
    try:
        f = open("/etc/network/interfaces", "a")
        if len(address) > 0 and address != "" and address.rstrip() != "64641964":
            f.write("   address " + address + "\n")
        if len(netmask) > 0 and netmask != "" and netmask.rstrip() != "64641964":
            f.write("   netmask " + netmask + "\n")
        if len(gateway) > 0 and gateway != "" and gateway.rstrip() != "64641964":
            f.write("   gateway " + gateway + "\n")
        f.close()
    except:
        pass
    cmd = []
    cmd.append("sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)


def testConnection(deviceName):
    #sys.stdout.flush()
    connected = True
    keepLooking = True
    downDevice = "wlan0"
    if deviceName == "wlan0":
        downDevice = "eth0"
    exitCode = 0

    #print downDevice
    try:
        cmd = []
        cmd.append("ifconfig")
        cmd.append(str(downDevice))
        cmd.append("down")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
    except:
        exitCode = 0

    #cmd = []
    #cmd.append("netstat")
    #cmd.append("-r")
    #exitCode, stdout, stderr = shellCommand.execute(cmd)
    #print 'made it1'
    #print 'code is'
    #print exitCode
    #if exitCode == 0:
    #time.sleep(1)
    #print 'made it2'
    #print deviceName
    gateway = get_gateway(deviceName)
    #print (get_gateway(deviceName))
    #print(stdout)
    #sys.stdout.flush()
    #for line in string.split(stdout, "\n"):
    #    #print(line)
    #    if keepLooking:
    #        if deviceName in line:
    #            keepLooking = False
    #            words = line.split()
    #            #print("begin")
    #            #print(str(words[2]))
    #            #print("done")
    if len(gateway) < 7:
        connected = False
    else:
        cmd = []
        cmd.append("ping")
        cmd.append(str(gateway))
        cmd.append("-c")
        cmd.append("5")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        #print("pinging " + str(cmd[1]))
        sys.stdout.flush()
        for line in string.split(stdout, "\n"):
            if "Unreachable" in line:
                connected = False
            elif "unknown" in line:
                connected = False

    try:
        cmd = []
        cmd.append("ifconfig")
        cmd.append(str(downDevice))
        cmd.append("up")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
    except:
        pass

    '''try:
        if not connected:
            if deviceName == "wlan0":
                cmd = []
                cmd.append("ifconfig")
                cmd.append(deviceName)
                cmd.append("down")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                cmd = []
                cmd.append("ifconfig")
                cmd.append(deviceName)
                cmd.append("up")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                cmd = []
                cmd.append("killall")
                cmd.append("wpa_cli")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                cmd = []
                cmd.append("killall")
                cmd.append("wpa_supplicant")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

    except:
        pass
    '''
    return connected
        #sys.stdout.flush()

##################################
#
#     G L O B A L S
#
# some easy to change vars below
#
#
#################################

# file format->
# Enter relative path & file for save
global returnMessage
returnMessage = " "

##################################
#
#     E N D   G L O B A L S
#
##################################

# First thing, get a timestamp for this
# executable run
######################################

#fileTimezone = "-05"

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################

p.add_option("-U", "--user",
             dest="user",
             help="The UserID of the person setting the DateTime (ex. 00002)")
p.add_option("-m", "--machineID",
             dest="machineID",
             help="The MachineID of the scale setting the DateTime (ex. 00002)")
p.add_option("-c", "--connected",
             dest="isConnected",
             help="Stores if network adapter is connected (ex. 1=yes, 0=no)")
p.add_option("-s", "--static",
             dest="isStatic",
             help="Is Static IP or DHCP (ex. 1=static 0=dhcp)")
p.add_option("-D", "--autoDNS",
             dest="isAutoDNS",
             help="Is Auto DNS used or Manual (ex. 1=Auto, 0=Manual)")
p.add_option("-I", "--ip",
             dest="ipAddress",
             help="The static/manual IP Address to use for the connection (ex. 192.168.0.4)")
p.add_option("-B", "--subnet",
             dest="subnetMask",
             help="The subnet Mask to use for the connection (ex. 255.255.255.0)")
p.add_option("-G", "--gateway",
             dest="gateway",
             help="The default gateway to use for the connection (ex. 192.168.0.1)")
p.add_option("-1", "--primary",
             dest="primaryServer",
             help="The primary DNS server to use for the connection (ex. 192.168.1.12)")
p.add_option("-2", "--secondary",
             dest="secondaryServer",
             help="The secondary DNS server to use for the connection (ex. 192.168.1.14)")
p.add_option("-3", "--tertiary",
             dest="tertiaryServer",
             help="The tertiary DNS server to use for the connection (ex. 192.168.1.12)")
p.add_option("-W", "--wireless",
             dest="isWireless",
             help="Is Wireless Adapter used or Wired (ex. 1=wireless, 0=wired)")
p.add_option("-S", "--SSID",
             dest="ssid",
             help="SSID of the Wireless Router (ex. StoreWireless)")
p.add_option("-M", "--security",
             dest="securityMethod",
             help="The security method to use for the connection (ex. Open)")
p.add_option("-E", "--encryption",
             dest="encryption",
             help="The encryption method to use for the connection (ex. WEP)")
p.add_option("-v", "--vpn",
             dest="isVPN",
             help="Is VPN or not (1=yes, 0=no)")
p.add_option("-V", "--vpnUsername",
             dest="vpnUser",
             help="The username for the VPN account.")
p.add_option("-P", "--vpnPassword",
             dest="vpnPassword",
             help="The password for the VPN account.")
p.add_option("-w", "--WEPKey1",
             dest="WEPKey1",
             help="Text for WEP Key 1")
p.add_option("-x", "--WEPKey2",
             dest="WEPKey2",
             help="Text for WEP Key 2")
p.add_option("-y", "--WEPKey3",
             dest="WEPKey3",
             help="Text for WEP Key 3")
p.add_option("-z", "--WEPKey4",
             dest="WEPKey4",
             help="Text for WEP Key 4")
p.add_option("-K", "--defaultWEPKey",
             dest="defaultKey",
             help="The Default WEP Key to use (ex. -K 2 use WEP Key 2 as default)")
p.add_option("-N", "--EAPUsername",
             dest="EAPUsername",
             help="Username for EAP type connections")
p.add_option("-n", "--EAPPassword",
             dest="EAPPassword",
             help="Password for EAP type connections")
p.add_option("-X", "--scan",
             dest="scan",
             help="Scan for wireless devices and return Signal Strength bars Qty with SSIDs (ex. use value 1 to scan, or value 0 for no scan)")
p.add_option("-A", "--wpaPassPhrase",
             dest="wpaPassPhrase",
             help="The WPA Passhrpase")
p.add_option("-C", "--Connect",
             dest="connectDevices",
             help="Power on/off devices 0 or 1 wired first wifi second, e.g., 01=wire off/wifi on - 11=wire on/wifi on - 10=wire on/wifi off - 00=wire off/wifi off)")
p.add_option("-F", "--Manufacturing",
             dest="manufacturingTest",
             help="Changes /etc/network/interfaces file or not, e.g., 64641964 = no change or use any other number changes it")
p.add_option("-L", "--serverCertFilename",
             dest="serverCertFilename",
             help="Filename of the server certificate. Just the filename, not full path.")
p.add_option("-Y", "--chainFilename",
             dest="chainFilename",
             help="Filename of the chain file certificate. Just the filename, not full path.")
p.add_option("-Z", "--clientCertFilename",
             dest="clientCertFilename",
             help="Filename of the client certificate. Just the filename, not full path.")
p.add_option("-a", "--linkRate",
             dest="linkRate",
             help="The link rate in Mbps.")
p.add_option("-b", "--automaticLinkRate",
             dest="automaticLinkRate",
             help="Whether to use automatic rate throttling or not. 0 = fixed rate, 1 = automatic rate")
p.add_option("-d", "--channel",
             dest="channel",
             help="The wireless channel desired.")
p.add_option("-5", "--DNSSuffix1",
             dest="dnsSuffix1",
             help="suffix 1.")
p.add_option("-6", "--DNSSuffix2",
             dest="dnsSuffix2",
             help="suffix 2")
p.add_option("-7", "--DNSSuffix3",
             dest="dnsSuffix3",
             help="suffix 3")
p.add_option("-8", "--DNSSuffix4",
             dest="dnsSuffix4",
             help="suffix 4")
p.add_option("-9", "--DNSSuffix5",
             dest="dnsSuffix5",
             help="suffix 5")
p.add_option("-4", "--DNSSuffix6",
             dest="dnsSuffix6",
             help="suffix 6")
p.add_option("-O", "--verbose",
             dest="verbose", default=None, action="store_true",
             help="Print detailed status")
p.add_option("-q", "--quiet",
             dest="verbose", default=None, action="store_false",
             help="No status printing (default)")


# Parse User's CLI args
#######################
options, args = p.parse_args()

# Use this for debugging
##########################

#print 'outputfile: %s'%options.outputFile
#print 'verbose   : %s'%options.verbose
#print 'options      : %r',options

# Holds value that network has been configured (return "good" if successful)
#with open("netting.txt", "w") as myfi:
#    myfi.writelines(sys.argv[1:])
#    #myfi.write(wpa_supplicantFileContents + "\n")
#myfi.close()

if os.name == "posix":
    if options.scan == "0":
        try:
            f = open("/usr/local/hobart/db/settingswizard/setNetworkCommands.txt", "w")
            f.writelines("options : %r" % (options))
            #for line in sys.argv
            f.close()
        except:
            pass
    #print("we")
    #print("wwww")
    # Preparing the network interface
    #################################

    # os.system("xemacs")- for Testing

    ##############################
    #
    # if scan=1 Just return Access Points within range, do not change network
    # This part figures out how many bars to display based on signal strength
    #
    ################################

    # need the wireless card name - it changes from wlan0 wlan1 wlan2 etc

    if len(str(options.connectDevices)) == 2:
        #User wants to set network deivce power states
        powerNetworkDevices(str(options.connectDevices))

    elif options.scan == "1":
        #__dir__ = os.path.dirname(os.path.abspath(__file__))
        #filepath = os.path.join(__dir__, 'apList.txt')

        #testFile = open(filepath, 'rU')
        #linestrings = testFile.read()
        #powerNetworkDevices("31")

        #linestrings = subprocess.check_output("iwlist wlan0 scan", shell=True)
        #print ("what")
        # Command below shows signal strength
        # watch -n 1 "awk 'NR==3 {print \"WiFi Signal Strength = \" \$3 \"00 %\"}''' /proc/net/wireless"
        stdout = ""
        f = open("stoppingWPA.txt", "w")

        try:
            cmd = []
            cmd.append("iwlist")
            cmd.append("wlan0")
            cmd.append("scan")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #f.write("iwlist wlan0 scan\n")
        except:
            #print("failed")
            #f.write("iwlist wlan0 scan failed\n")
            pass
        #print stdout
        SSIDList = [];
        linestrings = ""
        includeIt = True;

        #print(linestrings)
        #print("\n\n")
        strengthNumber = "0"
        encrypt = "AES"
        eraseLast = False
        shouldFindAnSSID = False

        failedToRun = False
        #for line in linestrings.splitlines():
        answer = stdout
        #f.write("AAAAAAAAAAAAA\nanswer\n\n\n\n")
        for line in stdout.splitlines():
            if line.find("support scanning") > -1:
                # Hop out of loop
                #time.sleep(2)
                try:
                    failedToRun = True
                    cmd = []
                    cmd.append("/usr/sbin/wpa_cli")
                    cmd.append("terminate")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    #f.write("wpa_cli terminate\n")
                    time.sleep(2)
                    cmd = []
                    cmd.append("ifconfig")
                    cmd.append("wlan0")
                    cmd.append("up")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    #f.write("ifconfig wlan0 up 1\n")
                    #print ("DAVIS")
                except:
                    cmd = []
                    cmd.append("ifconfig")
                    cmd.append("wlan0")
                    cmd.append("up")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    #f.write("ifconfig wlan0 up 2\n")
                    #print exitCode
                    #print stdout
                    #print stderr
        #print("TODD")

        if failedToRun:
            time.sleep(3)
            #print("happeninig")
            cmd = []
            cmd.append("iwlist")
            cmd.append("wlan0")
            cmd.append("scan")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #f.write("iwlist wlan0 scan 1\n")
            #f.write("BBBBBBBBBBBBBBBB\n" + stdout + "\n")
            answer = stdout

            #if stdout.find("Interface doesn't support scanning:
            #print("XXXXXXXXXXXXXXXXXXXXXXXXXXX")
            #print exitCode
            #print stdout
        #f.write("XXXXXXXXXXXXXXX\n\n\n\n")
        #f.write("WWWWWWWWWWWWWWWWW\nWWWWWWWWWWW\n\noutside answer\n" + answer + "\n")
        for line in answer.splitlines():
            #print("here")
            if line.find("ESSID:\"") > -1:
                shouldFindAnSSID = False
                s = string.split(line,":\"")
                s = string.split(s[1],"\"")
                if len(s[0]) > 0 and s[0] != '' and s[0] not in SSIDList:
                    #print(str(s[0]))
                    #print(str(len(s[0])) + '\n')
                    #f.write("SSID: " + s[0] + "\n")
                    SSIDList.append(s[0])
                    includeIt = True
                else:
                    includeIt = False

                if len(s[0]) == 0 or s[0] == '' or s[0] == None:
                    includeIt = False
                if not includeIt:
                    SSIDList.pop()
                    SSIDList.pop()
                    SSIDList.pop()
            elif line.find("Quality=") > -1:
                if shouldFindAnSSID == True:
                    SSIDList.pop()
                    SSIDList.pop()
                    SSIDList.pop()
                
                shouldFindAnSSID = True
                s = string.split(line,"=")
                noise = string.split(s[2]," ")
                s = string.split(s[1]," ")
                s = string.split(s[0],"/")

                '''if int(s[0]) == 0:
                    SSIDList.append('0')
                elif int(s[0]) > 56:
                    SSIDList.append('5')
                else:
                    SSIDList.append(str((int(s[0]) / 14) + 1))
                '''


                percentage = 100 * int(s[0])/70

                if percentage < 21:
                    SSIDList.append('1')
                elif percentage < 41:
                    SSIDList.append('2')
                elif percentage < 61:
                    SSIDList.append('3')
                elif percentage < 81:
                    SSIDList.append('4')
                elif percentage < 110:
                    SSIDList.append('5')

                SSIDList.append(str(percentage))
                SSIDList.append(str(noise[0]))

                #SSIDList.append(s[0])
                #print s[0]
        
        #testFile.close()
        if shouldFindAnSSID:
            SSIDList.pop()
            SSIDList.pop()
            SSIDList.pop()

        for SSID in SSIDList:
            returnMessage += str(SSID) + str("|")

        #if eraseLast:
        #    returnMessage = returnMessage[:-3]
        #powerNetworkDevices("30")
        f.close()

        print(str(returnMessage.lstrip(' ')))
        sys.stdout.flush()
        #if failedToRun:
        #    cmd = []
        #    cmd.append("ifup")
        #    cmd.append("wlan0")
        #    exitCode, stdout, stderr = shellCommand.execute(cmd)

    else:
        ##############################
        #
        # Start setting the network
        #
        ##############################
        #with open("netting.txt", "w") as myfi:
        #    myfi.writelines(sys.argv[1:])
        #    #myfi.write(wpa_supplicantFileContents + "\n")
        #myfi.close()
        #sys.exit(0)
        scaleSettings = {}
        dbFile = open("/usr/local/hobart/db/settingswizard/scalesettings", "r")
        for line in dbFile:
            line = string.strip(line)
            keyValuePair = line.split('=')
            ####################################################################
            #Server Cert Filename
            #csrFilename=
            #csrKeyFilename=
            if keyValuePair[0] == "serverCertFilename":
                # set customer brightness
                scaleSettings['serverCertFilename'] = str(keyValuePair[1])
            ####################################################################
            #Chain Filename
            elif keyValuePair[0] == "chainFilename":
                #set operator brightness
                scaleSettings['chainFilename'] = str(keyValuePair[1])
            ####################################################################
            #Client Cert Filename
            elif keyValuePair[0] == "clientCertFilename":
                scaleSettings['clientCertFilename'] = str(keyValuePair[1])
            ####################################################################
            elif keyValuePair[0] == "clientKeyFilename":
                scaleSettings['clientKeyFilename'] = str(keyValuePair[1])
            #CSR Filename
            elif keyValuePair[0] == "csrFilename":
                scaleSettings['csrFilename'] = str(keyValuePair[1])
            ####################################################################
            #CSR Key Filename
            elif keyValuePair[0] == "csrKeyFilename":
                scaleSettings['csrKeyFilename'] = str(keyValuePair[1])
            ####################################################################

        dbFile.close()
        connected = False
        weMustPrintNetInfoToQt = False
        weMustPrintNetInfoToDBFile = False
        exitCode = 1
        commandList = []
        networkAdapterName = ""
        encryptedPassphrase = str(options.wpaPassPhrase)

        # need the IP info regardless of wired or wireless

        # below will be blank if DHCP
        ip = str(options.ipAddress)
        subnet = str(options.subnetMask)
        gateway = str(options.gateway)
        dns1 = str(options.primaryServer)
        dns2 = str(options.secondaryServer)
        dns3 = str(options.tertiaryServer)
        EAPUsername = str(options.EAPUsername)
        EAPPassword = str(options.EAPPassword)
        keyManagement = "WPA-PSK"
        encryption = str(options.encryption)
        defaultKeyNum = str(options.defaultKey)
        defaultKey = ""
        isEAPTLS = False

        WEPKey1 = str(options.WEPKey1)
        WEPKey2 = str(options.WEPKey2)
        WEPKey3 = str(options.WEPKey3)
        WEPKey4 = str(options.WEPKey4)

        if defaultKeyNum == "1":
            defaultKey = str(WEPKey1)
        elif defaultKeyNum == "2":
            defaultKey = str(WEPKey2)
        elif defaultKeyNum == "3":
            defaultKey = str(WEPKey3)
        elif defaultKeyNum == "4":
            defaultKey = str(WEPKey4)

        ssid = str(options.ssid)
        wpa_supplicant = ""
        wpa_supplicantFileContents = ""
        eap = ""
        key = ""
        eapMask = ""
        leapMask = ""
        securityMethod = ""
        group = ""
        keyType = "" #WEP WPA LEAP PEAP None

        Security_Open           = "Open"
        Security_Shared_Key     = "Shared Key"
        Security_WPA_Personal   = "WPA-Personal"
        Security_WPA_PEAP       = "WPA-Enterprise (PEAP)"
        Security_WPA_LEAP       = "WPA-Enterprise (LEAP)"
        Security_WPA2_Personal  = "WPA2-Personal"
        Security_WPA2_PEAP      = "WPA2-Enterprise (PEAP)"
        Security_WPA2_LEAP      = "WPA2-Enterprise (LEAP)"
        Security_802X1_PEAP     = "802.1X (PEAP)"
        Security_802X1_LEAP     = "802.1X (LEAP)"

        networkAdapterName = "eth0"

        cmd = []
        cmd.append("/etc/init.d/wpa_supplicant")
        cmd.append("stop")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        cmd = []
        cmd.append("/etc/init.d/wpa_supplicantForSetupUtility")
        cmd.append("stop")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        for i in xrange(0,2):
            cmd = []
            cmd.append("killall")
            cmd.append("wpa_supplicant")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #time.sleep(1)


        ###############################################################
        # Extra wireless info

        if options.isWireless == "1":

            ####################
            # WIRELESS
            ####################

            powerNetworkDevices("00")
            networkAdapterName = "wlan0"
            #commandList.append("killall wpa_supplicant")
            #commandList.append("killall wpa_supplicant")
            # crazy to do it twice but
            # fixes some wpa_supplicant
            # zombie problems.
            cmd = []
            cmd.append("killall")
            cmd.append("dhcpcd")
            #print(str("2"))
            #sys.stdout.flush()
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            #time.sleep(0.5)
            cmd = []
            cmd.append("rm")
            cmd.append("/var/db/dhcpcd-wlan0.lease")
            #print(str("13"))
            #sys.stdout.flush()
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #time.sleep(0.5)
            if str(options.isStatic) != "1":
                print(str("13"))
                sys.stdout.flush()

            #time.sleep(1)

            #time.sleep(1)

            #if str(options.securityMethod) == "OPEN":
            if str(options.securityMethod) == "NA":
                # rm /var/db/dhcp-*
                # killall dhcpcd
                # ifconfig wlan0 down
                # iwconfig wlan0 key XXXXXXXXXXXXX [1]
                # iwconfig wlan0 essid XXXXXX
                # ifoconfig wlan0 up
                # dhcpcd wlan0
                # ping thegateway

                sys.stdout.flush()
                print(str("3"))
                sys.stdout.flush()

                cmd = []
                cmd.append("ifconfig")
                cmd.append(str(networkAdapterName))
                cmd.append("down")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

                #time.sleep(1)

                cmd = []
                cmd.append("iwconfig")
                cmd.append(str(networkAdapterName))
                cmd.append("key")
                cmd.append(defaultKey)
                cmd.append("[" + str(defaultKeyNum) + "]")
                sys.stdout.flush()
                exitCode, stdout, stderr = shellCommand.execute(cmd)

                cmd = []
                cmd.append("iwconfig")
                cmd.append(str(networkAdapterName))
                cmd.append("essid")
                cmd.append(str(ssid))
                print(str("4"))
                sys.stdout.flush()
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                #time.sleep(2)

                cmd = []
                cmd.append("ifconfig")
                cmd.append(str(networkAdapterName))
                cmd.append("up")
                print(str("5"))
                sys.stdout.flush()
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                #time.sleep(8)

            else:
                # Create the WPA SUPPLICANT CONFIGURATION FILE
                sys.stdout.flush()
                print(str("13"))
                sys.stdout.flush()

                if str(options.securityMethod) == "OPEN":
                    securityMethod = "OPEN"
                    keyManagement = "WEP"
                else:
                    keyManagement = "WPA-PSK"
                    pairwise = str(options.encryption)
                    if pairwise == "AES":
                        pairwise = "CCMP"
                    group = pairwise

                    #group = encryption

                    if (len(str(options.wpaPassPhrase))) < 64 and str(options.wpaPassPhrase) != "":
                        #do wpa_passphrase to get format ready

                        cmd = []
                        cmd.append("wpa_passphrase")
                        cmd.append(str(options.ssid))
                        cmd.append(str(options.wpaPassPhrase))
                        print("12")
                        sys.stdout.flush()
                        exitCode, stdout, stderr = shellCommand.execute(cmd)
                        #time.sleep(1)
                        encryptedPassphrase = string.strip(stdout)
                        #print stdout
                        #print stderr
                        #if exitCode != 0:
                        #    sys.exit(exitCode)

                        encryptedPassphrase = encryptedPassphrase.replace("#psk=","#PassPhrase")

                        start = encryptedPassphrase.find("psk=")
                        if start != -1:
                            start += 4

                            encryptedPassphrase = encryptedPassphrase[start:]
                            ep = string.split(encryptedPassphrase,"\n")

                            encryptedPassphrase = ep[0]


                # build the wpa string
                writeWPA_SUPPLICANTFile = False;

                channelText = ""
                if len(options.channel) > 0 and options.channel != "auto" and options.channel != None:
                    channelText = "   freq_list=" + options.channel + "\n"

                if str(options.securityMethod) == "OPEN" and str(options.encryption) == "WEP":
                    writeWPA_SUPPLICANTFile = True
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=NONE\n   group=WEP40 WEP104\n   auth_alg=OPEN\n"
                    wpa_supplicantFileContents += "   wep_tx_keyidx=" + str(int(options.defaultKey) -1) + "\n"
                    if len(str(options.WEPKey1)) > 4:
                        wpa_supplicantFileContents += "   wep_key0="
                        if len(str(options.WEPKey1)) == 5 or len(str(options.WEPKey1)) == 13:
                            wpa_supplicantFileContents += (str(options.WEPKey1)).encode("hex") + "\n"
                        elif len(str(options.WEPKey1)) == 10 or len(str(options.WEPKey1)) == 26:
                            wpa_supplicantFileContents += str(options.WEPKey1) + "\n"
                    if len(str(options.WEPKey2)) > 4:
                        wpa_supplicantFileContents += "   wep_key1="
                        if len(str(options.WEPKey2)) == 5 or len(str(options.WEPKey2)) == 13:
                            wpa_supplicantFileContents += (str(options.WEPKey2)).encode("hex") + "\n"
                        elif len(str(options.WEPKey2)) == 10 or len(str(options.WEPKey2)) == 26:
                            wpa_supplicantFileContents += str(options.WEPKey2) + "\n"
                    if len(str(options.WEPKey3)) > 4:
                        wpa_supplicantFileContents += "   wep_key2="
                        if len(str(options.WEPKey3)) == 5 or len(str(options.WEPKey3)) == 13:
                            wpa_supplicantFileContents += (str(options.WEPKey3)).encode("hex") + "\n"
                        elif len(str(options.WEPKey3)) == 10 or len(str(options.WEPKey3)) == 26:
                            wpa_supplicantFileContents += str(options.WEPKey3) + "\n"
                    if len(str(options.WEPKey4)) > 4:
                        wpa_supplicantFileContents += "   wep_key3="
                        if len(str(options.WEPKey4)) == 5 or len(str(options.WEPKey4)) == 13:
                            wpa_supplicantFileContents += (str(options.WEPKey4)).encode("hex") + "\n"
                        elif len(str(options.WEPKey4)) == 10 or len(str(options.WEPKey4)) == 26:
                            wpa_supplicantFileContents += str(options.WEPKey4) + "\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                elif str(options.securityMethod) == "OPEN" and str(options.encryption) == "None":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=NONE\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA-Personal" and str(options.encryption) == "AES":
                    #wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=WPA\n   key_mgmt=WPA-PSK\n   pairwise=" + str(pairwise) + "\n   psk=" + str(encryptedPassphrase) + "\n   group=" + str(pairwise) + "\n}"
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=WPA\n   key_mgmt=WPA-PSK\n   pairwise=CCMP\n   group=CCMP TKIP\n  psk=" + str(encryptedPassphrase) + "\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents +=  "\n   }"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA-Personal" and str(options.encryption) == "TKIP":
                    #wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=WPA\n   key_mgmt=WPA-PSK\n   pairwise=" + str(pairwise) + "\n   psk=" + str(encryptedPassphrase) + "\n   group=" + str(pairwise) + "\n}"
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=WPA\n   key_mgmt=WPA-PSK\n   pairwise=TKIP\n   group=TKIP\npsk=" + str(encryptedPassphrase) + "\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "WPA2-Personal" and str(options.encryption) == "AES":
                    #wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN\n   key_mgmt=WPA-PSK\n   pairwise=" + str(pairwise) + "\n   psk=" + str(encryptedPassphrase) + "\n   group=" + str(pairwise) + "\n}"
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-PSK\n   pairwise=CCMP\n   group=CCMP TKIP\n   psk=" + str(encryptedPassphrase) + "\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "WPA2-Personal" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-PSK\n   pairwise=TKIP\n   group=TKIP\n   psk=" + str(encryptedPassphrase) + "\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    ############################
                elif str(options.securityMethod) == "WPA EAP-TLS" and str(options.encryption) == "TKIP":
                    isEAPTLS = True
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-EAP\n   pairwise=CCMP TKIP\n   group=CCMP TKIP\n   eap=TLS\n"
                    wpa_supplicantFileContents += "   identity=\"" + getCertIdentity("/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".pem") + "\"\n"
                    wpa_supplicantFileContents += "   ca_cert=\"/usr/local/hobart/db/settingswizard/" + options.serverCertFilename + "\"\n"
                    wpa_supplicantFileContents += "   client_cert=\"/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".pem\"\n"
                    wpa_supplicantFileContents += "   private_key=\"/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".key\"\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "   priority=1\n}\n"
                    writeWPA_SUPPLICANTFile = True
                    #with open("netting.txt", "a") as myfi:
                    #    myfi.write("wpa eap-tls tkip\n")
                    #    myfi.write(wpa_supplicantFileContents + "\n")
                    #myfi.close()
                elif str(options.securityMethod) == "WPA EAP-TLS" and str(options.encryption) == "AES":
                    isEAPTLS = True
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-EAP\n   pairwise=CCMP TKIP\n   group=CCMP TKIP\n   eap=TLS\n"
                    wpa_supplicantFileContents += "   identity=\"" + getCertIdentity("/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".pem") + "\"\n"
                    wpa_supplicantFileContents += "   ca_cert=\"/usr/local/hobart/db/settingswizard/" + options.serverCertFilename + "\"\n"
                    wpa_supplicantFileContents += "   client_cert=\"/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".pem\"\n"
                    wpa_supplicantFileContents += "   private_key=\"/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".key\"\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "   priority=1\n}\n"
                    writeWPA_SUPPLICANTFile = True
                    #with open("netting.txt", "a") as myfi:
                    #    myfi.write("wpa eap-tls aes\n")
                    #    myfi.write(wpa_supplicantFileContents + "\n")
                    #myfi.close()
                elif str(options.securityMethod) == "WPA2 EAP-TLS" and str(options.encryption) == "AES":
                    isEAPTLS = True
                    #print ("made it")
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=WPA-EAP\n   pairwise=CCMP TKIP\n   group=CCMP TKIP\n   eap=TLS\n"
                    wpa_supplicantFileContents += "   identity=\"" + getCertIdentity("/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".pem") + "\"\n"
                    wpa_supplicantFileContents += "   ca_cert=\"/usr/local/hobart/db/settingswizard/" + options.serverCertFilename + "\"\n"
                    wpa_supplicantFileContents += "   client_cert=\"/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".pem\"\n"
                    wpa_supplicantFileContents += "   private_key=\"/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".key\"\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "   priority=1\n}\n"
                    writeWPA_SUPPLICANTFile = True
                    #with open("netting.txt", "a") as myfi:
                    #    myfi.write("wpa2 eap-tls aes\n")
                    #    myfi.write(wpa_supplicantFileContents + "\n")
                    #myfi.close()
                elif str(options.securityMethod) == "WPA2 EAP-TLS" and str(options.encryption) == "TKIP":
                    isEAPTLS = True
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-EAP\n   pairwise=CCMP TKIP\n   group=CCMP TKIP\n   eap=TLS\n"
                    wpa_supplicantFileContents += "   identity=\"" + getCertIdentity("/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".pem") + "\"\n"
                    wpa_supplicantFileContents += "   ca_cert=\"/usr/local/hobart/db/settingswizard/" + options.serverCertFilename + "\"\n"
                    wpa_supplicantFileContents += "   client_cert=\"/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".pem\"\n"
                    wpa_supplicantFileContents += "   private_key=\"/usr/local/hobart/db/settingswizard/" + options.clientCertFilename + ".key\"\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "   priority=1\n}\n"

                    writeWPA_SUPPLICANTFile = True
                    #with open("netting.txt", "a") as myfi:
                    #    myfi.write("wpa2 eap-tls tkip\n")
                    #    myfi.write(wpa_supplicantFileContents + "\n")
                    #myfi.close()

                elif str(options.securityMethod) == "802.1X (PEAP)" and str(options.encryption) == "WEP":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=IEEE8021X\n   eap=PEAP\n   phase2=\"auth=MSCHAPV2\"\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "802.1X (LEAP)" and str(options.encryption) == "WEP":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=IEEE8021X\n   eap=LEAP\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "WPA-Enterprise (PEAP)" and str(options.encryption) == "AES":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=WPA\n   key_mgmt=WPA-EAP\n   pairwise=CCMP\n   group=CCMP TKIP\n   eap=TTLS PEAP TLS\n   phase2=\"auth=MSCHAPV2\"\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n   pairwise=CCMP\n   group=CCMP TKIP\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "WPA-Enterprise (PEAP)" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=WPA\n   key_mgmt=WPA-EAP\n   pairwise=TKIP\n   group=TKIP\n   eap=TTLS PEAP TLS\n   phase2=\"auth=MSCHAPV2\"\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n   pairwise=TKIP\n   group=TKIP\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "WPA-Enterprise (LEAP)" and str(options.encryption) == "AES":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=WPA\n   key_mgmt=WPA-EAP\n   pairwise=CCMP\n   group=CCMP TKIP\n   eap=LEAP\n   auth_alg=LEAP\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n   pairwise=CCMP\n   group=CCMP TKIP\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "WPA-Enterprise (LEAP)" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=WPA\n   key_mgmt=WPA-EAP\n   pairwise=TKIP\n   group=TKIP\n   eap=LEAP\n   auth_alg=LEAP\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n   pairwise=TKIP\n   group=TKIP\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "WPA2-Enterprise (PEAP)" and str(options.encryption) == "AES":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-EAP\n   pairwise=CCMP\n   group=CCMP TKIP\n   eap=TTLS PEAP TLS\n   phase2=\"auth=MSCHAPV2\"\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n   pairwise=CCMP\n   group=CCMP TKIP\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "WPA2-Enterprise (PEAP)" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-EAP\n   pairwise=TKIP\n   group=TKIP\n   eap=TTLS PEAP TLS\n   phase2=\"auth=MSCHAPV2\"\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n   pairwise=TKIP\n   group=TKIP\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "WPA2-Enterprise (LEAP)" and str(options.encryption) == "AES":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-EAP\n   pairwise=CCMP\n   group=CCMP TKIP\n   eap=LEAP\n   auth_alg=LEAP\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n   pairwise=CCMP\n   group=CCMP TKIP\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                    #
                elif str(options.securityMethod) == "WPA2-Enterprise (LEAP)" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ctrl_interface=/var/run/wpa_supplicant\nctrl_interface_group=0\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-EAP\n   pairwise=TKIP\n   group=TKIP\n   eap=LEAP\n   auth_alg=LEAP\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n   pairwise=TKIP\n   group=TKIP\n"
                    wpa_supplicantFileContents += channelText
                    wpa_supplicantFileContents += "\n}"
                    writeWPA_SUPPLICANTFile = True
                else:
                    # It is an non standard
                    # Do not write wpa_supplicant.conf file
                    # Just do the ifconfig iwconfig calls

                    writeWPA_SUPPLICANTFile = False

                    # Do the WPA
                # 2 lines below get path of this qmSettingsNetwork.pyc file
                # __dir__ = os.path.dirname(os.path.abspath(__file__))
                # filepath = os.path.join(__dir__, 'apList.txt')

                # Must be root to clobber file: /etc/wpa_supplicant.conf
                if writeWPA_SUPPLICANTFile:
                    wpaSupplicantFileHandle = open("/etc/wpa_supplicant.conf", 'w')
                    wpaSupplicantFileHandle.write(wpa_supplicantFileContents)
                    wpaSupplicantFileHandle.close()


                #cmd = []
                #cmd.append("wpa_supplicant")
                #cmd.append("-Dwext")
                #cmd.append("-i")
                #cmd.append(str(networkAdapterName))
                #cmd.append("-c/etc/wpa_supplicant.conf")
                #cmd.append("-B")

                #print("13")
                #sys.stdout.flush()
                #exitCode, stdout, stderr = shellCommand.execute(cmd)
                #time.sleep(30)
                #if exitCode != 0:
                #    sys.exit(exitCode)

                # do dhclient and ip route for Static IP or non-autoDNS

        else:
            # WIRED
            #powerNetworkDevices("10")
            cmd = []
            cmd.append("ifconfig")
            cmd.append(str(networkAdapterName))
            cmd.append("down")
            sys.stdout.flush()
            print(str("3"))
            sys.stdout.flush()
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #time.sleep(1)

            cmd = []
            cmd.append("rm")
            cmd.append("/var/db/dhcpcd-eth0.lease")
            print(str("1"))
            sys.stdout.flush()
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #time.sleep(1)

            try:
                cmd = []
                cmd.append("killall")
                cmd.append("dhcpcd")
                sys.stdout.flush()
                print(str("2"))
                sys.stdout.flush()
                sys.stderr.flush()
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                #time.sleep(1)
            except:
                pass

            cmd = []
            cmd.append("ifconfig")
            cmd.append(str(networkAdapterName))
            cmd.append("up")
            #sys.stdout.flush()
            print(str("5"))
            sys.stdout.flush()
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #time.sleep(3)

            cmd = []
            cmd.append("ethtool")
            cmd.append("-s")
            cmd.append(str(networkAdapterName))
            cmd.append("autoneg")
            cmd.append("on")
            cmd.append("advertise")
            cmd.append("0x0F")
            sys.stdout.flush()
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #time.sleep(3)



        cmd = []
        cmd.append("ip")
        cmd.append("addr")
        cmd.append("flush")
        cmd.append("dev")
        cmd.append(str(networkAdapterName))
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        if str(options.manufacturingTest) != "64641964":
            if str(options.isStatic) != "1":
                setEtcNetworkInterfacesFileDHCP(str(networkAdapterName))
            else:
                setEtcNetworkInterfacesFileSTATIC(str(networkAdapterName), str(options.ipAddress), str(options.subnetMask), str(options.gateway))


        if options.isWireless == "1":
            sys.stdout.flush()
            print(str("13"))
            sys.stdout.flush()
            if isEAPTLS:
                print(str("50"))
                sys.stdout.flush()

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
            sys.stdout.flush()

        if options.isWireless == "1":
            print(str("13"))
            sys.stdout.flush()
            if isEAPTLS:
                print(str("50"))
                sys.stdout.flush()

            cmd = []
            cmd.append("iwconfig")
            cmd.append("wlan0")
            cmd.append("power")
            cmd.append("off")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            cmd = []
            cmd.append("/etc/init.d/wpa_supplicantForSetupUtility")
            cmd.append("start")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            #print("after13")
            #sys.stdout.flush()
            #print("stdout is: " + str(stdout))
            #sys.stdout.flush()
            #print("exCode is: " + str(exitCode))
            #sys.stdout.flush()

            #print(str("init.d wpa_supplicant starting"))
            if exitCode == 0:
                #print(str("wpa_cli starting"))
                #sys.stdout.flush()
                #sys.stdout.flush()
                cmd = []
                cmd.append("/usr/sbin/wpa_cli")
                cmd.append("-a")
                cmd.append("/usr/bin/wpa_actionForSetupUtility")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                #print("ZZstdOut is: " + str(stdout))
                #sys.stdout.flush()
                #print("ZZexCode is: " + str(exitCode))
                #sys.stdout.flush()
                if str(options.isStatic) != "1":
                    # COMMENT#ED THIS TODO EAP-TLS FAILURE
                    #if exitCode == 0:
                    if True:
                        if str(options.securityMethod) == "WPA EAP-TLS" or str(options.securityMethod) == "WPA2 EAP-TLS":
                            time.sleep(15)
                        cmd = []
                        cmd.append("dhcpcd")
                        cmd.append(str(networkAdapterName))
                        #sys.stdout.flush()
                        print(str("6"))
                        sys.stdout.flush()
                        exitCode, stdout, stderr = shellCommand.execute(cmd)
                    else:
                        print(str("NO"))
                        sys.stdout.flush()

            else:
                print(str("wpa failure"))
                sys.stdout.flush()

        elif str(options.isStatic) != "1" and options.isWireless == "0":
            # DHCP wired
            cmd = []
            cmd.append("killall")
            cmd.append("dhcpcd")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            sys.stdout.flush()
            print(str("6"))
            sys.stdout.flush()
            cmd = []
            cmd.append("dhcpcd")
            cmd.append(str(networkAdapterName))
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            time.sleep(5)

            #if exitCode == 0:
            # OLD redundant below
            if exitCode == 0:
                nothing = True
                #setEtcNetworkInterfacesFileDHCP(str(networkAdapterName))
            else:
                #Try it again
                cmd = []
                cmd.append("killall")
                cmd.append("dhcpcd")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                sys.stdout.flush()
                cmd = []
                cmd.append("dhcpcd")
                cmd.append(str(networkAdapterName))
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                #print str(exitCode)
                time.sleep(5)
            weMustPrintNetInfoToQt = True
            weMustPrintNetInfoToDBFile = True

        if str(options.isStatic) == "1":

            #STATIC WIRED and wireless
            cmd = []
            cmd.append("ifconfig")
            cmd.append(str(networkAdapterName))
            cmd.append(str(options.ipAddress))
            cmd.append("netmask")
            cmd.append(str(options.subnetMask))
            print(str("8"))
            sys.stdout.flush()
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #time.sleep(3)

            cmd = []
            cmd.append("ip")
            cmd.append("route")
            cmd.append("add")
            cmd.append("default")
            cmd.append("via")
            cmd.append(str(options.gateway))
            print(str("9"))
            sys.stdout.flush()
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #time.sleep(1)
            #setEtcNetworkInterfacesFileSTATIC(str(networkAdapterName), str(options.ipAddress), str(options.subnetMask), str(options.gateway))
            weMustPrintNetInfoToDBFile = True

        if str(options.isAutoDNS) == "0":
            # set the dns names
            print("10")
            sys.stdout.flush()
            suffix1 = ""
            suffix2 = ""
            suffix3 = ""

            if len(options.dnsSuffix1) > 0:
                suffix1 = options.dnsSuffix1
            if len(options.dnsSuffix2) > 0:
                suffix2 = options.dnsSuffix2
            if len(options.dnsSuffix3) > 0:
                suffix3 = options.dnsSuffix3
            setNameServers(options.primaryServer, options.secondaryServer, options.tertiaryServer, suffix1, suffix2, suffix3)


            #if len(suffix1) > 0 or len(suffix2) > 0 or len(suffix3) > 0:
            #    setDNSSuffixes(suffix1, suffix2, suffix3)

            #time.sleep(1)
        else:
            weMustPrintNetInfoToQt = True
            weMustPrintNetInfoToDBFile = True

        didWeConnect = False
        print("11")
        sys.stdout.flush()
        #time.sleep(1)
        cmd = []
        cmd.append("sync")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        if testConnection(str(networkAdapterName)):
            print("YES")
            didWeConnect = True
        else:
            print("NO")
            didWeConnect = False
        sys.stdout.flush()

        if options.isWireless == "1":
            # do the linkrate settings here
            if options.linkRate == "auto" or options.linkRate == None:
                #erase previous settings
                filler = 1
            elif options.linkRate != None and options.linkRate != "":
                cmd = []
                cmd.append("iwconfig")
                cmd.append("wlan0")
                cmd.append("rate")
                cmd.append(options.linkRate)
                if options.automaticLinkRate == "1" or options.automaticLinkRate == None:
                    cmd.append("auto")
                elif options.automaticLinkRate == "0":
                    cmd.append("fixed")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

        infoToPrint = 'starter'
        if didWeConnect:
            try:
                #time.sleep(2)
                infoToPrint = str(printNetworkInfo(str(networkAdapterName)))
            except:
                infoToPrint = "NONE"

            print('OUTPUT|' + str(infoToPrint))
            sys.stdout.flush()
            time.sleep(1)

    sys.exit(0)
    #################### SAVING FILE NOW  YES or NO

else:

    ##########################################################################
    # WINDOWS
    import ntNetworkUtils

    if options.isWireless == "2":
        # It is an unconfigured scale so turn off wired and wireless

        pass
    else:

        if options.scan != "1":
            print("3")
            sys.stdout.flush()

        #if options.isStatic == "0":
            #start the dhcp service
        #    ntNetworkUtils.enableDHCPClientService(True)
        #else:
        #    ntNetworkUtils.enableDHCPClientService(False)

        wirelessProfileName = 'WirelessProfile'
        wirelessProfileXMLFile = configInfo.getRootFolder() + '\\db\\settingswizard\\' + wirelessProfileName + '.xml'
        wirelessEAPUserDataXMLFile = configInfo.getRootFolder() + '\\db\\settingswizard\\EAPUserData.xml'
        wirelessAdapterType = 'wlan'
        wiredAdapterType = 'lan'
        wirelessAdapterName = 'Wireless Network Connection'
        wiredAdapterName = 'Local Area Connection'

        if len(str(options.connectDevices)) == 2:

            states = options.connectDevices

            if states[1:2] == "1":
                #turn on wifi
                ntNetworkUtils.setAdapterEnabled(wirelessAdapterName,  True)
            elif states[1:2] == "0":
                #turn off wifi
                ntNetworkUtils.setAdapterEnabled(wirelessAdapterName,  False)

            if states[0:1] == "1":
                #turn on wire
                ntNetworkUtils.setAdapterEnabled(wiredAdapterName,  True)
            elif states[0:1] == "0":
                #turn off wire
                ntNetworkUtils.setAdapterEnabled(wiredAdapterName,  False)

        elif options.scan == "1":

            # make sure wireless adapter is enabled
            state = ntNetworkUtils.getAdapterState(wirelessAdapterType,  wirelessAdapterName)
            if not state:
                ntNetworkUtils.startNetworkService(wirelessAdapterName)
                ntNetworkUtils.setAdapterEnabled(wirelessAdapterName,  True)
                i = 0
                while not state and i < 200:
                    state = ntNetworkUtils.getAdapterState(wirelessAdapterType,  wirelessAdapterName)
                    i += 1

            # get current ssid if valid
            currentSSID = ntNetworkUtils.getWlanAdapterSsid(wirelessAdapterName)

            # remove filter making ssid's invisible
            ntNetworkUtils.setWlanDenyallFilter(False)

            # gather signal strengths and send appropriate output to stdout
            ssidStrengthsBySsid = ntNetworkUtils.getWlanSsidStrengths()
            for ssid, strength in ssidStrengthsBySsid.iteritems():
                if strength > 0:
                    if strength >99:
                        sys.stdout.write("5|" + str(strength) + "|  |" + ssid + "|")
                    else:
                        sys.stdout.write(str(int(strength / 20) + 1) + "|" + str(strength) + "|  |" + ssid + "|")
                else:
                     sys.stdout.write("0|0| |" + ssid + "|")

        else:

            # config parser to store settings if successful
            import ConfigParser
            settingsParser = ConfigParser.SafeConfigParser()

            enabledAdapterType = None
            enabledAdapterName = None

            #configSettings = bootSettings.getBootSettings()
            #connectionType = int(configSettings['connectionType'])
            connectionType = int(options.isWireless)
            if connectionType == 1:
                cmd = []
                cmd.append('sc')
                cmd.append('stop')
                cmd.append('dot3Svc')
                osUtils.winRunElevated(cmd)
                cmd = []
                cmd.append('sc')
                cmd.append('start')
                cmd.append('WlanSvc')
                osUtils.winRunElevated(cmd)
            elif connectionType == 0:
                cmd = []
                cmd.append('sc')
                cmd.append('stop')
                cmd.append('WlanSvc')
                osUtils.winRunElevated(cmd)
                cmd = []
                cmd.append('sc')
                cmd.append('start')
                cmd.append('dot3Svc')
                osUtils.winRunElevated(cmd)


            # disabling wired device
            #time.sleep(2)
            print("3")
            sys.stdout.flush()

            if options.isWireless == "1":
                # WIRELESS

                #ntNetworkUtils.setAdapterEnabled(wiredAdapterName,  False)

                # make sure wireless adapter is enabled
                state = ntNetworkUtils.getAdapterState(wirelessAdapterType,  wirelessAdapterName)
                if not state:
                    exitCode, stdout, stderr = ntNetworkUtils.setAdapterEnabled(wirelessAdapterName,  True)
                    i = 0
                    while not state and i < 200:
                        state = ntNetworkUtils.getAdapterState(wirelessAdapterType,  wirelessAdapterName)
                        i += 1

                ntNetworkUtils.disconnectWlan()

                # setup filters for only the specified SSID
                ntNetworkUtils.setWlanDenyallFilter(False)

                # WRITE THE XML FILE TO IMPORT
                print("4")
                sys.stdout.flush()

                ssidText = options.ssid
                encryptionText = options.encryption
                securityMethodText = ''
                keyTypeText = ''
                passphraseText = options.wpaPassPhrase
                defaultKeyText = options.defaultKey
                key1Text = options.WEPKey1
                key2Text = options.WEPKey2
                key3Text = options.WEPKey3
                key4Text = options.WEPKey4
                usernameText = options.EAPUsername
                passwordText = options.EAPPassword

                # ADD the SECURITY TYPE
                if options.securityMethod == "OPEN":
                    securityMethodText = 'open'
                    keyTypeText = 'networkKey'
                elif options.securityMethod == "WPA-Personal":
                    securityMethodText = 'WPAPSK'
                    if len(passphraseText) == 64:
                        keyTypeText = 'networkKey'
                    else:
                        keyTypeText = 'passPhrase'
                elif options.securityMethod == "WPA2-Personal":
                    securityMethodText = 'WPA2PSK'
                    if len(passphraseText) == 64:
                        keyTypeText = 'networkKey'
                    else:
                        keyTypeText = 'passPhrase'
                elif options.securityMethod == "802.1X (PEAP)":
                    securityMethodText = options.securityMethod
                    keyTypeText = '802PEAP'
                elif options.securityMethod == "802.1X (LEAP)":
                    securityMethodText = options.securityMethod
                    keyTypeText = '802LEAP'
                elif options.securityMethod == "WPA-Enterprise (PEAP)":
                    securityMethodText = 'WPA-Enterprise (PEAP)'
                    keyTypeText = 'PEAP'
                elif options.securityMethod == "WPA-Enterprise (LEAP)":
                    securityMethodText = 'WPA-Enterprise (LEAP)'
                    keyTypeText = 'LEAP'
                elif options.securityMethod == "WPA2-Enterprise (PEAP)":
                    securityMethodText = 'WPA2-Enterprise (PEAP)'
                    keyTypeText = 'PEAP'
                elif options.securityMethod == "WPA2-Enterprise (LEAP)":
                    securityMethodText = 'WPA2-Enterprise (LEAP)'
                    keyTypeText = 'LEAP'
                elif options.securityMethod == "WPA2 EAP-TLS" or options.securityMethod == "WPA EAP-TLS":

                    #remove current certs and store the new certs

                    import ntCertificateUtils
                    clientCertsInfo = ntCertificateUtils.getClientCertsInfo()
                    for aCert in clientCertsInfo:
                        #print aCert
                        line = aCert.rstrip()
                        keyValuePair = line.split('=')
                        if keyValuePair[0] == "client":
                            aCertFilePath = keyValuePair[1]
                            if len(keyValuePair) == 4:
                                #print "doing root"
                                if keyValuePair[3] == "NONE":
                                    keyValuePair[3] = ""
                                try:
                                    ntCertificateUtils.deleteClientCertificate(keyValuePair[2],keyValuePair[3])
                                except:
                                    pass

                    ntCertificateUtils.emptyClientCertificates()
                    #print currentCertsInfo

                    currentCertsInfo = ntCertificateUtils.getCurrentCertsInfo()
                    for aCert in currentCertsInfo:
                        #print aCert
                        line = aCert.rstrip()
                        keyValuePair = line.split('=')
                        if keyValuePair[0] == "root":
                            aCertFilePath = keyValuePair[1]
                            if len(keyValuePair) == 4:
                                #print "doing root"
                                if keyValuePair[3] == "NONE":
                                    keyValuePair[3] = ""
                                try:
                                    valid, answer = ntCertificateUtils.storeTrustedRootCertificate(aCertFilePath, keyValuePair[3])
                                    ntCertificateUtils.addCAtoNTAuth(aCertFilePath)

                                except:
                                    pass
                        elif keyValuePair[0] == "client":
                            print "working"
                            aCertFilePath = keyValuePair[1]
                            if len(keyValuePair) == 4:
                                #print "doing client"
                                if keyValuePair[3] == "NONE":
                                    keyValuePair[3] = ""
                                try:
                                    #time.sleep()
                                    valid, answer = ntCertificateUtils.storeClientCertificate(aCertFilePath, keyValuePair[3] ,True)
                                except:
                                    print "failed"
                                    pass

                    securityMethodText = options.securityMethod
                    keyTypeText = "TLS"
                    encryptionText = options.encryption

                #os.system("C:\Program Files\System32\certutil.exe",
                #print('ssid: ' + ssidText)
                #print('enc: ' + encryption)
                #print('encryption: ' + encryption)
                #print('securityMethod: ' + securityMethodText)
                #print('keyType: ' + keyTypeText)
                #print('passphrase: ' + passphraseText)
                #print('defaultKey: ' + defaultKeyText)
                #print('key1: ' +key1Text)
                #print('key2: ' +key2Text)
                #print('key3: ' +key3Text)
                #print('key4: ' +key4Text)
                #print('username: ' + usernameText)
                #print('password: ' + passwordText)
                #print "BEGIN"
                #print wirelessProfileXMLFile
                answer = createWirelessProfile.writeXML(wirelessProfileXMLFile, wirelessEAPUserDataXMLFile, ssidText, encryptionText, securityMethodText, keyTypeText, passphraseText, defaultKeyText, key1Text, key2Text, key3Text, key4Text, usernameText, passwordText)
                time.sleep(3)
                #print "END"
                enabledAdapterType = wirelessAdapterType
                enabledAdapterName = wirelessAdapterName

                wirelessSection = 'wireless'
                settingsParser.add_section(wirelessSection)
                settingsParser.set(wirelessSection,  'ssid',  options.ssid)
                settingsParser.set(wirelessSection,  'securityMethod',  options.securityMethod)
                settingsParser.set(wirelessSection,  'profileName',  wirelessProfileName)
                settingsParser.set(wirelessSection,  'profileXMLFile',  wirelessProfileXMLFile)
                settingsParser.set(wirelessSection,  'eapUserDataXMLFile',  wirelessEAPUserDataXMLFile)

            else:

                # WIRED

                ntNetworkUtils.disconnectWlan()

                # setup filters to deny all
                ntNetworkUtils.setWlanDenyallFilter(False)

                # disable wireless adapter
                #ntNetworkUtils.setAdapterEnabled(wirelessAdapterName,  False)

                # enabled wired adapter
                ntNetworkUtils.setAdapterEnabled(wiredAdapterName,  True)

                enabledAdapterType = wiredAdapterType
                enabledAdapterName = wiredAdapterName


            if options.isStatic == "1":
                #ntNetworkUtils.enableDHCPClientService(False)
                ntNetworkUtils.setAddress(enabledAdapterName,  options.ipAddress,  options.subnetMask,  options.gateway)

                addressSection = 'address'
                #settingsParser.add_section(addressSection)
                #settingsParser.set(addressSection,  'ipAddress',  options.ipAddress)
                #settingsParser.set(addressSection,  'subnetMask',  options.subnetMask)
                #settingsParser.set(addressSection,  'gateway',  options.gateway)
            else:
                print("6")
                sys.stdout.flush()
                #ntNetworkUtils.enableDHCPClientService(True)
                ntNetworkUtils.setDhcp(enabledAdapterName)

            if options.isAutoDNS == "1":
                print("10")
                sys.stdout.flush()
                ntNetworkUtils.setAutoDns(enabledAdapterName)
            else:
                print("10")
                sys.stdout.flush()
                ntNetworkUtils.deleteAllDnsServers(enabledAdapterName)

                #dnsServersSection = 'dnsservers'
                #settingsParser.add_section(dnsServersSection)

                '''if not options.primaryServer:
                    ntNetworkUtils.addDnsServer(enabledAdapterName,  options.primaryServer)
                    settingsParser.set(dnsServersSection,  'primary',  options.primaryServer)

                if not options.secondaryServer == "0":
                    ntNetworkUtils.addDnsServer(enabledAdapterName,  options.secondaryServer)
                    settingsParser.set(dnsServersSection,  'secondary',  options.secondaryServer)

                if not options.tertiaryServer == "0":
                    ntNetworkUtils.addDnsServer(enabledAdapterName,  options.tertiaryServer)
                    settingsParser.set(dnsServersSection,  'tertiary',  options.tertiaryServer)


                #if the dns servers are blank then remove them after the connection is made
                if options.primaryServer == "0" and options.secondaryServer == "0" and options.tertiaryServer == "0":
                    #clear out the dbns servers
                    ntNetworkUtils.deleteAllDnsServers(enabledAdapterName)

                '''

                '''
                #remove just the first dns
                else if options.primaryServer == "0":
                    try:
                        ntNetworkUtils.deletePrimaryDNS(enabledAdapterName)
                    except:
                        pass
                #remove just the second dns
                else if options.secondaryServer == "0":
                    try:
                        ntNetworkUtils.deleteSecondaryDNS(enabledAdapterName)
                    except:
                        pass
                #remove just the third dns
                else if options.tertiaryServer == "0":
                    try:
                        ntNetworkUtils.deleteTertiaryDNS(enabledAdapterName)
                    except:
                        pass
                '''

            if options.isWireless == "1":
                exitCode, stdout, stderr = ntNetworkUtils.disconnectWlan()

                # setup profile
                #print("55")
                #sys.stdout.flush()
                exitCode, stdout, stderr = ntNetworkUtils.deleteAllProfilesForAdapter(wirelessAdapterType,  wirelessAdapterName)

                #print "exitCode is: " + str(exitCode)
                #print "addProfileForAdapter stdout is: " + stdout
                #print "addProfileForAdapter stderr is: " + stderr

                # If PEAP, also need to import EAPUserProfile.xml
                if keyTypeText == "PEAP" or keyTypeText == "LEAP" or keyTypeText == "802PEAP" or keyTypeText == "802LEAP":

                    #print "going into EAP"
                    # call wl
                    #ntNetworkUtils.importEapProfile(wirelessEAPUserDataXMLFile,  wirelessProfileName)

                    time.sleep(1)
                    cmd = []
                    cmd.append(os.path.join('C:/','Program Files','Hobart','bin','Hobart-EAPUtility.exe'))
                    cmd.append('sp')
                    wirelessProfileXMLFile = configInfo.getRootFolder() + '\\db\\settingswizard\\' + wirelessProfileName + '.xml'
                    wirelessEAPUserDataXMLFile
                    cmd.append(wirelessProfileXMLFile)
                    cmd.append(wirelessEAPUserDataXMLFile)
                    cmd.append(options.ssid)
                    cmd.append('WirelessProfile')
                    cmd.append(options.EAPUsername)
                    cmd.append(options.EAPPassword)
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    #print "XXXXXXXXXX"
                    #print cmd
                    #print "XXXXXXXXXX"
                    print stdout
                    #print "XXXXXXXXXX"
                    time.sleep(3)
                else :
                    exitCode, stdout, stderr = ntNetworkUtils.addProfileForAdapter(wirelessAdapterType,  wirelessAdapterName,  wirelessProfileXMLFile)

                    # connect
                    exitCode, stdout, stderr = ntNetworkUtils.connectWlan(wirelessAdapterName,  wirelessProfileName,  options.ssid)
                #print "exitCode is: " + str(exitCode)
                #print "connectWlan stdout is: " + stdout
                #print "connectWlan stderr is: " + stderr

            # wait for connection
            #print enabledAdapterType
            #print enabledAdapterName
            ntNetworkUtils.waitForConnection(enabledAdapterType, enabledAdapterName, 10)

            if options.isStatic == "1":
                #ntNetworkUtils.enableDHCPClientService(False)
                ntNetworkUtils.setAddress(enabledAdapterName,  options.ipAddress,  options.subnetMask,  options.gateway)
            elif options.isStatic == "0":
                ntNetworkUtils.setDhcp(enabledAdapterName)
            time.sleep(1)
            detectedIpAddress, detectedSubnetMask, detectedGateway = ntNetworkUtils.getAddressValues(enabledAdapterName)

            # set channels and linkrate
            # Channel
            if options.isAutoDNS == "1":
                print("10")
                sys.stdout.flush()
                ntNetworkUtils.setAutoDns(enabledAdapterName)
            else:
                print("10")
                sys.stdout.flush()
                ntNetworkUtils.deleteAllDnsServers(enabledAdapterName)

                dnsServersSection = 'dnsservers'
                #settingsParser.add_section(dnsServersSection)
                primaryIsSet = False
                secondaryIsSet = False
                tertiaryIsSet = False
                DNSSoFar = 0
                #print "before evaluation"
                if not options.primaryServer == "0":
                    #print "we have a Primary"
                    primaryIsSet = True
                    DNSSoFar = 1
                    ntNetworkUtils.addDnsServer(enabledAdapterName,  options.primaryServer)
                    #settingsParser.set(dnsServersSection,  'primary',  options.primaryServer)
                else:
                    #print "no primary"

                    primaryIsSet = False
                    DNSSoFar = 0

                if not options.secondaryServer == "0":
                    #print "we have a Secondary"
                    secondaryIsSet = True
                    if DNSSoFar == 1:
                        #print "adding secondary"
                        sys.stdout.flush()
                        ntNetworkUtils.addSecondaryDNSServer(enabledAdapterName,  options.secondaryServer)
                    elif DNSSoFar == 0:
                        # Make it the primary DNS
                        ntNetworkUtils.addPrimaryDNSServer(enabledAdapterName, options.secondaryServer)
                        DNSSoFar = DNSSoFar + 1
                else:
                    #print "no secondary"
                    secondaryIsSet = False

                if not options.tertiaryServer == "0":
                    #print "we have a tertiary"
                    tertiaryIsSet = True
                    if DNSSoFar == 0:
                        # add it as a primary dns
                        ntNetworkUtils.addPrimaryDNSServer(enabledAdapterName,  options.tertiaryServer)
                    elif DNSSoFar == 1:
                        # add it as a secondary dns
                        ntNetworkUtils.addSecondaryDNSServer(enabledAdapterName, options.tertiaryServer)
                    elif DNSSoFar == 2:
                        ntNetworkUtils.addTertiaryDNSServer(enabledAdapterName, options.tertiaryServer)
                        DNSSoFar = DNSSoFar + 1
                else:
                    #print "no tertiary"
                    tertiaryIsSet = False

                if DNSSoFar > 0:
                    # delete the 0.0.0.0 DNS entry
                    #print "deleting the 0.0.0.0 entry"
                    ntNetworkUtils.deleteZeroDNSEntry(enabledAdapterName)


                #if the dns servers are blank then remove them after the connection is made
                if options.primaryServer == "0" and options.secondaryServer == "0" and options.tertiaryServer == "0":
                    #clear out the dbns servers
                    #print "Clearing ALL DNS NOW"
                    ntNetworkUtils.deleteAllDnsServers(enabledAdapterName)


            # Linkrate

            # test connection
            print("11")
            sys.stdout.flush()

            # Add DNSSUFFIXs
            currentSearchList = get_reg("SearchList")
            #print("the initial Search List is: " + str(currentSearchList))
            name = "SearchList"
            currentSearchList = ""
            addComma = False
            if options.DNSSuffix1 != "" and options.DNSSuffix1 :
                currentSearchList = options.DNSSuffix1
                addComma = True

            if options.DNSSuffix2 != "" and options.DNSSuffix2:
                if addComma:
                    currentSearchList = currentSearchList + "," + options.DNSSuffix2
                else:
                    currentSearchList = currentSearchList + options.DNSSuffix2
                addComma = True

            if options.DNSSuffix3 != "" and options.DNSSuffix3:
                if addComma:
                    currentSearchList = currentSearchList + "," + options.DNSSuffix3
                else:
                    currentSearchList = currentSearchList + options.DNSSuffix3
                addComma = True

            if options.DNSSuffix4 != "" and options.DNSSuffix4:
                if addComma:
                    currentSearchList = currentSearchList + "," + options.DNSSuffix4
                else:
                    currentSearchList = currentSearchList + options.DNSSuffix4
            if options.DNSSuffix5 != "" and options.DNSSuffix5:
                if addComma:
                    currentSearchList = currentSearchList + "," + options.DNSSuffix5
                else:
                    currentSearchList = currentSearchList + options.DNSSuffix5
            if options.DNSSuffix6 != "" and options.DNSSuffix6:
                if addComma:
                    currentSearchList = currentSearchList + "," + options.DNSSuffix6
                else:
                    currentSearchList = currentSearchList + options.DNSSuffix6
            set_reg(name, currentSearchList)

            #print("the final Search List is: " + str(currentSearchList))

            #print "IP:" + str(detectedIpAddress)
            #print "Subnet" + str(detectedSubnetMask)
            #print "detectedGateway:" + str(detectedGateway)
            #sys.stdout.flush()
            connected = ntNetworkUtils.ping(detectedGateway)
            #print "connected:" + str(connected)

            # STATIC OR DHCP
            if options.isStatic == "1":
                #ntNetworkUtils.enableDHCPClientService(False)
                ntNetworkUtils.setAddress(enabledAdapterName,  options.ipAddress,  options.subnetMask,  options.gateway)

                addressSection = 'address'
                settingsParser.add_section(addressSection)
                settingsParser.set(addressSection,  'ipAddress',  options.ipAddress)
                settingsParser.set(addressSection,  'subnetMask',  options.subnetMask)
                settingsParser.set(addressSection,  'gateway',  options.gateway)
            else:
                #print("6")
                #sys.stdout.flush()
                #ntNetworkUtils.enableDHCPClientService(True)
                pass
                #ntNetworkUtils.setDhcp(enabledAdapterName)
            detectedIpAddress, detectedSubnetMask, detectedGateway = ntNetworkUtils.getAddressValues(enabledAdapterName)

            if not connected:
                for x in range(1,5):
                    time.sleep(2)
                    #print "retrying test"
                    connected = ntNetworkUtils.ping(detectedGateway)
                    if connected:
                        break

            if connected:
                print("YES")
                sys.stdout.flush()

                # gather output
                if options.isStatic == "1":
                    output = options.ipAddress + "|" + options.gateway + "|" + options.subnetMask
                    output += "|" + options.primaryServer + "|" + options.secondaryServer + "|" + options.tertiaryServer
                else:
                    output = detectedIpAddress
                    output += "|"
                    output += detectedGateway
                    output += "|"
                    output += detectedSubnetMask

                    detectedDnsServers = ntNetworkUtils.getDnsServers(enabledAdapterName)
                    for server in detectedDnsServers:
                        output += "|" + server

                print("OUTPUT|" + output)
                sys.stdout.flush()

                # write out settings if this was not for a manufacture test
                if str(options.manufacturingTest) != "64641964":
                    try:
                        networkSettingsFile = open(os.path.join(configInfo.getRootFolder(), 'db', 'settingswizard', 'networkSettings.cfg'),  'w')
                        if networkSettingsFile:
                            settingsParser.write(networkSettingsFile)
                            networkSettingsFile.close()
                    except:
                        pass

            else:
                output = ""
                if options.isStatic == "1":
                    #ntNetworkUtils.enableDHCPClientService(False)
                    ntNetworkUtils.setAddress(enabledAdapterName,  options.ipAddress,  options.subnetMask,  options.gateway)

                    addressSection = 'address'
                    #settingsParser.add_section(addressSection)
                    #settingsParser.set(addressSection,  'ipAddress',  options.ipAddress)
                    #settingsParser.set(addressSection,  'subnetMask',  options.subnetMask)
                    #settingsParser.set(addressSection,  'gateway',  options.gateway)
                else:
                    #print("6")
                    #sys.stdout.flush()
                    #ntNetworkUtils.enableDHCPClientService(True)
                    ntNetworkUtils.setDhcp(enabledAdapterName)
                    time.sleep(5)
                    detectedIpAddress, detectedSubnetMask, detectedGateway = ntNetworkUtils.getAddressValues(enabledAdapterName)
                    if detectedIpAddress and detectedGateway and detectedSubnetMask:
                        output = detectedIpAddress
                        output += "|"
                        output += detectedGateway
                        output += "|"
                        output += detectedSubnetMask

                    detectedDnsServers = ntNetworkUtils.getDnsServers(enabledAdapterName)
                    for server in detectedDnsServers:
                        output += "|" + server

                time.sleep(1)
                connected = ntNetworkUtils.ping(detectedGateway)
                if connected:
                    print("OUTPUT|" + output)
                    sys.stdout.flush()
                else:
                    print("NO")
                    sys.stdout.flush()

                #time.sleep(3)
                #if options.isWireless == "1":
                #    ntNetworkUtils.disconnectWlan()
                #else:
                #    ntNetworkUtils.setAdapterEnabled(wiredAdapterName,  False)
    sys.exit(0)
