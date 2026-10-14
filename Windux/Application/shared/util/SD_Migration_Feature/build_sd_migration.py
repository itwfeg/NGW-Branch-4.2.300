import tarfile
import time
import buildFeature
import moduleFinder
import shutil
import os
from optparse import OptionParser

parser = OptionParser(usage="usage: %prog [options] filename",
                      version="%prog 1.0")

FEATURE_NAME = 'sd_migration'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'
RELEASE = '3.1.0'

UBOOT_3_3_0 = "hti-linux-uboot-3.1.0-11222017_090257.package.tgz"
STARTUP_SEQUENCE = 'startupSequence.py'
MY_QUESTION_BOX = 'MyQuestionBox.py'
EXPORT_DIR = "Modules"

def build_tarball():
    with tarfile.open(FEATURE_FILENAME, 'w:gz') as feature_tarball:
        #feature_tarball.add(UBOOT_3_3_0)
        feature_tarball.add(STARTUP_SEQUENCE)
        feature_tarball.add(MY_QUESTION_BOX)

def get_time_stamp():
     timestamp =time.strftime("-%m%d%y_%I%M00")
     return timestamp


def main(nz):
    release_tag = RELEASE + get_time_stamp()

    build_tarball()
    if(nz):
        try:
            shutil.rmtree(EXPORT_DIR)
        except:
            pass
        os.makedirs(EXPORT_DIR)
        mod_findr = moduleFinder.ModuleHandler(FEATURE_INSTALLER_NAME)
        for module in mod_findr.getModules():
            shutil.copy(module, os.path.abspath(EXPORT_DIR))
        shutil.copyfile(FEATURE_INSTALLER_NAME, os.path.join(EXPORT_DIR, 'install.py'))
        shutil.copy(FEATURE_FILENAME, os.path.abspath(EXPORT_DIR))

    else:
        buildFeature.buildFeature(release_tag, FEATURE_NAME, FEATURE_FILENAME, FEATURE_INSTALLER_NAME, DEVICE, SUPPORTED_SYSTEM)


if __name__ == '__main__':
    # -n copies all of the dependencies into the Modules Dir
    parser.add_option('-n', '--nozip', action="store_true", dest="nz", help="Does not create a tar zip")
    options, args = parser.parse_args()
    main(options.nz)
