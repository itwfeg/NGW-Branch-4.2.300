#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import socket
import sys
import time


def sendMessageWithResponse(server='localhost', port=5076, message='', timeout=1200,  statusInResponse=1):
    server_address = (server, port)
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.settimeout(15)

    try:
        connection.connect(server_address)
    except:
        return ''

    connection.sendall(message.encode())
    connection.settimeout(int(timeout))

    try:
        current_response = connection.recv(1024)

        if statusInResponse:
            while not 'error' in current_response.decode("utf-8") and not 'done' in current_response.decode("utf-8"):
                current_response += connection.recv(1024)

                if not current_response:
                    raise ValueError('No response received from socket, quitting')

                time.sleep(0.1)
        else:
            return current_response
    finally:
        connection.close()

    return current_response.decode("utf-8").strip('\n').split('\n')


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

    response = sendMessageWithResponse(args.server, args.port, message, args.timeout, 1)

    print(response)

    for response_line in response:
        if 'error' in response_line:
            sys.exit(1)

    sys.exit(0)
