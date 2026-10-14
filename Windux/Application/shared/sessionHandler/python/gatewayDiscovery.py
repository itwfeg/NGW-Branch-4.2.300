#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import fcntl
import os
import socket
import struct
import time

#
# Retrieve adapter info.
#
def get_adapter_info( adapterName ):
    s = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )

    info = []

    try:
        packedAdapterName = struct.pack( '256s', adapterName[:15] )

        # SIOCGIFADDR
        ifr = fcntl.ioctl( s.fileno(), 0x8915, packedAdapterName )
        info.append( socket.inet_ntoa( ifr[20:24] ) )

        # SIOCGIFNETMASK
        ifr = fcntl.ioctl( s.fileno(), 0x891b, packedAdapterName )
        info.append( socket.inet_ntoa( ifr[20:24] ) )

        # SIOCGIFBRDADDR
        ifr = fcntl.ioctl( s.fileno(), 0x8919, packedAdapterName )
        info.append( socket.inet_ntoa( ifr[20:24] ) )

    except:
        pass

    return info

# generate list of adapters
adapters = []
adapters.append( 'eth0' )
adapters.append( 'wlan0' )

# advertise forever...
while True:

    # process each address
    for adapter in adapters:
        info = get_adapter_info( adapter )

        if ( 3 != len(info) ):
            continue

        try:
            addr = info[0]
            brdaddr = info[2]

            # generate message
            message = []
            message.append( 'hobartGateway' )
            message.append( addr )

            if (os.path.isfile('/etc/hobart/blackbox')):
                message.append( 'hobart' )
            elif (os.path.isfile('/etc/hobart/multitech')):
                message.append( 'multitech' )
            elif (os.path.isfile('/etc/hobart/vantron')):
                message.append( 'vantron' )
            else:
                message.append( 'unknown' )

            # broadcast
            s = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
            s.setsockopt( socket.SOL_SOCKET, socket.SO_BROADCAST, 1 )
            s.sendto( '|'.join( message ) + '\n', ( brdaddr, 5090 ) )

        except:
            raise
            pass

    time.sleep( 30 )


