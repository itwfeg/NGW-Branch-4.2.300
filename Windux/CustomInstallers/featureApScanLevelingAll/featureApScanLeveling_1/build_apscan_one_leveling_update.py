import os
import sys
import platform
import tarfile
import buildFeature

FEATURE_NAME = 'apscan_one_leveling_update'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = 'install_' + FEATURE_NAME + '.py'
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'
PYTHON_FILENAME = 'setNetwork.py'

#BUSINESS_LOGIC_BINARY = 'chobart_d'                           # wanted to leave for future reference
#VERSION_INFO = "version.ini"                                  # wanted to leave for future reference
#SOFTWARE_VERSION_INFO = 'SOFTWARE_VERSION_INFO'               # wanted to leave for future reference
#SCALEUI_BINARY = 'scaleui'                                    # wanted to leave for future reference

def build_tarball():
    with tarfile.open(FEATURE_FILENAME, 'w:gz') as feature_tarball:
        feature_tarball.add(PYTHON_FILENAME)
        #feature_tarball.add(SCALEUI_BINARY)        # wanted to leave this in here for future reference

def main():
    release_tag = '2.1.201-01142015_023308'

    build_tarball()
    buildFeature.buildFeature(release_tag, FEATURE_NAME, FEATURE_FILENAME, FEATURE_INSTALLER_NAME, DEVICE, SUPPORTED_SYSTEM)

if __name__ == '__main__':
    main()
