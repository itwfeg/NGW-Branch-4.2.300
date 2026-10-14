#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys


def isArkBoard():

    rval = 0

    if os.name == "posix":

        board = "unknown"

        if 0 == os.system('grep -q "imx6q-ark" /proc/device-tree/compatible'):
            board = "ark"
        elif 0 == os.system('grep -q "imx6dl-ark" /proc/device-tree/compatible'):
            board = "ark"

        if board == "ark":
            rval = 1
            #print "True"

    return rval


def isComboBoard():

    rval = 0

    if os.name == "posix":

        board = "unknown"

        if 0 == os.system('grep -q "imx6q-itw" /proc/device-tree/compatible'):
            board = "combo"
        elif 0 == os.system('grep -q "imx6dl-itw" /proc/device-tree/compatible'):
            board = "combo"

        if board == "combo":
            rval = 1
            #print "True"

    return rval


def isBcmBoard():

    rval = 0

    if os.name == "posix":

        board = "unknown"

        if 0 == os.system('grep -q "imx6q-hb6mx" /proc/device-tree/compatible'):
            board = "bcm"
        elif 0 == os.system('grep -q "imx6dl-hb6mx" /proc/device-tree/compatible'):
            board = "bcm"
            
        if board == "bcm":
            rval = 1
            #print "True"

    return rval


if __name__ == '__main__':

    isComboBoardValue = isComboBoard()

    isBcmBoardValue = isBcmBoard()

    isArkBoardValue = isArkBoard()

    print((str( isComboBoardValue ) + ',' + str( isBcmBoardValue ) + ',' + str( isArkBoardValue )))

    sys.exit(0)
 
