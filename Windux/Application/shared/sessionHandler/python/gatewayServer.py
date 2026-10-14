#!/usr/bin/env python

import datetime
import os
import select
import socket
import struct
import sys
import time
import serial


# udp socket for broadcast (i.e. Telit)
# to disable this set to 'None'
udpSock = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
udpAddress = ( "127.0.0.1", 5080 )

# udp queue
udpQueue = []


#
# Message list that may be loaded/written to the filesystem.
#
class spoolFile:

    # Initialization.
    def __init__( self, filename, maxSize ):
        self.filename = filename
        self.maxSize = maxSize
        self.data = []

        self.load()

    # Load from file.
    def load( self ):
        try:
            f = open( self.filename, 'rb' )

            while True:
                data = f.read( 2 )

                if not data:
                    break
                elif 2 != len( data ):
                    break

                dataLen = struct.unpack_from( '!H', data )
                dataLen = dataLen[0]

                data = f.read( dataLen )

                if dataLen != len(data):
                    break

                self.push( data )

            f.close()

        except:
            pass

        print(('loaded ' + str(self.size()) + ' records from spool'))

    # Save to file.
    def save( self ):
        try:
            f = open( self.filename, 'wb' )

            for d in self.data:
                f.write( struct.pack( '!H', len(d) ) )
                f.write( d )

            f.close()

        except:
            pass

    # Retrieve spool size.
    def size( self ):
        return len(self.data)

    # Check if full.
    def full( self ):
        return self.maxSize <= self.size()

    # Check if empty.
    def empty( self ):
        return 0 == self.size()

    # Add message to spool.
    def push( self, d ):
        if self.full():
            self.pop()

        self.data.append( d )

    # Remove message from spool.
    def pop( self ):
        self.data.pop( 0 )

    # Retrieve element from spool.
    def front( self ):
        return self.data[0]

    # Remove all messages from spool.
    def clear( self ):
        self.data = []

#
# Process client socket.
#
def processClient( sock, sockInfo, spool ):

    print(('processing client ' + sockInfo['address'] + '...'))

    while True:
        if ( len( sockInfo['buffer'] ) < 2 ):
            break

        messageLen = struct.unpack_from( '!H', sockInfo['buffer'] )
        messageLen = messageLen[0]

        if ( len( sockInfo['buffer'] ) < (2+messageLen) ):
            break

        fullMessage = sockInfo['buffer'][0:2+messageLen]
        message = sockInfo['buffer'][2:2+messageLen]

        sockInfo['buffer'] = sockInfo['buffer'][2+messageLen:]

        # header structure:
        #  message type (1)
        #  message version (1)
        #  unix epoch timestamp (4)
        #  serial number (16)
        #  model number (16)
        if len(message) < 38:
            print('    message format error')

        else:
            header = struct.unpack_from( '!BBL16s16s', message )
            print(('    message ' + str(header[0]) + '/v' + str(header[1]) + ' len ' + str(messageLen) + ' [' + str(header[2]) + ']'))

            # append to UDP queue
            if udpSock is not None:
                udpQueue.append( fullMessage )

            # process message
            spool.push( message )
            spool.save()

####################################################################################################
# Gateway Server Script
####################################################################################################

SERIAL_PORT = '/dev/ttymxc4'
SERIAL_PORT_BAUD_RATE = '115200'
SERIAL_PORT_PARITY = serial.PARITY_NONE
SERIAL_PORT_STOPBITS = serial.STOPBITS_ONE
SERIAL_PORT_BYTE_SIZE = serial.EIGHTBITS
SERIAL_PORT_TIMEOUT = 1

HOST_ADDRESS = ''
HOST_PORT = 5080
HOST_MAX_CONNECTIONS = 128

HOST_INACTIVE_SOCKET_TIMER = datetime.timedelta( minutes=15 )
HOST_INACTIVE_LIMIT = datetime.timedelta( minutes=75 )

HOST_UDP_BROADCAST_TIMER = datetime.timedelta( seconds=10 )

# client sockets
clients = {}
serverSocket = None

# offline spool
spool = spoolFile( '/tmp/gateway.spool', 2048 )

# socket removal timer
inactiveSocketTimer = datetime.datetime.now() + HOST_INACTIVE_SOCKET_TIMER

# udp broadcast timer
udpBroadcastTimer = datetime.datetime.now() + HOST_UDP_BROADCAST_TIMER

