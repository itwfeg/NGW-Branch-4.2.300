import os
import shutil
import sys
import time
import installUtils
import messageBox
import re
import file_io

PYTHON_PATH = '/usr/local/hobart/python'
sys.path.append(PYTHON_PATH)

FEATURE_NAME = 'turn_off_printer_debug'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = 'install_' + FEATURE_NAME + '.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

CONFIG_DEBUG = 'configdebug.dat'

exitCode = 0

exitCode |= installUtils.setPythonRoot()

# extract tarball
exit_code, process_id = messageBox.display('Preparing to install Disable Debug feature, please wait...', timer=5)
exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH,FEATURE_FILENAME), FEATURE_INSTALL_PATH)
messageBox.destroy(process_id)

# Copy configdebug.dat
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,CONFIG_DEBUG), os.path.join("/", "etc", "hobart", CONFIG_DEBUG))


# Change permissions of the files that were copied to the scale
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "etc", "hobart", CONFIG_DEBUG)))


exit_code, process_id = messageBox.display('Installation is complete, remove thumb drive.', timer=5)
messageBox.destroy(process_id)


# more feature files
exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)

# sync filesystem
os.system("/bin/sync")

sys.exit(0)
