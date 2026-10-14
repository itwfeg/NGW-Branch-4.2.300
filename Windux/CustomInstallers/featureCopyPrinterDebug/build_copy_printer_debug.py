import os
import sys
import platform
import tarfile
sys.path.append('../../Application/shared/sessionHandler/python/')
PYTHON_PATH = '../../Application/shared/sessionHandler/python/'
sys.path.append(PYTHON_PATH)
import file_io
import buildFeature


FEATURE_NAME = 'copy_printer_debug'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = 'install_' + FEATURE_NAME + '.py'
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'


def build_tarball():

    with tarfile.open( FEATURE_FILENAME, 'w:gz' ) as feature_tarball:
        pass

def main():
    release_tag = '2.3.2-04212016_142329'

    build_tarball()
    buildFeature.buildFeature(release_tag, FEATURE_NAME, FEATURE_FILENAME, FEATURE_INSTALLER_NAME, DEVICE, SUPPORTED_SYSTEM)

if __name__ == '__main__':
    main()
