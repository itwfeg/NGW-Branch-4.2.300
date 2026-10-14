# -------------------------------------------------------------------
# SD Migration Utility
#
# Author: Eric Landes
# Date: 1/8/2018
# Company: ITW Food Equipment Group
#
# Description:
# This feature will migrate user data and settings from a 2.x scale to a fresh 3.x uSD card
# NOTE: This feature relies on the running of first-boot to complete
# -------------------------------------------------------------------
import os
import shutil
import osUtils
import ignore_dir_util
from distutils import dir_util
import sys
import installUtils
import messageBox
import time
import string
import tarfile
import shellCommand

PYTHON_PATH = '/usr/local/hobart/python'
sys.path.append(PYTHON_PATH)
BACKEND_PATH = '/usr/local/hobart/backend'
sys.path.append(BACKEND_PATH)
sys.path.append('/usr/lib/python2.7/dist-packages')
sys.path.append('/usr/lib/python2.7/site-packages')
# These imports have to wait until after the system path has been added to.
import sendDeviceEventMessage
import scaleSettingsInterface
import socket
import select
import connectNetworkStartupOnce
import getHostname
import getMacAddress
from MyQuestionBox import QuestionBox
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import Qt

# Setup Feature
FEATURE_NAME = 'sd_migration'
FEATURE_FILENAME = FEATURE_NAME + '.tgz'
FEATURE_INSTALLER_NAME = FEATURE_NAME + '_install.py'
FEATURE_INSTALL_PATH = os.path.join(installUtils.getFeatureRoot(), FEATURE_NAME)
DEVICE = 'hti'
SUPPORTED_SYSTEM = 'linux'

FEATURES_PATH = '/usr/local/hobart/features'

UBOOT_3_3_0 = "hti-linux-uboot-3.1.0-11222017_090257.package.tgz"
STARTUP_SEQUENCE = 'startupSequence.py'

DB_FILE_PATH = '/usr/local/hobart/db/scale/dbfiles/dbftl.db'
SD_PATH = '/media/sd_two/'
MIGRATION_PATH = os.path.join('/', SD_PATH, 'migration')
IMPORT_HOME_PATH = '/migration/'
BACKUP_COMMAND = 'executeTask_BackupEntireDB.ht'
BACKUP_PATH = '/'

HOBART_FOLDER_PATH = '/usr/local/hobart/'
AUDIO_DATA_PATH = '/usr/local/hobart/audio'

USER_DATA_PATH = '/usr/local/hobart/user'
SYSTEM_DATA_PATH = '/usr/local/hobart/system'
PLAYLIST_DATA_PATH = '/usr/local/hobart/playlists'
IMAGES_DATA_PATH = '/usr/local/hobart/images'
LAYOUTS_PATH = '/usr/local/hobart/layouts'
VIDEO_PATH = '/usr/local/hobart/videos'

NETWORK_PATH = 'etc/network/'
RESOLVE_PATH = 'etc/resolv.conf'
CONF_HEAD_PATH = 'etc/resolv.conf.head'
CONF_TAIL_PATH = 'etc/resolv.conf.tail'
WPA_PATH = 'etc/wpa_supplicant.conf'
HOSTNAME_PATH = 'etc/hostname'

CONFIG_PATH = 'usr/local/hobart/cat3WeigherConfig.ht'

DOWNLOAD_PATH = 'usr/local/hobart/download'

SETTINGS_WIZ_PATH = 'usr/local/hobart/db/settingswizard/'


def install_feature(feature_filename):
    install_upgrade_path = os.path.join(PYTHON_PATH, 'installUpgrade.py')
    feature_filepath = os.path.join(FEATURE_INSTALL_PATH, feature_filename)
    return os.system('/usr/bin/python {0} {1}'.format(install_upgrade_path, feature_filepath))

# These files will be excluded from the migration
excludeFiles = {}
excludeFiles['audio'] = []
excludeFiles['audio'].append("bothChannels.aac")
excludeFiles['audio'].append("leftChannel.aac")
excludeFiles['audio'].append("rightChannel.aac")
excludeFiles['audio'].append("scanbeep.wav")

excludeFiles['images'] = []
if os.name == 'posix':
    excludeFiles['images'].append("system")
else:
    excludeFiles['images'].append('System')


