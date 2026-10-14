#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import zpl

class zebra:
    def __init__(self):
        self.idVendor = "0x0a5f"
        self.idProduct = "0x0081"
        self.printerAttached = False
        self.printerName = ""

        self.objectName = 'zebra'
        self.logLocation = '/var/log/'
        #self.logLocation = '/home/aaron/python-projects/zebra'

        #create debug log file
        self.errorLog = open( self.logLocation + self.objectName  + "-log.txt",  "a")
        self.init()

    def init(self):
        cmd = []
        cmd.append("lsusb")
        cmd.append("-d")
        cmd.append(self.idVendor +':'+ self.idProduct)
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        if exitCode != 1:
            self.printerAttached = True
            cmd = []
            cmd.append("lpstat")
            cmd.append("-d")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            index = len("system default destination: ")
            self.printerName = stdout[index :].rstrip("\n")

    def isTicketPrinterAttached(self):
        return self.printerAttached
#
#
    def generateCustomerTicket(self, cust_number):
        location = "/etc/hobart/cups/"
        templateFile = "serving_template.zpl"
        now_servingFile = "now_serving.zpl"
        zpl_commands = zpl.zpl_commands()

        if self.isTicketPrinterAttached():

            #open our zpl template
            serving_template = open(location + templateFile, "r+")
            template = serving_template.read()
            serving_template.close()

            #create our now now serving zpl file
            now_serving = open(location + now_servingFile, "wb")
            servingStr = ""
            if template != None:
                cmd = []
                cmd.append("date")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

                #truncate the date time string
                index = stdout.find("EST")
                date = stdout[: index]

                #inset the customer number
                index = template.find("32^FD")

                #adjust centering of number on the ticket
                if cust_number < 10:
                    numStr = template[: index + len("32^FD")]
#
                else:
                    xIndex = template.find("^FO191")
                    numStr = template[: xIndex ]
                    zpl_commands.setFieldOriginX("150")
                    numStr += zpl_commands.getFieldOrigin()
                    xIndex = template.find("180")
                    numStr += template[ (xIndex -1): index + len("32^FD") ]
#
                numStr += str(cust_number)
                numStr += (zpl_commands.getFieldSeperator() + '\n')

                #find where the date should be placed
                index  =  template.find("^FO90")
                dateFormat = template[ index : ]
                index = dateFormat.find("14^FD")

                servingStr = dateFormat[: index + len("14^FD")]
                servingStr += date
                servingStr += (zpl_commands.getFieldSeperator() + '\n')
                servingStr += (zpl_commands.getEndFormat() + '\n')
                servingStr =  numStr + servingStr

                now_serving.write(servingStr)
                now_serving.flush()
                now_serving.close()
#
#
    def printCustomerTicket(self,  ticket):
        ticket = "/etc/hobart/cups/" + ticket

        cmd = []
        cmd.append("lpr")
        cmd.append("-P")
        cmd.append(self.printerName)
        cmd.append("-o")
        cmd.append("raw")
        cmd.append(ticket)

        exitCode, stdout, stderr = shellCommand.execute(cmd)

        if exitCode != 0:
            self.errorLog.write(stderr)
            self.errorLog.flush()
            self.errorLog.close()


