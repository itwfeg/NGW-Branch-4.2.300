#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import re
import file_io
import os
import string

#----------------------------------------------------------------------------
# Used during the build process to update BuildNumber.cpp with the releasetag
#----------------------------------------------------------------------------

def updateBuildNumber( pathToBuildNumberCppSource, releaseTag ):

    buildNumberSourceLine = re.compile('^const std::string BuildNumber::buildNumber = ')

    status, data = file_io.readFromFile( pathToBuildNumberCppSource )
    
    if status == 'success':
        
        newLines = []
        
        for i in range( 0, len( data ) ):

            if buildNumberSourceLine.match( data[i] ):
                newLines.append('const std::string BuildNumber::buildNumber = "htx-%s";' % ( releaseTag ) )
            else:
                newLines.append( data[i] )

        status, data = file_io.writeToFile( pathToBuildNumberCppSource, newLines )

        if status != 'success':
            return ('error', None)
        else:
            return ('success', None)
    else:
        return ('error', None)

if __name__ == '__main__':
    status, data = updateBuildNumber( sys.argv[1], sys.argv[2] )
    if status == 'error':
        sys.exit(1)
    else:
        sys.exit(0)
