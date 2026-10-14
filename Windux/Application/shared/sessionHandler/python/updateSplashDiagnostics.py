#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import file_io

def updateSplashDiagnostics(imageFilename):
    """
    Updates SPI Flash with diagnositcs bitmap
    """

    result = 'error'

    # check if block device file exists for mtd5
    if os.path.exists(os.path.join("/","dev", "mtdblock5" )):

        try:

            # open the mtd5 partition file
            inFile = open(os.path.join("/","dev", "mtd5"), 'rb')

            # read first two bytes
            initialBytes = inFile.read(2)

            # close the partition file
            inFile.close()

            # see if partition has image already there
            if initialBytes == b'\xff\xff':

                # if blank flash the image data to mtd5

                if os.name == "posix":
                    cmd = []
                    cmd.append(os.path.join("/","usr","sbin","flashcp"))
                    cmd.append("-v")
                    cmd.append(imageFilename)
                    cmd.append(os.path.join("/", "dev", "mtd5"))

                else:
                    # No Windows implementation needed.
                    pass

                exitCode, stdout, stderr = shellCommand.execute(cmd)

                if exitCode == 0:
                    result = 'success'

            else:
                if os.name == "posix":

                    cmd = []

                        # The section is split, first half is bitmap info and second half is SW config
                    if os.path.exists(os.path.join("/","etc","hobart","slicer")):
                        cmd.append(os.path.join("/","usr","sbin","flash_erase"))
                        cmd.append(os.path.join("/", "dev", "mtd5"))
                        cmd.append(0)
                        cmd.append(32) # erase 128K bytes e.g. 32 blocks * 4096 bytes
                    else:
                        cmd.append(os.path.join("/","usr","sbin","flash_erase"))
                        cmd.append(os.path.join("/", "dev", "mtd5"))
                        cmd.append(0)
                        cmd.append(16) # erase 65536 bytes e.g. 16 blocks * 4096 bytes

                    exitCode, stdout, stderr = shellCommand.execute(cmd)

                    # HT-413
                    # switched from flashcp to cat since flashcp will erase the mtd before copy
                    exitCode = os.system( "/bin/cat " + imageFilename + " > /dev/mtd5" )

                    if exitCode == 0:
                        result = 'success'

        except IOError as exception_details:
            result = 'error: Can not open file: ' + imageFilename + ', Reason:' + exception_details[1]

    return result

def updateSplashErrorBitmaps( diagImage, missingImage ):
    """
    Updates SPI Flash with diagnostic and SD missing bitmaps
    """

    result = 'error'

    # ARK
    # dedicated mtd for diag and missing bmps
    if os.path.exists(os.path.join("/","dev", "mtdblock10" )) or os.path.exists(os.path.join("/","dev", "mtdblock11" )):

        if os.name == "posix":

            if diagImage is not None:
                print('write diag bmp...')

                cmd = []
                cmd.append(os.path.join("/","usr","sbin","flashcp"))
                cmd.append(diagImage)
                cmd.append(os.path.join("/", "dev", "mtd10"))

                exitCode, stdout, stderr = shellCommand.execute(cmd)

                if exitCode != 0:
                    result = 'error'
                    return result

            if missingImage is not None:
                print('write missing bmp...')

                cmd = []
                cmd.append(os.path.join("/","usr","sbin","flashcp"))
                cmd.append(missingImage)
                cmd.append(os.path.join("/", "dev", "mtd11"))

                exitCode, stdout, stderr = shellCommand.execute(cmd)

                if exitCode != 0:
                    result = 'error'
                    return result

            result = 'success'
            return result

    # check if block device file exists for mtd5
    elif os.path.exists( os.path.join( '/', 'dev', 'mtdblock5' ) ):

        try:
            # read image files
            with open( diagImage, 'rb' ) as f:
                diag = f.read()

            with open( missingImage, 'rb' ) as f:
                missing = f.read()

            print(('diag image length: ' + str(len(diag))))
            print(('missing image length: ' + str(len(missing))))

            # write out the partition contents to a file
            #   offset  size    contents
            #   0x00000 0x10000 diagnostics bitmap
            #   0x10000 0x10000 sd card missing bitmap
            #   0x20000 0x20000 software configuration
            with open( '/tmp/udiag', 'wb' ) as f:
                f.write( b'\xFF' * 131072 )

                f.seek( 0 )
                f.write( diag )

                f.seek( 65536 )
                f.write( missing )

            if os.name == 'posix':

                print('updating flash...')

                # The section is split, first half is bitmap info and second half is SW config
                cmd = []
                cmd.append( os.path.join( '/', 'usr', 'sbin', 'flash_erase' ) )
                cmd.append( os.path.join( '/', 'dev', 'mtd5' ) )
                cmd.append( 0 )
                cmd.append( 32 ) # erase 131072 bytes e.g. 32 blocks * 4096 bytes
                exitCode, stdout, stderr = shellCommand.execute( cmd )

                # HT-413
                # switched from flashcp to cat since flashcp will erase the mtd before copy
                exitCode = os.system( "/bin/cat /tmp/udiag > /dev/mtdblock5" )

                if exitCode == 0:
                    result = 'success'

        except IOError as exception_details:
            result = 'error: Can not open files: ' + diagImage + ' ' + missingImage + ', Reason:' + exception_details[1]

    return result

def updateDiagBitmap():

    if os.path.exists(os.path.join("/","etc","hobart","slicer")):
        if updateSplashErrorBitmaps(os.path.join("/", "uboot", "slicer", "u-diagnostics.bmp"), None) == 'success':
            return True
    else:
        if updateSplashErrorBitmaps(os.path.join("/", "uboot", "u-diagnostics.bmp"), None) == 'success':
            return True

    return False

def updateMissingBitmap():

    if os.path.exists(os.path.join("/","etc","hobart","slicer")):
        if updateSplashErrorBitmaps(None, os.path.join("/", "uboot", "slicer", "u-error.bmp")) == 'success':
            return True
    else:
        if updateSplashErrorBitmaps(None, os.path.join("/", "uboot", "u-missing.bmp")) == 'success':
            return True

    return False

def main():

    if os.path.exists(os.path.join("/","etc","hobart","slicer")):
        if updateSplashErrorBitmaps(os.path.join("/", "uboot", "slicer", "u-diagnostics.bmp"), os.path.join("/", "uboot", "slicer", "u-error.bmp")) == 'success':
            return True
    else:
        if updateSplashErrorBitmaps(os.path.join("/", "uboot", "u-diagnostics.bmp"), os.path.join("/", "uboot", "u-missing.bmp")) == 'success':
            return True

    return False

if __name__ == "__main__":

    exitCode = 1

    if main():
        exitCode = 0

    sys.exit(exitCode)
