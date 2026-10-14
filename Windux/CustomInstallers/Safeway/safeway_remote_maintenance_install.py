import os
import sys
import configInfo
import installUtils
import messageBox
import glob

TESTING = 0

WORKING_PATH = os.path.join(configInfo.getRootFolder(), "usr", "local", "hobart")

PYTHON_PATH = os.path.join(WORKING_PATH, "python")
sys.path.append(PYTHON_PATH)

FEATURE_NAME = 'safeway_remote_maintenance'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_HANDLER_NAME = FEATURE_NAME + '_setup'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'
INSTALLER_FILE = 'install.py'

exitCode = 0

errorLog = open( "/var/log/safeway-log.txt",  "wb")

exitCode |= installUtils.setPythonRoot()

# extract tarball
exit_code, process_id = messageBox.display('Preparing to install the\nSafeway Remote Maintenance feature.\n\nPlease wait...', timer=5)
exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH,FEATURE_FILENAME), FEATURE_INSTALL_PATH)
errorLog.write("extractTarballAsIs returned: " + str(exitCode) + "\n")

# We need to run the script that will modify configc.dat

import safeway_remote_maintenance_setup

if TESTING == 1:
    print 'About to run: safeway_remote_maintenance.update_configc()'

safeway_remote_maintenance_setup.update_configc()

if TESTING == 1:
    print 'Done with: safeway_remote_maintenance.update_configc()'

import createSafewayUser

if TESTING == 1:
    print 'Creating a user for Safeway'

createSafewayUser.createSafewayUser()
errorLog.write("Executed createSafewayUser()" +"\n")

messageBox.destroy(process_id)

# Done. Now remove the python files we no longer need, keeping the feature tarball and install script.

if TESTING == 0:
    filesToRemove = glob.glob(os.path.join(FEATURE_INSTALL_PATH, "*.py*"))
    keepFile = os.path.join(FEATURE_INSTALL_PATH, INSTALLER_FILE)

    for curFile in filesToRemove:
        # Remove the python files except install.py
        try:
            if curFile != keepFile:
                #errorLog.write("Removing: " + curFile + "\n")
                os.remove(curFile)
            #else:
            #    errorLog.write("Keeping: " + curFile + "\n")
        except:
            pass

exit_code, process_id = messageBox.display('Installation is complete. About to reboot the scale.\n\nPlease remove the thumb drive.', timer=10)

if TESTING == 0:
    # sync filesystem and reboot
    os.system("/bin/sync")
    os.system("/sbin/reboot")

sys.exit