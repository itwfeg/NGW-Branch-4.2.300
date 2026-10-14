import sys
import os
import shellCommand
import file_io

def updateSplashVersion(imageFilename):
    """
    Updates SPI Flash with version bitmap
    """

    result = 'error'

    # check if block device file exists for mtd2
    if os.path.exists(os.path.join("/","dev", "mtdblock2" )):

        try:

            print 'write splash version...'

            if os.name == "posix":

                # The section is split, first half is bitmap info and second half is SW config
                cmd = []
                cmd.append(os.path.join("/","usr","sbin","flash_erase"))
                cmd.append(os.path.join("/", "dev", "mtd2"))
                cmd.append(0)
                cmd.append(16) # erase 65536 bytes e.g. 16 blocks * 4096 bytes

                exitCode, stdout, stderr = shellCommand.execute(cmd)

                # HT-413
                # switched from flashcp to cat since flashcp will erase the mtd before copy
                exitCode = os.system( "/bin/cat " + imageFilename + " > /dev/mtdblock2" )

                if exitCode == 0:
                    result = 'success'

        except IOError, exception_details:
            result = 'error: Can not open file: ' + imageFilename + ', Reason:' + exception_details[1]

    return result

def main():
    if updateSplashVersion(os.path.join("/", "uboot", "u-version.bmp")) == 'success':
        return True

    return False

if __name__ == "__main__":

    exitCode = 1

    if main():
        exitCode = 0

    sys.exit(exitCode)
