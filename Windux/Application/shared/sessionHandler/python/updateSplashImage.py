#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import file_io
import cpuCount
import getBoardType
import getModelName

def updateSplashImage(imageFilename):
    """
    Updates SPI Flash with image bitmap
    """

    result = 'error'

    # check if block device file exists for mtd7
    if os.path.exists(os.path.join("/","dev", "mtdblock7" )):

        try:
            print('write splash image...')

            # if blank flash the image data to mtd7

            if os.name == "posix":
                
                cmd = []

                if os.path.exists(os.path.join("/","etc","hobart","slicer")):
                    cmd.append(os.path.join("/","usr","sbin","flash_erase"))
                    cmd.append(os.path.join("/", "dev", "mtd7"))
                    cmd.append(0)
                    cmd.append(32) # erase 128K bytes e.g. 32 blocks * 4096 bytes
                else:
                    cmd.append(os.path.join("/","usr","sbin","flash_erase"))
                    cmd.append(os.path.join("/", "dev", "mtd7"))
                    cmd.append(0)
                    cmd.append(16) # erase 65536 bytes e.g. 16 blocks * 4096 bytes

                exitCode, stdout, stderr = shellCommand.execute(cmd)

            else:
                # TODO:
                # need windows implementation
                pass

            exitCode, stdout, stderr = shellCommand.execute(cmd)

            if os.name == "posix":
                cmd = []
                cmd.append(os.path.join("/","usr","sbin","flashcp"))
                #cmd.append("-v")
                cmd.append(imageFilename)
                cmd.append(os.path.join("/", "dev", "mtd7"))

            else:
                # TODO:
                # need windows implementation
                pass

            exitCode, stdout, stderr = shellCommand.execute(cmd)

            if exitCode == 0:
                result = 'success'

        except IOError as exception_details:
            result = 'error: Can not open file: ' + imageFilename + ', Reason:' + exception_details[1]

    return result

def main():

    result = 'error'

    # Determine the board type based on the # of CPU's
    numCpu = cpuCount.getCPUCount()
    numCpu = numCpu.replace( "\n", "" )

    model = getModelName.getModelName()
    substr = "FS"
    print("updateSpashImage ModelName: " + model)

    id = "NA"
    if numCpu == "1":
        id = 'Solo'
    elif numCpu == "2":
        id = 'Dual'
    elif numCpu == "4":
        id = 'Quad'

    if substr in model.upper():
        result = updateSplashImage(os.path.join("/", "uboot", "fs", "fs8.bmp"))
        print("FS = true use hfs8.bmp")
    elif getBoardType.isArkBoard():
        # SOF-3203: Need to update flash with the HTm image
        result = updateSplashImage(os.path.join("/", "uboot", "htm8.bmp"))
    else:
        if id == 'Solo':
            result = updateSplashImage(os.path.join("/", "uboot", "hts8.bmp"))
        elif id == 'Dual':
            result = updateSplashImage(os.path.join("/", "uboot", "hti8.bmp"))
        elif id == 'Quad':
            result = updateSplashImage(os.path.join("/", "uboot", "htx8.bmp"))

    if 'success' == result:
        return True

    print(result)
    return False

if __name__ == "__main__":

    exitCode=1

    if main():
        exitCode = 0

    sys.exit(exitCode)
