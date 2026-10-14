#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# need to fix serialFlash first

import sys
import os
import shellCommand
import serialFlash

def sync():
    #read the manufactuing section of the serial flash.
    flash = serialFlash.mfgSection()
    revision_flash = ""
    revision_file = ""
    
    if flash.isSectionBlank() != True:
        #read the hardware version string stored in the serial flash.
        revision_flash = flash.getBoardRevision()
        if revision_flash != "":            
            if os.path.exists(os.path.join("/", "etc", "hobart", "hwrev.dat")):
                # Read hardware revision string file.
                hwFile = open(os.path.join("/", "etc", "hobart", "hwrev.dat"),  'r+')
                revision_file = hwFile.read()
                # Sync on difference
                if revision_file != revision_flash:
                    # Sync the file to the serial flash version 

                    # SOF-1661: Clobber the existing file and write the new value, instead of trying to update the contents
                    hwFile.close()
                    hwFile = open(os.path.join("/", "etc", "hobart", "hwrev.dat"),  'w+')
                    hwFile.write(revision_flash)
                
                hwFile.close()
            
            else:
                hwFile = open(os.path.join("/", "etc", "hobart", "hwrev.dat"),  'w')
                hwFile.write(revision_flash)
                hwFile.close()

   
  
 
    #create new u-boot environment variable for the hardware revision.
    #cmd = []
    #cmd.append("fw_setenv")
    #cmd.append("hwrev")
    #cmd.append(hwRevision)
    #exitCode, stdout,stderr = shellCommand.execute(cmd)

if __name__ == "__main__":
    sync()
 
