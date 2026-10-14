#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Preprocessor directives
##########################
import sys
import socket

def getHostname():
    exitCode = -1
    hostname = ""
    try:
        hostname = socket.gethostname()
        exitCode = -1
    except:
        pass
        
    return (exitCode, hostname)


if __name__=='__main__':
    exitCode, hostname = getHostname()
    print(hostname)
    sys.exit(exitCode)

