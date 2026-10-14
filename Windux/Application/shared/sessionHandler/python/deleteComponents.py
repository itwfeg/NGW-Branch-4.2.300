#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import sys
import glob
import string
import shutil


def delete_components():

    if os.name == 'posix':
        iniFileLocation = '/etc/hobart/version.ini'
    elif os.name == 'nt':
        iniFileLocation = 'C:\\Program Files\\Hobart\\version.ini'

    valid_components = ['component.2.dependency',  'component.7.dependency',  'component.7.version',  'component.8.dependency',  'component.8.version',  'component.2.version']

    try:
        # open the file to read it
        f = open(iniFileLocation,  'r' )

        # read all of the lines of components
        ini_lines = f.readlines()

        # close the file
        f.close()
    except:
        print("Failed to read from %s" % iniFileLocation)

    try:
        # now open the file back up to write to it
        f = open(iniFileLocation,  'w')

        # if the read line is a valid component then write it back to the file
        for component in ini_lines:
            split_on_equals = component.split('=')
            base_component = split_on_equals[0].strip()
            #print base_component

            if base_component in valid_components:
                f.write(bytes(component, 'utf-8'))

        f.close()

    except:
        f.close()

    return 0

if __name__=='__main__':

    exitCode = 1

    exitCode = delete_components()

    sys.exit(exitCode)
