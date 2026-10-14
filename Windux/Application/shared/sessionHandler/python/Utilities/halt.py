#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################

# Preprocessor directives
##########################
import sys
import os
import shellCommand

def shutdown():
    cmd = []

    if os.name == "posix":
        # LINUX OS
        cmd.append("/sbin/shutdown")
        cmd.append("-h")
        cmd.append("now")
        
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    else:

        # Windows OS
        cmd.append("shutdown")
        cmd.append("/t")
        cmd.append("0")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    return exitCode

if __name__ == '__main__':
    exitCode = shutdown()
    sys.exit(exitCode)
