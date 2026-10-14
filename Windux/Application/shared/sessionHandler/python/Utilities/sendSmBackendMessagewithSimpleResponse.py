#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import sendSmBackendMessage
import sys

def sendMessageWithResponse(server='localhost', port=5076, message='', timeout=1200):
    return sendSmBackendMessage.sendMessageWithResponse(server, port, message, timeout, 0)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="send network message to sm backend, and wait for response")
    parser.add_argument("server", action="store", default='localhost', help="server (typically localhost)")
    parser.add_argument("port", action="store", type=int, choices=list(range(5076,6001)), help="destination port")
    parser.add_argument("message", action="store", help="ascii text message to send")
    parser.add_argument("--newline",help="add newline to message", action="store_true")
    parser.add_argument("--timeout",help="change default timeout", action="store", default=1200)

    args = parser.parse_args()

    message = args.message

    if args.newline:
        message = args.message + '\n'

    response = sendMessageWithResponse(args.server, args.port, message, args.timeout)

    print(response)

    sys.exit(0)
