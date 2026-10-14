#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import shellCommand
    
if __name__ == "__main__":
    exitCode = 0

    cmd = []
    cmd.append(os.path.join("/","bin","date" ))
    cmd.append("-s")
    cmd.append("2/13/2020 8:30:00")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.rstrip()
    #print(stdout)
    if stdout == "Thu Feb 13 08:30:00 EST 2020":
        # Set the hardware clock
        #######################
        cmd = []
        cmd.append("hwclock")
        cmd.append("--utc")
        cmd.append("--systohc")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode == 0:
            # Validate hardware clock
            #######################
            cmd = []
            cmd.append("hwclock")
            cmd.append("--utc")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            if exitCode == 0:                
                if len(stdout)>32:
                    hwday = stdout[8:10]
                    hwyear = stdout[20:24]
                    if ((int("2020") == int(hwyear)) or (int("13") == int(hwday))):
                        print('0')
                    else:
                        print('1')
                else:
                    print('1')
            else:
                print('1')        
        else:
            print('1')
    else:
        print('1')
