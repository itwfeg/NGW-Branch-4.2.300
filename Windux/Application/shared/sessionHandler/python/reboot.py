#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : January 3, 2013
##  OVERVIEW: This script reboots the scale.
##  REQS    : root authority
##  RETURNS : none
######################################################

# Preprocessor directives
##########################
import sys
import os
import shellCommand


def reboot():
    cmd = []

    if os.name == "posix":
        # LINUX OS
        os.system("/sbin/init 6")
        exitCode = 0

    else:

        # Windows OS
        cmd.append("shutdown")
        cmd.append("/r")
        cmd.append("/t")
        cmd.append("0")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    return exitCode

if __name__ == '__main__':
    exitCode = reboot()
    sys.exit(exitCode)