excludeFiles['layouts'] = []
excludeFiles['layouts'].append("defaultLayout.xml")
excludeFiles['layouts'].append("TLXML11.xml")
excludeFiles['layouts'].append("TLXML12.xml")
excludeFiles['layouts'].append("TLXML21.xml")
excludeFiles['layouts'].append("TLXML22.xml")
excludeFiles['layouts'].append("TLXML31.xml")
excludeFiles['layouts'].append("TLXML32.xml")
excludeFiles['layouts'].append("TLXML41.xml")
excludeFiles['layouts'].append("TLXML42.xml")
excludeFiles['layouts'].append("TLXML51.xml")
excludeFiles['layouts'].append("TLXML52.xml")
excludeFiles['layouts'].append("TLXML61.xml")
excludeFiles['layouts'].append("TLXML62.xml")
excludeFiles['layouts'].append("TLXML71.xml")
excludeFiles['layouts'].append("TLXML72.xml")
excludeFiles['layouts'].append("annun.png")
excludeFiles['layouts'].append("annunw.png")
excludeFiles['layouts'].append("background.png")
excludeFiles['layouts'].append("backgroundFS.png")
excludeFiles['layouts'].append("background_pick5.png")
excludeFiles['layouts'].append("bakery.png")
excludeFiles['layouts'].append("bakeryFS.png")
excludeFiles['layouts'].append("bakeryw.png")
excludeFiles['layouts'].append("bakeryw_adpanel.png")
excludeFiles['layouts'].append("bakeryw_fspanel.png")
excludeFiles['layouts'].append("deli.png")
excludeFiles['layouts'].append("deliFS.png")
excludeFiles['layouts'].append("deliw.png")
excludeFiles['layouts'].append("deliw_adpanel.png")
excludeFiles['layouts'].append("deliw_fspanel.png")
excludeFiles['layouts'].append("floral.png")
excludeFiles['layouts'].append("floralFS.png")
excludeFiles['layouts'].append("floralw.png")
excludeFiles['layouts'].append("floralw_adpanel.png")
excludeFiles['layouts'].append("floralw_fspanel.png")
excludeFiles['layouts'].append("grey.png")
excludeFiles['layouts'].append("greyFS.png")
excludeFiles['layouts'].append("greyw.png")
excludeFiles['layouts'].append("greyw_adpanel.png")
excludeFiles['layouts'].append("greyw_fspanel.png")
excludeFiles['layouts'].append("mainmarket.png")
excludeFiles['layouts'].append("meat.png")
excludeFiles['layouts'].append("meatFS.png")
excludeFiles['layouts'].append("meatw.png")
excludeFiles['layouts'].append("meatw_adpanel.png")
excludeFiles['layouts'].append("meatw_fspanel.png")
excludeFiles['layouts'].append("outOfService.png")
excludeFiles['layouts'].append("prePackAnnun.png")
excludeFiles['layouts'].append("produce.png")
excludeFiles['layouts'].append("produceFS.png")
excludeFiles['layouts'].append("producew.png")
excludeFiles['layouts'].append("producew_adpanel.png")
excludeFiles['layouts'].append("producew_fspanel.png")
excludeFiles['layouts'].append("seafood.png")
excludeFiles['layouts'].append("seafoodFS.png")
excludeFiles['layouts'].append("seafoodw.png")
excludeFiles['layouts'].append("seafoodw_adpanel.png")
excludeFiles['layouts'].append("seafoodw_fspanel.png")
excludeFiles['layouts'].append("zeroAnnun.png")
excludeFiles['layouts'].append("Readme.txt")

excludeFiles['1024_600'] = []
excludeFiles['1024_600'].append("defaultLayout.xml")
excludeFiles['1024_600'].append("TLXML11.xml")
excludeFiles['1024_600'].append("TLXML12.xml")
excludeFiles['1024_600'].append("TLXML21.xml")
excludeFiles['1024_600'].append("TLXML22.xml")
excludeFiles['1024_600'].append("TLXML31.xml")
excludeFiles['1024_600'].append("TLXML32.xml")
excludeFiles['1024_600'].append("TLXML41.xml")
excludeFiles['1024_600'].append("TLXML42.xml")
excludeFiles['1024_600'].append("TLXML51.xml")
excludeFiles['1024_600'].append("TLXML52.xml")
excludeFiles['1024_600'].append("TLXML61.xml")
excludeFiles['1024_600'].append("TLXML62.xml")
excludeFiles['1024_600'].append("TLXML71.xml")
excludeFiles['1024_600'].append("TLXML72.xml")
excludeFiles['1024_600'].append("adpanel.png")
excludeFiles['1024_600'].append("annun.png")
excludeFiles['1024_600'].append("annunw.png")
excludeFiles['1024_600'].append("background.png")
excludeFiles['1024_600'].append("backgroundFS.png")
excludeFiles['1024_600'].append("background_pick5.png")
excludeFiles['1024_600'].append("bakery.png")
excludeFiles['1024_600'].append("bakeryFS.png")
excludeFiles['1024_600'].append("bakeryw.png")
excludeFiles['1024_600'].append("bakeryw_adpanel.png")
excludeFiles['1024_600'].append("bakeryw_fspanel.png")
excludeFiles['1024_600'].append("deli.png")
excludeFiles['1024_600'].append("deliFS.png")
excludeFiles['1024_600'].append("deliw.png")
excludeFiles['1024_600'].append("deliw_adpanel.png")
excludeFiles['1024_600'].append("deliw_fspanel.png")
excludeFiles['1024_600'].append("floral.png")
excludeFiles['1024_600'].append("floralFS.png")
excludeFiles['1024_600'].append("floralw.png")
excludeFiles['1024_600'].append("floralw_adpanel.png")
excludeFiles['1024_600'].append("floralw_fspanel.png")
excludeFiles['1024_600'].append("grey.png")
excludeFiles['1024_600'].append("greyFS.png")
excludeFiles['1024_600'].append("greyw.png")
excludeFiles['1024_600'].append("greyw_adpanel.png")
excludeFiles['1024_600'].append("greyw_fspanel.png")
excludeFiles['1024_600'].append("mainmarket.png")
excludeFiles['1024_600'].append("meat.png")
excludeFiles['1024_600'].append("meatFS.png")
excludeFiles['1024_600'].append("meatw.png")
excludeFiles['1024_600'].append("meatw_adpanel.png")
excludeFiles['1024_600'].append("meatw_fspanel.png")
excludeFiles['1024_600'].append("outOfService.png")
excludeFiles['1024_600'].append("prePackAnnun.png")
excludeFiles['1024_600'].append("produce.png")
excludeFiles['1024_600'].append("produceFS.png")
excludeFiles['1024_600'].append("producew.png")
excludeFiles['1024_600'].append("producew_adpanel.png")
excludeFiles['1024_600'].append("producew_fspanel.png")
excludeFiles['1024_600'].append("seafood.png")
excludeFiles['1024_600'].append("seafoodFS.png")
excludeFiles['1024_600'].append("seafoodw.png")
excludeFiles['1024_600'].append("seafoodw_adpanel.png")
excludeFiles['1024_600'].append("seafoodw_fspanel.png")
excludeFiles['1024_600'].append("zeroAnnun.png")

