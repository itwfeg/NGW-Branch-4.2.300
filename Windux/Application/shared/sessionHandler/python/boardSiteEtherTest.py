#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import shellCommand
    
if __name__ == "__main__":
    exitCode = 0
    
    # check if the network cable is plugged in
    cmd = []
    cmd.append(os.path.join("/","bin","cat" ))
    cmd.append(os.path.join("/","sys","class","net", "eth0", "carrier" ))
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.rstrip()
    if stdout != '0':
        #check if interface is already up
        cmd = []
        cmd.append(os.path.join("/","bin","cat" ))
        cmd.append(os.path.join("/","sys","class","net", "eth0", "operstate" ))
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        stdout = stdout.rstrip()
        if stdout != "up":
            #shutdown the network manager
            cmd = []
            cmd.append(os.path.join("/","usr","bin","pkill" ))
            cmd.append("-9")
            cmd.append("ifplugd")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            #shutdown the dhcpcd manager
            cmd = []
            cmd.append(os.path.join("/","usr","bin","pkill" ))
            cmd.append("-9")
            cmd.append("dhcpcd")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            
            #get wireless info
            cmd = []
            cmd.append(os.path.join("/","sbin","ifconfig" ))
            cmd.append("eth0")
            cmd.append("up")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            
            if exitCode == 0:
                configuration = stdout
                cmd = []
                cmd.append(os.path.join("/","sbin","dhcpcd" ))
                cmd.append("eth0")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                
        if exitCode == 0:
            import socket
            try:
                hostname = socket.gethostname()
                ip = socket.gethostbyname(hostname)
                print( ip)
            except socket.error:
                #this happens when ifplugd is running and not dhcpcd! (first boot network not setup)
                cmd = []
                cmd.append(os.path.join("/","sbin","dhcpcd" ))
                cmd.append("eth0")
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                if exitCode == 0:
                    hostname = socket.gethostname()
                    ip = socket.gethostbyname(hostname)
                    print( ip)
        
