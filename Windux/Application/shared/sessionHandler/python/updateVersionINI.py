#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import re
import file_io
import os
import string

#-----------------------------------------------------------------------
# Used during the build process to update version.ini with the releaseid
#-----------------------------------------------------------------------

def updateVersionINI( pathToVersionINI, releaseID ):

    versionIniSourceLine = re.compile('^component\.2\.version = ')

    status, data = file_io.readFromFile( pathToVersionINI )
    
    if status == 'success':
        
        newLines = []
        
        for i in range( 0, len( data ) ):

            if versionIniSourceLine.match( data[i] ):
                newLines.append('component.2.version = 2.%s' % ( releaseID ) )
            else:
                newLines.append( data[i] )

        status, data = file_io.writeToFile( pathToVersionINI, newLines )

        if status != 'success':
            return ('error', None)
        else:
            return ('success', None)
    else:
        return ('error', None)

if __name__ == '__main__':
    status, data = updateVersionINI( sys.argv[1], sys.argv[2] )
    if status == 'error':
        sys.exit(1)
    else:
        sys.exit(0)
        
