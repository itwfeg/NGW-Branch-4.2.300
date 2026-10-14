#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import shellCommand


if __name__ == "__main__":
    exitCode = 0
    
    #check if the interface is available
    cmd = []
    cmd.append(os.path.join("/","usr","sbin","lspci" ))
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.rstrip()
    if stdout!="":        
        #get wireless info
        cmd = []
        cmd.append(os.path.join("/","sbin","ifconfig" ))
        cmd.append("wlan0")
        cmd.append("up")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        
        if exitCode == 0:
            configuration = stdout
            cmd = []
            cmd.append(os.path.join("/","usr","sbin","iwlist" ))
            cmd.append("wlan0")
            cmd.append("scan")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            if exitCode == 255:
                #wifi card is busy let try again!
                while exitCode == 255:
                    cmd = []
                    cmd.append(os.path.join("/","usr","sbin","iwlist" ))
                    cmd.append("wlan0")
                    cmd.append("scan")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    if exitCode == 0:
                        if len(stdout) != 0:
                            print('0')
                        else: 
                            #no access point found!
                            print('1')
                    else:
                        #no access point found!
                        print('1')
            else:
                print(stdout)
    else:
        #missing WIFI card or bad PCIe interface
        print('2')
    
