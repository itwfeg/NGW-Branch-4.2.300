#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import configInfo

if os.name == 'posix':
    #os.system("amixer sset Headphone,0 " + str(options.level) + "%," + str(options.level) + "%")
    cmd = []
    cmd.append("aplay")
    cmd.append("sampleAudio.mp3")
elif os.name == 'nt':
    #print("Windows Sound Volume")
    cmd = []
    cmd.append(configInfo.getRootFolder() + os.path.join(os.sep, 'bin', 'sounder.exe'))
    cmd.append(configInfo.getRootFolder() + os.path.join(os.sep, 'Audio', 'beep.wav'))
    
exitCode, stdout, stderr = shellCommand.executeShellCommandBackground(cmd=cmd)
