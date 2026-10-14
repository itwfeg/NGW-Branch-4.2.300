#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# is this used anywhere? it's a lot of lines just to call 'sync'

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : September 9, 2013
##  OVERVIEW: This script reports the macAddress of the scale.
##  REQS    : None
##  RETURNS : Qmx or Qmt based on os.name
######################################################

# Preprocessor directives
##########################
import sys
import os
import shellCommand

#Sync under Linux syncs the file system in RAM to to the physical device the filesystem is mounted on. This is critical in a system where power can be pulled at any time and no power holdup to flush the ram to physical disk.
#
#Under Windows we need ntfs to flush its buffers at prescribed intervals to SATA DOM. Sprinkling syncs everywhere is not ideal but in most cases the only writes are to sqlite db or during software upgrade. We do not want every write forced to device as oppsed to RAM unless perfoance is good. Linux and uSd and every write flushed is a performance nightmare.
#
#We need windows or dom to flush filesytem RAM and need file based write filter. Do not necessarily need 
#Syncs everywhere so this may not need ported except for removable devices. There is no remove drive safely for our application you must sync.
#
#Power can be lost at any time so devise the HTx to handle it.

def sync():
    if os.name == 'posix':
        # LINUX OS
        cmd = []
        cmd.append("sync")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
    else:
        # WINDOWS OS
        exitCode = 0
    return (exitCode)

if __name__=='__main__':
    exitCode = sync()
    sys.exit(exitCode)

