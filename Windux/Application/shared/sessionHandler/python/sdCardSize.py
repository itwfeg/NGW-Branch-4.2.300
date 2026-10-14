#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import subprocess
import platform


def getSdCardSize():

    """
    Return size of SD card (8 GB, 16 GB or 32 GB) based on the size of the main partition (in blocks).
    block size = 1024
    """

    if os.name == 'posix':

        block_size = 1024
        total = None
        sdCardSize = 0

        st = os.statvfs( '/' )
        total = ( st.f_blocks * st.f_frsize )
        
        mainSize = total / block_size
        
        #print mainSize
        
        if mainSize > 5000000 and mainSize < 8000000:
        
            # Main partition has total of 6061632 blocks
            sdCardSize = 8
        
        if mainSize > 11000000 and mainSize < 15000000:
        
            # Main partition has total of 13286512 blocks
            sdCardSize = 16

        if mainSize > 26000000 and mainSize < 30000000:
        
            # Main partition has total of 28768380 blocks
            sdCardSize = 32

        return sdCardSize

    else:

        return -1


if __name__ == '__main__':

    sdSize = getSdCardSize()

    print( sdSize )
    
    sys.exit( 0 )
