#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# this script does nothing?

import os
import sys
import shellCommand
import time
import string

def testCanbus():
    exitCode = 1
    if os.name == "posix":
        return exitCode
    # Windows
    elif os.name == "nt":
        return exitCode

if __name__ == "__main__":
    exitCode = testCanbus()
    sys.exit(exitCode)
