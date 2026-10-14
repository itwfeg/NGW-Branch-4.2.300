#!/usr/bin/env python3

import os
import string
import file_io
import re
import sys
import hteVersionInfo


# replace version number in NSIS script files
def updateNSISwithNewVersion(productName, filename):
    version = '0.0.0'
    productVersionLine = re.compile('^!define PRODUCT_VERSION \"[0-9]+.[0-9]+.[0-9]+\"$')
    status, data = file_io.readFromFile(filename)
    if status == 'success':
        newLines = []
        for i in range(0, len(data)):
            if productVersionLine.match(data[i]):
                if productName == 'scale_management':
                    version = hteVersionInfo.SCALE_MANAGEMENT_VERSION
                elif productName == 'remote_scale_access':
                    version = hteVersionInfo.REMOTE_SCALE_ACCESS_VERSION
                elif productName == 'license_key_generator':
                    version = hteVersionInfo.LICENSE_KEY_GENERATOR_VERSION
                elif productName == 'usb_key_creator':
                    version = hteVersionInfo.USB_KEY_CREATOR_VERSION

                newLines.append('!define PRODUCT_VERSION "%s"' % version)
            else:
                newLines.append(data[i])

        status, data = file_io.writeToFile(filename, newLines)
        if status != 'success':
            return 'error', None
        else:
            return 'success', None
    else:
        return 'error', None


if __name__ == '__main__':
    status, data = updateNSISwithNewVersion(sys.argv[1], sys.argv[2])
    if status == 'error':
        sys.exit(1)
    else:
        sys.exit(0)
