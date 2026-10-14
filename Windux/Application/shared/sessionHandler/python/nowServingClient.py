#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import socket
import sys


def connect_and_send(host, port, message):
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.settimeout(5)
    connection.connect((host, int(port)))
    to_send = message + b'\n' if type(message) == bytes else (message + '\n').encode()
    connection.send(to_send)
    return connection.recv(1024).decode("utf-8")


if __name__ == '__main__':
    host, port, message = sys.argv[1:]
    print((connect_and_send(host, port, message)))
