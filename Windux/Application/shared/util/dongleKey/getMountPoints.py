import dbus

BLOCK_IFACE = 'org.freedesktop.UDisks2.Block'
DRIVE_IFACE = 'org.freedesktop.UDisks2.Drive'
FILESYSTEM_IFACE = 'org.freedesktop.UDisks2.Filesystem'
PARTITION_IFACE = 'org.freedesktop.UDisks2.Partition'

# retrieve dbus interface
bus = dbus.SystemBus()
ud_manager_obj = bus.get_object("org.freedesktop.UDisks2", "/org/freedesktop/UDisks2")
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

    print str(objects[drive][DRIVE_IFACE]['Vendor'])

    if FILESYSTEM_IFACE in interfaces:
        for mount_point_b in objects[k][FILESYSTEM_IFACE]['MountPoints']:
            print ''.join([chr(x) for x in mount_point_b if x])

    print str(objects[drive][DRIVE_IFACE]['Serial'])

    if PARTITION_IFACE in interfaces:
        print int(objects[k][PARTITION_IFACE]['Size'])

