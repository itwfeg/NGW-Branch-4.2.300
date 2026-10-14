#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os

def getScaleSettingsValue(key):
    with open('/usr/local/hobart/db/settingswizard/scalesettings') as f:
        for line in f:
            line = line.rstrip()
            if key in line:
                tokens = line.split('=')
                try:
                    return tokens[1].strip()
                except:
                    return ''

try:
    ITW_FEG_VARIABLES_PORT = int(getScaleSettingsValue("snmpVariablesPort"))
except:
    ITW_FEG_VARIABLES_PORT = 161

try:
    ITW_FEG_TRAPS_PORT = int(getScaleSettingsValue("snmpTrapsPort"))
except:
    ITW_FEG_TRAPS_PORT = 162

try:
    ITW_FEG_TRAPS_IP = getScaleSettingsValue("snmpTrapsIP")
except:
    ITW_FEG_TRAPS_IP = "localhost"   # send to blackbox someday

if __name__ == "__main__":
    pass
