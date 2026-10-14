
import os

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

    def split_pnp_device_id(self, device, token_number=-2):
        if os.name == 'nt':
            tokens = device.PNPDeviceID.split('&')
            return tokens[token_number]
        elif os.name == 'posix':
            pass

    def find_device_based_on_mount(self, mount_point):
        for curdevice in self.deviceList.keys():
            if curdevice == mount_point[:-1]:
                return curdevice
        return None

    def find_devices_in_class(self, device_class=DEVICE_TYPE_USB):
        if os.name == 'nt':
            # TODO: only supports USB devices at this time under Windows
            c = wmi.WMI()
            usb_devices = c.Win32_DiskDrive(InterfaceType=device_class)
            return usb_devices

        elif os.name == 'posix':
            self.bus = dbus.SystemBus()
            ud_manager_obj = self.bus.get_object("org.freedesktop.UDisks", "/org/freedesktop/UDisks")
            ud_manager = dbus.Interface(ud_manager_obj, 'org.freedesktop.UDisks')
            devices = ud_manager.EnumerateDevices()
            for dev in devices:
                device_obj = self.bus.get_object("org.freedesktop.UDisks", dev)
                device_props = dbus.Interface(device_obj, dbus.PROPERTIES_IFACE)
                if device_props.Get('org.freedesktop.UDisks.Device', "DeviceIsDrive"):
                    vendor = device_props.Get('org.freedesktop.UDisks.Device', "DriveVendor")
                    model = device_props.Get('org.freedesktop.UDisks.Device', "DriveModel")
                    interface = device_props.Get('org.freedesktop.UDisks.Device', "DriveConnectionInterface")
                    serial_number = device_props.Get('org.freedesktop.UDisks.Device', "DriveSerial")
                    media = device_props.Get('org.freedesktop.UDisks.Device', "DriveMediaCompatibility")
                    if device_props.Get('org.freedesktop.UDisks.Device', "DeviceIsRemovable"):
                        storageType = 'Removable'
                    else:
                        storageType = 'Fixed'
                    test = device_props.Get('org.freedesktop.UDisks.Device', 'DeviceFile')
                    self.deviceList[device_props.Get('org.freedesktop.UDisks.Device', "DeviceFile")]  = {  
                        'device' : dev, 
                        'mount_points' : [],
                        'vendor' : str(vendor),
                        'description' : str(model), 
                        'media' : media,
                        'storageType' : storageType,
                        'interface' : str(interface),
                        'serial_number' : str(serial_number)}
                elif device_props.Get('org.freedesktop.UDisks.Device', "DeviceIsMounted"):
                    self.mountList[str(device_props.Get('org.freedesktop.UDisks.Device', "DeviceFile"))] = {'mount_points' : str(device_props.Get('org.freedesktop.UDisks.Device', "DeviceMountPaths")[0])}

            # now we have to attach mount points to device entries
            for dev in self.mountList.keys():
                #print dev
                #print self.mountList[dev]['mount_points']
                if dev[:-1] in self.deviceList.keys():
                    self.deviceList[dev[:-1]]['mount_points']  = self.deviceList[dev[:-1]]['mount_points']  + [{'partition' : dev,  'mount_point' : self.mountList[dev]['mount_points']}]
 
            # print self.deviceList

            if device_class == '*':
                return self.deviceList
            else:
                newList = {}
                for i in self.deviceList.keys():
                    if self.deviceList[i]['interface'] == device_class.lower():
                        newList[i] = self.deviceList[i]
                return newList
                
    def get_device_id(self, device):
        if os.name == 'nt':
            return device.PNPDeviceID
        elif os.name == 'posix':
            return device

    # only relevant for posix OS
    def get_mount_points(self, device):
        if os.name == 'nt':
            return []
        elif os.name == 'posix':
            return self.deviceList[device]['mount_points']
        
    # getDeviceLetter on posix system returns mount point of device not
    # device letter like on windows
    def get_device_letter(self, device):
        if os.name == 'nt':
            physical_disk = device
            for partition in physical_disk.associators ("Win32_DiskDriveToDiskPartition"):
                for logical_disk in partition.associators ("Win32_LogicalDiskToPartition"):
                    return logical_disk.Caption
        elif os.name == 'posix':
            try:
                mount_point = self.deviceList[device]['mount_points'][0]['mount_point']
            except:
                mount_point = None
            return mount_point
        
    def get_vendor_name(self, device):
        if os.name == 'nt':
            vendor = self.split_pnp_device_id(device, 1)
            return vendor[4:]
        elif os.name == 'posix':
            return self.deviceList[device]['vendor']

    def get_storage_type(self, device):
        if os.name == 'nt':
            data_item = self.split_pnp_device_id(device, 0)
            tokens = data_item.split('\\')
            return tokens[1]
        elif os.name == 'posix':
            return self.deviceList[device]['storageType']
            
    def get_product_description(self, device):
        if os.name == 'nt':
            prod_desc = self.split_pnp_device_id(device, 2)
            return prod_desc[5:]
        elif os.name == 'posix':
            return self.deviceList[device]['description']

    def get_connection_interface(self, device):
        if os.name == 'posix':
            return self.deviceList[device]['interface']
        else:
            return None

    def get_serial_number(self, device):
        if os.name == 'nt':
            data_item = self.split_pnp_device_id(device, -2)
            tokens = data_item.split('\\')
            if len(tokens) > 1:
                return tokens[1].upper()
            else:
                return tokens[0].upper()
        elif os.name == 'posix':
            return self.deviceList[device]['serial_number'].upper()

if __name__ == "__main__":
    usbDevices = UsbDeviceQuery()
    if os.name == 'nt':
        devices = usbDevices.find_devices_in_class(usbDevices.DEVICE_TYPE_USB)
    else:
        devices = usbDevices.find_devices_in_class(usbDevices.DEVICE_TYPE_ALL)
    i = 0
    for dev in devices:
        i = i + 1
        print('')
        print('Device %d' % i)
        print('Device Id: %s' % usbDevices.get_device_id(dev))
        print('Storage Type: %s' % usbDevices.get_storage_type(dev))
        print('Vendor Name: %s' % usbDevices.get_vendor_name(dev))
        print('Description: %s' % usbDevices.get_product_description(dev))
        print('Serial Number: %s' % usbDevices.get_serial_number(dev))
        print('Device Letter: %s' % usbDevices.get_device_letter(dev))
        print('Mount Points:')
        mountPoints = usbDevices.get_mount_points(dev)
        for i in range(0, len(mountPoints)):
            print('  Parition: %s' % mountPoints[i]['partition'])
            print('  Mount Point: %s' % mountPoints[i]['mount_point'])
        print('Interface: %s' % usbDevices.get_connection_interface(dev))
