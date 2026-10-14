# build feature file for NGW Weigher Firmware Upgrade

import sys
sys.path.append('/home/hobart/projects/branch-4.1.0/Windux/Application/shared/sessionHandler/python/')
import argparse
import tarfile
import buildFeature
import time
import os

FEATURE_NAME = 'weigher_upgrade'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

BUSINESS_LOGIC_BINARY = 'chobart_d'
ZIP_FILE = 'XPEWeigher.zip'
CONFIG_LINUX_FILE='config_linux.dat'
DEBUG_FILE='configdebug.dat'

RELEASE_NUMBER = '1.5.0'

def get_time_stamp():
    return time.strftime("-%y%m%d")

def build_tarball():
    with tarfile.open(FEATURE_FILENAME, 'w:gz') as feature_tarball:
        feature_tarball.add(BUSINESS_LOGIC_BINARY)
        feature_tarball.add(ZIP_FILE)
        feature_tarball.add(CONFIG_LINUX_FILE)
        feature_tarball.add(DEBUG_FILE)

def main():
    release_tag = RELEASE_NUMBER + get_time_stamp()

    build_tarball()
    buildFeature.buildFeature(release_tag, FEATURE_NAME, FEATURE_FILENAME, FEATURE_INSTALLER_NAME, DEVICE, SUPPORTED_SYSTEM)


if __name__ == '__main__':
    main()
