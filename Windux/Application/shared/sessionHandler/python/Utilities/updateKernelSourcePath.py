#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import re
import file_io
import os

def updateKernelSourcePath(buildPath):
    if os.path.basename(buildPath).lower() == "windux":
        # remove the Windux component which is working directory for build system
        # since we need to use OS tree
        buildPath = os.path.dirname(buildPath)
    kernelSourcePathLine = re.compile('^TSWO_KERNEL_EXTERNAL_SRC="[^"]*"$')
    filename = os.path.join(buildPath, "Os", "linux", "embedded", "factory", "hobart", ".config")
    newKernelPath = os.path.join(buildPath, "Os", "linux", "embedded", "kernel-source")
    status, data = file_io.readFromFile(filename)
    if status == 'success':
        newLines = []
        for i in range(0, len(data)):
            if kernelSourcePathLine.match(data[i]):
                newLines.append('TSWO_KERNEL_EXTERNAL_SRC="%s"' % (newKernelPath))
            else:
                newLines.append(data[i])

        status, data = file_io.writeToFile(filename, newLines)
        if status != 'success':
            return ('error', None)
        else:
            return ('success', None)
    else:
        return ('error', None)

if __name__ == '__main__':
    status, data = updateKernelSourcePath(sys.argv[1])
    if status == 'error':
        sys.exit(1)
    else:
        sys.exit(0)
