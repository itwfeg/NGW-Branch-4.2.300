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

FEATURE_NAME = 'copy_printer_debug'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = 'install_' + FEATURE_NAME + '.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

exportDevicePath = os.path.join(os.sep, "media", "thumb")

exitCode = 0

exitCode |= installUtils.setPythonRoot()

# extract tarball
exit_code, process_id = messageBox.display('Preparing to copy application debug log, please wait...', timer=5)
# No files are in the tarball for this feature ...
#exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH,FEATURE_FILENAME), FEATURE_INSTALL_PATH)
messageBox.destroy(process_id)

# Copy debug.log
shutil.copyfile(os.path.join("/", "usr", "local", "hobart", "debug.log"), os.path.join(exportDevicePath, "debug.log"))


# sync filesystem
os.system("/bin/sync")

exit_code, process_id = messageBox.display('Copy is complete, remove thumb drive.', timer=5)
messageBox.destroy(process_id)

# more feature files
exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)

sys.exit(0)

