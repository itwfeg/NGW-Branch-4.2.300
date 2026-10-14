#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import ping
import sys
import socket
import portScanner
import traceroute
import time
import os

def hteSmScan(ipAddress, printing = False, log = True):

    message = 'Network Scan Results\n\n'

    message += 'Ping Results:\n'

    exitCode, stdout = ping.pingSystem(ipAddress, '1', 2000)

    if exitCode != 0:
        message += 'error: Device ' + ipAddress + ' can not be pinged\n'
    else:
        message += 'success: Device ' + ipAddress + ' can be pinged\n'

        scanIps = [socket.gethostbyname('localhost'), ipAddress]

        for ip in scanIps:
            if ip == socket.gethostbyname('localhost'):
                # see what ports are open on the scale
                portsToScan = [22, 5076, 6000]
            else:
                # see what ports are open on the HTe SM PC
                portsToScan = [22, 5076]

            message += '\nPort Scan Results for ' + ip + '\n'

            openPorts = portScanner.portScanner(ip, portsToScan)
            numOpenPorts = 0
            for port in portsToScan:
                if port not in openPorts:
                    if ip == socket.gethostbyname('localhost'):
                        message += 'error: Port ' + str(port) + ' is not open on this scale device\n'
                    else:
                        message += 'error: Port ' + str(port) + ' is not open in network to HTe SM PC with IP address ' + ip + '\n'
                else:
                    numOpenPorts = numOpenPorts + 1

            if numOpenPorts == len(portsToScan):
                portData = ''
                for i in portsToScan:
                    portData = portData + str(i) + ','
                message += 'success: Ports ' + portData + ' are open\n'

        exitCode, stdout = traceroute.trace(ipAddress)
        if exitCode == 0:

            message += '\nTrace Route Results for ' + ipAddress + '\n'
            message += stdout
            if printing:
                print(message)

        else:
            message += '\nError: Traceroute to %s failed\n' % (ipAddress)
            if printing:
                print(message)

    if log:
        logFilename = os.path.join("/","usr","local","hobart","info.log")

        try:
            line = time.strftime("%a %b %H:%M:%S %Y HTe SM Scan Results:\n")
            outputFile = open(logFilename, 'a')
            outputFile.write(line)
            outputFile.write(message)
            outputFile.close()
        except IOError as exception_details:
            print(exception_details)

    return message

if __name__ == '__main__':
    hteSmScan(sys.argv[1], printing=True)
    sys.exit(0)
