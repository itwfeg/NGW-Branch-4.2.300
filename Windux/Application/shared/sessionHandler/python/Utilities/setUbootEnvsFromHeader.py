#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import file_io
import re
import argparse
import sys

def cleanEnvLine(line):
    line = line.rstrip("\\")
    line = line.strip()
    line = line.replace('"', '')
    return line

def getUbootEnv(filename):
    exitCode = 1
    envSettingsLine = re.compile("^\s*#define\\s+CONFIG_EXTRA_ENV_SETTINGS\\s+\\\\$")

    status, data = file_io.readFromFile(filename)
    if status == 'success':
        collectData = False
        envSettings = ''
        for line in data:
            line = line.strip()
            if envSettingsLine.match(line):
                collectData = True
            elif line == '':
                if collectData:
                    break
            else:
                if collectData:
                    envSettings = envSettings + cleanEnvLine(line)

        # replace \0 which is null terminator with byte 0
        envSettings = envSettings.replace('\\0', '\0')

        # write out the environment variable settings
        status, data = file_io.writeBufferToFile('envSettings.dat', envSettings)
        if status == 'success':
            exitCode = 0
        else:
            exitCode = 1
    else:
        exitCode = 1

    return exitCode

if __name__ == '__main__':

    # NOTE:
    # This patch file must contain all of the U-boot environment variables in it and if this file name ever changes
    # we must adjust this line
    #filename="../Os/linux/embedded/factory/hobart/factory-hobart-20150130/build_armv7l-timesys-linux-gnueabihf/u-boot-2014.07/u-boot-2014.07/include/configs/hb6mxs.h"
    filename = "./hb6mxs.h"

    parser = argparse.ArgumentParser(description="extract u-boot environment settings from patch file")
    parser.add_argument("uboot_patch_file",
                        action="store",
                        default=filename,
                        nargs='?',
                        help="u-boot patch filename")
    args = parser.parse_args()

    exitCode = getUbootEnv(args.uboot_patch_file)
    sys.exit(exitCode)
