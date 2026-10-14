import os
import sys
import platform
import tarfile
sys.path.append('../../Application/shared/sessionHandler/python/')
PYTHON_PATH = '../../Application/shared/sessionHandler/python/'
sys.path.append(PYTHON_PATH)
import file_io
import buildFeature




FEATURE_NAME = 'globalfrequentshopperupdate'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = 'install_' + FEATURE_NAME + '.py'
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

BUSINESS_LOGIC_BINARY = 'chobart_d'
SCALEUI_BINARY = 'scaleui'
VERSION_INFO = "version.ini"
SOFTWARE_VERSION_INFO = 'SOFTWARE_VERSION_INFO'
PYTHON_FILENAME_0 = 'connectNetworkStartupNew.py'
PYTHON_FILENAME_1 = 'encryptDecrypt.py'
PYTHON_FILENAME_2 = 'killStartupNetworking.py'
PYTHON_FILENAME_3 = 'moduleFinder.py'
PYTHON_FILENAME_4 = 'processDeviceEvents.py'
PYTHON_FILENAME_5 = 'saveConfigs.py'
PYTHON_FILENAME_6 = 'setNetwork.py'
PYTHON_FILENAME_7 = 'startupSequence.py'
PYTHON_FILENAME_8 = 'updateCrontab.py'
PYTHON_FILENAME_9 = 'updateTimeNTP.py'
DB_FILENAME = 'dbftl.db'
ASOUND_CONF = 'asound.conf'
VPU_FILE_D = 'vpu_fw_imx6d.bin'
VPU_FILE_Q = 'vpu_fw_imx6q.bin'


def build_tarball():
    with tarfile.open( FEATURE_FILENAME, 'w:gz' ) as feature_tarball:
        feature_tarball.add( BUSINESS_LOGIC_BINARY )
        feature_tarball.add( SCALEUI_BINARY )
        feature_tarball.add( VERSION_INFO )
        feature_tarball.add( SOFTWARE_VERSION_INFO )
        feature_tarball.add( PYTHON_FILENAME_0 )
        feature_tarball.add( PYTHON_FILENAME_1 )
        feature_tarball.add( PYTHON_FILENAME_2 )
        feature_tarball.add( PYTHON_FILENAME_3 )
        feature_tarball.add( PYTHON_FILENAME_4 )
        feature_tarball.add( PYTHON_FILENAME_5 )
        feature_tarball.add( PYTHON_FILENAME_6 )
        feature_tarball.add( PYTHON_FILENAME_7 )
        feature_tarball.add( PYTHON_FILENAME_8 )
        feature_tarball.add( PYTHON_FILENAME_9 )
        feature_tarball.add( DB_FILENAME )
        feature_tarball.add( ASOUND_CONF )
        feature_tarball.add( VPU_FILE_D )
        feature_tarball.add( VPU_FILE_Q )

ASOUND_CONF

def main():
    release_tag = '2.3.2-04212016_142329'

    build_tarball()
    buildFeature.buildFeature(release_tag, FEATURE_NAME, FEATURE_FILENAME, FEATURE_INSTALLER_NAME, DEVICE, SUPPORTED_SYSTEM)

if __name__ == '__main__':
    main()
