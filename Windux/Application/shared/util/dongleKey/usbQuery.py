import sys
import os
import string

if os.name == 'posix':
    import dbus
else:
    import wmi

class UsbDeviceQuery:

    DEVICE_TYPE_USB = 'USB'
    DEVICE_TYPE_ALL = '*'

    def __init__(self):
        if os.name == 'posix':
            self.deviceList = {}
            self.mountList = {}
            self.bus = None

    def splitPNPDeviceID(self,device,tokenNumber=-2):
        if os.name == 'nt':
            tokens = device.PNPDeviceID.split('&')
            return tokens[tokenNumber]
        elif os.name == 'posix':
            pass

    def findDeviceBasedOnMount(self, mountPoint):
        for curdevice in self.deviceList.keys():
            if curdevice == mountPoint[:-1]:
                return curdevice
        return None

    def findDevicesInClass(self, device_class=DEVICE_TYPE_USB):
        if os.name == 'nt':
            # TODO: only supports USB devices at this time under Windows
            c = wmi.WMI ()
            usbDevices = c.Win32_DiskDrive(InterfaceType=device_class)
            return usbDevices

        elif os.name == 'posix':
            BLOCK_IFACE = 'org.freedesktop.UDisks2.Block'
            DRIVE_IFACE = 'org.freedesktop.UDisks2.Drive'
            FILESYSTEM_IFACE = 'org.freedesktop.UDisks2.Filesystem'

            # retrieve dbus interface
            self.bus = dbus.SystemBus()
            ud_manager_obj = self.bus.get_object("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2")
            om = dbus.Interface(ud_manager_obj, 'org.freedesktop.DBus.ObjectManager')

            # iterate objects
            objects = om.GetManagedObjects()

            for k in objects.keys():

                interfaces = [str(x) for x in objects[k].keys()]

                # get device name
                if BLOCK_IFACE not in interfaces:
                    continue

                device_b = objects[k][BLOCK_IFACE]['Device']
                device = ''.join([chr(x) for x in device_b if x])

                if 0 == len(device):
                    continue

                # retrieve drive information
                drive = objects[k][BLOCK_IFACE]['Drive']

                if drive not in objects.keys():
                    continue

                drive_interfaces = [str(x) for x in objects[drive].keys()]

                if DRIVE_IFACE not in drive_interfaces:
                    continue

                vendor = str(objects[drive][DRIVE_IFACE]['Vendor'])
                model = str(objects[drive][DRIVE_IFACE]['Model'])
                interface = str(objects[drive][DRIVE_IFACE]['ConnectionBus'])
                serial_number = str(objects[drive][DRIVE_IFACE]['Serial'])
                media = str(objects[drive][DRIVE_IFACE]['MediaCompatibility'])

                if objects[drive][DRIVE_IFACE]['Removable']:
                    storageType = 'removable'
                else:
                    storageType = 'fixed'

                if 'usb' != interface:
                    continue

                # retrieve filesystem
                mount_points = []

                if FILESYSTEM_IFACE in interfaces:
                    for mount_point_b in objects[k][FILESYSTEM_IFACE]['MountPoints']:
                        mount_point = {}
                        mount_point['mount_point'] = ''.join([chr(x) for x in mount_point_b if x])
                        mount_point['partition'] = device

                        mount_points.append( mount_point )

                self.deviceList[device] = {
                    'device' : device,
                    'mount_points' : mount_points,
                    'vendor' : vendor,
                    'description' : model,
                    'media' : media,
                    'storageType' : storageType,
                    'interface' : interface,
                    'serial_number' : serial_number }

            #print self.deviceList

            if device_class == '*':
                return self.deviceList

            # filter device list to requested class
            filteredDeviceList = {}

            for k in self.deviceList.keys():
                if self.deviceList[k]['interface'] == device_class.lower():
                    filteredDeviceList[k] = self.deviceList[k]

            return filteredDeviceList

    def getDeviceId(self, device):
        if os.name == 'nt':
            return device.PNPDeviceID
        elif os.name == 'posix':
            return device

    # only relevant for posix OS
    def getMountPoints(self, device):
        if os.name == 'nt':
            return []
        elif os.name == 'posix':
            return self.deviceList[device]['mount_points']

    # getDeviceLetter on posix system returns mount point of device not
    # device letter like on windows
    def getDeviceLetter(self, device):
        if os.name == 'nt':
            physical_disk = device
            for partition in physical_disk.associators ("Win32_DiskDriveToDiskPartition"):
                for logical_disk in partition.associators ("Win32_LogicalDiskToPartition"):
                    return logical_disk.Caption
        elif os.name == 'posix':
            try:
                mountPoint = self.deviceList[device]['mount_points'][0]['mount_point']
            except:
                mountPoint = None
            return mountPoint

    def getVendorName(self, device):
        if os.name == 'nt':
            vendor = self.splitPNPDeviceID(device,1)
            return vendor[4:]
        elif os.name == 'posix':
            return self.deviceList[device]['vendor']

    def getStorageType(self, device):
        if os.name == 'nt':
            dataItem = self.splitPNPDeviceID(device,0)
            tokens = dataItem.split('\\')
            return tokens[1]
        elif os.name == 'posix':
            return self.deviceList[device]['storageType']

    def getProductDescription(self, device):
        if os.name == 'nt':
            prodDesc = self.splitPNPDeviceID(device,2)
            return prodDesc[5:]
        elif os.name == 'posix':
            return self.deviceList[device]['description']

    def getConnectionInterface(self, device):
        if os.name == 'posix':
            return self.deviceList[device]['interface']
        else:
            return None

    def getSerialNumber(self, device):
        if os.name == 'nt':
            dataItem = self.splitPNPDeviceID(device, -2)
            tokens = dataItem.split('\\')
            if len(tokens) > 1:
                return tokens[1].upper()
            else:
                return tokens[0].upper()
        elif os.name == 'posix':
            return self.deviceList[device]['serial_number'].upper()


if __name__ == "__main__":
    usbDevices = UsbDeviceQuery()
    if os.name == 'nt':
        devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_USB)
    else:
        devices = usbDevices.findDevicesInClass(usbDevices.DEVICE_TYPE_ALL)
    i = 0
    for dev in devices:
        i = i + 1
        print('')
        print('Device %d' % i)
        print('Device Id: %s' % usbDevices.getDeviceId(dev))
        print('Storage Type: %s' % usbDevices.getStorageType(dev))
        print('Vendor Name: %s' % usbDevices.getVendorName(dev))
        print('Description: %s' % usbDevices.getProductDescription(dev))
        print('Serial Number: %s' % usbDevices.getSerialNumber(dev))
        print('Device Letter: %s' % usbDevices.getDeviceLetter(dev))
        print('Mount Points:')
        mountPoints = usbDevices.getMountPoints(dev)
        for i in range(0, len(mountPoints)):
            print('  Parition: %s' % mountPoints[i]['partition'])
            print('  Mount Point: %s' % mountPoints[i]['mount_point'])
        print('Interface: %s' % usbDevices.getConnectionInterface(dev))
