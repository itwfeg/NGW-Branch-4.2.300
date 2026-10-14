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

FEATURE_NAME = 'globalfrequentshopperupdate'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = 'install_' + FEATURE_NAME + '.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
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

exitCode = 0

exitCode |= installUtils.setPythonRoot()

# extract tarball
exit_code, process_id = messageBox.display('Preparing to install Global Frequent Shopper update feature, please wait...')
exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH,FEATURE_FILENAME), FEATURE_INSTALL_PATH)
messageBox.destroy(process_id)

# stop chobart_d and scaleui
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

# Copy Python files
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,PYTHON_FILENAME_0), os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_0))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,PYTHON_FILENAME_1), os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_1))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,PYTHON_FILENAME_2), os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_2))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,PYTHON_FILENAME_3), os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_3))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,PYTHON_FILENAME_4), os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_4))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,PYTHON_FILENAME_5), os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_5))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,PYTHON_FILENAME_6), os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_6))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,PYTHON_FILENAME_7), os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_7))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,PYTHON_FILENAME_8), os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_8))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,PYTHON_FILENAME_9), os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_9))

# Copy new database file to the dbempty directory
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,DB_FILENAME), os.path.join("/", "usr", "local", "hobart", "db", "scale", "dbempty", DB_FILENAME))

# Copy asound.conf for video playback fix
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,ASOUND_CONF), os.path.join("/", "etc", ASOUND_CONF))

# Copy fixes for HTs not playing videos
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,VPU_FILE_D), os.path.join("/", "lib", "firmware", "vpu", VPU_FILE_D))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,VPU_FILE_Q), os.path.join("/", "lib", "firmware", "vpu", VPU_FILE_Q))


# Change permissions of the files that were copied to the scale
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "bin", BUSINESS_LOGIC_BINARY)))
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "bin", SCALEUI_BINARY)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "etc", "hobart", VERSION_INFO)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", SOFTWARE_VERSION_INFO)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_0)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_1)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_2)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_3)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_4)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_5)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_6)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_7)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_8)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "python", PYTHON_FILENAME_9)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "usr", "local", "hobart", "db", "scale", "dbempty", DB_FILENAME)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "etc", ASOUND_CONF)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "lib", "firmware", "vpu", VPU_FILE_D)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "lib", "firmware", "vpu", VPU_FILE_Q)))

messageBox.destroy(process_id)

exit_code, process_id = messageBox.display('Installation is complete, rebooting scale, remove thumb drive.', timer=5)

# more feature files
exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)

# sync filesystem and reboot
os.system("/bin/sync")
os.system("/sbin/reboot")

sys.exit(0)
