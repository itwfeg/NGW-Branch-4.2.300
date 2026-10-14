#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#-----------------------------------------------------------------------------------------------------------------------
# Python port of the openauth class
# @copyright Copyright (c) 2018 Hobart Corporation
# @author Eric Landes
import sys

import hmac
import hashlib
import struct
import time
import datetime
import base64

def getPassKey():

    #--------------------------------------------------------
    # Array of passwords for 8 years, beginning January 2017.
    # Each password is for one quarter of the year
    #  i.e. Jan/Feb/Mar = quarter 1; Apr/May/Jun = quarter 2
    #--------------------------------------------------------

    passCodes = ["AJXV7S74URM5QMA7", "WLPEV4W53PVJQ3XQ", "DVJZAKD64EG6ZYXA", "PH62F5353VVM7DUN",
                 "ENT27MBWHMHRY6WF", "Y66GQUURU45AP3MF", "665HVGMA55YZTPY6", "EB56V25UTW65VP4E",
                 "MTHV622Y2A3V42SS", "ZU23VN5C56WMQNUR", "7XQAXFX465W7E5TR", "2QSLUMEEN7YPCR6M",
                 "DXMU6L6SCN476GAK", "64H7HRWV5S5F72EV", "R6FRLSZECW6ELBV5", "M2GWXM75BKCKQU66",
                 "3KPG54PPMAG6VNJB", "7E4TZSA6HVDND5K2", "L23AEKMEJZXGDWBU", "GHG5BC3ZCBL3D5ST",
                 "TJYHFZHZ2FBA3KLV", "MSR5NK6KBXHN4STR", "F6FZXBD5GLWU47ZN", "T6BQLLMPMVU6NY6P",
                 "VNBHE62V7DZQ53X2", "GFPM5A643HLCUVPC", "MA65XUEUQMKVAJGX", "RR6E6556YFCYLFFH",
                 "J5NMSZ2DAXNCMN65", "WGZ7JZT54MPWKUBH", "DZ2THM7QKN67ZR3R", "JW7QU35E5FKGQFC5"]


    currMonth = datetime.date.today().month
    currYear = datetime.date.today().year


    yearCalc = (currYear % 8) * 4



    quarterCalc = 0;
    if( currMonth >= 1 and currMonth <= 3 ):
        quarterCalc = 0
    elif( currMonth >= 4 and currMonth <= 6 ):
        quarterCalc = 1
    elif( currMonth >= 7 and currMonth <= 9 ):
        quarterCalc = 2
    elif( currMonth >= 10 and currMonth <= 12 ):
        quarterCalc = 3

    passOffset = yearCalc + quarterCalc;

    rvalQString = passCodes[passOffset]

    return rvalQString

def HOTP(K, C, digits=6, digestmod=hashlib.sha1):
    """
    HOTP accepts key K and counter C
    optional digits parameter can control the response length
    returns the OATH integer code with {digits} length
    """
    C_bytes = struct.pack(b"!Q", C)
    hmac_digest = hmac.new(key=K, msg=C_bytes,
                           digestmod=digestmod).hexdigest()
    return Truncate(hmac_digest)[-digits:]


def TOTP(K, digits=6, window=30, clock=None, digestmod=hashlib.sha1):
    """
    TOTP is a time-based variant of HOTP.
    It accepts only key K, since the counter is derived from the current time
    optional digits parameter can control the response length
    optional window parameter controls the time window in seconds
    returns the OATH integer code with {digits} length
    """
    if clock is None:
        clock = time.time()
    C = int(clock / window)
    return HOTP(K, C, digits=digits, digestmod=digestmod)


def Truncate(hmac_digest):
    """
    Truncate represents the function that converts an HMAC
    value into an HOTP value as defined in Section 5.3.
    http://tools.ietf.org/html/rfc4226#section-5.3
    """
    offset = int(hmac_digest[-1], 16)
    binary = int(hmac_digest[(offset * 2):((offset * 2) + 8)], 16) & 0x7fffffff
    return str(binary)


if __name__ == "__main__":

    print((TOTP(base64.b32decode(getPassKey()), digits=6, window=14400)))

