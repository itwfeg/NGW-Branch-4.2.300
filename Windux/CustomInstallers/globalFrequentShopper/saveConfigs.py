'''
Created on Jan 8, 2013

@author: Alex Zavala
'''
import sys
from optparse import OptionParser
import encryptDecrypt

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-S", "--SSID",
             dest="ssid",
             help="SSID of the Wireless Router (ex. StoreWireless)")
p.add_option("-M", "--security",
             dest="securityMethod",
             help="The security method to use for the connection (ex. Open)")
p.add_option("-E", "--encryption",
             dest="encryption",
             help="The encryption method to use for the connection (ex. WEP)")
p.add_option("-K", "--defaultWEPKey",
             dest="defaultKey",
             help="The Default WEP Key to use (ex. -K 2 use WEP Key 2 as default)")
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
p.add_option("-N", "--EAPUsername",
             dest="EAPUsername",
             help="Username for EAP type connections")
p.add_option("-n", "--EAPPassword",
             dest="EAPPassword",
             help="Password for EAP type connections")
p.add_option("-A", "--wpaPassPhrase",
             dest="wpaPassPhrase",
             help="The WPA Passphrase")
p.add_option("-s", "--static",
             dest="isStatic",
             help="1 = is static IP, 0 = DHCP")
p.add_option("-I", "--IPAddress",
             dest="ipAddress",
             help="static IP address, can be blank")
p.add_option("-B", "--subnetMask",
             dest="subnetMask",
             help="subnet mask IP address")
p.add_option("-G", "--gateway",
             dest="gateway",
             help="gateway IP address, can be blank")
p.add_option("-D", "--isAtuoDNS",
             dest="isAutoDNS",
             help="is auto DNS, 0 = yes, 1 = no")
p.add_option("-1", "--primaryDNS",
             dest="primaryDNS",
             help="primary DNS IP address, can be blank")
p.add_option("-2", "--seondaryDNS",
             dest="secondaryDNS",
             help="secondary DNS IP address, can be blank")
p.add_option("-3", "--tertiaryDNS",
             dest="tertiaryDNS",
             help="tertiary DNS IP address, can be blank")
p.add_option("-Z", "--passPhrase",
             dest="passphrase",
             help="The encryption passphrase")
p.add_option("-a", "--serverCertFilename",
             dest="serverCertFilename",
             help="The filename of the trusted root certificate for EAP TLS connections")
p.add_option("-b", "--clientCertSerialNumFilename",
             dest="clientCertSerialNumFilename",
             help="The filename of the client certificate based on serial number for EAP TLS connections")
p.add_option("-c", "--clientCertFilename",
             dest="clientCertFilename",
             help="The filename of the actual client certificate for EAP TLS connections")
p.add_option("-d", "--clientCertPassword",
             dest="clientCertPassword",
             help="The password needed to extract the client cert pfx/p12 file into a .pem and .key")
p.add_option("-e", "--linkRate",
             dest="linkRate",
             help="The link rate desired for a wireless connection.")
p.add_option("-f", "--automaticLinkRate",
             dest="automaticLinkRate",
             help="Whether automatic linkrate or fixed link rate is desired. 1=auto 0=fixed")
p.add_option("-g", "--channel",
             dest="channel",
             help="The channel in MHz to connect to.")
p.add_option("-5", "--DNSSuffix1",
             dest="DNSSuffix1",
             help="DNS Suffix 1")
p.add_option("-6", "--DNSSuffix2",
             dest="DNSSuffix2",
             help="DNS Suffix 2")
p.add_option("-7", "--DNSSuffix3",
             dest="DNSSuffix3",
             help="DNS Suffix 3")
p.add_option("-8", "--DNSSuffix4",
             dest="DNSSuffix4",
             help="DNS suffix 4")
p.add_option("-9", "--DNSSuffix5",
             dest="DNSSuffix5",
             help="DNS suffix 5")
p.add_option("-4", "--DNSSuffix6",
             dest="DNSSuffix6",
             help="DNS suffix 6")
p.add_option("-L", "--exportLocation",
             dest="exportLocation",
             help="Where to save the exported configs to.")

# Parse User's CLI args
#######################
options, args = p.parse_args()

with open("saveConfigs.txt", "w") as myfi:
    #myfi.writeline("WORKED")
    myfi.writelines(sys.argv[1:])
    #myfi.write(wpa_supplicantFileContents + "\n")
myfi.close()

#Encrypts and saves the file configuration
passphrase = options.passphrase
exportLocation = options.exportLocation
networkConfigs = encryptDecrypt.addParseOptionsInOrder(options)
plaintext = encryptDecrypt.formatFile(networkConfigs)
#print "len is " + str(len(exportLocation))
if ( 0 == len(exportLocation) ):
    #print "A"
    encryptDecrypt.saveConfigs(plaintext, passphrase)
    #print "B"
else:
    encryptDecrypt.saveConfigsToFile(plaintext, passphrase, exportLocation)
