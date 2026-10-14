# create usb service key

import crypto
import os
import sys
import keyInfo
import usbQuery
import string
import file_io

while True:
    input = raw_input("Enter dongle key type to create [service=1, manufacturing=2, supervisor=3, selfservice=4]: ")
    try:
        input = int(string.strip(input))
        if input >=1 and input <= 4:
            dongleKeyType = input
            break
    except:
        pass
        
deviceInfo = []
print ''
print 'Insert USB thumb drive that will be converted to USB Dongle Key into a USB port.'
print 'Remove ALL other USB devices connected to this computer to simplify the process.'
print ''

enterPressed = raw_input("Press <ENTER> once device is mounted under Windows Explorer.")

print ''
print 'Scanning all attached USB Devices:'
usbDevices = usbQuery.UsbDeviceQuery()
devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_USB)
i = 0
for dev in devices:
    i = i + 1
    print ''
    print 'USB Device Number: ', i
    #print usbDevices.getDeviceId(dev)
    print '  Storage Type: ', usbDevices.getStorageType(dev)
    print '  Vendor Name: ',usbDevices.getVendorName(dev)
    print '  Product Description: ', usbDevices.getProductDescription(dev)
    serialNumber = usbDevices.getSerialNumber(dev)
    if serialNumber == None or string.strip(serialNumber) == '':
        print '  Serial Number: Device not suitable for service key creation'
    else:
        print '  Serial Number: ', serialNumber
    deviceLetter = usbDevices.getDeviceLetter(dev)
    print '  Device Letter: ', deviceLetter
    deviceInfo.append({'device letter' : deviceLetter, 'serial number' : serialNumber})

if len(deviceInfo) == 0:
    print ''
    print 'No attached USB devices, please insert USB key to convert'
    print 'Dongle Key creation aborted'
    sys.exit(1)
    
while 1:
    print ''
    answer = raw_input("Enter USB Device Number from list above of the\nUSB thumb drive you want to convert to Dongle Key\n\nNOTE: Examine info provided for each attached USB device\nto assist in your response\nIn most cases the response should be 1\nEnter 0 to abort: ")
    try:
        deviceNum = int(answer)
        break
    except:
        pass
        
if deviceNum == 0:
    print ''
    print 'Dongle Key creation aborted by user'
    sys.exit(1)

if string.strip(deviceInfo[deviceNum-1]['serial number']) == "":
    print ''
    print 'USB device selected has no serial number'
    print 'Did you verify drive mounted under Windows Explorer before'
    print 'pressing <ENTER> above?'
    print 'Dongle Key creation aborted.'
    sys.exit(1)

if string.strip(deviceInfo[deviceNum-1]['device letter']) == "":
    print ''
    print 'USB device selected has no logical device letter assigned'
    print 'Did you verify drive mounted under Windows Explorer before'
    print 'pressing <ENTER> above?'
    print 'Dongle Key creation aborted.'
    sys.exit(1)
    
if dongleKeyType == 1:
    key = keyInfo.service_key
    key_filename = keyInfo.service_filename
elif dongleKeyType == 2:
    key = keyInfo.manufacturing_key
    key_filename = keyInfo.manufacturing_filename
elif dongleKeyType == 3:
    key = keyInfo.supervisor_key
    key_filename = keyInfo.supervisor_filename
elif dongleKeyType == 4:
    key = keyInfo.selfservice_key
    key_filename = keyInfo.selfservice_filename
    
if os.path.exists(os.path.join(deviceInfo[deviceNum-1]['device letter'])):
    
    try:
        data = crypto.encrypt(key, deviceInfo[deviceNum-1]['serial number'])

        status, result = file_io.writeBufferToFile(os.path.join(deviceInfo[deviceNum-1]['device letter'],key_filename),data,'wb')

        if status == 'success':
            print ''
            print 'Dongle Key created.'
            print 'Eject USB thumb drive from Windows OS'
            print 'DO NOT simply pull the thumb drive out of the port'
            status_code = 0
        else:
            print ''
            print 'Can not write to dongle key file to device'
            print result
            print 'Is the USB key write-protected?'
            print 'Dongle Key creation aborted'
            status_code = 1
    except:
        print ''
        print 'Issue creating dongle key file'
        print result
        print 'Is the USB key write-protected?'
        print 'Dongle Key creation aborted'
        status_code = 1
    
    sys.exit(status_code)
    