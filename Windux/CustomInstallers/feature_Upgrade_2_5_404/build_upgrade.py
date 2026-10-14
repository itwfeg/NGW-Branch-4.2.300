import os
import sys
import platform
import tarfile
sys.path.append('/home/hobart/Source/Hobart/Fastrack/branches/branch-2.1.0-safeway/Windux/Application/shared/sessionHandler/python/')
import buildFeature

FEATURE_NAME = 'upgrade'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

#ADDED FOR 2.5.404
BUSINESS_LOGIC_BINARY = 'chobart_d'
SCALEUI_BINARY = 'scaleui'
VERSION_INFO = "version.ini"
SOFTWARE_VERSION_INFO = 'SOFTWARE_VERSION_INFO'
#ADDED FOR 2.5.404 ^^^^^^^^^^

def build_tarball():
    with tarfile.open(FEATURE_FILENAME, 'w:gz') as feature_tarball:
        #ADDED FOR 2.5.404
	feature_tarball.add( BUSINESS_LOGIC_BINARY )
        feature_tarball.add( SCALEUI_BINARY )
        feature_tarball.add( VERSION_INFO )
        feature_tarball.add( SOFTWARE_VERSION_INFO )
	#ADDED FOR 2.5.404 ^^^^^^^^^^

def main():
    release_tag = '2.5.404-08022016_143017'

    build_tarball()
    buildFeature.buildFeature(release_tag, FEATURE_NAME, FEATURE_FILENAME, FEATURE_INSTALLER_NAME, DEVICE, SUPPORTED_SYSTEM)

if __name__ == '__main__':
    main()
