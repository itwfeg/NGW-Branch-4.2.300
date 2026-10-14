#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# argument 1: host
# argument 2: port number
# optional argument 3+: additional port numbers

import socket
import sys

timeout = 5

def portScanner(remoteServerIp, portsToScan=[]):

    portsOpen = []

    try:
        for port in portsToScan:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(timeout)
            result = sock.connect_ex((remoteServerIp, port))
            if result == 0:
                portsOpen.append(port)
            sock.close()
        return portsOpen

    except socket.error:
        print("Couldn't connect to server")
        return portsOpen

if __name__ == '__main__':
    openPorts = portScanner(sys.argv[1], [int(x) for x in sys.argv[2:]])
    output = ''
    for i in openPorts:
        output = output + '%s,' % (i)
    if len(output) > 1:
        print((output[:-1]))
    else:
        print('No open ports')