excludeFiles['1366_768'] = []
excludeFiles['1366_768'].append("defaultLayout.xml")
excludeFiles['1366_768'].append("TLXML11.xml")
excludeFiles['1366_768'].append("TLXML12.xml")
excludeFiles['1366_768'].append("TLXML21.xml")
excludeFiles['1366_768'].append("TLXML22.xml")
excludeFiles['1366_768'].append("TLXML31.xml")
excludeFiles['1366_768'].append("TLXML32.xml")
excludeFiles['1366_768'].append("TLXML41.xml")
excludeFiles['1366_768'].append("TLXML42.xml")
excludeFiles['1366_768'].append("TLXML51.xml")
excludeFiles['1366_768'].append("TLXML52.xml")
excludeFiles['1366_768'].append("TLXML61.xml")
excludeFiles['1366_768'].append("TLXML62.xml")
excludeFiles['1366_768'].append("TLXML71.xml")
excludeFiles['1366_768'].append("TLXML72.xml")
excludeFiles['1366_768'].append("adpanel.png")
excludeFiles['1366_768'].append("annun.png")
excludeFiles['1366_768'].append("annunw.png")
excludeFiles['1366_768'].append("background.png")
excludeFiles['1366_768'].append("backgroundFS.png")
excludeFiles['1366_768'].append("background_pick5.png")
excludeFiles['1366_768'].append("bakery.png")
excludeFiles['1366_768'].append("bakeryFS.png")
excludeFiles['1366_768'].append("bakeryw.png")
excludeFiles['1366_768'].append("bakeryw_adpanel.png")
excludeFiles['1366_768'].append("bakeryw_fspanel.png")
excludeFiles['1366_768'].append("deli.png")
excludeFiles['1366_768'].append("deliFS.png")
excludeFiles['1366_768'].append("deliw.png")
excludeFiles['1366_768'].append("deliw_adpanel.png")
excludeFiles['1366_768'].append("deliw_fspanel.png")
excludeFiles['1366_768'].append("floral.png")
excludeFiles['1366_768'].append("floralFS.png")
excludeFiles['1366_768'].append("floralw.png")
excludeFiles['1366_768'].append("floralw_adpanel.png")
excludeFiles['1366_768'].append("floralw_fspanel.png")
excludeFiles['1366_768'].append("grey.png")
excludeFiles['1366_768'].append("greyFS.png")
excludeFiles['1366_768'].append("greyw.png")
excludeFiles['1366_768'].append("greyw_adpanel.png")
excludeFiles['1366_768'].append("greyw_fspanel.png")
excludeFiles['1366_768'].append("mainmarket.png")
excludeFiles['1366_768'].append("meat.png")
excludeFiles['1366_768'].append("meatFS.png")
excludeFiles['1366_768'].append("meatw.png")
excludeFiles['1366_768'].append("meatw_adpanel.png")
excludeFiles['1366_768'].append("meatw_fspanel.png")
excludeFiles['1366_768'].append("outOfService.png")
excludeFiles['1366_768'].append("prePackAnnun.png")
excludeFiles['1366_768'].append("produce.png")
excludeFiles['1366_768'].append("produceFS.png")
excludeFiles['1366_768'].append("producew.png")
excludeFiles['1366_768'].append("producew_adpanel.png")
excludeFiles['1366_768'].append("producew_fspanel.png")
excludeFiles['1366_768'].append("seafood.png")
excludeFiles['1366_768'].append("seafoodFS.png")
excludeFiles['1366_768'].append("seafoodw.png")
excludeFiles['1366_768'].append("seafoodw_adpanel.png")
excludeFiles['1366_768'].append("seafoodw_fspanel.png")
excludeFiles['1366_768'].append("zeroAnnun.png")
excludeFiles['1366_768'].append("Readme.txt")
excludeFiles['1366_768'].append("1366_768")

excludeFiles['videos'] = []
excludeFiles['videos'].append("Accumulate_Items_Video.mp4")
excludeFiles['videos'].append("FlashKey_Basics_Video.mp4")
excludeFiles['videos'].append("Printer_Overview_Video.mp4")
excludeFiles['videos'].append("Scale_Navigation_Video.mp4")
excludeFiles['videos'].append("Search_Capabilities_Video.mp4")
excludeFiles['videos'].append("Super_ConfigOp_Video.mp4")
excludeFiles['videos'].append("Super_Cool_Video.mp4")
excludeFiles['videos'].append("Super_FlashkeyEd_Video.mp4")
excludeFiles['videos'].append("Super_OpScreen_Video.mp4")
excludeFiles['videos'].append("Super_Overview_Video.mp4")
excludeFiles['videos'].append("Tare_Functionality_Video.mp4")
excludeFiles['videos'].append("NGW_ChangingLabels.mp4")
excludeFiles['videos'].append("NGW_Cleaning.mp4")
excludeFiles['videos'].append("NGW_CleaningthePrinthead.mp4")
excludeFiles['videos'].append("NGW_LoadingFilm.mp4")
excludeFiles['videos'].append("NGW_Training.mp4")


