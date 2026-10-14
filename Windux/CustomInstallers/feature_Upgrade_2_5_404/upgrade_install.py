import os
import shutil
import sys
import time
import installUtils
import messageBox
import re
import file_io

PYTHON_PATH = '/usr/local/hobart/python'
SCALEUI_PATH = '/usr/bin/'
VERSION_PATH = '/etc/hobart'
WPA_PATH = '/usr/sbin/'
sys.path.append(PYTHON_PATH)

FEATURE_NAME = 'upgrade'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

#ADDED FOR 2.5.404
BUSINESS_LOGIC_BINARY = 'chobart_d'
SCALEUI_BINARY = 'scaleui'
VERSION_INFO = "version.ini"
SOFTWARE_VERSION_INFO = 'SOFTWARE_VERSION_INFO'
#ADDED FOR 2.5.404 ^^^^^^^^^^

def install_feature(feature_filename):
    install_upgrade_path = os.path.join(PYTHON_PATH, 'installUpgrade.py')
    feature_filepath = os.path.join(FEATURE_INSTALL_PATH, feature_filename)
    return os.system('/usr/bin/python {0} {1}'.format(install_upgrade_path, feature_filepath))

exitCode = 0

exitCode |= installUtils.setPythonRoot()

exit_code, process_id = messageBox.display('Preparing to install 2.5.404 upgrade files, please wait...')
exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH,FEATURE_FILENAME), FEATURE_INSTALL_PATH)
messageBox.destroy(process_id)

# ADDED FOR 2.5.404
# stop chobart_d, and scaleui
exit_code, process_id = messageBox.display('Stopping scale user interface and business logic, please wait...')
os.system("/usr/bin/kill -9 $(/usr/bin/pgrep %s)" % (SCALEUI_BINARY))
os.system("/usr/bin/kill -s TERM $(/usr/bin/pgrep %s)" % (BUSINESS_LOGIC_BINARY))
time.sleep(10)
os.system("/usr/bin/kill -9 $(/usr/bin/pgrep %s)" % (BUSINESS_LOGIC_BINARY))

# remove scaleui and chobart_d
try:
    os.remove(os.path.join("/", "usr", "bin", BUSINESS_LOGIC_BINARY))
except:
    pass
try:
    os.remove(os.path.join("/", "usr", "bin", SCALEUI_BINARY))
except:
    pass

# Copy chobart_d and scaleui to /usr/bin
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,BUSINESS_LOGIC_BINARY), os.path.join("/", "usr", "bin", BUSINESS_LOGIC_BINARY))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,SCALEUI_BINARY), os.path.join("/", "usr", "bin", SCALEUI_BINARY))

# Copy version.ini and SOFTWARE_VERSION_INFO
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,VERSION_INFO), os.path.join("/", "etc", "hobart", VERSION_INFO))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,SOFTWARE_VERSION_INFO), os.path.join("/", SOFTWARE_VERSION_INFO))

# Change permissions of the files that were copied to the scale
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "bin", BUSINESS_LOGIC_BINARY)))
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "bin", SCALEUI_BINARY)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "etc", "hobart", VERSION_INFO)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", SOFTWARE_VERSION_INFO)))

messageBox.destroy(process_id)

exit_code, process_id = messageBox.display('Installation is complete, rebooting scale, remove thumb drive.', timer=5)

# more feature files
exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)

exit_code, process_id = messageBox.display('Installation is complete.', timer=5)

# sync filesystem and reboot
os.system("/bin/sync")
os.system("/sbin/reboot")
# ADDED FOR 2.5.404 ^^^^^^^^^^

sys.exit(0)
