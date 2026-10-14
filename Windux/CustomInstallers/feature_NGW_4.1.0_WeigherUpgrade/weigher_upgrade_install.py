import os
import shutil
import sys
import installUtils
#import installUpgrade
import messageBox
PYTHON_PATH = '/usr/local/hobart/python'
sys.path.append(PYTHON_PATH)
import license
import time

FEATURE_NAME = 'weigher_upgrade'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DOWNLOAD_PATH = os.path.join('/','usr','local','hobart','download')
PENDING_PATH = os.path.join('/','usr','local','hobart','pending')
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

ZIP_FILE = 'XPEWeigher.zip'
CONFIG_LINUX_FILE='config_linux.dat'
DEBUG_FILE='configdebug.dat'
BUSINESS_LOGIC_BINARY = 'chobart_d'
SCALEUI_BINARY = 'scaleui'


# get scale firmware version 
def getScaleVersion():
    myfile = open("//SOFTWARE_VERSION_INFO","r")
    version = myfile.readline().split(":")
    return version[1].strip()    

# ==== Start installing ===========================

exitCode = 0
exitCode |= installUtils.setPythonRoot()

print '\n\nPreparing to upgrade weigher, please wait ...'
exit_code, process_id = messageBox.display("Preparing to install weigher firmware upgrade, please wait ...")
exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH,FEATURE_FILENAME), FEATURE_INSTALL_PATH)
messageBox.destroy(process_id)

# --- read scale config ------
print 'identify scale model and version'
scaleVersion = getScaleVersion()
if not scaleVersion.startswith('4.1'):
    exit_code, process_id = messageBox.display('Unsupported firmware version\nThis feature will be installed only in 4.1.100 versions', timer=8)
    exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)
    print 'telling the bad news... cannot upgrade'
    sys.exit(1)


# --- stopping the scale application -------
exit_code, process_id = messageBox.display('Stopping scale user interface and business logic, please wait ...')
os.system("/usr/bin/kill -9 $(/usr/bin/pgrep %s)" % (SCALEUI_BINARY))
os.system("/usr/bin/kill -s TERM $(/usr/bin/pgrep %s)" % (BUSINESS_LOGIC_BINARY))
time.sleep(10)
os.system("/usr/bin/kill -9 $(/usr/bin/pgrep %s)" % (BUSINESS_LOGIC_BINARY))

# --- save the 4.1.100 version of chobart_d ---
os.system("mv /usr/bin/chobart_d /usr/bin/chobart_d-4.1.100")

# --- Copy new "temp" copy of chobart_d to the the scale ---
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,BUSINESS_LOGIC_BINARY), os.path.join("/", "usr", "bin", BUSINESS_LOGIC_BINARY))
os.system("/bin/chmod u=rwx,go=rx %s" % (os.path.join("/", "usr", "bin", BUSINESS_LOGIC_BINARY)))

# --- copy config_linux.dat file that supports weigher firmware upgrade --- 
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,CONFIG_LINUX_FILE), os.path.join('/etc/hobart/',CONFIG_LINUX_FILE))

# --- copy weigher upgrade file --- 
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,ZIP_FILE), os.path.join(DOWNLOAD_PATH,ZIP_FILE))

# --- save the 4.1.100 version of configdebug.dat ---
os.system("mv /etc/hobart/configdebug.dat /etc/hobart/configdebug.dat-4.1.100")
# --- copy configdebug.dat file that supports weigher firmware upgrade ... will help spit out the firmware version --- 
shutil.copyfile(os.path.join(FEATURE_INSTALL_PATH,DEBUG_FILE), os.path.join('/etc/hobart/',DEBUG_FILE))


# --- reset the USB hub ---
os.system("/bin/echo 0 >> /usr/local/gpio/hub1_enable/value")
time.sleep(.100)
os.system("/bin/echo 1 >> /usr/local/gpio/hub1_enable/value")

# --- restart application ---
print 'restarting applications'
os.system('/usr/bin/chobart_d &')
# os.system('DISPLAY=:0 /usr/bin/scaleui &')
# Don't want scaleui to display, so using GCD non-existent device
os.system('DISPLAY=:0 /usr/bin/scaleui -platform linuxfb:fb=/dev/fb2 &')

# --- wait application to process upgrade file --- 
exit_code, process_id = messageBox.display("The installation is upgrading the Weigher code, please wait...\n\nThis will take a few minutes.")
print 'waiting for the scale to consume the special .zip file'
waiting = True
while waiting:
    if os.listdir(DOWNLOAD_PATH)==[] and os.listdir(PENDING_PATH)==[]:
         waiting = False
         print 'leaving'
    else: 
         print 'waiting 5 secs'
         time.sleep(5)

messageBox.destroy(process_id)


# --- reset the USB hub ... a 2nd time ---
os.system("/bin/echo 0 >> /usr/local/gpio/hub1_enable/value")
time.sleep(.100)
os.system("/bin/echo 1 >> /usr/local/gpio/hub1_enable/value")


#--------------------------------------------------
# Verify the firmware version has been upgraded ...
#--------------------------------------------------
time.sleep(30)

loopcnt = 0
done = False
success = False

try:
    while loopcnt < 10 and done == False:
        # The configdebug.dat turned on "WeigherCommManager.WeigherSystemStatus", which will spit out "softwareWeigherVersion"
        os.system("grep 'softwareWeigherVersion' /usr/local/hobart/debug.log > /root/weigher_upgrade.log")
        result = os.system("grep 'softwareWeigherVersion' /usr/local/hobart/debug.log | grep '1.4'")
        
        if result == 0:
            done = True
            success = True
        else:
            loopcnt = loopcnt + 1
            time.sleep(4)
except:
    pass

if success == True:
    exit_code, process_id = messageBox.display("The weigher firmware was successfully upgraded to 1.4.")
    time.sleep(10)
else:
    exit_code, process_id = messageBox.display("The upgrade failed to upgrade weigher firmware to 1.4.")
    time.sleep(10)


# --- stopping the scale application -------
exit_code, process_id = messageBox.display('Stopping scale user interface and business logic, please wait ...')
os.system("/usr/bin/kill -9 $(/usr/bin/pgrep %s)" % (SCALEUI_BINARY))
os.system("/usr/bin/kill -s TERM $(/usr/bin/pgrep %s)" % (BUSINESS_LOGIC_BINARY))
time.sleep(10)
os.system("/usr/bin/kill -9 $(/usr/bin/pgrep %s)" % (BUSINESS_LOGIC_BINARY))

# --- restore the 4.1.100 version of chobart_d ---
os.system("mv /usr/bin/chobart_d-4.1.100  /usr/bin/chobart_d")

# --- restore the configdebug.dat file ---
os.system("mv /etc/hobart/configdebug.dat-4.1.100 /etc/hobart/configdebug.dat")


# --- restart application ---
print 'restarting applications'
os.system('/usr/bin/chobart_d &')
# os.system('DISPLAY=:0 /usr/bin/scaleui &')
# Don't want scaleui to display, so using GCD non-existent device
os.system('DISPLAY=:0 /usr/bin/scaleui -platform linuxfb:fb=/dev/fb2 &')


print 'installation complete, removing installation files and rebooting scale...'
exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)
exit_code, process_id = messageBox.display('Installation is complete, rebooting wrapper ...', timer=5)

# sync filesystem and reboot
os.system("/bin/sync")
os.system("/sbin/reboot")

sys.exit(0)
