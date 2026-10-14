# Build feature upgrade file for NGW 2.13.100

import sys
sys.path.append('/home/hobart/projects/branch-2.13.0/Windux/Application/shared/sessionHandler/python/')
import argparse
import tarfile
import buildFeature
import time 
import os

FEATURE_NAME = 'ngw_wrapper_upgrade'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

BUSINESS_LOGIC_BINARY = 'chobart_d'
CONFIGC = 'config_linux.dat'
SCRIPT_FILE = 'displayWaitMsg.py'
SCRIPT_FILE2 = 'processMonitor.py'
SCRIPT_FILE3 = 'outputActuatorTimings.py'
SCRIPT_FILE4 = 'installUpgrade.py'
SCRIPT_FILE5 = 'hwRev.py'
SCRIPT_FILE6 = 'serialFlash.py'
SCRIPT_FILE7 = 'startupSequence.py'
SCRIPT_FILE8 = 'updateSplashDiagnostics.py'
SCRIPT_FILE9 = 'updateSplashImage.py'
SCRIPT_FILE10 = 'updateSplashVersion.py'
SCALEUI_BINARY = 'scaleui'
SOFTWARE_VERSION = 'SOFTWARE_VERSION_INFO'
VERSION_INFO = 'version.ini'
FF = 'htseries-any-ngw-8.77.0-release.package.tgz'

RELEASE_NUMBER = '2.13.100'

def get_time_stamp():
    return time.strftime("-20%y%m%d")

def build_tarball():
    with tarfile.open(FEATURE_FILENAME, 'w:gz') as feature_tarball:
        feature_tarball.add(BUSINESS_LOGIC_BINARY)
        feature_tarball.add(CONFIGC)
        feature_tarball.add(SCRIPT_FILE)
        feature_tarball.add(SCRIPT_FILE2)
        feature_tarball.add(SCRIPT_FILE3)
        feature_tarball.add(SCRIPT_FILE4)
        feature_tarball.add(SCRIPT_FILE5)
        feature_tarball.add(SCRIPT_FILE6)
        feature_tarball.add(SCRIPT_FILE7)
        feature_tarball.add(SCRIPT_FILE8)
        feature_tarball.add(SCRIPT_FILE9)
        feature_tarball.add(SCRIPT_FILE10)
        feature_tarball.add(SCALEUI_BINARY)
        feature_tarball.add(SOFTWARE_VERSION)
        feature_tarball.add(VERSION_INFO)
        feature_tarball.add(FF)
        

def main():
    release_tag = RELEASE_NUMBER + get_time_stamp()

    build_tarball()
    buildFeature.buildFeature(release_tag, FEATURE_NAME, FEATURE_FILENAME, FEATURE_INSTALLER_NAME, DEVICE, SUPPORTED_SYSTEM)


if __name__ == '__main__':
    main()
