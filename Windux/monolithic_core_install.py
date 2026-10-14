import sys
import installUtils
import installUpgrade
import os
import shutil
import languageSettings
import logging
import time
import datetime
# NOTE:
# startupSequence imported here so it will be pulled into monolithic feature
# DO NOT REMOVE THIS LINE
import startupSequence

exitCode = 0
exitCode |= installUtils.setPythonRoot()
language = languageSettings.currentLanguage()

featureName = "monolithic_core"
featureFilename = "%s.tgz" % (featureName)
MONOLITHIC_CORE_VERSION = "2.0.3"
MONOLITHIC_LOG='/usr/local/hobart/mono-setup.log'

logging.basicConfig(filename=MONOLITHIC_LOG, level=logging.DEBUG)

logging.info("Mono Version: {0}".format(MONOLITHIC_CORE_VERSION))
logging.info("Mono Start Time local: {0}".format(datetime.datetime.now()))
logging.info("Mono Start Time (UTC): {0}".format(datetime.datetime.utcnow()))
os.system("ifconfig >> {0}".format(MONOLITHIC_LOG))

partitions = ['mmcblk0p3', 'mmcblk1p3', 'mmcblk0p1', 'mmcblk1p1']
foundver = False

for part in partitions:
    ver_part = os.path.join(os.sep, "media", part, "SOFTWARE_VERSION_INFO")
    ver_root = os.path.join(os.sep, "SOFTWARE_VERSION_INFO")

    if os.path.isfile(ver_part) and os.path.isfile(ver_root):
        try:
            if os.path.samefile(ver_part, ver_root):
                recoveryPartition = os.path.join(os.sep, "media", "mmcblk0p2")

                if 'mmcblk1' in part:
                    recoveryPartition = os.path.join(os.sep, "media", "mmcblk1p2")
                elif 'mmcblk2' in part:
                    recoveryPartition = os.path.join(os.sep, "media", "mmcblk2p2")

                recovery = os.path.join(recoveryPartition, "usr", "local", "hobart")
                live = os.path.join(os.sep, "media", part)
                liveHobart = os.path.join(live, "usr", "local", "hobart")
                foundver = True
                break
        except:
            pass

if not foundver:
    print("Could not find any software version file")
    sys.exit(1)

resources = os.path.join(recovery, "resources")
isPatchUpgrade = False
removeUpgrade = True
mbId = 0
dbBackupInPlace = True
resources = os.path.join(recoveryPartition, "usr", "local", "hobart", "resources")
restoreScript = os.path.join(resources, "restore.sh")
scaleAppRestore = os.path.join(recovery, "scaleApp")
coreTGZ = "hte.tar.gz"
db = os.path.join(liveHobart, "db")
dbRestore = os.path.join(recovery, "db")
removeResources = True
uboot_directory = os.path.join(recoveryPartition, 'usr', 'local', 'hobart', 'u-boot')

recoveryFilesToCheck = [os.path.join(recoveryPartition, 'boot', 'uImage'),
                        os.path.join(recoveryPartition, 'etc', 'init.d', 'recovery.sh'),
                        os.path.join(recoveryPartition, 'etc', 'init.d', 'rc.local'),  # this is a directory
                        ]

for file in recoveryFilesToCheck:
    if not os.path.exists(file):
        logging.error("{0} not found. Invalid Recovery Partition".format(file))
        exitCode = 1
        sys.exit(exitCode)

# extract the tarball with the other tarballs inside the initial tarball
# exitCode |= installUtils.extractTarballAsIs(featureFilename, os.path.join(installUtils.getFeatureRoot(),  featureName))

# HT-501: Need to have a eGTouch0.param file
if not os.path.exists(os.path.join("/", "etc", "eGTouch0.param")):
    exitCode |= os.system("/bin/cp /usr/local/hobart/features/monolithic_core/eGTouch0.param /etc/.")

# extract the uboot tarball to the features directory
exitCode |= installUtils.extractTarballAsIs("uboot.tgz", os.path.join(installUtils.getFeatureRoot(), featureName))

try:
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'u-boot.imx'),
                    os.path.join(uboot_directory, 'u-boot.imx'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'u-boot-s.imx'),
                    os.path.join(uboot_directory, 'u-boot-s.imx'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'u-boot-q.imx'),
                    os.path.join(uboot_directory, 'u-boot-q.imx'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'u-boot-itw.imx'),
                    os.path.join(uboot_directory, 'u-boot-itw.imx'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'u-boot-s-itw.imx'),
                    os.path.join(uboot_directory, 'u-boot-s-itw.imx'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'u-boot-q-itw.imx'),
                    os.path.join(uboot_directory, 'u-boot-q-itw.imx'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'u-boot-ark.imx'),
                    os.path.join(uboot_directory, 'u-boot-ark.imx'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'u-version.bmp'),
                    os.path.join(uboot_directory, 'u-version.bmp'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'htm8.bmp'),
                    os.path.join(uboot_directory, 'htm8.bmp'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'htx8.bmp'),
                    os.path.join(uboot_directory, 'htx8.bmp'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'hti8.bmp'),
                    os.path.join(uboot_directory, 'hti8.bmp'))
    shutil.copyfile(os.path.join(installUtils.getFeatureRoot(), featureName, 'hts8.bmp'),
                    os.path.join(uboot_directory, 'hts8.bmp'))
except:
    exitCode |= 1

try:
    shutil.copyfile("recovery.sh", os.path.join(recoveryPartition, "etc", "init.d", "recovery.sh"))
    shutil.copyfile("rc.local", os.path.join(recoveryPartition, "etc", "init.d", "rc.local"))
    shutil.copyfile("fstab", os.path.join(recoveryPartition, "etc", "fstab"))

except:
    exitCode |= 1
logging.debug("Testing env")
# uboot bug is causing monolithic issues so we are updating this environment variable
code = os.system("fw_printenv test_kernel_version")
if not code:
    os.system("fw_setenv test_kernel_version ext2load mmc 1:\$\{active_partition\} 10800000 /boot/\$\{dtbname\}")
    logging.debug("env Changed")
logging.debug("Making Dir")
# make the usr/local/hobart directory
exitCode |= os.system("/bin/mkdir -p %s" % os.path.join(recoveryPartition, "usr", "local", "hobart", "resources"))
logging.debug("Starting backup ")
# backup all files such as db and unique configuration settings to recovery partition so they can be restored after live
# partition update
installUpgrade.backupCriticalFiles("live.tgz", resources, restoreScript, scaleAppRestore, mbId, language, db,
                                   dbBackupInPlace,
                                   live, liveHobart, isPatchUpgrade, removeUpgrade, dbRestore, coreTGZ, removeResources)
logging.debug("backup complete")
# sync the filesystem
exitCode |= installUtils.syncFilesystem()
logging.debug("sync done")

# remove installer files since this is feature that is not advertised
# exitCode |= installUtils.removeInstallerFiles(featureName)


logging.debug("Set uboot param")
# set uboot parameters
exitCode |= installUtils.ubootSetBootParameters(partition=2, action='upgrade_mono')
logging.debug("rebooting")
# reboot the scale which will come up in recovery partition and extract live tarball

os.system("ifconfig wlan0 down")
os.system("sync")
time.sleep(2)
exitCode |= installUtils.reboot()
logging.debug("rebooted")
logging.debug(exitCode)
sys.exit(exitCode)
