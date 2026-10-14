#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
if os.name == 'posix':
    import pyudev
import re
import osUtils
import shellCommand
import sendDeviceEventMessage
import verifyDongleKey

if (os.path.exists(os.path.join("/","etc","hobart","slicer"))):
    import indicateKeyEntered
import keyInfo
import time

mountLookup = {}
mountLookup['sda1'] = "/media/thumb"
mountLookup['sdb1'] = "/media/thumb1"
mountLookup['sdc1'] = "/media/thumb2"
mountLookup['sdd1'] = "/media/thumb3"

# handle thumb drives that do not have a boot sector
# thumb drives without boot sectors will never have a valid
# key since they are not discovered by verifyDongleKey
mountLookup['sda'] = "/media/thumb"
mountLookup['sdb'] = "/media/thumb1"
mountLookup['sdc'] = "/media/thumb2"
mountLookup['sdd'] = "/media/thumb3"

flashDrive = re.compile('^sd[a-d]1*$')


def removeStaleMounts():
    # remove any /media/thumb* directories on uSd at startup of this script
    for i in list(mountLookup.keys()):
        if os.path.exists(mountLookup[i]):
            # remove directory recursively
            osUtils.rmDestPath(mountLookup[i])
    os.system("sync")
    
def setKeyMask():
    keyMask = 0
    for i in range(0,len(keyInfo.all_keys)):
        if verifyDongleKey.isKeyFound(keyInfo.all_keys[i]):
            #print keyInfo.all_keys[i]
            if keyInfo.all_keys[i] == keyInfo.supervisor_key_name:
                keyMask = keyMask | 1
            elif keyInfo.all_keys[i] == keyInfo.service_key_name:
                keyMask = keyMask | 4
            elif keyInfo.all_keys[i] == keyInfo.selfservice_key_name:
                keyMask = keyMask | 8
            elif keyInfo.all_keys[i] == keyInfo.systemtest_key_name:
                keyMask = keyMask | 16
            elif keyInfo.all_keys[i] == keyInfo.manufacturing_key_name:
                keyMask = keyMask | 32
    return keyMask

def handle_device_event(action,device):

    print('background event {0}: {1}'.format(action,device))

    if action == 'add':

        print('device is %s' % (device.device_path))
        deviceShortName = os.path.basename(device.device_path)

        if deviceShortName not in mountLookup.keys():
            print('unsupported device')
            return

        if flashDrive.match(deviceShortName) != None:

            # remove all stale thumb mount points before adding
            #removeStaleMounts()
            
            print("device match found")
            # if mount point does not exist
            if not os.path.exists(mountLookup[deviceShortName]):
                #print 'creating directory for mount point'
                # create directory for mount point
                print('making destination directory')
                osUtils.mkDestPath(mountLookup[deviceShortName])
                print('syncing filesystem')
                os.system("sync")

            # mount the drive
            cmd = []
            cmd.append("/bin/mount")
            cmd.append("/dev/%s" % (deviceShortName))
            cmd.append(mountLookup[deviceShortName])
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            print('mounted drive')
            
            #print 'flash drive %s mounted' % (mountLookup[deviceShortName])
            # set key mask for all thumb devices found
            try:
                keyMask = setKeyMask()
            except:
                keyMask = 0
            print(('keyMask is %d' % (keyMask)))

            if (os.path.exists(os.path.join("/","etc","hobart","slicer"))):
                indicateKeyEntered.modifyKeyFiles(keyMask)
 
            try:
                message = "RTEA" + chr(0x1F) + "ky" + str(keyMask) + chr(0x1E) + "RTFF" + chr(0x1E)
                sendDeviceEventMessage.sendMessage(server='localhost',port=6000,message=message)
            except:
                pass
                
    elif action == 'remove':

        print('device is %s' % (device.device_path))
        deviceShortName = os.path.basename(device.device_path)

        if deviceShortName not in mountLookup.keys():
            print('unsupported device')
            return

        if flashDrive.match(deviceShortName):

            try:
                # set key mask for all thumb devices found
                keyMask = setKeyMask()
            except:
                # no USB keys inserted if this happens since
                # USB bus is rescanned for USB block devices and 
                # there will not be any if no USB devices plugged in
                keyMask = 0

            print(('keyMask is %d' % (keyMask)))

            if (os.path.exists(os.path.join("/","etc","hobart","slicer"))):
                indicateKeyEntered.modifyKeyFiles(keyMask)

            message = "RTEA" + chr(0x1F) + "ky" + str(keyMask) + chr(0x1E) + "RTFF" + chr(0x1E)

            try:
                sendDeviceEventMessage.sendMessage(server='localhost',port=6000,message=message)
            except:
                pass
                
            print((mountLookup[deviceShortName]))

            # if mount point still exists remove it
            if os.path.exists(mountLookup[deviceShortName]):
                # try to unmount
                print((mountLookup[deviceShortName]))
                cmd = []
                cmd.append("/bin/umount")
                # Lazy umount and should be removed but tried when chobart_d was leaving files open and thumb drive remained busy
                # cmd.append("-l")
                cmd.append(mountLookup[deviceShortName])
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                
                print(('flash drive %s unmounted' % (mountLookup[deviceShortName])))

                os.system("sync")

                # must wait until unmount succeeds before removing directory
                time.sleep(4)
                
                # remove directory recursively
                osUtils.rmDestPath(mountLookup[deviceShortName])
                print('removing directory mounted')
                os.system("sync")

            # remove any directory that is not mounted
            # if it is mounted remove will fail
            #removeStaleMounts()
            
def nt_handle_device_event(event):
    
    if event.EventType == 2 or event.EventType == 3:
        #print 'flash drive %s mounted' % (mountLookup[deviceShortName])
        # set key mask for all thumb devices found
        try:
            keyMask = setKeyMask()
        except:
            keyMask = 0
        print(('keyMask is %d' % (keyMask)))

        try:
            message = "RTEA" + chr(0x1F) + "ky" + str(keyMask) + chr(0x1E) + "RTFF" + chr(0x1E)
            sendDeviceEventMessage.sendMessage(server='localhost',port=6000,message=message)
        except:
            pass

if os.name == 'posix':
    removeStaleMounts()

    context = pyudev.Context()
    monitor= pyudev.Monitor.from_netlink(context)
    monitor.filter_by(subsystem='block')
    observer = pyudev.MonitorObserver(monitor, handle_device_event, name='monitor-observer')
    observer.daemon = False
    observer.start()
    
elif os.name == 'nt':
    import wmi;
    c = wmi.WMI()
    eventMonitor = c.Win32_VolumeChangeEvent.watch_for()
    while True:
        event = eventMonitor()
        nt_handle_device_event(event)
