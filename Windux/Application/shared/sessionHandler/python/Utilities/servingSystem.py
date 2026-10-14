#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import struct
import time
import sys
import os
import select
import socket
import string
import netInterface
import netManager
import usb.core
import usb.util
import ticketPrinter

BUTTON_VENDOR_ID = 1118
BUTTON_PRODUCT_ID = 1917

# decimal vendor and product values
dev = usb.core.find(idVendor=BUTTON_VENDOR_ID, idProduct=BUTTON_PRODUCT_ID)

# or, uncomment the next line to search instead by the hexidecimal equivalent
#dev = usb.core.find(idVendor=0x45e, idProduct=0x77d)

# first endpoint
interface = 0
endpoint = dev[0][(0,0)][0]

if dev.is_kernel_driver_active(interface) is True:
  # tell the kernel to detach
  dev.detach_kernel_driver(interface)
  # claim the device
  usb.util.claim_interface(dev, interface)

# TODO: Need to figure out how to get file descriptor of device
event_input_device = dev

FORMAT = 'llHHI'
EVENT_SIZE = struct.calcsize(FORMAT)

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_address = ('localhost', 5080)
print('servingSystem starting up on localhost port 5080')

server.bind(server_address)
server.listen(5)

inputs = [event_input_device, server]
outputs = []

shutdown = False

while not shutdown:

    # block on event coming in
    readable, writable, exceptional = select.select(inputs, outputs, inputs)

    type = 0
    code = 0
    value = 0

    # event occurred either socket connection or usb event from button press
    for s in readable:

        # socket message coming in to process
        if s is server:

            # accept network connection
            connection, client_address = s.accept()
            data = connection.recv(1024)
            connection.close()

            if data == 'nextNumber':
                pass

            elif data == 'setNumber':
                pass

            elif data == 'shutdown':
                shutdown = True

        # else USB button event occurred
        else:

            try:
                data = event_input_device.read(endpoint.bEndpointAddress,endpoint.wMaxPacketSize)
                if data == 'F5':
                    # TODO: button was pressed
                    # get next ticket number
                    # print ticket
                    pass
            except usb.core.USBError as e:
                data = None
                if e.args == ('Operation timed out',):
                    continue

usb.util.release_interface(event_input_device, interface)
usb.attach_kernel_driver(interface)
server.close()
