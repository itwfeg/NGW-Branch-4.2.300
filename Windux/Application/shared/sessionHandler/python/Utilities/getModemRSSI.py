#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# AUTHOR  : Randy Blankley
# COMPANY : ITW Hobart
# DATE    : October 23, 2018
#
# OVERVIEW
# This script will retrieve modem RSSI using AT commands.
#
###################################################################################################

import argparse
import os
import serial
import sys
import time

## Send an AT command to modem device with response
#  @param f  serial device
#  @param cmd  command to send, with terminator
#  @param resp  expected response with terminator(s), empty string if none is expected
#  @param timeout  how long to wait for @p resp before giving up
#  @return  response buffer and @c True upon success, @c False otherwise
def send_cmd_with_resp(f, cmd, resp='', timeout=5.0):

    # write command
    #print('send: ' + cmd)

    f.write(cmd if type(cmd) is bytes else cmd.encode('utf-8'))

    if not resp:
        return (True, [])

    # wait for response
    timer = time.time() + timeout

    result = False
    buffer = []

    while not result:

        # read
        for c in f.read().decode("utf-8"):
            buffer.append(c)
            buffer_str = ''.join(str(x) for x in buffer)

            if resp in buffer_str:
                result = True
                break

        # check for timeout
        if timer <= time.time():
            #print 'recv timeout!'
            break

    buffer_str = ''.join(str(x) for x in buffer)
    #print 'recv: ' + buffer_str

    return (result, buffer_str)

## Send an AT command to modem device
#  @param f  serial device
#  @param cmd  command to send, with terminator
#  @param resp  expected response with terminator(s), empty string if none is expected
#  @param timeout  how long to wait for @p resp before giving up
#  @return  @c True upon success, @c False otherwise
def send_cmd(f, cmd, resp='', timeout=5.0):
    result, buffer = send_cmd_with_resp(f, cmd, resp, timeout)

    return result

## Parse RSSI value
#  @ return  string representing RSSI strength
def parse_rssi(value):
    values = {
        0: '-113,Poor',
        1: '-111,Poor',
        2: '-109,Marginal',
        3: '-107,Marginal',
        4: '-105,Marginal',
        5: '-103,Marginal',
        6: '-101,Marginal',
        7: '-99,Marginal',
        8: '-97,Marginal',
        9: '-95,Marginal',
        10: '-93,OK',
        11: '-91,OK',
        12: '-89,OK',
        13: '-87,OK',
        14: '-85,OK',
        15: '-83,Good',
        16: '-81,Good',
        17: '-79,Good',
        18: '-77,Good',
        19: '-75,Good',
        20: '-73,Excellent',
        21: '-71,Excellent',
        22: '-69,Excellent',
        23: '-67,Excellent',
        24: '-65,Excellent',
        25: '-63,Excellent',
        26: '-61,Excellent',
        27: '-59,Excellent',
        28: '-57,Excellent',
        29: '-55,Excellent',
        30: '-53,Excellent',
        31: '-51,Excellent',
        99: 'RSSI not known or not detectable'
    }

    return values.get(value, 'Error retrieving RSSI')

## Retrieve RSSI
#  @return  @c True upon success, @c False otherwise
def main():

    #############################################
    # Linux
    #############################################
    if os.name == "posix":

        # Note:
        # 
        # When the device is booted it will automatically create a ppp connection using the
        # /dev/ttyACM0 device. Our problem is that to send an SMS we need access to the ACM0
        # device to issue AT commands.
        # 
        # Now there is also a /dev/ttyACM3 device that accepts AT commands (but does not support
        # ppp connections). Since ACM0 is in use by ppp we can use ACM3 instead.
        # 
        # This script will only work when a ppp connection is established. Below you will find
        # an ATZ command commented out. This command initializes the modem device. If we issue this
        # command while ppp is up it will break our ppp connection (because of re-initialize).
        # 
        # It is possible to send an SMS when no ppp connection is used. For this we need the ATZ
        # command.
        # 
        # So there are two options:
        # 1) when ppp is connected open up ACM3 and _do not_ send the ATZ
        # 2) when ppp _is not_ connected open up ACM0 (or ACM3??) and send the ATZ

        modemDevice = '/dev/ttyACM3'
        timeout = 8.0

        pppConnected = os.path.exists('/sys/class/net/ppp0/operstate')
        shouldExit = True
        shouldReset = False

        # no modem device found
        if not os.path.exists(modemDevice):
            print('No modem found')
            return False

        # modem not connected
        if not pppConnected and shouldExit:
            print('Modem not connected')
            return False

        try:
            f = serial.Serial(modemDevice, 230400, timeout=timeout)

            if not f.isOpen():
                print('Error opening modem')
                return False

            # reset modem
            if not pppConnected and shouldReset:
                if not send_cmd(f, 'ATZ\r', 'OK\r\n', timeout):
                    print('Error sending command')
                    f.close()
                    return False

            # turn off echo mode
            if not send_cmd(f, 'ATE0\r', 'OK\r\n', timeout):
                print('Error sending command')
                f.close()
                return False

            # retrieve RSSI
            result, buffer = send_cmd_with_resp(f, 'AT+CSQ\r', 'OK\r\n', timeout)

            if (not result) or ('+CSQ:' not in buffer):
                print('Error retrieving RSSI')
                f.close()
                return False

            # parse rssi value
            rssi = buffer.split('+CSQ:', 1)[1]

            if ',' in rssi:
                print((parse_rssi(int(rssi.split(',', 1)[0]))))
                f.close()
                return True

            print('Error retrieving RSSI')
            f.close()
            return False

        except:
            print('Exception while sending SMS')

    #############################################
    # Windows
    #############################################
    else:
        print('Unsupported OS')

    return False

if __name__ == '__main__':

    exitCode = 1

    # process
    if main():
        exitCode = 0

    sys.exit(exitCode)

