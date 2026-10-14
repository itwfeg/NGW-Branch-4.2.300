#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# AUTHOR  : Randy Blankley
# COMPANY : ITW Hobart
# DATE    : November 15, 2017
#
# OVERVIEW
# This script will read SMS messages using AT commands.
#
###################################################################################################

# not tested with python3

import argparse
import os
import serial
import sys
import time

## Parse CMGL (list messages) message
#  @param buffer  buffer to parse
#  @return  list of lists
def parse_cmgl_msg(buffer):
    msgs = []

    # TODO

    return msgs

## Parse CMGR (read messages) message
#  @param buffer  buffer to parse
#  @return  list of lists
def parse_cmgr_msg(buffer):
    msgs = []

    # TODO

    return msgs

## Parse CPMS (preferred message storage) message
#  @param buffer  buffer to parse
#  @return  list of lists
def parse_cpms_msg(buffer):

    msgs = []
    lines = buffer.splitlines()

    for line in lines:
        if '+CPMS: ' not in line:
            continue

        # strip out the storage areas
        buffer_str = line.replace('+CPMS: ','').replace('\r','').replace('\n','')

        areas = buffer_str.split(',')
        storage = []

        # parse each storage area into sections
        for area in areas:
            start = False
            stop = False

            if '(' in area:
                start = True
            if ')' in area:
                stop = True

            storage.append(area.replace('(','').replace(')','').replace('"',''))

            if stop:
                msgs.append(storage)
                storage = []

        if storage:
            msgs.append(storage)

        break

    return msgs

## Send an AT command to modem device
#  @param f  serial device
#  @param cmd  command to send, with terminator
#  @param resp  expected response with terminators, empty string if none is expected
#  @param timeout  how long to wait for @p resp before giving up
#  @return  @c True upon success, @c False otherwise
def send_cmd(f, cmd, resp='', timeout=5.0):

    msgs = []

    # write command
    print(('send: ' + cmd))

    f.write(cmd if type(cmd) is bytes else cmd.encode('utf-8'))

    if not resp:
        return (True, msgs)

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

                if 'CMGL' in cmd:
                    msgs = parse_cmgr_msg(buffer_str)
                elif 'CMGR' in cmd:
                    msgs = parse_cmgr_msg(buffer_str)
                elif 'CPMS' in cmd:
                    msgs = parse_cpms_msg(buffer_str)

                break

        # check for timeout
        if timer <= time.time():
            print('recv timeout!')
            break

    buffer_str = ''.join(str(x) for x in buffer)
    print(('recv: ' + buffer_str))

    return (result, msgs)

## Read SMS messages
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
        # It is possible to read SMS msgs when no ppp connection is used. For this we need the ATZ
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
                result = send_cmd(f, 'ATZ\r', 'OK\r\n', timeout)
                if not result[0]:
                    f.close()
                    return False

            # turn off echo mode
            result = send_cmd(f, 'ATE0\r', 'OK\r\n', timeout)
            if not result[0]:
                f.close()
                return False

            # enter SMS text mode
            result = send_cmd(f, 'AT+CMGF=1\r', 'OK\r\n', timeout)
            if not result[0]:
                f.close()
                return False

            # check SMS storage options
            result = send_cmd(f, 'AT+CPMS=?\r', 'OK\r\n', timeout)
            if not result[0]:
                f.close()
                return False

            areas = result[1]
            if not areas:
                f.close()
                return False

            # iterate through the 'readable' storage areas
            for area in areas[0]:

                # set area
                result = send_cmd(f, ('AT+CPMS="%s"\r' % area), 'OK\r\n', timeout)
                if not result[0]:
                    f.close()
                    return False

                sizes = result[1]
                if not sizes:
                    continue

                # check how many message pending
                # message format is
                #  used_space1, max_space1, used_space2, max_space2, used_space3, max_space3
                #  -or-
                #  area1, used_space1, max_space1, area2, used_space2, max_space2, area3, used_space3, max_space3
                size = sizes[0]
                pending = 0

                if 9 == len(size) and area in size[0]:
                    pending = int(size[1])
                elif 6 == len(size):
                    pending = int(size[0])

                # no messages
                if not pending:
                    print(('no messages pending in ' + area))

                # list messages
                else:
                    result = send_cmd(f, 'AT+CMGL="ALL"\r', 'OK\r\n', timeout)
                    if not result[0]:
                        f.close()
                        return False

            f.close()
            return True

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

    ## setup parser
    #parser = argparse.ArgumentParser(description='Send SMS Message')

    #parser.add_argument('phone', type=str, action='store', help='SMS Destination Phone Number')
    #parser.add_argument('msg',  type=str, action='store', help='Message to Send')

    ## parse
    #args = parser.parse_args()

    #if not args.phone:
    #    print 'Please specify a phone number'
    #elif not args.msg:
    #    print 'Please specify a message'

    # read messages
    if main():
        exitCode = 0

    sys.exit(exitCode)

