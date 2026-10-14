#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import re
import file_io
import os

#--------------------------------------------------------------
# getSmBackendVersion.py gets the SM_BACKEND_VERSION from the
# smBackend.py file and returns the string.  SM_BACKEND_VERSION
# is in the format:
#
#   SM_BACKEND_VERSION="1.0.29"
#
# In this example, 1.0.29 would be returned (without quotes).
#
# WEH-57
# NOTE: As of 10/29/2015 smBackend.py has been moved to a directory
#       that is not listed in config.dat or config_linux.dat
#
#--------------------------------------------------------------

def getSmBackendVersion( pathToSmBackend ):

    smBackendSourceLine = re.compile('^SM_BACKEND_VERSION=')

    #status, data = file_io.readFromFile( pathToSmBackend )

    status, data = file_io.readFromFile( pathToSmBackend )

    # If we can't load from the specified path, try the default instead
    if status != 'success':
        if os.name == 'posix':
            pathToSmBackend = '/usr/local/hobart/backend/smBackend.py'
        else:
            pathToSmBackend = "C:\\Program Files\\Hobart\\Backend\\smBackend.py"
        status, data = file_io.readFromFile( pathToSmBackend )

    if status == 'success':
        
        for i in range( 0, len( data ) ):

            if smBackendSourceLine.match( data[i] ):
                
                strData = data[i].strip()
                
                # Get data after the equal sign
                strData = strData.split( "=" )
                strBackendVer = strData[1]
                
                # Remove leading and trailing double quotes
                strBackendVer = strBackendVer[1:-1]
                print(strBackendVer)

        if status != 'success':
            return ('error', None)
        else:
            return ('success', None)
    else:
        # It's not anywhere
        return ('error', None)

if __name__ == '__main__':

    status, data = getSmBackendVersion( sys.argv[1] )
    
    if status == 'error':
        sys.exit(1)
    else:
        sys.exit(0)