def ignorePath(dirname, filenames):
    files_to_ignore = []
    # if the basename of the full directory path is in keys of excludeFiles
    if os.path.basename(dirname) in excludeFiles.keys():
        # for each filename in the directory
        for i in xrange(0, len(filenames)):
            # if the filename is in list for direname key
            if filenames[i] in excludeFiles[os.path.basename(dirname)]:
                # ignore the file
                files_to_ignore.append(filenames[i])
    return files_to_ignore


def exportFiles(sys_path, export_path):
    print sys_path, export_path
    # thumbd (destination) must have hobart on end of path when passed in from Qt Backup/Restore application
    try:
        dirTreeItems = []
        dirTreeItems.append("audio")  # on WIN this src dir is /Audio, on LINUX /audio
        dirTreeItems.append("images")  # on WIN this src dir is /Images, on LINUX /images
        dirTreeItems.append("layouts")  # on WIN this src dir is /Layouts, on LINUX /layouts
        dirTreeItems.append("playlists")  # on WIN this src dir is /Playlists, on LINUX /playlists
        dirTreeItems.append("user")  # on WIN this src dir is /User, on LINUX /user
        dirTreeItems.append("videos") # on WIN this src dir is /Videos, on LINUX /video
        dirTreeItems.append("system/graphics")
        dirTreeItems.append("system/fonts")
        # NOTE that when we create these root directory names, they do not necessarily match the directory names
        #      that we are exporting from.  For example, on an HTx system, all of the dirs listed above
        #      are all actually uppercase. This is an issue on imports if we import from this export. Also on
        #      the export(HTx), the root dirs are uppercase, but the sub dirs under them will be pulled in with
        #      their uppercase leading chars because we export under these as is under these directories

        for curTreeItem in dirTreeItems:
            exit_code, process_id = messageBox.display('Backing up ' + curTreeItem + ', Please wait...')
            # htPath source tree is all files and directories under current tree item being processed
            src = os.path.join(sys_path, curTreeItem)

            # thumbd (destination) of copy is the destination plus current tree item
            dst = os.path.join(export_path, curTreeItem)

            print src, dst
            # does the directory already exists on thumbdrive device?

            # copy the system files(s) to the thumbdrive destination directory
            if os.name == 'posix':
                # cmd = '/bin/cp --recursive --force %s "%s"' % (src,dst)
                # file_io.writeToFile(os.path.join("/","tmp","out2.dat"),
                #            [cmd])
                # os.system(cmd)

                try:
                    ignore_dir_util.copy_tree(src, dst, verbose=0, ignore=ignorePath)
                except Exception, e:
                    skipped_files.append(src)
                    print e
                    pass
            else:
                ignore_dir_util.copy_tree(src, dst, verbose=0, ignore=ignorePath)
                # os.system('xcopy /E /K /I /C /Q /H /R /Y "%s" "%s"' % (src,dst))
            messageBox.destroy(process_id)
        return 0
    except Exception, e:
        print e
        return 1


def scaleSettings_file_exists(file):
    try:
        with open(file) as f:
            return True
    except IOError:
        return False


def setValue(key, value, scale_Settings_file):
    success = False
    if scaleSettings_file_exists(scale_Settings_file):
        try:
            # Read the contents and replace inline
            import fileinput
            for line in fileinput.FileInput(scale_Settings_file, inplace=1):
                if line.startswith(key + '='):
                    line = key + '=' + value
                    success = True
                print line.rstrip()
            if not success:
                with open(scale_Settings_file, 'a') as file:
                    file.write(key + '=' + value)
                    success = True
        except:
            success = False
    return success


def getScaleSettings():
    scaleSettings = {}
    copyDBFile = False
    deleteDBFile = False
    if not os.path.exists("/usr/local/hobart/db/settingswizard/scalesettings"):
        copyDBFile = True
        deleteDBFile = False

    else:
        sizeOfDBFile = os.path.getsize("/usr/local/hobart/db/settingswizard/scalesettings")
        if sizeOfDBFile == 0:
            copyDBFile = True

    if copyDBFile:
        if deleteDBFile:
            os.remove("/usr/local/hobart/db/settingswizard/scalesettings")
        shutil.copy2('/usr/local/hobart/db/settingswizard/scalesettingsBACKUP',
                     '/usr/local/hobart/db/settingswizard/scalesettings')

    dbFile = open("/usr/local/hobart/db/settingswizard/scalesettings", "r")
    for line in dbFile:
        line = string.strip(line)
        keyValuePair = line.split('=')
        if 2 == len(keyValuePair):
            scaleSettings[keyValuePair[0]] = (keyValuePair[1])
    dbFile.close()
    return scaleSettings


