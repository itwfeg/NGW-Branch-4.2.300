#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import re
import file_io
import os
import string
import glob
import zipfile

#-----------------------------------------------------------------------
# Used during the build process to update version.ini with
# the dependency and version from each peripheral file
#-----------------------------------------------------------------------

def updateFirmwareVersionINI( pathToVersionINI, pathToFirmwareFiles ):

    status, fileData = file_io.readFromFile( pathToVersionINI )

    if status == 'success':
        versionIniComponentLine = re.compile('^component')
        versionText = re.compile('version')
        vf = "version.ini"
        Vf = "Version.ini"

        # Copy version.ini to versionList.dat
        # Get a list of all the ZIP files in the pathToFirmwareFiles

        # For each file - unzip and get the version component
        for filename in glob.glob(os.path.join(pathToFirmwareFiles, "*.zip")):
            zf = zipfile.ZipFile(filename, 'r')

#            print 'Unzipping ' + filename
#            print zf.namelist()
#            print '--------'

            for name in zf.namelist():
                if ((name == vf) or (name == Vf)):
#                    print 'Processing file [' + name + ']'

                    zf.extract(name, ".")
                    zf.close()

                    status, data = file_io.readFromFile( os.path.join('.', name) )
                    if status == 'success':

                        for i in range( 0, len( data ) ):
                            if versionIniComponentLine.match( data[i] ):
                                fileData.append( data[i] )
                            else:
                                print(('[' + data[i] + '] does not start with [component]'))

#                            if versionText.match( data[i] ):
#                                print data[i] + ' does match'

                    os.remove( os.path.join('.', name) )

        status, data = file_io.writeToFile( pathToVersionINI, fileData )
        if status != 'success':
            print('error')
#        else:
#            print 'success'

        return ('Done', None)
    else:
        return ('error', None)


if __name__ == '__main__':
    status, data = updateFirmwareVersionINI( sys.argv[1], sys.argv[2] )
    if status == 'error':
        sys.exit(1)
    else:
        sys.exit(0)
        
