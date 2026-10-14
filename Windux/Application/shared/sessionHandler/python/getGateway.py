#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import subprocess

def getGateway():
    gateway = None
    if os.name == "posix":

        #p = subprocess.Popen("/sbin/ip route | awk '/default/ {print $3}'", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
        p_stdout = p.stdout.read()
        #p_stderr = p.stderr.read()
        print(p_stdout)
        sys.stdout.flush()
    elif os.name == 'nt':
        import ntNetworkUtils
        gateway = ntNetworkUtils.getGateway()
    return gateway
    
import socket, struct

def get_default_gateway_linux():
    """Read the default gateway directly from /proc."""
    with open("/proc/net/route") as fh:
        for line in fh:
            fields = line.strip().split()
            if fields[1] != '00000000' or not int(fields[3], 16) & 2:
                continue

            return socket.inet_ntoa(struct.pack("<L", int(fields[2], 16)))

if __name__ == "__main__":

    #exitCode = getGateway("")
    if os.name == 'posix':
        gateway = get_default_gateway_linux()
        print(gateway)
    elif os.name == 'nt':
        gateway = getGateway()
        print(gateway)
    
    sys.exit(0)
