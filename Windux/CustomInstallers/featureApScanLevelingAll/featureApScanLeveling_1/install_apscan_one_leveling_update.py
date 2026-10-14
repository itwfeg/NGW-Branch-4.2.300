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

FEATURE_NAME = 'apscan_one_leveling_update'
FEATURE_TARBALL = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = 'install_' + FEATURE_NAME + '.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'
PYTHON_FILENAME = 'setNetwork.py'

# SCALEUI_BINARY = 'scaleui'                # kept for future reference
# BUSINESS_LOGIC_BINARY = 'chobart_d'       # kept for future reference

exitCode = 0

exitCode |= installUtils.setPythonRoot()

# extract tarball
exit_code, process_id = messageBox.display('Preparing to install AP Scan Leveling update feature, please wait...')

# remove setNetwork
try:
    os.remove(os.path.join("/", PYTHON_PATH, PYTHON_FILENAME))
except:
    pass

exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH,FEATURE_TARBALL), FEATURE_INSTALL_PATH)

# copy the python file to the python directory
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH, PYTHON_FILENAME), os.path.join(PYTHON_PATH, PYTHON_FILENAME))

time.sleep(10)

messageBox.destroy(process_id)
