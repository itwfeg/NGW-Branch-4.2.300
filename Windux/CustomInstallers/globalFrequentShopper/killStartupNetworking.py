#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os

pythonPath = os.path.join("/","usr","local","hobart","python")
appsPath = os.path.join("/","usr","local","hobart","apps")

def killPersistentConnector():

    if os.name == 'posix':
        os.system('/usr/bin/pkill -f "python connectNetworkStartupNew.py"')
        os.system('/usr/bin/pkill -f "/usr/bin/python /usr/local/hobart/python/connectNetworkStartupNew.py"')

if __name__ == "__main__":
    killPersistentConnector()
    sys.exit(0)
