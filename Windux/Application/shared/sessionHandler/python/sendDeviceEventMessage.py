#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket
import sys
import argparse

def sendMessageWithResponse(server='localhost',port=5080,message=''):
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (server, port)
        #print >>sys.stderr, 'connecting to %s port %s' % server_address
        sock.connect(server_address)
        sock.sendall(message if type(message) == bytes else message.encode())
        response = []
        try:
            allData = ""
            data = sock.recv(4096)
            while data and len(data) > 0:
                allData += data
                data = sock.recv(4096)
            if allData.find('\x00') != -1:
                response.extend(allData.split('\x00'))
            else:
                response.append(data)
        except:
            pass
        sock.close()
        return response
    except:
        return 'error'
        
def sendMessage(server='localhost',port=5077,message=''):
    """
    5077 is processDeviceEvents server and can accept following messages
    message = 'weightOnPlatter'
    message = 'shutdown'
    message = 'beeperVolume=[0-255]'
    message = 'operatorDisplayBrightness=[0-255]'
    message = 'customerDisplayBrightness=[0-255]'
    message = 'dimDisplayTimeout=[0-10]'
    
    5076 is smBackend which handles all scale management backend messages
    message = 'shutdown|1'
    """
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (server, port)
        #print >>sys.stderr, 'connecting to %s port %s' % server_address
        sock.connect(server_address)
        sock.sendall(message if type(message) == bytes else message.encode())
        sock.close()
        return 0
    except:
        return 1
        
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="send network message to specified server, port")
    parser.add_argument("server",
                        action="store",
                        default='localhost',
                        help="server (typically localhost)")

    parser.add_argument("port",
                        action="store",
                        type=int,
                        choices=list(range(5076,6001)),
                        help="destination port")

    parser.add_argument("message",
                        action="store",
                        help="ascii text message to send")

    parser.add_argument("--newline",help="add newline to message",
                       action="store_true")

    parser.add_argument("--timeout",help="change default timeout",
                       action="store", default=1200)

    args = parser.parse_args()

    message = args.message
    if args.newline:
        message = args.message + '\n'

    exitCode = sendMessage(args.server,args.port,message)
    sys.exit(exitCode)
