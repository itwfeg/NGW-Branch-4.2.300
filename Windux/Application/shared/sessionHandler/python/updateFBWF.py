#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import osUtils
import configInfo
import shellCommand
import os

def update(cmd, type):
    updateSettingsFiles = os.path.join("C:\\Updates", "applysettings.cmd")
    binPath=os.path.join(configInfo.getRootFolder(), "bin")

    # Do not perform this if FBWF is already off
    cmdfbwf = []
    cmdfbwf.append(os.path.join(binPath, "FBWFIsEnabled.cmd"))
    exitcode, stdout, stderr = shellCommand.execute(cmdfbwf)

    if stdout.find("1") > -1:
        # write to settings file, overwriting if already exists
        f = open(updateSettingsFiles, 'w')
        f.write(shellCommand.convertCmdForShell(cmd))
        f.write("\n")
        f.close()

        cmdUpdate = []
        cmdUpdate.append(os.path.join(binPath, "stageUpdate.cmd"))
        osUtils.winRunElevated(cmdUpdate)
            
