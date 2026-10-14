#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import shellCommand
    
if __name__ == "__main__":
    exitCode = 0
    skipped = False
    
    #verify our devices are found on the USB bus #2
    cmd = []
    cmd.append(os.path.join("/","usr","bin","lsusb" ))
    cmd.append("-s")
    cmd.append("2:")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    # we should have 7 devices on bus 2
    stdout = stdout.rstrip()
    index = 0
    cnt = 0
    while index < len(stdout):
        index = stdout.find('Bus 002',  index )
        if index == -1:
            break
        cnt = cnt + 1
        index += 3
    # if we have 8 devices on bus 2 then test successful 
    
    #print(stdout)
    
    if cnt != 7:
        #sometimes devices enumerations not inorder
        if stdout.find('009') != -1 or stdout.find('010') != -1:
            skipped = True
        
        # do we see the WIFI card device 3
        if stdout.find('Atheros')  != -1:
            # do we see the Touchscreen device 4
            if stdout.find('TouchScreen') != -1:    
                # do we see the root hub
                if stdout.find('001') != -1:
                    #do we see hub 1 
                    if stdout.find('002') != -1:
                        if stdout.find('005') != -1:
                            if stdout.find('006') != -1:
                                if stdout.find('007') != -1:
                                    if stdout.find('008') != -1:
                                        #all devices found test passed
                                        print('0')
                                    else:
                                        if skipped:
                                            print('0')
                                        else:
                                            print('8')
                                else:
                                    if skipped:
                                        print('0')  
                                    else:
                                        #USB bus issue: Device 7 missing
                                        print('7')  
                            else:
                                # USB bus issue: Device 6 missing
                                print('6')                            
                        else:
                            # USB bus issue: Device 5 missing
                            print('5')                        
                    else:
                        # USB bus issue: Device 2 Hub 1 missing
                        print('2')                    
                else:
                    # USB bus issue: Device 1 Root hub missing
                    print('1')                
            else:
                # USB bus issue: Device 4 TouchScreen missing
                print('4')
        else:
            # USB bus issue: Device 3  WIFI card missing
            print('3')
    else:
        print('0')








