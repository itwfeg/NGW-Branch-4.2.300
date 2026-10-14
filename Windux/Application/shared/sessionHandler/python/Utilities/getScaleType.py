#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import platform

def getScaleType():
    type = 'unknown'
    if  platform.system() == 'Linux':
        #check to see if scale type file is present
        if os.path.exists(os.path.join("/", "etc", "hobart",  "scale-type.dat")):
            file= open(os.path.join("/","etc", "hobart",  "scale-type.dat"), 'rb')
            scaleType = file.read().decode('utf-8').rstrip()
            file.close()

    return(scaleType)

if __name__=='__main__':
    print((getScaleType()))
    sys.exit(0)
