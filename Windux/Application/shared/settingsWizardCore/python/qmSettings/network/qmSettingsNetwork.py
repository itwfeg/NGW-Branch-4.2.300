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

#Sample command -> qmSettingsNetwork.pyc -TBD

# Will set network to provided values

# Preprocessor directives
##########################
import sys, os, string
from optparse import OptionParser
import time, subprocess

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


if os.name == "posix":
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
    
    if options.scan == "1":
        __dir__ = os.path.dirname(os.path.abspath(__file__))
        filepath = os.path.join(__dir__, 'apList.txt')

        testFile = open(filepath, 'rU')
        linestrings = testFile.read()
       
        #linestrings = subprocess.check_output("iwlist wlan0 scan", shell=True)

        # Command below shows signal strength
        # watch -n 1 "awk 'NR==3 {print \"WiFi Signal Strength = \" \$3 \"00 %\"}''' /proc/net/wireless"

        SSIDList = [];

        includeIt = True;

        strengthNumber = "0"
        encrypt = "AES"
        
        for line in linestrings.splitlines():
            if line.find("ESSID:\"") > -1:                
		s = string.split(line,":\"")
                s = string.split(s[1],"\"")
                if len(s[0]) > 0 and s[0] != '' and s[0] not in SSIDList:
                    #print(str(s[0]) + '\n')
		    #print len(s)
		    SSIDList.append(s[0])
                    includeIt = True
                else:
                    includeIt = False
            elif line.find("Quality=") > -1 and includeIt:
                s = string.split(line,"=")
                s = string.split(s[1]," ")
                s = string.split(s[0],"/")
                #s = s[0]

                SSIDList.append(str((int(s[0]) / 14) + 1))

                # Uncomment below if wireless hardware supports security and ecnryption
                #reporting from AP polling 
           # else:
           #     found = False
           #     if line.find("Encryption key:off") > -1:
           #         SSIDList.append("0pen")
           #     elif line.find("Encryption key:on") > -1:
           #         SSIDList.append("WEP")
           #     elif line.find("WPA2") > -1:
           #         SSIDList.append("WPA2")
           #         found = True
           #     elif line.find("IE: WPA Version 1") > -1 and not found:
           #         SSIDList.append("WPA1")
           #         found = False
           #     elif line.find("Group Cipher : TKIP") > -1:
           #         encrypt = "TKIP"
               
           # SSIDList.append(encrypt)
        #testFile.close()
                           
        for SSID in SSIDList:
            returnMessage += str(SSID) + str("|")

        print(str(returnMessage.lstrip(' ')))

    else: 
        ##############################
        #
        # Start setting the network
        #
        ##############################

        connected = False
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

        ###############################################################
        # Extra wireless info
        
        if options.isWireless == "1":

            
            ####################
            # WIRELESS
            ####################

         
            networkAdapterName = "wlan0"
            #commandList.append("killall wpa_supplicant")
            #commandList.append("killall wpa_supplicant")
            os.system("killall wpa_supplicant")
	    os.system("killall wpa_supplicant")
	    # crazy to do it twice but
            # fixes some wpa_supplicant
            # zombies problems.

            if str(options.securityMethod) == "OPEN":
                # Do not use wpa_supplicant, just use iwconfig
                os.system("iwconfig " + str(networkAdapterName) + " essid " + str(ssid) + " key " + str(defaultKey) + "[" + str(options.defaultKeyNum) + "]" )
            else:
   
                # Create the WPA SUPPLICANT CONFIGURATION FILE
                
                if str(options.encryption) == "AES":
                    pairwise = "CCMP TKIP"
                    group = encryption
                        
                    if(len(str(options.wpaPassPhrase))) < 64:
                        #do wpa_passphrase to get format ready
            
                        encryptedPassphrase = subprocess.check_output("wpa_passphrase " + str(options.ssid) + " " + str(options.wpaPassPhrase), shell=True)
                
                        encryptedPassphrase = encryptedPassphrase.replace("#psk=","#PassPhrase")
                
                        start = encryptedPassphrase.find("psk=")
                        if start != -1:
                            start += 4
                    
                            encryptedPassphrase = encryptedPassphrase[start:]
                            ep = string.split(encryptedPassphrase,"\n")
                            
                            encryptedPassphrase = ep[0]

                    
                # build the wpa string
                writeWPA_SUPPLICANTFile = False;    
                if str(options.securityMethod) == "WPA-Personal" and str(options.encryption) == "AES":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=WPA\n   key_mgmt=WPA-PSK\n   psk=\"" + str(encryptedPassphrase) + "\"\n   pairwise=CCMP TKIP\n   group=CCMP TKIP\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA-Personal" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=WPA\n   key_mgmt=WPA-PSK\n   psk=\"" + str(encryptedPassphrase) + "\"\n   pairwise=TKIP\n   group=TKIP\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA-Enterprise (PEAP)" and str(options.encryption) == "AES":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=WPA-EAP\n   eap=TTLS PEAP TLS\n   phase2=\"auth=MSCHAPV2\"\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA-Enterprise (PEAP)" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=WPA-EAP\n   eap=TTLS PEAP TLS\n   phase2=\"auth=MSCHAPV2\"\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA-Enterprise (LEAP)" and str(options.encryption) == "AES":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=WPA-EAP\n   eap=LEAP\n   auth_alg=LEAP\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA-Enterprise (LEAP)" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=WPA-EAP\n   eap=LEAP\n   auth_alg=LEAP\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA2-Personal" and str(options.encryption) == "AES":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-PSK\n   psk=\"" + str(encryptedPassphrase) + "\"\n   pairwise=CCMP TKIP\n   group=CCMP TKIP\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA2-Personal" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   proto=RSN WPA\n   key_mgmt=WPA-PSK\n   psk=\"" + str(encryptedPassphrase) + "\"\n   pairwise=TKIP\n   group=TKIP\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA2-Enterprise (PEAP)" and str(options.encryption) == "AES":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=WPA-EAP\n   eap=TTLS PEAP TLS\n   phase2=\"auth=MSCHAPV2\"\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA2-Enterprise (PEAP)" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=WPA-EAP\n   eap=TTLS PEAP TLS\n   phase2=\"auth=MSCHAPV2\"\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA2-Enterprise (LEAP)" and str(options.encryption) == "AES":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=WPA-EAP\n   eap=LEAP\n   auth_alg=LEAP\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "WPA2-Enterprise (LEAP)" and str(options.encryption) == "TKIP":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=WPA-EAP\n   eap=LEAP\n   auth_alg=LEAP\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n}"
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "802.1X (PEAP)" and str(options.encryption) == "WEP":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=IEEE8021X\n   eap=TTLS PEAP TLS\n   phase2=\"auth=MSCHAPV2\"\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n}"                 
                    writeWPA_SUPPLICANTFile = True
                elif str(options.securityMethod) == "802.1X (LEAP)" and str(options.encryption) == "WEP":
                    wpa_supplicantFileContents = "ap_scan=1\nctrl_interface=/var/run/wpa_supplicant\n\nnetwork={\n   ssid=\"" + str(ssid) + "\"\n   scan_ssid=1\n   key_mgmt=IEEE8021X\n   eap=LEAP\n   auth_alg=LEAP\n   identity=\"" + str(options.EAPUsername) + "\"\n   password=\"" + str(options.EAPPassword) + "\"\n}"
                    writeWPA_SUPPLICANTFile = True
                else:
                    # It is an OPEN WEP 
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
        #commandList.append("wpa_supplicant -Dwext -i" + str(networkAdapterName) + " -c/etc/wpa_supplicant.conf -B")
	os.system("wpa_supplicant -Dwext -i" + str(networkAdapterName) + " -c/etc/wpa_supplicant.conf -B")
        # do dhclient and ip route for Static IP or non-autoDNS
        
	if(str(options.isStatic) == "0"):
		os.system("dhcpcd wlan0")
	else:
		os.system("iwconfig wlan0 " + str(options.ipAddress) + " netmask " + str(options.subnetMask))
		os.system("ip route add default via " + str(options.gateway))

	#if(str(options.isAutoDNS) == "0"):
		
	

	#########################################
	#########################################
        returnMessage = "YES"
        
        # WIRED do the ip,subnet,gateway and dns settings here for 
        networkAdapterName = "eth0"
        
        # only wired connection should be used and 
        
        ###############################
        #    MAIN SYSTEM CALLS
        
        # Take User's wired or wireless adapter choice down
        # commandList.insert(0,"ifconfig " + str(networkAdapterName) + "down")
        
        print returnMessage

	#################### SAVING FILE NOW  YES or NO
	
else:
        # WINDOWS BLOCK GOES HERE
    print("Windows")
