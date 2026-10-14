#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import ntplib
import sys
from time import ctime
import argparse

# ATTN: Hillsboro
# Enter your Network Time Protocol server below
ntpServer = "pool.ntp.org"

def getCurrentTime(server=ntpServer):

    exitCode = 1
    systemTime = ""

    try:

        c = ntplib.NTPClient()
        #response = c.request(ntpServer, version=3)
        response = c.request(server, version=3)
        ntpTime = ctime(response.tx_time)
        values = ntpTime.split()

        months = []
        months.append("JAN")
        months.append("FEB")
        months.append("MAR")
        months.append("APR")
        months.append("MAY")
        months.append("JUN")
        months.append("JUL")
        months.append("AUG")
        months.append("SEP")
        months.append("OCT")
        months.append("NOV")
        months.append("DEC")

        monthName = values[1].upper()

        for i in range(0,len(months)):
            if monthName == months[i]:
                monthNumber = i + 1
                break

        timeValues = values[3].split(':')

        month = monthNumber
        day = values[2]
        year = values[4]
        hours = timeValues[0]
        minutes = timeValues[1]
        seconds = timeValues[2]

        exitCode = 0
    except:
        raise
        month = None
        day = None
        year = None
        hours = None
        minutes = None
        seconds = None
        exitCode = 1

    return (exitCode, month, day, year, hours, minutes, seconds)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="get current time from specified NTP server")
    parser.add_argument("server",
                        action="store",
                        nargs='?',
                        default=ntpServer,
                        help="ntp server FQDN or ip address")

    args = parser.parse_args()

    systemTime = ''

    exitCode, month, day, year, hours, minutes, seconds = getCurrentTime(args.server)
    if exitCode == 0:
        systemTime = '%02d %s %s %s %s %s' % (month, day, year, hours, minutes, seconds)
    else:
        systemTime = ''
    print(systemTime)
    sys.exit(exitCode)
