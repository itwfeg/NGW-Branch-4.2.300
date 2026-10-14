#!/usr/bin/env python2
# -*- coding: utf-8 -*-

import os
import shellCommand
    
if __name__ == "__main__":
    exitCode = 0

    cmd = []
    cmd.append(os.path.join("/","bin","cat" ))
    cmd.append(os.path.join("/","sys","class","mtd","mtd7","name" ))
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.rstrip()
    if stdout == "splash":
        cmd = []
        cmd.append(os.path.join("/","usr","sbin","flash_erase" ))
        cmd.append("/dev/mtd7")
        cmd.append("0")
        cmd.append("0")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        
        if exitCode == 0:
            cmd = []
            cmd.append(os.path.join("/","usr","sbin","flashcp" ))
            cmd.append("/uboot/htm8.bmp")
            cmd.append("/dev/mtd7")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
        
        if exitCode == 0:            
            cmd = []
            cmd.append(os.path.join("/","usr","bin","head" ))
            cmd.append("-c")
            cmd.append("2")
            cmd.append("/dev/mtd7")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        if stdout == "BM":
            cmd = []
            cmd.append(os.path.join("/","usr","sbin","flash_erase" ))
            cmd.append("/dev/mtd7")
            cmd.append("0")
            cmd.append("0")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
        
        print(exitCode) 
            
