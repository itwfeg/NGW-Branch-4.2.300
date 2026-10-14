import usb.core
import usb.util
import sys
import os
import time
import shellCommand
import messageBox


if os.name == 'posix':
    import pyudev


class hobartScanner:
    def __init__(self):
        if os.name == 'posix':
            self.idVendor = 0x05e0
            self.up_idProduct = 0x1900          #unprogrammed camera product id 
            self.p_idProduct = 0x1701           #programmed camera product id
            self.mb_message1 = "Programmed Hobart Scanner Detected"
            self.mb_message2 = "Unprogrammed Hobart Scanner Detected"
            self.mb_message3 = "No Scanner Device Detected"
        
    def scanBus(self):
        #find all hobart camera modules 
        deviceList = usb.core.find(find_all=True,  idVendor=0x05e0, idProduct=0x1701)
        if len(deviceList) != 0:
            exitCode,  id = messageBox.display(message=self.mb_message1,   timer=5000)
            if exitCode != 0:
                print "Message box command failed for message 1!"
        else:
            deviceList = usb.core.find(find_all=True,  idVendor=0x05e0, idProduct=0x1900)
            if len(deviceList) != 0:
                exitCode,  id = messageBox.display(message=self.mb_message2,   timer=5000)
                if exitCode != 0:
                    print "Message box command failed for message 2!"
            else:
                exitCode,  id = messageBox.display(message=self.mb_message3,   timer=5000)
                if exitCode != 0:
                    print "Message box command failed for message 2!"
                
def handle_device_event(action, device):
    print 'device is %s' % (device.device_path)
    scanner = hobartScanner()
    if action == 'add':
        scanner.scanBus()
    elif action == 'remove':
        exitCode,  id = messageBox.display(message=" Scanner Test Ready \n Please Insert Device",   timer=5000)
        if exitCode != 0:
            print "Message box command failed for message!"
            
            
def main( ):
    scanner = hobartScanner()
    if os.name == 'posix':
        exitCode,  id = messageBox.display(message=" Scanner Test Ready \n Please Insert Device",   timer=5000)
        # setup to monitor system events
        context = pyudev.Context()
        monitor= pyudev.Monitor.from_netlink(context)
        monitor.filter_by(subsystem='usb')
        observer = pyudev.MonitorObserver(monitor, handle_device_event, name='monitor-observer')
        observer.daemon = False
        observer.start()
        while 1:
            time.sleep(1)

if __name__ == "__main__":
    main()
