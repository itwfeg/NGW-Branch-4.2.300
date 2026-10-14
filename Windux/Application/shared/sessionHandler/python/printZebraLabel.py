#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import bmpFormat
import binascii
import zpl
def main(argv1):
    length = 0
    pitch = 0
    zplFile =""
    bitMapFile = ""
    labelZpl = None

    if len(argv1) >= 1:
        bitMapFile = argv1[0]
    if  len(argv1) >= 2:
        xCoordinate = argv1[1]
    else:
        xCoordinate = "0"
    if  len(argv1) >= 3:
        yCoordinate = argv1[2]
    else:
        yCoordinate = "0"
    if  len(argv1) >= 4:
        numOfLabels = int(argv1[3])
    else:
        numOfLabels = 1

    if os.path.exists(bitMapFile):
        bmpHeader = bmpFormat.bmpHeader(bitMapFile)

        #For Python 3, the calculation resulted in a float instead of an int,
        #so had to convert the division to an int to get the same result as Python 2
        rowSize = int( ((bmpHeader.getBitsPerPixel() * bmpHeader.getBitMapWidth() + 31) / 32) ) * 4

        zplCmd = zpl.zpl_commands()
        zplFile +=zplCmd.getStartFormat()
        zplCmd.setFieldOrigin(xCoordinate,  yCoordinate)
        zplFile += zplCmd.getFieldOrigin()
        zplCmd.setFieldCompressionAscii()
        zplCmd.setGraphicFieldStreamSize(bmpHeader.getImageSize())
        zplCmd.setGraphicFieldSize(bmpHeader.getImageSize())
        zplCmd.setGraphicFieldWidth( rowSize )
        zplFile += zplCmd.getGraphicsField()
        zplFile += ","

        numPadBytes = rowSize - (bmpHeader.getBitMapWidth() / 8)
        padMask = bmpHeader.getBitMapWidth() % 4

        asciiStream = binascii.hexlify(bmpHeader.getBitmap())
        invertStream = []
        index = 0
        rowIndex = 0
        rowStream = []
        print((len(asciiStream)))
        while index < len(asciiStream) - 1:
            rowIndex += 1
            if rowIndex <= rowSize - numPadBytes:
                hexValue = int(asciiStream[index:index+2], 16)
				#this inverts the bits
                #hexValue = ~hexValue & 0xFF
                rowStream.append("%0.2X" % (hexValue))
            else:
                if rowIndex == rowSize - numPadBytes:
                    hexValue = int(asciiStream[index:index+2], 16)
					#this inverts the bits
                	#hexValue = ~hexValue & ((2 ** padMask -1) << (8 - padMask))
                    rowStream.append('%0.2X' % (hexValue))
                else:
                    hexValue = 0
                    rowStream.append('%0.2X' % (hexValue))
            if rowIndex == rowSize:
                invertStream.insert(0, rowStream)
                rowStream = []
                rowIndex = 0
            index += 2

        zplFile += ''.join(''.join(row) for row in invertStream) + zplCmd.getFieldSeperator() + zplCmd.getEndFormat()

        if os.name == 'nt':
            # TBD:
            # this is not final solution for Windows
            filename = os.path.join(".", "label.zpl")
        else:
            filename = os.path.join("/", "tmp", "label.zpl")

        labelZpl = open(filename,  'w')

        index = 0
        while index < numOfLabels:
            print(index)
            labelZpl.write(zplFile)
            index += 1
				
        labelZpl.flush()
        labelZpl.close()
        #print the zpl label to the zebra printer
        cmd = []
        cmd.append("lp")
        cmd.append(filename)
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode != 0:
            print("Print label to zebra failed.")
    else:
        print(("Bitmap file '%s' not found." % bitMapFile))

if __name__ == "__main__":
    main(sys.argv[1:])


