import os
import sys
import platform
import tarfile
sys.path.append('../../Windux/Application/shared/sessionHandler/python/')
import buildFeature

FEATURE_NAME = 'extenddisplay'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = 'install_' + FEATURE_NAME + '.py'
DEVICE = 'htx'
SUPPORTED_SYSTEM = 'win'

PYTHON_FILENAME = 'startupSequenceWin32.py'
VERSION_INFO = "version.ini"
SOFTWARE_VERSION_INFO = 'SOFTWARE_VERSION_INFO'


def build_tarball():
    with tarfile.open(FEATURE_FILENAME, 'w:gz') as feature_tarball:
        feature_tarball.add(PYTHON_FILENAME)
        feature_tarball.add(VERSION_INFO)
        feature_tarball.add(SOFTWARE_VERSION_INFO)

def main():
    release_tag = '1.18.0-03282016_140000'

    build_tarball()
    buildFeature.buildFeature(release_tag, FEATURE_NAME, FEATURE_FILENAME, FEATURE_INSTALLER_NAME, DEVICE, SUPPORTED_SYSTEM)

if __name__ == '__main__':
    main()
