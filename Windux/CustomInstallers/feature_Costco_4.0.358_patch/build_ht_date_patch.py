# build feature file for Spanish date patch 

import argparse
import tarfile
import buildFeature
import time 
import os

FEATURE_NAME = 'costco_patch'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

#BUSINESS_LOGIC_BINARY = 'chobart_d'
SCALE_USER_INTERFACE = 'scaleui'
VERSION_INFO = 'version.ini'
SOFTWARE_VERSION_INFO = 'SOFTWARE_VERSION_INFO'
#CONFIG_LINUX = 'config_linux.dat'


RELEASE_NUMBER = '4.0.358'

def get_time_stamp():
    return time.strftime("-%m%d%y")

def build_tarball():
    with tarfile.open(FEATURE_FILENAME, 'w:gz') as feature_tarball:
#        feature_tarball.add(BUSINESS_LOGIC_BINARY)
        feature_tarball.add(SCALE_USER_INTERFACE)
        feature_tarball.add(VERSION_INFO)
        feature_tarball.add(SOFTWARE_VERSION_INFO)
#        feature_tarball.add(CONFIG_LINUX)

def main():
    release_tag = RELEASE_NUMBER + get_time_stamp()

    build_tarball()
    buildFeature.buildFeature(release_tag, FEATURE_NAME, FEATURE_FILENAME, FEATURE_INSTALLER_NAME, DEVICE, SUPPORTED_SYSTEM)


if __name__ == '__main__':
    main()
