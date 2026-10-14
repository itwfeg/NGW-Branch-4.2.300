#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import string
import os
import time
import shellCommand
import file_io
import osUtils
import platform

''''Purpose: userspace interface to the software keys partition of the on board serial flash.
    This script will only handle the first block on the serial flash software keys partition 
    which will limit the amount of key data stored.
'''

CONST_PART_SIZE = 4096
BLANK_KEY = b'\xff'

def readAllKeys():
    '''' read all the keys in the first block of the parition '''
    result = None
    if platform.system() == 'Linux':
        if os.path.exists(os.path.join("/","dev", "mtdblock6" )):
            try:

                # open the software keys partition 
                inFile = open(os.path.join("/","dev", "mtd6"), 'rb')

                # read the entire partition  
                partition = inFile.read(CONST_PART_SIZE)
                result = ('success', partition)
            
            except IOError as exception_details:
                result = ('error', 'Reading Keys partition failed: ' + ', Reason:' + exception_details[1])
        else:
            result = ('error', 'Reading Keys partition failed: ' + ', Reason:Key block does not exist')

    elif platform.system() == 'Windows':
        if not os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin" )):
            result = ('error', 'Reading Keys partition failed: ' + ', Reason:Missing binary!')
        else:
            KEY_FILE = 'C:\\Program Files\\Hobart\\Bin\\keys.bin'
            
            try:
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'd',  '130000',  KEY_FILE]
                attempts = 8
                
                # The HTx scale takes some time to boot and mount all devices... it is possible when
                # executing the above binary the FT2232D has not been mounted.  In these situations
                # exit code is valid but no file gets generated.  So we need to retry a number of
                # times until we get a valid read.  I have also seen timing issues where the device
                # is found but failes to open... yikes!
                while (0 < attempts):
                    # read the flash
                    exitCode, stdout, stderr = osUtils.winRunElevated(args)
                    #print stdout

                    # ensure file exists and file is of appropriate size
                    if os.path.exists(KEY_FILE):
                        statinfo = os.stat(KEY_FILE)
                        if (CONST_PART_SIZE == statinfo.st_size):
                            break

                    # failed... maybe the device will be valid in a short while?
                    time.sleep( 0.25 )
                    attempts -= 1

            except:
                result = ('error', 'Reading Keys partition failed!')
                return result
                
            # open the software keys partition 
            if not os.path.exists(KEY_FILE):
                result = ('error', 'Reading Keys partition failed: ' + ', Reason:Key block does not exist')
            else:
                inFile = open(KEY_FILE,  'rb')       

                # read the entire partition  
                partition = inFile.read(CONST_PART_SIZE)
                inFile.close()
            
                # truncate partition
                for i in range(0, len(partition)):
                    if ((partition[i] == BLANK_KEY) or (partition[i] == b'\x00')):
                        if i == 0:
                            partition = None
                        else:
                            partition = partition[:i]
                        break

                result = ('success', partition)

                os.remove(KEY_FILE)
    else:
        result = ('error', 'Reading Keys partition failed: ' + ', Reason:Unknown operating system!')
    
    return result

def eraseAllKeys():

    if platform.system() == 'Linux':
    	
        cmd = []
    	
        cmd = []
        cmd.append(os.path.join("/","usr","sbin","flash_eraseall"))
        cmd.append(os.path.join("/", "dev","mtd6"))
        
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode == 0:
            return ('success', None)
        
    elif platform.system() == 'Windows':
        if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin")):
            KEY_FILE = 'C:\\Program Files\\Hobart\\Bin\\keys.bin'

            #write out the partition contents to a file 
            outFile = open(KEY_FILE,  'w')
            outFile.write(b'\xFF' * CONST_PART_SIZE)
            outFile.close()
            
            #write the contents of the manufacturing partition file to the serial flash
            try:
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'u',  '130000',  KEY_FILE]
                osUtils.winRunElevated(args)
            except:
                os.remove(KEY_FILE)
                return ('error', 'Write Keys partition failed!')

            os.remove(KEY_FILE)
            return ('success', None)

    return ('error', stderr)
    
def writeAllKeys(keys):
    '''' writes a key  to the fisrt available space in the scaleKeys partition '''

    # check if block device file exists for mtd6
    result = None
    if platform.system() == 'Linux':
        if os.path.exists(os.path.join("/","dev", "mtdblock6" )):
            try:

                #write the new key information to file
                outFile = open(os.path.join("/", "tmp", "keys.dat"),  'w')
                outFile.write(keys.decode("utf-8"))
                outFile.close()

                # if blank, flash the image data to mtd6
                cmd = []
                cmd.append(os.path.join("/","usr","sbin","flashcp"))
                cmd.append("/tmp/keys.dat")
                cmd.append(os.path.join("/", "dev", "mtd6"))

                exitCode, stdout, stderr = shellCommand.execute(cmd)
                if exitCode == 0:
                    os.remove(os.path.join("/", "tmp", "keys.dat"))
                    result = ('success',  None)
                
            except IOError as exception_details:
                result = ('error', 'Flashing Keys partition failed: ' + ', Reason:' + exception_details[1])
        else:
            result = ('error', 'Reading Keys partition failed: ' + ', Reason:Key block does not exist')
            
    elif platform.system() == 'Windows':
        if os.path.exists(os.path.join("C:\Program Files", "Hobart", "Bin")):
            KEY_FILE = 'C:\\Program Files\\Hobart\\Bin\\keys.bin'

            #write out the partition contents to a file 
            outFile = open(KEY_FILE,  'w')
            outFile.write(b'\xFF' * CONST_PART_SIZE)
            outFile.seek(0)
            outFile.write(keys)
            outFile.close()

            #write the contents of the manufacturing partition file to the serial flash
            try:
                args = ['C:\\Program Files\\Hobart\\Bin\\SPIFlashTest.exe',  'u',  '130000',  KEY_FILE]
                osUtils.winRunElevated(args)
            except:
                os.remove(KEY_FILE)
                return ('error', 'Write Keys partition failed!')

            result = ('success', None)
            os.remove(KEY_FILE)
            
        else:
            result = ('error', 'Write Keys partition failed: ' + ', Reason:Missing binary!')
    else:
        result = ('error', 'Write Keys partition failed: ' + ', Reason:Unknown operating system!')
        
    return result

if __name__ == '__main__':
    exitCode = 0
    result, allKeys = readAllKeys()
    
    if 'success' != result:
        exitCode = -1
        print('READ ALL KEYS FAILED!')
        
    if allKeys != None:
        print(allKeys)
        
    sys.exit(exitCode)
