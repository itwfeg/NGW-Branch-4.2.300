#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import shellCommand
if __name__ == "__main__":
    exitCode = 0
    #check if the interface is available
    cmd = []
    cmd.append(os.path.join("/","usr","bin","hciconfig" ))
    cmd.append("-a")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.rstrip()
    if stdout.find('hci0') != -1:
        #bring up the interface
        cmd = []
        cmd.append(os.path.join("/","usr","bin","hciconfig" ))
        cmd.append("hci0")
        cmd.append("up")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode == 0:            
            #check and make sure interface is up 
            cmd = []
            cmd.append(os.path.join("/","usr","bin","hciconfig" ))
            cmd.append("-a")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            if exitCode == 0:
                if stdout.find('UP') != -1:
                    #scan for devices
                    cmd = []
                    cmd.append(os.path.join("/","usr","bin","hcitool" ))
                    cmd.append("scan")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    stdout = stdout.rstrip()
                    if exitCode == 0:
                        if len(stdout) != 0:
                            #pass
                            print('0')
                        else: 
                            #failed to find device during scan
                            print('1')
                    else:
                        #failed
                        print('2')
                else:
                    #device not up
                    print('3')
            else:
                #cmd failed
                print('2')
    elif stdout.find('hci1') != -1:
        #bring up the interface
        cmd = []
        cmd.append(os.path.join("/","usr","bin","hciconfig" ))
        cmd.append("hci1")
        cmd.append("up")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode == 0:
            #check and make sure interface is up 
            cmd = []
            cmd.append(os.path.join("/","usr","bin","hciconfig" ))
            cmd.append("-a")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            if exitCode == 0:
                if stdout.find('UP') != -1:
                    #scan for devices
                    cmd = []
                    cmd.append(os.path.join("/","usr","bin","hcitool" ))
                    cmd.append("scan")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    stdout = stdout.rstrip()
                    if exitCode == 0:
                        if len(stdout) != 0:
                            #pass
                            print('0')
                        else: 
                            #failed to find device during scan
                            print('1')
                    else:
                        #failed
                        print('2')
                else:
                    #device not up
                    print('3')
            else:
                #cmd failed
                print('2')
    else:
            #no interface found!
            print('1')
