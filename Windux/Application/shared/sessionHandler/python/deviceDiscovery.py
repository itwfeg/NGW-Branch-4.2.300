#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import usb.core
import usb.util
import sys
import os
import time
import platform
import re

class atmelDevice:
    def __init__(self):
        self.interface = 1
        self.idVendor = 0x03eb
        self.idProduct = 0x6124
        self.scaleStyle = 'unknown'
        self.atmel = None
        self.epInBulk = None
        self.epOutbulk = None
        self.hobDeviceVid = 0
        self.hobDevicePid = 0
        self.hobartDevice=None
        self.hobartDevices = []

        #atmel samba ack command
        self.ack_command = b'\x4E\x23'

        self.objectName = 'deviceDiscovery'
        
        if  platform.system() == 'Linux':
            self.logLocation = '/var/log/'
        else:
            self.logLocation = 'C:\\Program Files\\Hobart\\'

        #create debug log file
        self.errorLog = open( self.logLocation + self.objectName  + "-log.txt",  "wb")
        # broke scale booting up. Dont know why, looks simple enough. Adding this line broke scale boot so I removed it
        # self.errorLog.write("Log open: **********************")
        self.enumerate()
        self.determineScaleType()

        #if no atmel hobart devices found then check for prepack wrapper or cla
        self.setScaleModelParams()
        
        #if no atmel hobart devices found then check for prepack wrapper or cla
        if self.scaleStyle == 'unknown':
            self.findPrePackDevices()

        if self.scaleStyle == 'service counter' or self.scaleStyle == 'unknown':
            # broke scale booting up. Dont know why, looks simple enough. Adding this line broke scale boot so I removed it
            #self.errorLog.write("Reset: Hard reset of the weigher / printer controller.")
            # hard reset weigher / printer controller (FPC)
            os.system("/bin/echo 0 >> /usr/local/gpio/hub1_enable/value")
            time.sleep(1)
            os.system("/bin/echo 1 >> /usr/local/gpio/hub1_enable/value")

        self.recordScaleType()

    def enumerate(self):

        #-------------------------
        # Find Atmel samba devices
        #-------------------------
        deviceList = usb.core.find(find_all=True,  idVendor=0x03eb, idProduct=0x6124)

        for self.atmel in deviceList:
            if self.atmel is None:
                continue

            if platform.system() == 'Linux':
                if self.atmel.is_kernel_driver_active(self.interface):
                    self.atmel.detach_kernel_driver(self.interface)

                self.atmel.set_configuration()
                usb.util.claim_interface(self.atmel, 0)
                usb.util.claim_interface(self.atmel, 1)

            cfg = self.atmel.get_active_configuration()

            interface_number = cfg[(1,0)].bInterfaceNumber

            #get our interface descriptor from configuration
            intf = cfg[(1,0)]
            if intf is None:
                self.errorLog.write("Error: Enumeration Failure: No Interface found. ")
                return 1

            #get our Out bulk endpoint
            self.epOutbulk = usb.util.find_descriptor(
                intf,
                # match the first OUT endpoint
                custom_match = \
                lambda e: \
                    usb.util.endpoint_direction(e.bEndpointAddress) == \
                    usb.util.ENDPOINT_OUT
            )

            #get our In bulk endpoint
            self.epInBulk = usb.util.find_descriptor(
                intf,
                # match the first OUT endpoint
                custom_match = \
                lambda e: \
                    usb.util.endpoint_direction(e.bEndpointAddress) == \
                    usb.util.ENDPOINT_IN and usb.util.endpoint_type(e.bmAttributes) == \
                    usb.util.ENDPOINT_TYPE_BULK
            )

            if self.epOutbulk is None:
                self.errorLog.write("Error: Enumeration Failure: Bulk Out endpoint not found.")
                return 1

            if self.epInBulk is None:
                self.errorLog.write("Error: Enumeration Failure: Bulk In endpoint not found!")
                return 1

            try:
                #send ack command
                self.atmel.write(self.epOutbulk.bEndpointAddress,  self.ack_command,  1)
                ack = self.atmel.read(self.epInBulk.bEndpointAddress,  2, 1)

                self.readHobartVid()
                self.readHobartPid()
                self.identifyDevice()

            except usb.core.USBError as e:
                if e.args == ('Operation timed out'):
                    self.errorLog.write("Error: Enumeration Failure: Message Timeout")

            #we are finished with the device release the interface
            usb.util.release_interface(self.atmel, 0)
            usb.util.release_interface(self.atmel, 1)
            usb.util.dispose_resources(self.atmel)

            #if the FPC device is encountered, no sense looking further (service scale only scale type with an FPC)
            if self.hobDeviceVid == 0x204:
                break
            elif self.hobDeviceVid == 0x208:
                break

            return 0

        #----------------
        # Find FPC device
        #----------------
        deviceList = usb.core.find( find_all=True, idVendor=0x128c, idProduct=0x0204 )

        if len( deviceList ) != 0:
            self.scaleStyle = 'service counter'
            return 0

        deviceList = usb.core.find( find_all=True, idVendor=0x128c, idProduct=0x0208 )

        if len( deviceList ) != 0:
            self.scaleStyle = 'service counter'
            return 0

        self.scaleStyle = 'unknown'
        return 0

    def readHobartVid(self):
        #send samba command to read flash location 0x108000 2
        msg = []
        readVidCmd = 'h00108000,#'
        self.atmel.write(self.epOutbulk.bEndpointAddress, readVidCmd,  1)
        msg = self.atmel.read(self.epInBulk.bEndpointAddress,  2, 1)
        self.hobDeviceVid = msg[1]
        self.hobDeviceVid  <<= 8
        self.hobDeviceVid  |= msg[0]

    def readHobartPid(self):
        #send samba command to read flash location 0x108002
        msg = []
        readVidCmd = 'h00108002,#'
        self.atmel.write(self.epOutbulk.bEndpointAddress, readVidCmd,  1)
        msg = self.atmel.read(self.epInBulk.bEndpointAddress,  2, 1)
        self.hobDevicePid = msg[1]
        self.hobDevicePid  <<= 8
        self.hobDevicePid |= msg[0]


    def identifyDevice(self):
        #identify device as service scale or pre-pack device
        if self.hobDeviceVid == 0x128c:
            if self.hobDevicePid == 0x204:
                self.hobartDevice = 'ServiceCounter-FPC'
                self.hobartDevices.append(self.hobartDevice)
            elif self.hobDevicePid == 0x208:
                self.hobartDevice = 'ServiceCounter-FPC'
                self.hobartDevices.append(self.hobartDevice)
            elif self.hobDevicePid == 0x300:
                self.hobartDevice = 'PrePack-Wrapper'
                self.hobartDevices.append(self.hobartDevice)
            elif self.hobDevicePid == 0x205:
                self.hobartDevice =  'PrePack-Printer'
                self.hobartDevices.append(self.hobartDevice)
            elif self.hobDevicePid == 0x500:
                self.hobartDevice =  'PrePack-Cla'
                self.hobartDevices.append(self.hobartDevice)
            elif self.hobDevicePid == 0x401:
                self.hobDevice = 'PrePack-Weigher'
                self.hobartDevices.append(self.hobartDevice)

        return 0

    def determineScaleType(self):
        y = len(self.hobartDevices)
        x = 0
        while x != y:
            if self.hobartDevices[x] == 'ServiceCounter-FPC':
                self.scaleStyle = 'service counter'
                break
            elif self.hobartDevices[x] == 'PrePack-Printer' or 'PrePack-Weigher' or 'PrePack-Wrapper' or 'PrePack-Cla':
                self.scaleStyle = 'pre-pack'
                break
            x += 1
    
    #--------------------------------
    # Updates settings in configc.dat
    #--------------------------------
    # Updated as part of HT-1
    def setScaleModelParams(self):

        scaleType = "0"
        
        if( self.scaleStyle == 'pre-pack' ):
            scaleType = "2"
        elif( self.scaleStyle == 'service counter' or self.scaleStyle == 'unknown' ):
            # Same default as loadScaleModelCfg() in CfgInternal.cpp (ACR-25)
            scaleType = "3"

        if( scaleType != "0" ):
            if  platform.system() == 'Linux':
                configCDATFileLocation = '/etc/hobart/configc.dat'
            else:
                configCDATFileLocation = 'C:\\Program Files\\Hobart\\configc.dat'
            
            # check to see if configc.dat file is present
            if os.path.exists( configCDATFileLocation ):

                fileIn = open( configCDATFileLocation, 'rb' )
                fileData = fileIn.readlines()
                fileIn.close()

                fileOut = open( configCDATFileLocation, 'wb' )
                
                # HA-78
                # If Pre-Pack then need to allow Wrapper/Printer errors to be given again on stable weight.
                # Also for either model may need to find the parameter in confic.dat and change
                # its value (i.e. Need to be able to change 'T' to 'F' and 'F' to 'T')
                prT = re.compile('printerStatusNonRepeatingMsgsToUI=([yYtT])')
                prF = re.compile('printerStatusNonRepeatingMsgsToUI=([nNfF])')

                printerStatusNonRepeatingMsgsToUI_Updated = False
                
                for line in fileData:
                    line = line.decode('utf-8')
                    if( scaleType == "3" and prF.match(line) != None ):
                        # For Service Counter need to set back to 'T' in
                        # configc.dat in case it had previously been set to 'F'
                        line = line.replace(prF.match(line).groups()[0], "T")
                        printerStatusNonRepeatingMsgsToUI_Updated = True
                        
                    if( scaleType == "2" and prT.match(line) != None ):
                        # HA-78
                        # For Pre-Pack need to change from 'T' to 'F' in configc.dat
                        line = line.replace(prT.match(line).groups()[0], "F")
                        printerStatusNonRepeatingMsgsToUI_Updated = True
                        
                    if( scaleType == "2" and prF.match(line) != None ):
                        # HA-78
                        # For Pre-Pack, if already set to 'F' in configc.dat
                        # then don't need to append the parameter
                        printerStatusNonRepeatingMsgsToUI_Updated = True
                        
                    fileOut.write(line.encode('utf-8'))

                fileOut.close()
                
                #----------------------------------------------------------------------------------
                # If existing rows were not found and updated, then need to append them to the file
                #----------------------------------------------------------------------------------
                if ( scaleType == "2" and printerStatusNonRepeatingMsgsToUI_Updated == False ):
                
                    fileOut = open( configCDATFileLocation, 'a' )

                    if scaleType == "2" and printerStatusNonRepeatingMsgsToUI_Updated == False:
                        # HA-78
                        fileOut.write( 'printerStatusNonRepeatingMsgsToUI=F\n' )
                        
                    fileOut.close()
                    

    def recordScaleType(self):
        print((self.scaleStyle))

        with open( '/etc/hobart/scale-type' + ".dat",  "wb") as f:
            f.write(self.scaleStyle.encode('utf-8'))
            f.write('\n'.encode('utf-8'))

    def findPrePackDevices(self):
        #do we have a wrapper device?
        device = usb.core.find(idVendor=0x128c, idProduct=0x300)
        if device is not None:
            self.hobDevicePid = 0x300
            self.hobDeviceVid = 0x128c
        else:
            #do we have a cla?
            device = usb.core.find(idVendor=0x128c, idProduct=0x500)
            if device is not None:
                self.hobDevicePid = 0x500
                self.hobDeviceVid = 0x128c
            else:
                #do we have a handwrap station?
                device = usb.core.find(idVendor=0x128c, idProduct=0x200)
                if device is not None:
                    self.hobDevicePid = 0x200
                    self.hobDeviceVid = 0x128c

        self.identifyDevice()
        self.determineScaleType()

if __name__ == "__main__":
    device = atmelDevice()
