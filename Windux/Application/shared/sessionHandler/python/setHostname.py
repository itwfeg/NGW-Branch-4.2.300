#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys, os, string
import argparse

def setHostname(hostname):
    exitCode = 1
    if os.name == 'posix':
        import shellCommand
        # LINUX OS
        try:
            # now update /etc/hostname for permanent change to take place
            hostnameFileHandle = open("/etc/hostname", 'w')
            hostnameFileHandle.write(hostname+ "\n")
            hostnameFileHandle.close()
            cmd = []
            cmd.append("hostname")
            cmd.append("-F")
            cmd.append('/etc/hostname')
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        except:
            exitCode = 1
            
    elif os.name == 'nt':
        # For Windows, FBWF must be turned off to apply this change, requiring a double reboot
        # Qt does the initial reboot because the UI informs the User that it is about to happen.
        # Upon reboot, startupSequence detects the update cmd file and runs it
        # The cmd file applies the change, turns on FBWF and reboots again
        
        import wmi
        import configInfo
        import updateFBWF
        pythonInstallPath=configInfo.getPythonInstallPath()
        pythonPath=os.path.join(configInfo.getRootFolder(), "python")
        try:
            c = wmi.WMI()
            s = c.query("select * from Win32_ComputerSystem")[0]
            exitCode = s.Rename(hostname,  None,  None)[0]

            cmd = []
            cmd.append(os.path.join(pythonInstallPath, "pythonw"))
            cmd.append(os.path.join(pythonPath, "setHostname.py"))
            cmd.append(hostname)

            updateFBWF.update(cmd, "hostname")

        except:
            exitCode = 1
            
        # the above command requires a reboot is this being done in Qt since
        # it is not in this script to do that
        # Qt does the reboot because the UI informs the User that it is about to happen.

    return exitCode

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="set computing system hostname")
    parser.add_argument("hostname",
                        action="store", 
                        help="valid hostname for operating system")

    args = parser.parse_args()

    exitCode  = setHostname(args.hostname)
    sys.exit(exitCode)
