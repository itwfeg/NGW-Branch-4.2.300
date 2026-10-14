# -*- coding: utf-8 -*-

# not tested in python3

import sys
import os
import shellCommand
import platform

class scaleDisplays:
    def __init__(self):
        self.opDisplayType = None
        self.opDisplayResolution = None

        self.cusDisplayType = None
        self.cusDisplayResolution = None


        self.defaultOpDisplayType = "LDB-WSVGA"
        self.defaultCusDisplayType = "LDB-WVGA"

        self.init()

        self.setDisplayResolution()
        self.updateXorgConfig()


    def init(self):
        if  platform.system() == 'Linux':
            cmd = []
            cmd.append("fw_printenv")
            cmd.append("version")
            #get version of u-boot
            str = shellCommand.execute(cmd)
            str = str[1]
            str = str.strip('version=')
            str = str.strip('\n')
            str = str.replace('.',  '')
            #in case of engineering revision
            str = str.replace('a',  '0').replace('b',  '0').replace('c',  '0').replace('d',  '0').replace('e',  '0').replace('f',  '0').replace('g',  '0')
            version = int(str)

            #does it support multiple display types
            if  version >= 170:

                #get the operator display type from u-boot
                cmd = []
                cmd.append("fw_printenv")
                cmd.append("operator_display_type")

                type = shellCommand.execute(cmd)
                type = type[1]
                type = type.strip('operator_display_type=')
                type = type.strip('\n')
                self.opDisplayType = type

                if self.opDisplayType == "0":
                    self.opDisplayType = self.defaultOpDisplayType

                #get the customer display type from u-boot
                cmd = []
                cmd.append("fw_printenv")
                cmd.append("customer_display_type")

                type = shellCommand.execute(cmd)
                type = type[1]
                type = type.strip('customer_display_type=')
                type = type.strip('\n')
                self.cusDisplayType = type

                if self.cusDisplayType == "0":
                    self.cusDisplayType = self.defaultCusDisplayType

            else:
                self.opDisplayType = self.defaultOpDisplayType
                self.cusDisplayType = self.defaultCusDisplayType


    def getOperatorDisplayType(self):
        return self.opDisplayType
    def getCustomerDisplayType(self):
        return self.cusDisplayType

    def setDisplayResolution(self):
        #set the operator display resolution based on display type
        if self.opDisplayType == self.defaultOpDisplayType:
            self.opDisplayResolution  = "1024x600_60.00"
        elif self.opDisplayType == "LDB-WXGA-AU11":
            self.opDisplayResolution  = "1366x768_60.00"
        elif self.opDisplayType == "LDB-WXGA-IN14":
            self.opDisplayResolution  = "1366x768_60.00"

        #set the customer display resolution based on display type
        if self.cusDisplayType == self.defaultCusDisplayType:
            self.cusDisplayResolution  = "800x480_60.00"
        elif self.cusDisplayType == "LDB-WSVGA":
            self.cusDisplayResolution  = "1024x600_60.00"
        elif self.cusDisplayType == "LDB-WXGA-AU11":
            self.cusDisplayResolution  = "1366x768_60.00"
        elif self.cusDisplayType == "LDB-WXGA-IN14":
            self.cusDisplayResolution  = "1366x768_60.00"

    def updateXorgConfig(self):
        update = False

        #open the X configuration file
        xorg = open(os.path.join("/","etc",  "X11",  "xorg.conf"), "r")
        config = xorg.read()
        xorg.close()

        #find the operator resolution (Modes)
        #config = str(config)
        index = config.find("Modes")
        subStr = config[index + len("Modes"):]
        index = subStr.find('"', subStr.find('"')+1)
        if index != -1:
            xorgOperRes = subStr[:index]
            xorgOperRes = xorgOperRes.replace(' ',  '').replace('"',  '')

            #if our current display resolution is different from X then update
            if self.opDisplayResolution != xorgOperRes:
                #replace current  display mode with new desplay mode
                config = config.replace(xorgOperRes,  self.opDisplayResolution)
                update = True

        #find the customer display resolution
        if index != -1:
            index = config.find("Modes")
            subStr = config[index + len("Modes"):]
            #find the second occurance.
            index = subStr.find("Modes")
            subStr = subStr[index + len("Modes"):]
            index = subStr.find('"', subStr.find('"')+1)
            xorgCusRes = subStr[:index]
            xorgCusRes = xorgCusRes.replace(' ',  '').replace('"',  '')

            #if our current display resolution is different from X then update
            if self.cusDisplayResolution != xorgCusRes:
                #replace current  display mode with new desplay mode
                config = config.replace(xorgCusRes,  self.cusDisplayResolution)
                update = True

        if update == True:
            xorg = open(os.path.join("/","etc",  "X11",  "xorg.conf"), "w")
            xorg.write(config)
            xorg.flush()
            xorg.close()
