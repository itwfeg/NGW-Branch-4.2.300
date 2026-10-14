#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import file_io
import os
import string
import sys
import argparse

def currentLanguage():
    if os.name == 'posix':
        status,  data = file_io.readFromFile(os.path.join("/","usr","local", "hobart", "lang"))
    elif os.name == 'nt':
        import configInfo
        status, data = file_io.readFromFile(os.path.join(configInfo.getRootFolder(), "lang"))
        
    if status == 'success':
        if data[0].strip() == 'ES.MX':
            language = 'spanish'
        elif data[0].strip() == 'FR.CA':
            language = 'french'
        else:
            language = 'english'
        return language
    else:
        # set to english if error reading file
        return 'english'
    
if __name__ == "__main__":
    exitCode = 1
    parser = argparse.ArgumentParser(description="get current language settings (english, spanish, french)")

    args = parser.parse_args()

    language = currentLanguage()
    exitCode = 0

    print(language)
    sys.exit(exitCode)
