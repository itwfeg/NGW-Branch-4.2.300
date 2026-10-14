# -*- coding: utf-8 -*-

# not tested in python3

import configparser
import sys
import os
import string

class Parser:

    def __init__(self, configFile):
        self.configFile = configFile

    def getSettings(self):
        try:
            configFile = open(self.configFile,'r')
        except:
            return ('error','Can not open file',None)

        try:
            settings = configparser.SafeConfigParser()
        except:
            return ('error','Parsing error',None)

        try:
            settings.readfp(configFile)
            configFile.close()

        except configparser.Error as e:
            print(e)
            return ('error',e,None)

        return settings
