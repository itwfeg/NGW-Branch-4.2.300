#!/usr/bin/env python
# -*- coding: utf-8 -*-
# @Filename : rs485
# @Creation Date : 2019-12-02-13-18
# @Project: trunk
# @Organization: ITW Hobart, Inc.
# @AUTHOR : Scott Davis

import serial
import logging

# Test info:
# Miniterm usage on black box device
# The following command provides hex dump of bytes received from the RS485
# device on other end of RS485 cable
# python -m serial.tools.miniterm -p /dev/ttymxc4 -b 19200 --parity=N -D -D -D
# Perform the following command to output printable characters or escaped non printable characters
# python -m serial.tools.miniterm -p /dev/ttymxc4 -b 19200 --parity=N -D
# type Ctrl+T then hit 7 go set data bits to 7


class ModbusASCII:
    """
    NOTE:
        Not really modbus ASCII due to no LRC in message from Corona device
    """
    colon = 0x3A
    cr = 0x0D
    lf = 0x0A

    # message_terminators = [0x3A, 0x3E, 0x3E]
    message_terminators = [cr, lf]  # CR, LF before and with 0x7F
    message_prefix = colon          # Colon before and with 0x7F

    def __init__(self, port_name='/dev/ttymxc4'):

        self.port_name = port_name
        self.serial_device = serial.Serial(baudrate=19200,
                                           bytesize=serial.SEVENBITS,
                                           parity=serial.PARITY_NONE,
                                           stopbits=serial.STOPBITS_ONE,
                                           timeout=None,
                                           xonxoff=False,
                                           rtscts=False,
                                           dsrdtr=False)
        self.serial_device.port = port_name
        self.serial_port_open = False

    def _start_of_message_found(self, byte_value):
        som = False
        if ord(byte_value) == self.message_prefix:
            som = True
        return som

    def device_is_open(self):
        return self.serial_device.isOpen()

    def _end_of_message_found(self, message):
        """
        Determines if message ends with CR and LF by checking ordinal value
        before boolean
        :param message: modbus ASCII message
        :return: True or False if ends with CR LF
        """
        eom = False
        if len(message) >= len(self.message_terminators):
            if ord(message[-1]) == (self.message_terminators[-1]) and \
                    ord(message[-2]) == (self.message_terminators[-2]):
                eom = True
        return eom

    def open(self):
        try:
            self.serial_device.open()
            self.serial_device.flushInput()
            self.serial_device.flushOutput()
            self.serial_port_open = True
        except Exception, e:
            logging.error("opening serial port: " + str(e))

    def read_message(self):
        status_code = 'error'
        message = []

        if self.serial_device.isOpen():
            try:
                # For Modbus ASCII
                # look for the start of message character which is a colon
                while True:
                    byte_value = self.serial_device.read(size=1)
                    if ord(byte_value) == self.message_prefix:
                        message.append(byte_value)
                        break
                    # print 'loop 1'

                # for Modbus ASCII
                # look for the end of a message which is CR LF
                while True:
                    byte_value = self.serial_device.read(size=1)
                    # print byte_value
                    message.append(byte_value)
                    if self._end_of_message_found(message):
                        break
                    # print 'loop 2'

                # print message
                status_code = 'ok'

            except Exception, e:
                logging.error("reading modbus ASCII message: " + str(e))
                status_code = 'error'
        else:
            message = None
            status_code = 'error'

        return status_code, message

    def write_message(self, message):
        if self.serial_port_open:
            self.serial_device.write(message)
            status_code = 'ok'
        else:
            status_code = 'error'
        return status_code

    def close(self):
        self.serial_device.close()