# open server socket
try:
    serverSocket = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
    serverSocket.setblocking( 0 )
    serverSocket.setsockopt( socket.SOL_SOCKET, socket.SO_REUSEADDR, 1 )

    serverSocket.bind( (HOST_ADDRESS, HOST_PORT) )
    serverSocket.listen( HOST_MAX_CONNECTIONS )

    clients[serverSocket] = {'address': HOST_ADDRESS, 'port': HOST_PORT, 'lastComm': datetime.datetime.now(), 'buffer': b''}

    print ('listening for new connections...')
	
except socket.error as xxx_todo_changeme1:
    (value, message) = xxx_todo_changeme1.args
    serverSocket.close()

    print(('error opening server: ' + message))
    sys.exit( -1 )

except:
    serverSocket.close()

    print('error opening server')
    sys.exit( -1 )

try:
    serial_port_open = False
    serial_port = serial.Serial(port=SERIAL_PORT,
                                     baudrate=SERIAL_PORT_BAUD_RATE,
                                     parity=SERIAL_PORT_PARITY,
                                     stopbits=SERIAL_PORT_STOPBITS,
                                     bytesize=SERIAL_PORT_BYTE_SIZE,
                                     timeout=SERIAL_PORT_TIMEOUT)

    serial_port_file_number = serial_port.fileno()

    # serial_port.open()
    serial_port.flushInput()
    serial_port.flushOutput()
    serial_port_open = True

    clients[serial_port_file_number] = {'address': 'UART:' + str(serial_port_file_number),
                                             'port': SERIAL_PORT,
                                             'lastComm': datetime.datetime.now(),
                                             'buffer': b''}

    print ('listening for new serial port diagnostic data...')

except:
    print ('WARNING: can not create or open serial port ')
    print ('No serial port connection for black box device')
    serial_port.close()


#
# message loop
#
while True:

    services = list(clients.keys())

    # perform select
    reads, writes, errors = select.select( services, [], services, 15.0 )

    now = datetime.datetime.now()
    deletes = []

    # service errors
    for service in errors:
        print ('socket in error state ' + clients[service]['address'])
        deletes.append( service )

    # service reads
    for service in reads:

        if service is serverSocket:

            # accept incoming connection
            print('accept...')
            clientSocket, address = service.accept()

            if clientSocket:
                # setup client serviceet
                service.setblocking( 0 )

                # add client to list
                print(('new connection from ' + address[0] + ':' + str( address[1] )))
                clients[clientSocket] = {'address': address[0], 'port': address[1], 'lastComm': now, 'buffer': ''}

                print(('active sockets: ' + str( len( clients ) )))

        elif service not in deletes:
            badservice = True

            try:
                # receive pending data
                print (('recv from ' + clients[service]['address'] + '...'))

                if service is serial_port_file_number:
                    message_length = 2
                    message_size = serial_port.read(size=message_length)
                    data = message_size + serial_port.read(size=int.from_bytes(message_size, 'big'))
                else:
                    data = service.recv( 4096 )

                if not data:
                    pass
                elif len( data ) <= 0:
                    pass
                else:
                    # append data to buffer
                    clients[service]['buffer'] += data
                    badservice = False

            except socket.error as xxx_todo_changeme:
                (value, message) = xxx_todo_changeme.args
                print(('error during recv: ' + message))

            except socket.timeout:
                print('timeout during recv')

            except:
                print('error during recv')

            if badservice:
                deletes.append( service )
            else:
                # process
                processClient( service, clients[service], spool )
                clients[service]['lastComm'] = now



    # check for inactive sockets
    if inactiveSocketTimer <= now:
        for service in services:
            if service is serverSocket:
                continue
            elif service is serial_port_file_number:
                continue
            elif service in deletes:
                continue

            delta = now - clients[service]['lastComm']

            if HOST_INACTIVE_LIMIT <= delta:
                print (('client ' + clients[service]['address'] + ' will be removed due to inactivity'))
                deletes.append( service )

        inactiveSocketTimer = now + HOST_INACTIVE_SOCKET_TIMER

    # check udp broadcast
    if len( udpQueue ) and udpBroadcastTimer < now:
        print(('send udp (' + str(len(udpQueue)) + ')'))
        udpSock.sendto( udpQueue[0], udpAddress )
        udpQueue.pop( 0 )

        udpBroadcastTimer = now + HOST_UDP_BROADCAST_TIMER

    # delete bad sockets
    if len( deletes ):
        print('culling clients...')

        for service in deletes:
            try:
                service.close()
            except:
                pass

            del clients[service]

        print(('active sockets: ' + str( len( clients ) )))
