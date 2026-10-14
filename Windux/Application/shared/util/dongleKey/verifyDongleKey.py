import usbQuery
import crypto
import sys
import os
import file_io
import keyInfo
import argparse

deviceInfo =[]

# get serial number from file on device
def getSerialNumber(filename, key):
    # try and read file from the usb storage device
    try:
        status, data = file_io.readFileIntoBuff(filename,'rb')
        decrypted = crypto.decrypt(key,data)
        decryptedKey = decrypted.upper()
        return ("ok", decryptedKey)
    except:
        return("error", None)

def isKeyFound(keyType='service'):
    key, key_filename = keyInfo.getKeyInfo(keyType)

    if key == None or key_filename == None:
        print('invalid argument key type')
        return False

    i = 0
    usbDevices = usbQuery.UsbDeviceQuery()
    devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_USB)
    foundKey = False

    for dev in devices:
        i = i + 1
        #print ''
        #print 'USB Device ', i
        #print usbDevices.getDeviceId(dev)
        #print usbDevices.getStorageType(dev)
        #print usbDevices.getVendorName(dev)
        #print usbDevices.getProductDescription(dev)
        try:
            serialNumber = usbDevices.getSerialNumber(dev)
            #print serialNumber
            deviceLetter = usbDevices.getDeviceLetter(dev)
            #print deviceLetter
            deviceInfo.append({'device letter' : deviceLetter, 'serial number' : serialNumber})
            keyFile = os.path.join(deviceLetter,key_filename)
            #print keyFile
            if os.path.exists(keyFile):
                status, data = getSerialNumber(keyFile, key)

                if status == "ok":
                    #print data
                    #print serialNumber
                    if data == serialNumber:
                        foundKey = True
                else:
                    pass
            else:
                pass
        except:
            pass

    return foundKey

if __name__ == "__main__":
    exitCode = 1
    parser = argparse.ArgumentParser(description="detects if usb thumb drive contains key and is valid")
    parser.add_argument("keyType",
                        action="store",
                        choices=[keyInfo.service_key_name,
                                 keyInfo.manufacturing_key_name,
                                 keyInfo.selfservice_key_name,
                                 keyInfo.supervisor_key_name,
                                 keyInfo.systemtest_key_name,
                                 'all'],
                        help="enter key type to check for on usb thumb drive")

    args = parser.parse_args()

    if args.keyType == 'all':
        allFound = 0

        for i in range(0,len(keyInfo.all_keys)):
            if isKeyFound(keyInfo.all_keys[i]):
                print('found key %s' % (keyInfo.all_keys[i]))
                allFound = allFound + 1

        if allFound == len(keyInfo.all_keys):
            exitCode = 0

    else:
        if isKeyFound(args.keyType):
            print('found key %s' % (args.keyType))
            exitCode = 0

    sys.exit(exitCode)
