#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand

CONST_PART_SIZE = 4096
BLANK_KEY = b'\xff'

def main():

    partition = []
    if os.path.exists(os.path.join("/","dev", "mtdblock3" )):
        try:

            # open the hardware config partition 
            inFile = open(os.path.join("/","dev", "mtd3"), 'rb')

            # read the entire partition  
            partition = inFile.read(CONST_PART_SIZE)
            inFile.close()
            
            #partition = partition.decode('utf-8')

            start = partition.find( "<BDRev>".encode('utf-8') )
            end = partition.find( "</BDRev>".encode('utf-8') )

            if (0 < start) and (start < end) and (0 < end - start - 7):
                # hardware revision string should be in the first 4 bytes
                hwRevision = (partition[start+7:end]).decode('utf-8')
                print(hwRevision)

                #write revision string to a file for safe keeping.
                outFile = open(os.path.join("/", "etc", "hobart", "hwrev.dat"),  'w')
                outFile.write(hwRevision)
                outFile.close()

                #create new u-boot environment variable for the hardware revision.
                cmd = []
                cmd.append("fw_setenv")
                cmd.append("hwrev")
                cmd.append(hwRevision)
                exitCode, stdout,stderr = shellCommand.execute(cmd)

        except IOError as exception_details:
            result = 'error: Reading hardware configuration partition failed: ' + ', Reason:' + exception_details[1]

    return

if __name__ == "__main__":
    main()

