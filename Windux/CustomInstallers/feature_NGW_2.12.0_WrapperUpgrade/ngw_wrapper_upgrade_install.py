import os
import shutil
import sys
import installUtils
#import installUpgrade
import messageBox
PYTHON_PATH = '/usr/local/hobart/python'
sys.path.append(PYTHON_PATH)
import license
import getModelName
import time
import os.path

FEATURE_NAME = 'ngw_wrapper_upgrade'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DOWNLOAD_PATH = os.path.join('/','usr','local','hobart','download')
PENDING_PATH = os.path.join('/','usr','local','hobart','pending')
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

# feature file stuff
BUSINESS_LOGIC_BINARY = 'chobart_d'
CONFIGC = 'config_linux.dat'
SCRIPT_FILE = 'displayWaitMsg.py'
SCRIPT_FILE2 = 'processMonitor.py'
SCRIPT_FILE3 = 'outputActuatorTimings.py'
SCRIPT_FILE4 = 'installUpgrade.py'
SCALEUI_BINARY = 'scaleui'
SOFTWARE_VERSION = 'SOFTWARE_VERSION_INFO'
VERSION_INFO = 'version.ini'
FF = 'htseries-any-ngw-8.61.0-release.package.tgz'
FF2 = 'htseries-any-aws3-8.36.0-release.package.tgz'

exitCode = 0

exitCode |= installUtils.setPythonRoot()

print 'Upgrade installation (2.12.0) Wrapper upgrade'
exit_code, process_id = messageBox.display('Preparing to install Wrapper calibrate, please wait...')
exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH,FEATURE_FILENAME), FEATURE_INSTALL_PATH)
messageBox.destroy(process_id)

# ------------
#------------------------------
print 'Stopping scale user interface and business logic'
exit_code, process_id = messageBox.display('Stopping scale user interface and business logic, please wait...')
os.system("/usr/bin/kill -9 $(/usr/bin/pgrep %s)" % (SCALEUI_BINARY))
os.system("/usr/bin/kill -s TERM $(/usr/bin/pgrep %s)" % (BUSINESS_LOGIC_BINARY))
time.sleep(10)
os.system("/usr/bin/kill -9 $(/usr/bin/pgrep %s)" % (BUSINESS_LOGIC_BINARY))

# --- clean /root/firmware directory
os.system("rm /root/firmware/htseries-any-ngw-*.tgz")
os.system("rm /root/firmware/htseries-any-aws3-*.tgz")

# --- clean /root/timings directory
os.system("rm /root/timings/*")

try:
    os.remove(os.path.join("/", "usr", "bin", BUSINESS_LOGIC_BINARY))
except:
    pass
try:
    os.remove(os.path.join("/", "usr", "bin", SCALEUI_BINARY))
except:
    pass

# copy chobart_d and scaleui to /usr/bin
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,BUSINESS_LOGIC_BINARY), os.path.join("/", "usr", "bin", BUSINESS_LOGIC_BINARY))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,SCALEUI_BINARY), os.path.join("/", "usr", "bin", SCALEUI_BINARY))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,CONFIGC), os.path.join("/", "etc", "hobart", CONFIGC))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,SCRIPT_FILE), os.path.join("/", "usr", "local", "hobart", "python", SCRIPT_FILE))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,SCRIPT_FILE2), os.path.join("/", "usr", "local", "hobart", "python", SCRIPT_FILE2))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,SCRIPT_FILE3), os.path.join("/", "usr", "local", "hobart", "python", SCRIPT_FILE3))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,SCRIPT_FILE4), os.path.join("/", "usr", "local", "hobart", "python", SCRIPT_FILE4))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,VERSION_INFO), os.path.join("/", "etc", "hobart", VERSION_INFO))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,SOFTWARE_VERSION), os.path.join("/", SOFTWARE_VERSION))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,FF), os.path.join("/", "root", "firmware", FF))
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,FF2), os.path.join("/", "root", "firmware", FF2))

# chmod of the scaleui and chobart_d to executable
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "bin", BUSINESS_LOGIC_BINARY)))
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "bin", SCALEUI_BINARY)))
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "local", "hobart", "python", SCRIPT_FILE)))
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "local", "hobart", "python", SCRIPT_FILE2)))
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "local", "hobart", "python", SCRIPT_FILE3)))
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "local", "hobart", "python", SCRIPT_FILE4)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "etc", "hobart", CONFIGC)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "etc", "hobart", VERSION_INFO)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", SOFTWARE_VERSION)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "root", "firmware", FF)))
os.system("/bin/chmod u=rw,go=r %s" % (os.path.join("/", "root", "firmware", FF2)))
messageBox.destroy(process_id)

exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)

print 'Installation is complete, rebooting scale...'
exit_code, process_id = messageBox.display('Installation is complete, rebooting scale...', timer=5)

# sync file system 
os.system("/bin/sync")
os.system("/sbin/reboot")

sys.exit(0)

