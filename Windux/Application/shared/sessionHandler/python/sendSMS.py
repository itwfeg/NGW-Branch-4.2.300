#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# AUTHOR  : Randy Blankley
# COMPANY : ITW Hobart
# DATE    : January 27, 2017
#
# OVERVIEW
# This script will send an SMS message using AT commands.
#
###################################################################################################

import argparse
import os
import serial
import sys
import time

## Send an AT command to modem device
#  @param f  serial device
#  @param cmd  command to send, with terminator
#  @param resp  expected response with terminator(s), empty string if none is expected
#  @param timeout  how long to wait for @p resp before giving up
#  @return  @c True upon success, @c False otherwise
def send_cmd(f, cmd, resp='', timeout=5.0):

    # write command
    print(('send: ' + cmd))

    f.write(cmd if type(cmd) is bytes else cmd.encode('utf-8'))

    if not resp:
        return True

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
            print('recv timeout!')
            break

    buffer_str = ''.join(str(x) for x in buffer)
    print(('recv: ' + buffer_str))

    return result

## Send SMS message to phone
#  @param phone  phone number to send to
#  @param msg  message to send
#  @return  @c True upon success, @c False otherwise
def main(phone, msg):

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

        # we don't need internet connectivity to send SMS, so don't check
        # modem not connected
        #if not pppConnected and shouldExit:
        #    print('Modem not connected')
        #    return False

        try:
            f = serial.Serial(modemDevice, 230400, timeout=timeout)

            if not f.isOpen():
                print('Error opening modem')
                return False

            # reset modem
            if not pppConnected and shouldReset:
                if not send_cmd(f, 'ATZ\r', 'OK\r\n', timeout):
                    f.close()
                    return False

            # turn off echo mode
            if not send_cmd(f, 'ATE0\r', 'OK\r\n', timeout):
                f.close()
                return False

            # enter SMS text mode
            if not send_cmd(f, 'AT+CMGF=1\r', 'OK\r\n', timeout):
                f.close()
                return False

            # send phone number
            if not send_cmd(f, ('AT+CMGS="%s"\r' % phone), '\r\n> ', timeout):
                f.close()
                return False

            # send message body
            result = False

            if send_cmd(f, ('%s' % msg)):

                # accept - Ctrl+Z
                send_cmd(f, chr(26), 'OK\r\n', timeout)
                result = True

            else:

                # cancel - ESC
                send_cmd(f, chr(27), 'OK\r\n', timeout)

            f.close()
            return result

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

    # setup parser
    parser = argparse.ArgumentParser(description='Send SMS Message')

    parser.add_argument('phone', type=str, action='store', help='SMS Destination Phone Number')
    parser.add_argument('msg',  type=str, action='store', help='Message to Send')

    # parse
    args = parser.parse_args()

    if not args.phone:
        print('Please specify a phone number')
    elif not args.msg:
        print('Please specify a message')

    # send message
    elif main(args.phone,args.msg):
        exitCode = 0

    sys.exit(exitCode)

