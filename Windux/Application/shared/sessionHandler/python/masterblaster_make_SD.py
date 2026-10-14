#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# ----------------------------------------------------------------------------------------------------------------------
# This script is a modified translation of makeSD.sh. It script is intended to be run by the masterblaster application
# using pexpect since we are running dpkg 1.16 on the scales a workaround has been implemented to install the office
# viewer feature. It is not recommended to run this script on a non scale system. use makeSD.sh instead

import sys
import os
import pexpect
import time
import logging
import re
from optparse import OptionParser
import subprocess

MOUNT_POINT = '/media/thumb'
LOG_FILE_PATH = "/usr/local/hobart/masterblaster.log"
HTI_BUILD_PATH = None


# This log file is also written to by masterblasterApp.py
# tee will log the output of a shell command
# ----------Uncomment one of these lines at all times--------------------
TEE_COMMAND = " 2>&1 | tee -a " + LOG_FILE_PATH  # LOTS OF DEBUG
#TEE_COMMAND = ""  # NOT SO MUCH DEBUG
# -----------------------------------------------------------------------
# logging is for logging the python script
logging.basicConfig(filename=LOG_FILE_PATH, filemode='a', level=logging.DEBUG, format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
parser = OptionParser(usage="usage: %prog [options] ",
                      version="%prog 1.0")


def formatSd(sdSize, sdDev='/dev/mmcblk0'):
    # the recovery partition is always the same size so i only need to determine the size of the hobart partition
    p1size = ''
    if sdSize == '8':
        p1size = '6'
    elif sdSize == '16':
        p1size = '13'
    elif sdSize == '32':
        p1size = '28'
    else:
        return 1

    #os.system("echo o; echo n; echo p; echo 1; echo; echo +{0}G;  echo n; echo p; echo 2; echo; echo +1350M;  echo w".format(p1size) + " | " + "sudo fdisk {0}".format(sdDev))
    log = open(LOG_FILE_PATH, 'a')

    fdisk = pexpect.spawn("sudo fdisk {0}".format(sdDev), logfile=log)
    fdisk.sendline()
    timeout = time.time()
    while True:
        input = fdisk.readline().strip()
        if input == "Command (m for help):":
            break
        # print input
        if time.time() > timeout + 30:
            return 1
    # partiton 1
    fdisk.sendline("o")
    fdisk.sendline("n")
    fdisk.sendline("p")
    fdisk.sendline("1")
    fdisk.sendline()
    fdisk.sendline("+{0}G".format(p1size))
    # partiton 2
    fdisk.sendline("n")
    fdisk.sendline("p")
    fdisk.sendline("2")
    fdisk.sendline()
    fdisk.sendline("+1350M")
    fdisk.sendline("w")
    index = fdisk.expect(["The partition table has been altered.", "Calling ioctl() to re-read partition table", pexpect.EOF], timeout=60*10)
    if index == 2:
        return 1
    fdisk.close()
    log.close()
# gets the release tag from SOFTWARE_VERSION_INFO and compares it to the release tag found in the filepath
def checkVersionInfo(buildFilepath):
    try:
        with open(os.path.join(MOUNT_POINT,'hobart', 'SOFTWARE_VERSION_INFO')) as versionInfoFile:
            version = versionInfoFile.readline().strip().split(':')
            logging.debug(version[1])
            head, tail = os.path.split(buildFilepath)
            logging.debug(tail)
            m = re.search(r"[\d]+[.][\d]+.[\d]+", tail)
            logging.debug(m.group(0))
            logging.debug(version[1].lstrip().rstrip() == m.group(0))
            if version[1].lstrip().rstrip() == m.group(0):
                with open(os.path.join(MOUNT_POINT, 'recovery', 'SOFTWARE_VERSION_INFO')) as recoveryVersionInfoFile:
                    version = recoveryVersionInfoFile.readline().strip().split(':')
                    logging.debug(version[1])
                    head, tail = os.path.split(buildFilepath)
                    logging.debug(tail)
                    m = re.search(r"[\d]+[.][\d]+.[\d]+", tail)
                    logging.debug(m.group(0))
                    logging.debug(version[1].lstrip().rstrip() == m.group(0))
                    return version[1].lstrip().rstrip() == m.group(0)
    except:
        pass
    return False


def main(noOfficeInstall=False):
    sdDev = '/dev/mmcblk0'
    sdSizeGB = 8
    sdPart = "p"
    logging.debug("SkipOfficeViewer={0}".format(noOfficeInstall))
    input = input("Enter SD card device location: ").strip()
    if input:
       sdDev = input
    if "mmcblk" in sdDev:
        sdPart = "p"
    else:
        sdPart = ""
    input = input("Enter SD size (8GB, 16GB or 32GB: ").strip()
    if not(input == '8' or input == '16' or input == '32' or input == ""):
        logging.error("Invalid size given. Expected: 8,16,32")
        print("Invalid size given. Expected: 8,16,32")
        sys.exit(1)
    elif input:
        sdSizeGB = input
    input = input("Enter release path: ").strip()
    if input == "" or not os.path.exists(input):
        logging.error("Release Could not be found")
        print("Release Could not be found")
        sys.exit(1)
    else:
        HTI_BUILD_PATH = input
    if not os.path.exists(sdDev):
        logging.error("SD Card Not Found")
        print("SD Card Not Found")
        sys.exit(1)
    else:
        logging.debug("SD Card Found Unmounting...")
        print("SD Card Found Unmounting...")
        # Unmount usb if it is mounted. Could have multiple partitions

    os.system("umount " + sdDev + TEE_COMMAND)
    os.system("umount " + sdDev + "{0}1".format(sdPart) + TEE_COMMAND)
    os.system("umount " + sdDev + "{0}2".format(sdPart) + TEE_COMMAND)
    try:
        os.mkdir(MOUNT_POINT)
    except:
        pass
    logging.debug("Masterblaster Started")
    logging.debug("Creating Partitons...")
    print("Creating Partitons...")
    formatSd(sdSizeGB, sdDev)
    logging.debug("Refteshing Partition Table")
    os.system("partprobe" + TEE_COMMAND)


    if not (os.path.exists(sdDev + "{0}1".format(sdPart)) and os.path.exists(sdDev + "{0}2".format(sdPart))):
        logging.error("Creation of Partitions Failed...")
        print("Creation of Partitions Failed...")
        sys.exit(1)

    logging.debug("Formatting Partiton 1...")
    print("Formatting Partiton 1...")
    if os.system("mkfs.ext4 -F -Lhobart {0}{1}1".format(sdDev, sdPart) + TEE_COMMAND):
        print("Failed To Format Partition 1")
        sys.exit(1)
    print("Formatting Partiton 2...")
    if os.system("mkfs.ext4 -F -Lrecovery {0}{1}2".format(sdDev, sdPart) + TEE_COMMAND):
        print("Failed To Format Partition 2")
        sys.exit(1)

    if os.path.exists(os.path.join(MOUNT_POINT, "hobart", "SOFTWARE_VERSION_INFO")):
        logging.debug("recovery present from previous format. umount and remove directory...")
        print("recovery present from previous format. umount and remove directory...")
        os.system("umount {0}/hobart".format(MOUNT_POINT))
        os.system("umount {0}/recovery".format(MOUNT_POINT))
    print("Eject SD card from device and reinsert into reader/writer device")

    while 1:
        eval(input("Press <Enter> after card re-inserted and it mounts in your VM"))
        if os.path.ismount(os.path.join(MOUNT_POINT, "hobart")) and os.path.ismount(os.path.join(MOUNT_POINT, "recovery",)):
            logging.debug("Card Found")
            print("Card Found")
            break
        else:
            logging.debug("SD card not mounted yet")
            print("SD card not mounted yet, re-insert")

    logging.debug("SD card formatted and mounted...")
    print("SD card formatted and mounted...")
    logging.debug("Applying root file system to SD card.")
    print("Applying root file system to SD card.")
    p = subprocess.Popen("tar xvzf {0} -C {1}".format(HTI_BUILD_PATH, MOUNT_POINT) + TEE_COMMAND, shell=True,
                         stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, close_fds=True)
    (w, out, err) = (p.stdin, p.stdout, p.stderr)
    # print extraction output
    while True:
        line = out.readline().strip()
        print(line)
        if not line:
            break
        elif line.startswith("tar: "):
            os.system('pkill -TERM -P {0}'.format(p.pid))

    p.wait()
    logging.debug("tar exited with code {0}".format(p.returncode))
    print("Root file system applied...")
    # print errors if they occured
    if p.returncode:
        print("ERROR")
        print("Exiting")
        logging.error(err.read())
        os.system("umount {0}{1}1".format(sdDev, sdPart) + TEE_COMMAND)
        os.system("umount {0}{1}2".format(sdDev, sdPart) + TEE_COMMAND)
        sys.exit(1)
    os.system("sync")
    if not noOfficeInstall:
        os.system("cd /")
        logging.debug("Installing Office Viewer and PDF Viewer features ...")
        print("Installing Office Viewer and PDF Viewer features ...")
        p = subprocess.Popen("tar xvzf {0}/hobart/usr/local/hobart/preinstall/hti-linux-feature-office_viewer*.tgz -C {0}/hobart"
                             .format(MOUNT_POINT) + TEE_COMMAND, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, close_fds=True)
        (w, out, err) = (p.stdin, p.stdout, p.stderr)
        # print extraction output
        while True:
            line = out.readline().strip()
            print(line)
            if not line:
                break
            elif line.startswith("tar: "):
                os.system('pkill -TERM -P {0}'.format(p.pid))

        p.wait()
        print("Office Viewer Extracted...")
        # print errors if they occured
        if p.returncode:
            first = True
            while True:
                line = err.readline().strip()
                if not line:
                    if not first:
                        print("Exiting")
                        os.system("umount {0}{1}1".format(sdDev, sdPart) + TEE_COMMAND)
                        os.system("umount {0}{1}2".format(sdDev, sdPart) + TEE_COMMAND)
                        time.sleep(5)
                        sys.exit(1)
                    break
                else:
                    if first:
                        first = False
                        print("ERROR")
                    print(line)

        print("Extracting PDF viewer...")
        p = subprocess.Popen("tar xvzf {0}/hobart/usr/local/hobart/preinstall/hti-linux-feature-pdf_viewer*.tgz -C {0}/hobart"
                             .format(MOUNT_POINT) + TEE_COMMAND, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, close_fds=True)
        (w, out, err) = (p.stdin, p.stdout, p.stderr)
        # print extraction output
        while True:
            line = out.readline().strip()
            print(line)
            if not line:
                break
            elif line.startswith("tar: "):
                os.system('pkill -TERM -P {0}'.format(p.pid))

        p.wait()
        print("PDF Viewer Extracted...")
        # print errors if they occured
        if p.returncode:
            first = True
            while True:
                line = err.readline().strip()
                if not line:
                    if not first:
                        print("Exiting")
                        os.system("umount {0}{1}1".format(sdDev, sdPart) + TEE_COMMAND)
                        os.system("umount {0}{1}2".format(sdDev, sdPart) + TEE_COMMAND)
                        time.sleep(5)
                        sys.exit(1)
                    break
                else:
                    if first:
                        first = False
                        print("ERROR")
                    print(line)



        os.system("sudo touch {0}/hobart/var/lib/dpkg/status".format(MOUNT_POINT) + TEE_COMMAND)

        #os.makedirs(os.path.join(MOUNT_POINT, "hobart", "var", "lib", "dpkg", "status"))

        # There is an issue with dpkg 1.16 using the /var/lib/available on the sd card and not using the "available" file on the
        # black box as a workaround we copy the black box's available file and then delete it when we are done
        os.system("cp " + os.path.join(MOUNT_POINT, "hobart", "var", "lib", "dpkg", "available") + " " + os.path.join(MOUNT_POINT, "hobart", "var", "lib", "dpkg", "availableold") + TEE_COMMAND)
        exit = os.system("sudo cp /var/lib/dpkg/available " + os.path.join(MOUNT_POINT, "hobart", "var", "lib", "dpkg", "available") + TEE_COMMAND)
        if exit:
            print("Could not set up dpkg")
            time.sleep(5)
            sys.exit(1)
        else:
            print("Dpkg setup success")
        print("Installing Office Viewer...")
        # false if no errors found during installation of office viewer
        installFailedFlag = False
        # run install office python script

        #p = subprocess.Popen("sudo PYTHONPATH=$PYTHONPATH:/usr/local/hobart/python python /usr/local/hobart/python/installOfficeViewer.py {0}/hobart/ \"\""  .format(MOUNT_POINT) +
        #                     TEE_COMMAND, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
        #                    stderr=subprocess.PIPE, close_fds=True)
        retval = os.getcwd()
        os.chdir("/")
        # cant use until / is put before tmp in all installOfficeViewer.py
        p = subprocess.Popen("sudo PYTHONPATH=$PYTHONPATH:/usr/local/hobart/python python {0}/hobart/usr/local/hobart/python/installOfficeViewer.py {0}/hobart \"\""  .format(MOUNT_POINT) +
                             TEE_COMMAND, shell=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE, close_fds=True)

        (w, out, err) = (p.stdin, p.stdout, p.stderr)
        # print extraction output
        while True:
            line = out.readline().strip()
            print(line)
            if not line:
                break
            elif line.startswith("dpkg: error: "):
                os.system('pkill -TERM -P {0}'.format(p.pid))
                installFailedFlag = True

        print("Office Viewer Installation Complete")
        os.chdir(retval)
        #verify office viewer installation
        logging.debug("Verfiying OfficeViewer")
        if not os.path.exists(os.path.join(MOUNT_POINT, "hobart", "opt", "libreofficedev4.4", "LICENSE")):
            logging.error("opt/libreofficedev4.4/LICENSE not found")
            installFailedFlag = True
        # print errors if there are any
        if not os.path.exists(os.path.join(MOUNT_POINT, "hobart", "usr", "local", "hobart", "features", "pdf_viewer")):
            logging.error("usr/local/hobart/features/pdf_viewer not found")
            installFailedFlag = True
        p.wait()
        if installFailedFlag:
            print("Error Installing Office Viewer")
            logging.error("Subprocess exited with code {0}".format(p.returncode))
            os.system("umount {0}{1}1".format(sdDev, sdPart) + TEE_COMMAND)
            os.system("umount {0}{1}2".format(sdDev, sdPart) + TEE_COMMAND)
            time.sleep(5)
            sys.exit(1)

    # /usr/local/hobart/releases/Rel_3.2.0/hti-linux-usd-3.2.0-02092018_141624.tgz

    print("Verifying software version information...")
    installFailedFlag = False
    installFailedFlag = installFailedFlag or not checkVersionInfo(HTI_BUILD_PATH)
    if not os.path.exists(os.path.join(MOUNT_POINT, "hobart", "etc", "hobart")):
        installFailedFlag = True
        logging.error("/etc/hobart not found")

    FILES_TO_VERIFY = [os.path.join(MOUNT_POINT, "hobart", "usr", "bin", "scaleui"),
                       os.path.join(MOUNT_POINT, "hobart", "usr", "bin", "chobart_d"),
                       os.path.join(MOUNT_POINT, "hobart", "usr", "local", "hobart", "python", "startupSequence.py"),
                       os.path.join(MOUNT_POINT, "hobart", "usr", "local", "hobart", "python", "startScaleApplication.py")]
    for file in FILES_TO_VERIFY:
        if not os.path.isfile(file):
            installFailedFlag = True
            logging.error("{0} not found".format(file))
            break

    if installFailedFlag:
        logging.error("Verification Of Software Version Failed...")
        print("Verification Of Software Version Failed...")
        os.system("umount {0}{1}1".format(sdDev, sdPart) + TEE_COMMAND)
        os.system("umount {0}{1}2".format(sdDev, sdPart) + TEE_COMMAND)
        time.sleep(5)
        sys.exit(1)
    print("Cleaning up temp files...")
    logging.debug("Cleaning up temp files...")
    os.system("sudo rm -rf /tmp/Libre*" + TEE_COMMAND)
    os.system("sudo rm -f {0}/hobart/usr/local/hobart/preinstall/hti-linux-feature-office_viewer*.* {0}/hobart/usr/local/hobart/preinstall/hti-linux-feature-pdf_viewer*.*".format(MOUNT_POINT) + TEE_COMMAND)
    os.system("sync" + TEE_COMMAND)
    os.system("umount {0}{1}1".format(sdDev, sdPart) + TEE_COMMAND)
    os.system("umount {0}{1}2".format(sdDev,sdPart) + TEE_COMMAND)
    logging.debug("SD card is ready for target...")
    logging.debug("Masterblaster Finished")
    print("SD card is ready for target...")

if __name__ == "__main__":
    # -n Skips the install office viewer section
    parser.add_option('-n', '--noOffice', action="store_true", dest="no", help="Does not install officeViewer nor PDFViewer")
    options, args = parser.parse_args()
    main(noOfficeInstall=options.no)
