import os
import osUtils
import shutil
import sys


def main():
    if osUtils.is64BitPython():
        sys.exit(0)

    certificate_filepath = 'C:\\Python27\\Lib\\site-packages\\requests\\cacert.pem'
    destination_filepath = '.\\dist\\usbKeyUpdater\\requests\\cacert.pem'
    os.makedirs('.\\dist\\usbKeyUpdater\\requests\\')
    shutil.copyfile(certificate_filepath, destination_filepath)


if __name__ == '__main__':
    main()
