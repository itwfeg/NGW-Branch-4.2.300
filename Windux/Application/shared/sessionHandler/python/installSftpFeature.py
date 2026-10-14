#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import sys
import shutil
import os

# NOTE:
# Must have python version 2.7 or greater to run this script
# engdev1 has version 2.6 so this will be issue until updated

import sftpUpdate

if __name__ == '__main__':

    import installUtils

    featureName = "sftpUpdate"
    featureFilename = "sftpUpdate.py"

    exitCode = 0

    exitCode |= installUtils.setPythonRoot()

    exitCode |= installUtils.stopApplication(featureFilename)

    filename = '/usr/local/hobart/python/sftpUpdate.py'

    os.remove(filename)

    shutil.copyfile('sftpUpdate.py', '/usr/local/hobart/python/sftpUpdate.py')

    exitCode |= installUtils.removeInstallerFiles(featureName)

    exitCode |= installUtils.startApplication(featureFilename)

    sys.exit(exitCode)