# ---------------------------------------------------------------------------------------------------------------------------
# Setup Feature
exitCode = 0
exitCode |= installUtils.setPythonRoot()
exit_code, process_id = messageBox.display('Preparing to install SD Migration Feature, please wait...')
print "Extracting files"
exitCode |= installUtils.extractTarballAsIs(os.path.join(FEATURE_INSTALL_PATH, FEATURE_FILENAME), FEATURE_INSTALL_PATH)
skipped_files = []
# ---------------------------------------------------------------RESTORE-----------------------------------------------------
# Restore should only need to be run if the startupSequence.py does not copy or the scale does not go through firstboot
# WHAT IT DOES:
#   1. drops the weigher config file into /usr/local/hobart/downloads. It may take 2min for the file to be recognized
#   2. sets scale volume
#   3. Forces network to read scaleSettings file and reconnect
#   4. Prompts User to delete /migration folder

# MIGRATED is a file used to flag if we should backup or restore. NOTE: This file will be removed in firstboot
if os.path.isfile(os.path.join('/', IMPORT_HOME_PATH, 'MIGRATED')):
    print "RESTORE"
    #    exit_code, process_id = messageBox.display('Restoring features, please wait...')
    #
    #    for zip in os.listdir(os.path.join('/', IMPORT_HOME,  'features')):
    #        print zip
    #        feature_folder_name = zip[:-4]
    #        print feature_folder_name
    #        if not os.path.exists(os.path.join('/', FEATURES_PATH, os.path.basename(feature_folder_name))):
    #            #shutil.copytree(zip, os.path.join('/', FEATURES_PATH, os.path.basename(zip)))
    #            feature = tarfile.open(os.path.join('/', IMPORT_HOME, 'features',  zip))
    #            feature.extractall(os.path.join('/', FEATURES_PATH))
    #            messageBox.destroy(process_id)
    #            exit_code, process_id = messageBox.display('Installing '+ feature_folder_name + ', please wait...')
    #            if os.path.isfile(os.path.join('/', FEATURES_PATH, os.path.basename(zip),  "install.py")):
    #                os.system("python " + os.path.join('/', FEATURES_PATH, os.path.basename(zip),  "install.py"))
    #        else:
    #            print feature_folder_name  + ' Already exists. Skipping...'

    exit_code, process_id = messageBox.display('Restoring Weigher Configuration, Please wait....')
    #copy weigher config if it exists
    try:
        shutil.copyfile(os.path.join('/', IMPORT_HOME_PATH, 'cat3WeigherConfig.ht'),
                        os.path.join('/', DOWNLOAD_PATH, 'cat3WeigherConfig.ht'))
    except:
        messageBox.destroy(process_id)
        exit_code, process_id = messageBox.display('Weigher configuration could not be restored, Skipping...')
        pass
    messageBox.destroy(process_id)


    exit_code, process_id = messageBox.display('Restoring network, Please wait....')
    # set volume level
    oVolume = ""
    if scaleSettingsInterface.getValue('oVolume') == "on":
        oVolume = "128"
    elif scaleSettingsInterface.getValue('oVolume') == "off":
        oVolume = "0"
    exitCode = sendDeviceEventMessage.sendMessage(server='localhost', port=5077,
                                                  message='beeperVolume=%s' % (str(oVolume)))
    time.sleep(0.5)
    # force network connection reset
    exit_status = connectNetworkStartupOnce.connectToNetwork()

    if exit_status:
        print "Network Restore Failed"
        messageBox.destroy(process_id)
        exit_code, process_id = messageBox.display('Network could not be restored, Skipping...')
    messageBox.destroy(process_id)

    os.environ['DISPLAY'] = ':0'
    myApp = QApplication([])
    # Create dialog
    d = QuestionBox()
    d.addText("Restore Complete." + os.linesep + "Would You like to delete migration files?")
    d.setWindowModality(True)
    d.showMaximized()
    d.setWindowState(Qt.WindowFullScreen)
    d.activateWindow()

    # Find and kill window created by installUpgrade.py because its hiding my question box
    os.system("kill $(pgrep -fl 'Copying Upgrade from Source' | grep -v 'pgrep' | cut -d ' ' -f1)")
    print 'Waiting on Input'
    # Show dialog
    if d.exec_():
        exit_code, process_id = messageBox.display('Removing migration files')
        dir_util.remove_tree(IMPORT_HOME_PATH)
        messageBox.destroy(process_id)
    exit_code, process_id = messageBox.display('Feature install success. Cleaning up...')
    exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)
    messageBox.destroy(process_id)
    if os.path.isfile(os.path.join('/', IMPORT_HOME_PATH, 'MIGRATED')):
        os.remove(os.path.join('/', IMPORT_HOME_PATH, 'MIGRATED'))

    exit_code, process_id = messageBox.display('Installation is complete, shutting down scale...', timer=15)
    print ("done")

    # sync filesystem and reboot
    os.system("/bin/sync")
    os.system("/sbin/reboot")
    sys.exit(0)

