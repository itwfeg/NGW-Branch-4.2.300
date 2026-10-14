import os
import sys
import platform

# This setting points us to the path where our current Python scripts are so we can build the tarball.
# Modify it, depending on where your code is located.
if platform.system() == 'Linux':
    PYTHON_PATH = '/usr/local/hobart/python'
else:
    PYTHON_PATH = r'B:\Projects\FasTrack\branch-2.1.0-safeway\Windux\Application\shared\sessionHandler\python'
sys.path.append(PYTHON_PATH)

#print sys.path

# Now we can import our local scripts
import installUtils
import tarfile
import buildFeature


FEATURE_NAME = 'safeway_remote_maintenance'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

# This is the script we will eventually run to modify configc.dat
BUSINESS_LOGIC_SCRIPT = FEATURE_NAME + '_setup.py'

# This script will create the user for Safeway access.
USER_CREATE_SCRIPT = 'createSafewayUser.py'

def build_tarball():
    try:
        with tarfile.open(FEATURE_FILENAME, 'w:gz') as feature_tarball:
            feature_tarball.add(BUSINESS_LOGIC_SCRIPT)
            feature_tarball.add(USER_CREATE_SCRIPT)
    except IOError:
        print '********'
        print '!!!!!!!!    Unable to open/create tarfile: ' + FEATURE_FILENAME + '.  Do you have it open in 7-Zip???'
        print '********'

def main():
    release_tag = '2.2.202-12022015_000000'     # I made this one up
    #print 'release_tag: ' + release_tag

    build_tarball()
    buildFeature.buildFeature(release_tag, FEATURE_NAME, FEATURE_FILENAME, FEATURE_INSTALLER_NAME, DEVICE, SUPPORTED_SYSTEM)

if __name__ == '__main__':
    main()
