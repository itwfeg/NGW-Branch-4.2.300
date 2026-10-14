#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import shellCommand
import argparse

if os.name == 'posix':
    wiredDevice = 'eth0'
    wirelessDevice = 'wlan0'
elif os.name == 'nt':
    wiredDevice = 'Local Area Connection'
    wirelessDevice = 'Wireless Network Connection'

def setInterfaceState(name, state):
    cmd = []
    if os.name == 'posix':
        cmd.append("ifconfig")
        cmd.append(name)
        cmd.append(state)
        exitCode,stdout,stderr = shellCommand.execute(cmd)
        return (exitCode,stdout,stderr)

    elif os.name == 'nt':
        try:
            import wmi
            c = wmi.WMI()
            n = c.query("select * from Win32_NetworkAdapter where NetConnectionID = '" + name + "'")[0]
            if state == 'up':
                n.Enable()
            else:
                n.Disable()
        except:
            return (1,  "",  'Unable to set the state of "' + name + '" to "' + state + '"')
        return (0,  '"' + name + '" is now "' + state + '"',  "")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="control interface associated with network device")
    parser.add_argument("interface",
                        action="store",
                        choices=['wifi','wire','both'],
                        help="specify interface device, ex. wifi, wire or both")

    parser.add_argument("control",
                        action="store",
                        choices=['up','down'],
                        help="specify up to bring device on-line or down to remove")

    args = parser.parse_args()

    if args.interface == 'wifi':
        exitCode, stdout, stderr = setInterfaceState(wirelessDevice,args.control)
    elif args.interface == 'wire':
        exitCode, stdout, stderr = setInterfaceState(wiredDevice,args.control)
    elif args.interface == 'both':
        exitCode, stdout, stderr = setInterfaceState(wiredDevice, args.control)
        exitCode, stdout, stderr = setInterfaceState(wirelessDevice, args.control)
    sys.exit(exitCode)
