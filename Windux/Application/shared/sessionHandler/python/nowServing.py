#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import zpl
import shellCommand
import ticketPrinter

def main(argv):
    location = "/etc/hobart/cups/"
    #location = "/home/aaron/fastrack/zebra-project/now-serving/"
    templateFile = "serving_template.zpl"
    now_servingFile = "now_serving.zpl"
    printer = ticketPrinter.zebra()
    cust_number = argv 
    zpl_commands = zpl.zpl_commands()
    
    
    if printer.isTicketPrinterAttached():
        
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
            if int(cust_number,  10) < 10:
                numStr = template[: index + len("32^FD")]
            else:
                xIndex = template.find("^FO191")
                numStr = template[: xIndex ]
                zpl_commands.setFieldOriginX("150")
                numStr += zpl_commands.getFieldOrigin()
                xIndex = template.find("180")
                numStr += template[ (xIndex -1): index + len("32^FD") ]
            numStr += cust_number
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
            
            #send the customer ticket to the zebra printer
            printer.printCustomerTicket(now_servingFile)
        
if __name__ == "__main__":
    main(sys.argv[1])
    
