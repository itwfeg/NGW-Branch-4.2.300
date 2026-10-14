import usbQuery
import crypto
import sys
import os
import file_io
import keyInfo
import argparse

deviceInfo = []


# get serial number from file on device
def get_serial_number(filename, key):
    # try and read file from the usb storage device
    try:
        status, data = file_io.read_file_into_buff(filename,'rb')
        decrypted_key = crypto.decrypt(key, data).upper()
        return "ok", decrypted_key
    except:
        return"error", None


def is_key_found(key_type='service'):

    key, key_filename = keyInfo.get_key_info(key_type)

    if key is None or key_filename is None:
        print('invalid argument key type')
        return False
    
    usb_devices = usbQuery.UsbDeviceQuery()
    devices = usb_devices.find_devices_in_class(usb_devices.DEVICE_TYPE_USB)
    i = 0
    found_key = False
    for dev in devices:
        i = i + 1
        # print ''
        # print 'USB Device ', i
        # print usb_devices.getDeviceId(dev)
        # print usb_devices.getStorageType(dev)
        # print usb_devices.getVendorName(dev)
        # print usb_devices.getProductDescription(dev)
        try:
            serial_number = usb_devices.get_serial_number(dev)
            # print serial_number
            device_letter = usb_devices.get_device_letter(dev)
            # print device_letter
            deviceInfo.append({'device letter': device_letter, 'serial number': serial_number})

            key_file = os.path.join(device_letter, key_filename)
            # print key_file
            if os.path.exists(key_file):
                status, data = get_serial_number(key_file, key)
                if status == "ok":
                    # print data
                    # print serial_number
                    if data == serial_number:
                        found_key = True
                else:
                    pass
            else:
                pass
        except:
            pass

    return found_key


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
        for i in range(0, len(keyInfo.all_keys)):
            if is_key_found(keyInfo.all_keys[i]):
                print('found key %s' % (keyInfo.all_keys[i]))
                allFound = allFound + 1
        if allFound == len(keyInfo.all_keys):
            exitCode = 0

    else:
        if is_key_found(args.keyType):
            print('found key %s' % args.keyType)
            exitCode = 0

    sys.exit(exitCode)
