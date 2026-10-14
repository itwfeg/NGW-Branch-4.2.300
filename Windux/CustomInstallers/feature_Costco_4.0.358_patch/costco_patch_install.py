import os
import shutil
import sys
import installUtils
import installUpgrade
import messageBox
PYTHON_PATH = '/usr/local/hobart/python'
sys.path.append(PYTHON_PATH)
import getModelName
import time

FEATURE_NAME = 'costco_patch'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

# feature file stuff
#BUSINESS_LOGIC_BINARY = 'chobart_d'
SCALE_USER_INTERFACE = 'scaleui'
VERSION_INFO = 'version.ini'
SOFTWARE_VERSION_INFO = 'SOFTWARE_VERSION_INFO'
#CONFIG_LINUX = 'config_linux.dat'

   
# get scale firmware version 
def getScaleVersion():
    myfile = open("//SOFTWARE_VERSION_INFO","r")
    version = myfile.readline().split(":")
    return version[1].strip()


def getBuildDate():
    myfile = open("//SOFTWARE_VERSION_INFO","r")
    myfile.readline()
    version = myfile.readline().split(":")
    return version[1].strip()

# --- starting ---
    
exitCode = 0

exitCode |= installUtils.setPythonRoot()

# make sure the scale is at least 4.0.0 to install on top of it. 
scaleVersion = getScaleVersion()
buildDate = getBuildDate();
print "detected scale version: " + scaleVersion
if not buildDate.startswith('03/22/2021'):
    exit_code, process_id = messageBox.display('No need to update the device.\nThis upgrade is only installed on versions build on 03/22/2021', timer=8)
    exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)
    exitCode = -1
    sys.exit(1)


print "starting installation"
exit_code, process_id = messageBox.display('Preparing firmware upgrade for 4.0.358,\nplease wait...')
exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH, FEATURE_FILENAME), FEATURE_INSTALL_PATH)
messageBox.destroy(process_id)


# ------------------------------
# - Install scale app
# ------------------------------
print "installing scale app"
exit_code, process_id = messageBox.display('Stopping scale application, wait...')
os.system("/usr/bin/kill -s TERM $(/usr/bin/pgrep %s)" % (SCALE_USER_INTERFACE))
#time.sleep(10)
#os.system("/usr/bin/kill -9 $(/usr/bin/pgrep %s)" % (BUSINESS_LOGIC_BINARY))

try:
    os.remove(os.path.join("/", "usr", "bin", SCALE_USER_INTERFACE))
except:
    pass

print "setting permissions" 
# copy chobart_d to /usr/bin
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,SCALE_USER_INTERFACE), os.path.join("/", "usr", "bin", SCALE_USER_INTERFACE))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,VERSION_INFO), os.path.join("/", "etc", "hobart", VERSION_INFO))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,SOFTWARE_VERSION_INFO), os.path.join("/", SOFTWARE_VERSION_INFO))

# chmod of the scaleui and chobart_d to executable
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "bin", SCALE_USER_INTERFACE)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "etc", "hobart", VERSION_INFO)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", SOFTWARE_VERSION_INFO)))

messageBox.destroy(process_id)

exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)

print "done" 
exit_code, process_id = messageBox.display('Instalation completed, rebooting scale...', timer=5)

# sync filesystem and reboot
os.system("/bin/sync")
os.system("/sbin/reboot")

sys.exit(0)

