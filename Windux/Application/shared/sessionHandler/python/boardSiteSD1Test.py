#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import shellCommand
    
if __name__ == "__main__":
    exitCode = 0

    #is SD card present
    if os.path.exists(os.path.join("/", "media", "mmcblk1p1")):
        cmd = []
        cmd.append(os.path.join("/","bin","touch" ))
        cmd.append(os.path.join("/","media","mmcblk1p1", "testFile.txt" ))
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode == 0:
            cmd = []
            cmd.append(os.path.join("/","bin","ls"))
            cmd.append(os.path.join("/","media","mmcblk1p1", "testFile.txt" ))            
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            stdout = stdout.rstrip()
            if stdout == "/media/mmcblk1p1/testFile.txt":
                cmd = []
                cmd.append(os.path.join("/","bin","rm"))
                cmd.append("-f")
                cmd.append(os.path.join("/","media","mmcblk1p1", "testFile.txt" ))
                exitCode, stdout, stderr = shellCommand.execute(cmd)
                if exitCode == 0:
                    cmd = []
                    cmd.append(os.path.join("/","bin","ls"))
                    cmd.append(os.path.join("/","media","mmcblk1p1", "testFile.txt" ))
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    if exitCode == 2:
                        #SD card tests passed
                        print('0')
                    else:
                        #SD card erase failure
                        print('3')
                else:
                    #SD card erase failure
                    print('3')
            else:
                #SD card write failure
                print('2')
        else:
            #SD card write failure
            print('2')
    else:
        #SD card missing
        print('1')
        
            
