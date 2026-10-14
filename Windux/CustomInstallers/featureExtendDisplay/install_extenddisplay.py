import os
import shutil
import sys
import time
import installUtils
import messageBox
import re
import file_io
import shellCommand

PYTHON_PATH = 'C:\\Program Files\\Hobart\\Python'
sys.path.append(PYTHON_PATH)

FEATURE_NAME = 'extenddisplay'
FEATURE_TARBALL = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = 'install_' + FEATURE_NAME + '.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)

PYTHON_FILENAME = 'startupSequenceWin32.py'
VERSION_INFO = 'version.ini'
SOFTWARE_VERSION_INFO = 'SOFTWARE_VERSION_INFO'


exitCode = 0

exitCode |= installUtils.setPythonRoot()

# extract tarball
exit_code, process_id = messageBox.display('Preparing to install update for extending desktop, please wait...')

# remove setNetwork
try:
    os.remove(os.path.join("/", PYTHON_PATH, PYTHON_FILENAME))
except:
    pass

exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH,FEATURE_TARBALL), FEATURE_INSTALL_PATH)

# Copy the python file to the python directory
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH, PYTHON_FILENAME), os.path.join(PYTHON_PATH, PYTHON_FILENAME))

# Copy files containing version info
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH, VERSION_INFO), os.path.join("C:\\", "Program Files", "Hobart", VERSION_INFO))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH, SOFTWARE_VERSION_INFO), os.path.join("C:\\", "Program Files", "Hobart", SOFTWARE_VERSION_INFO))

time.sleep(10)

messageBox.destroy(process_id)

exit_code, process_id = messageBox.display('Installation is complete, rebooting scale, remove thumb drive.', timer=10)

time.sleep(10)

# remove feature files
exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)

# reboot

cmd = []
cmd.append("shutdown")
cmd.append("/r")
cmd.append("/t")
cmd.append("0")
exitCode, stdout, stderr = shellCommand.execute(cmd)

sys.exit(0)