else:
    # -------------------------------------------------------BACKUP---------------------------------------------------------------
    # Runs if the MIGRATED file is not found
    # WHAT IT DOES:
    #   1. Check for sd card to mount
    #   2. If needed copy startupSequence.py
    #   3. make migration directories
    #   4. copy license files
    #   5. Send command to chobart_d to create weigher config file
    #   6. Send command to chobart_d to create verIndData file
    #   7. Copy multimedia
    #   8. Copy network Config

    # Create /migration on 3.x card. Files will be copied here
    print "BACKUP"
    print "creating dirs"

    # Should we be selecting the second sd based on a search instead of assuming it will always be mmcblk1p1
    try:
        cmd = []
        cmd.append('readlink')
        cmd.append('-f')
        cmd.append('/dev/root')
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        partition = None

        if 'mmcblk0p3' in stdout or 'mmcblk0p1' in stdout:
            if os.path.exists('/dev/mmcblk1p3'):
                partition = 'mmcblk1p3'
            elif os.path.exists('/dev/mmcblk1p1'):
                partition = 'mmcblk1p1'
        elif 'mmcblk1p3' in stdout or 'mmcblk1p1' in stdout:
            if os.path.exists('/dev/mmcblk0p3'):
                partition = 'mmcblk0p3'
            elif os.path.exists('/dev/mmcblk0p1'):
                partition = 'mmcblk0p1'

        if partition is None:
            print 'Could not find Device to mount'
            raise RuntimeError("Could Not Locate Media")

        device = os.path.join(os.sep, 'dev', partition)
        print device

        # umount device
        if os.path.exists(SD_PATH):
            os.system("umount " + SD_PATH)
            os.system("rmdir " + SD_PATH)

        # mount device
        os.system("mkdir " + SD_PATH)
        os.system("mount " + device + " " + SD_PATH + " -t ext4")

        if not os.path.isfile(os.path.join(os.sep, SD_PATH, 'SOFTWARE_VERSION_INFO')):
            print 'Could not find SD to mount'
            raise RuntimeError("Could Not Mount")

        os.system("rm -rf " + MIGRATION_PATH)
        os.system("mkdir " + MIGRATION_PATH)
    except:
        messageBox.destroy(process_id)
        exit_code, process_id = messageBox.display('Failed to mount SD card. Exiting...', timer=5)
        time.sleep(5)
        sys.exit(-1)
    messageBox.destroy(process_id)

    # TODO: Check version once branch merge
    # I modified the startupSequence.py file to import licenses from /migration/mtd6 if they exist
    # 3.1.1 is the current version when this was created so all later versions should already include the modified
    # startupSequence.py
    with open(os.path.join('/', SD_PATH, 'SOFTWARE_VERSION_INFO'), 'r') as version_info:
        if tuple(int(x) for x in version_info.readline().split(':')[1].split('.')) <= (3, 1, 1):
            shutil.copyfile(os.path.join('/', FEATURE_INSTALL_PATH, STARTUP_SEQUENCE),
                            os.path.join('/', SD_PATH, 'usr', 'local', 'hobart', 'python', STARTUP_SEQUENCE))

    try:
        # os.system("rm -rf " + os.path.join('/',MIGRATION_LOCATION,  "features"))
        # os.system("mkdir " + os.path.join('/',MIGRATION_LOCATION,  "features"))
        mfile = open(os.path.join('/', MIGRATION_PATH, 'MIGRATED'), 'w')
        mfile.close()
    except:
        messageBox.destroy(process_id)
        exit_code, process_id = messageBox.display('Failed to make directories. Exiting...', timer=5)
        sys.exit(-1)

    # firstboot looks for /migration/mtd6 to copy license from
    exit_code, process_id = messageBox.display('Backing up licenses, Please wait...')
    shutil.copyfile("/dev/mtd6", os.path.join('/', MIGRATION_PATH, 'mtd6'))
    messageBox.destroy(process_id)

    # CONCEPT
    # Check for features in the features folder
    # Check to make sure its not me
    # Create tarzip from feature folder in the migration location
    # unpack and run install in firstboot

    #    exit_code, process_id = messageBox.display('Backing up features, please wait...')
    #    print 'Backing up features, please wait...'
    #    for dir in os.listdir(FEATURES_PATH):
    #        if(not (dir == FEATURE_NAME)):
    #            output_filename = os.path.join('/', MIGRATION_LOCATION, 'features',  os.path.basename(dir) + ".tgz")
    #            print 'Feature found: ' + output_filename
    #            with tarfile.open(output_filename, "w:gz") as tar:
    #                tar.add(os.path.join('/', FEATURES_PATH,  dir), dir)
    #    messageBox.destroy(process_id)

    '''
    exit_code, process_id = messageBox.display('Backing up Configuration, please wait...')
    # Weigher Config file will be created by chobart_d at /usr/local/hobart/cat3WeigherConfig.ht
    if os.path.isfile(os.path.join('/', CONFIG_PATH)):
        os.remove(os.path.join('/', CONFIG_PATH))
    print 'Backing up Configuration, please wait...'
    unit_separator = '\x1f'
    record_separator = '\x1e'
    record = unit_separator.join(('RTDO', 'TNreadCat3WeigherConfig')) + record_separator + 'RTFF' + record_separator
    sendDeviceEventMessage.sendMessage(server='localhost', port=6000, message=record)
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('localhost', 5079)
    server.bind(server_address)
    server.listen(5)
    inputs = [server]
    outputs = []
    # timeout waiting from message from chobart_d
    # after 2 min
    timeout = 60.0 * 2
    # sleep for 5 sec in between checking for file
    sleep_time = 5
    # wait for chobart_d to add file to
    config_flag = True
    while not os.path.isfile(os.path.join('/', CONFIG_PATH)):
        time.sleep(sleep_time)
        if timeout <= 0:
            print 'Config File Timeout'
            config_flag = False
            skipped_files.append('Weigher Configuration     Timeout waiting on server')
            break
        else:
            timeout -= sleep_time
    previous_size = os.path.getsize(os.path.join('/', CONFIG_PATH))
    time.sleep(sleep_time)
    while os.path.getsize(os.path.join('/', CONFIG_PATH)) > previous_size:
        previous_size = os.path.getsize(os.path.join('/', CONFIG_PATH))
        time.sleep(sleep_time)
        print 'Waiting on download file. Time: ' + time.time() + 'Size: ' + previous_size
    if config_flag:
        print 'config downloaded'
        shutil.copyfile(os.path.join('/', CONFIG_PATH), os.path.join('/', MIGRATION_PATH, 'cat3WeigherConfig.ht'))
        shutil.copyfile(os.path.join('/', CONFIG_PATH), os.path.join('/', SD_PATH, DOWNLOAD_PATH, 'cat3WeigherConfig.ht'))
    else:
        messageBox.destroy(process_id)
        exit_code, process_id = messageBox.display('Configuration Backup Failed')
        print 'Configuration Backup Failed'
        time.sleep(5)
        # close server
    server.close()
    messageBox.destroy(process_id)
'''
    exit_code, process_id = messageBox.display('Backing up database files, please wait...')
    print 'Backing up database files, please wait...'
    # Backup can be found at /etc/hobart/verIndData
    sendDeviceEventMessage.sendMessage(server='localhost', port=6000,
                                       message="RTDO" + chr(0x1F) + "TNBackupEntireDB" + chr(0x1E))
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    server_address = ('localhost', 5079)
    server.bind(server_address)
    server.listen(5)
    inputs = [server]
    outputs = []
    # timeout waiting from message from chobart_d
    # after 20 minutes
    timeout = 20.0 * 60.0
    # wait for chobart_d to respond with message
    messageReceived = False
    while not messageReceived:
        readable, writable, exceptional = select.select(inputs, outputs, inputs, timeout)
        # check if timeout reached
        if readable == [] and writable == [] and exceptional == []:
            # timeout reached so backup of db not possible
            break
        else:
            # received message from chobart_d
            for s in readable:
                # socket message coming in to process
                if s is server:
                    # accept network connection
                    connection, client_address = s.accept()
                    data = connection.recv(1024)
                    connection.close()
                    messageReceived = True
                    if data == 'htdfBackupSuccess':
                        messageBox.destroy(process_id)
                        exit_code, process_id = messageBox.display('Backup Success. Copying files, please wait...')
                        '''
                        try:
                            with open(os.path.join("/", "etc", "hobart", "verIndData"), 'a') as verIndData:
                                with open(os.path.join('/', CONFIG_PATH), 'r') as cat3config:
                                    verIndData.write(cat3config.read())
                        except:
                            print('Weigher config file not found')
                            pass
                        '''
                        shutil.copy(os.path.join("/", "etc", "hobart", "verIndData"), MIGRATION_PATH)
                        if os.path.isfile(os.path.join('/', 'media', 'sd_two', 'etc', 'hobart', 'verIndData')):
                            os.remove(os.path.join('/', 'media', 'sd_two', 'etc', 'hobart', 'verIndData'))
                        shutil.move(os.path.join("/", "etc", "hobart", "verIndData"),
                                    os.path.join('/', 'media', 'sd_two', 'etc', 'hobart'))
    # close server
    server.close()
    messageBox.destroy(process_id)

    # Backup user data
    print 'Backing up Multimedia, Please Wait...'
    exportFiles(HOBART_FOLDER_PATH, os.path.join('/', SD_PATH, 'usr', 'local', 'hobart'))

    exit_code, process_id = messageBox.display('Backing up Network please wait...')
    print 'Backing up Network please wait...'
    try:
        dir_util.copy_tree(os.path.join('/', NETWORK_PATH), os.path.join('/', SD_PATH, NETWORK_PATH))
    except:
        messageBox.display(os.path.join('/', NETWORK_PATH) + ' not found. Skipping...', timer=2)
        print os.path.join('/', NETWORK_PATH) + ' not found. Skipping...'
        skipped_files.append(os.path.join('/', NETWORK_PATH) + '   File not found')
        pass
    try:
        shutil.copyfile(os.path.join('/', RESOLVE_PATH), os.path.join('/', SD_PATH, RESOLVE_PATH))
    except:
        messageBox.display(os.path.join('/', RESOLVE_PATH) + ' not found. Skipping...', timer=2)
        print os.path.join('/', RESOLVE_PATH) + ' not found. Skipping...'
        skipped_files.append(os.path.join('/', RESOLVE_PATH) + '   File not found')
        pass
    try:
        shutil.copyfile(os.path.join('/', CONF_HEAD_PATH), os.path.join('/', SD_PATH, CONF_HEAD_PATH))
    except:
        messageBox.display(os.path.join('/', CONF_HEAD_PATH) + ' not found. Skipping...', timer=2)
        print os.path.join('/', CONF_HEAD_PATH) + ' not found. Skipping...'
        skipped_files.append(os.path.join('/', CONF_HEAD_PATH) + '   File not found')
        pass

    try:
        shutil.copyfile(os.path.join('/', CONF_TAIL_PATH), os.path.join('/', SD_PATH, CONF_TAIL_PATH))
    except:
        messageBox.display(os.path.join('/', CONF_TAIL_PATH) + ' not found. Skipping...', timer=2)
        print os.path.join('/', CONF_TAIL_PATH) + ' not found. Skipping...'
        skipped_files.append(os.path.join('/', CONF_TAIL_PATH) + '   File not found')
        pass

    try:
        shutil.copyfile(os.path.join('/', WPA_PATH), os.path.join('/', SD_PATH, WPA_PATH))
    except:
        messageBox.display(os.path.join('/', WPA_PATH) + ' not found. Skipping...', timer=2)
        print os.path.join('/', WPA_PATH) + ' not found. Skipping...'
        skipped_files.append(os.path.join('/', WPA_PATH) + '   File not found')
        pass
    try:
        shutil.copyfile(os.path.join('/', HOSTNAME_PATH), os.path.join('/', SD_PATH, HOSTNAME_PATH))
        shutil.copyfile(os.path.join('/', HOSTNAME_PATH), os.path.join(MIGRATION_PATH, 'hostname'))
    except:
        messageBox.display(os.path.join('/', HOSTNAME_PATH) + ' not found. Skipping...', timer=2)
        skipped_files.append(os.path.join('/', HOSTNAME_PATH) + '   File not found')
        print os.path.join('/', HOSTNAME_PATH) + ' not found. Skipping...'
        pass
    src_files = os.listdir(os.path.join('/', SETTINGS_WIZ_PATH))
    for file_name in src_files:
        full_file_name = os.path.join('/', SETTINGS_WIZ_PATH, file_name)
        if file_name == 'scalesettings':

            scaleSettings = getScaleSettings()
            for key in scaleSettings:
                key = key.split(':')
                # print key[0], scaleSettings[key[0]]
                setValue(key[0], scaleSettings[key[0]], os.path.join('/', SD_PATH, SETTINGS_WIZ_PATH, file_name))
            if not scaleSettingsInterface.getValue('hostname'):
                exitcode, hostname = getHostname.getHostname()
                if not setValue('hostname', hostname, os.path.join('/', SD_PATH, SETTINGS_WIZ_PATH, 'scalesettings')):
                    try:
                        key_val = 'hostname=' + hostname
                        with open(os.path.join('/', SD_PATH, SETTINGS_WIZ_PATH, 'scalesettings'), 'a') as settings_file:
                            settings_file.write(key_val)
                    except Exception, e:
                        # print e
                        messageBox.display('Hostname could not be added to the scale settings, Skipping...', timer=2)
                        print 'Hostname could not be added to the scale settings, Skipping...'
                        pass

        elif os.path.isdir(full_file_name):
            dir_util.copy_tree(full_file_name, os.path.join('/', SD_PATH, SETTINGS_WIZ_PATH, file_name))
        elif os.path.isfile(full_file_name):
            shutil.copyfile(full_file_name, os.path.join('/', SD_PATH, SETTINGS_WIZ_PATH, file_name))
    exitCode, wiredMac = getMacAddress.getMacAddress(lanType='wire')
    exitCode, wirelessMac = getMacAddress.getMacAddress(lanType='wireless')
    print wiredMac, wirelessMac
    try:
        with open(os.path.join('/', MIGRATION_PATH, 'MAC_ADDRESS'), 'w') as mac_file:
            mac_file.write('WiredMacAddress=' + wiredMac + os.linesep)
            mac_file.write('WirelessMacAddress=' + wirelessMac + os.linesep)
            mac_file.close()
    except:
        print('Could not open MAC_ADDRESS for writing. Skipping...')
        messageBox.display('Could not open MAC_ADDRESS for writing. Skipping...', timer=2)

    messageBox.destroy(process_id)

    exit_code, process_id = messageBox.display('Backup Success. Unmounting...')
    print 'Backup Success. Cleaning up...'
    os.system('sync')
    if os.system("umount " + SD_PATH):
        print ('SD unmount Failed... Trying Lazy unmount')
        if os.system("umount -l " + SD_PATH):
            print 'Lazy unmount failed shutdown before removing SD'
    else:
        os.system("rmdir " + SD_PATH)
    messageBox.destroy(process_id)

    '''
    os.environ['DISPLAY'] = ':0'
    myApp = QApplication([])
    # Create dialog and show
    d = QuestionBox()
    d.addText("Do You Want To Upgrade To UBoot 3.3.0?")
    d.addScrollBox("The following files were skipped:", skipped_files)
    d.setWindowModality(True)
    d.showMaximized()
    d.setWindowState(Qt.WindowFullScreen)
    d.activateWindow()

    # Find and kill window created by InstallUpgrade.py because it covers up my dialog box
    os.system("kill $(pgrep -fl 'Copying Upgrade from Source' | grep -v 'pgrep' | cut -d ' ' -f1)")
    print 'Waiting on Input'
    if d.exec_():
        # Should we be getting most recent copy of UBoot if wew can?
        install_feature(UBOOT_3_3_0)
    '''

    exit_code, process_id = messageBox.display('Feature install success. Cleaning up...')
    exitCode = installUtils.removeInstallerFiles(FEATURE_NAME)
    messageBox.destroy(process_id)

    exit_code, process_id = messageBox.display('Installation is complete, shutting down scale...', timer=15)
    print ("done")

    # sync filesystem and reboot
    os.system("/bin/sync")

    os.system("/sbin/shutdown -h -P now")

    sys.exit(0)
# SHOULD NEVER GET HERE
sys.exit(-1)
