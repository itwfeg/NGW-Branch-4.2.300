#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import shellCommand
import argparse

import getBoardType

def volumeSetting(volumeLevel):

    if os.name == 'posix':

        #USB volume level
        cmd = []
        cmd.append("amixer")
        cmd.append("-c")
        cmd.append("1")
        cmd.append("set")
        cmd.append("PCM")
        cmd.append(volumeLevel + "%")
        exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)

        # ARK board uses PCM for playback level
        if getBoardType.isArkBoard():
        
            # SOF-3355: Need to adjust lower volume levels to match ARA output
            tmpVolumeLevel = volumeLevel
            if int(volumeLevel) == 0:
                tmpVolumeLevel = 0
            elif int(volumeLevel) > 0 and int(volumeLevel) < 60:
                tmpVolumeLevel = ( int( volumeLevel ) / 4 ) + 65
            elif int(volumeLevel) >= 60 and int(volumeLevel) <= 75:
                tmpVolumeLevel = int( volumeLevel ) * 1.30
            # Any percentage over 75% ... do not adjust it
            volumeLevel = str(int(float(tmpVolumeLevel)))
            #print "Volume-Level=" + str( volumeLevel )

            # Speaker volume level
            cmd = []
            cmd.append("amixer")
            cmd.append("-c")
            cmd.append("0")
            cmd.append("set")
            cmd.append("PCM")
            cmd.append(volumeLevel + "%")
            exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)

        else:
            cmd = []
            cmd.append("amixer")
            cmd.append("-c")
            cmd.append("0")
            cmd.append("set")
            cmd.append("Speaker")
            # Scaling necessary volume / 4 throw away remainder and add 75
            if volumeLevel != "0":
                volumeNew = int(( float(volumeLevel) / 4 ) + 75)
                volumeLevel = str(volumeNew)
            #print(str(volumeLevel))
            cmd.append(volumeLevel + "%," + volumeLevel + "%")
            exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)

            cmd = []
            cmd.append("amixer")
            cmd.append("-c")
            cmd.append("0")
            cmd.append("set")
            cmd.append("Playback")
            # Scaling necessary volume / 4 throw away remainder and add 75
            if volumeLevel != "0":
                volumeNew = ( int(volumeLevel) / 2 ) + 50
                volumeLevel = str(volumeNew)
            #print(str(volumeLevel))
            cmd.append(volumeLevel + "%")
            exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)
        
            cmd = []
            cmd.append("amixer")
            cmd.append("-c")
            cmd.append("0")
            cmd.append("set")
            cmd.append("Headphone")
            cmd.append(volumeLevel + "%," + volumeLevel + "%")
            exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)
        
    elif os.name == 'nt':
        import configInfo
        nircmdPath = os.path.join(configInfo.getRootFolder(), 'python', 'nircmd.exe')
        # Windows speaker volume
        # nircmd max is 65535 so 655 is the multiplier
        if volumeLevel == "100":
            #unmute
            cmd = []
            cmd.append(nircmdPath)
            cmd.append('mutesysvolume')
            cmd.append('0')
            exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)
            #get volume level ready
            volumeLevel = '65535'
            
        elif volumeLevel == "0":
            #mute
            cmd = []
            cmd.append(nircmdPath)
            cmd.append('mutesysvolume')
            cmd.append('1')
            exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)
            
            #get volume level ready
            volumeLevel = '0'
        else:
            #unmute
            cmd = []
            cmd.append(nircmdPath)
            cmd.append('mutesysvolume')
            cmd.append('0')
            exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)
            
            #get volume level ready
            volumeLevel = int(volumeLevel) * 655
        cmd = []
        cmd.append(nircmdPath)
        cmd.append('setsysvolume')
        cmd.append(str(volumeLevel))
        exitCode, stdout, stderr = shellCommand.execute(cmd=cmd)
        
    return exitCode
    
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="set customer speaker volume")
    parser.add_argument("volume",
                        action="store",
                        type=int,
                        choices=list(range(0,101)),
                        default=100, 
                        help="speaker volume as a percentage")

    args = parser.parse_args()
    
    exitCode = volumeSetting(str(args.volume))

    sys.exit(exitCode)
