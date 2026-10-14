#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import string
import file_io
import re
import os
import argparse
import sys

def setAudioDevice(device):
    configDevice = ''
    if device == 'internal':
        configDevice = '    pcm "hw:0,0"'
    elif device == 'external':
        configDevice = '    pcm "hw:1,0"'
    soundConfigFile = os.path.join(os.sep, 'etc','asound.conf')
    pcmSwMixerBlockLine = re.compile("^\s*pcm.swmixer\s*{\s*$")
    slaveBlockLine = re.compile("^\s*slave\s*{\s*$")
    pcmDeviceLine = re.compile('^\s*pcm\s*\"hw:[0-9]+,[0-9]+\"$')
    pcmExtDeviceLine = re.compile('^\s*pcm\s*\"plughw:[0-9]+,[0-9]+\"$')

    status, data = file_io.readFromFile(soundConfigFile)
    if status == 'success':
        newLines = []
        foundPcmSwMixerBlockLine = False
        foundSlaveBlockLine = False

        for i in range(0, len(data)):
            if pcmSwMixerBlockLine.match(data[i]):
                foundPcmSwMixerBlockLine = True
                newLines.append(data[i])
            elif slaveBlockLine.match(data[i]):
                foundSlaveBlockLine = True
                newLines.append(data[i])
            elif pcmDeviceLine.match(data[i]) or pcmExtDeviceLine.match(data[i]):
                if foundPcmSwMixerBlockLine and foundSlaveBlockLine:
                    newLines.append(str(configDevice))
                    #newLines.append('    pcm "hw:%s,0"' % (device))
                else:
                    newLines.append(data[i])
            else:
                newLines.append(data[i])

        status, data = file_io.writeToFile(soundConfigFile, newLines)
        if status != 'success':
            return ('error', None)
        else:
            return ('success', None)
    else:
        return ('error', None)

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description="set the audio device to internal (internally wired speakers or headphone jack) or external (USB connected speakers)")

    parser.add_argument("device",
                        action="store",
                        choices=['internal', 'external'],
                        help="specify internal (wired speakers or headphone jack) or external (USB connected speakers)")

    args = parser.parse_args()

    status, details = setAudioDevice(args.device)
    if status == 'success':
        sys.exit(0)
    else:
        sys.exit(1)
