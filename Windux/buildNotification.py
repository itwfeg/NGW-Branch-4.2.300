#!/usr/bin/env python

import select
import socket
import string
import sys

#---------------------------------------------------------
# List of people that receive notifications for all builds
#---------------------------------------------------------
notificationList = []
notificationList.append( '9372313971' ) # Randy Blankley
notificationList.append( '9376233862' ) # Larry Pevoar
notificationList.append( '9378237186' ) # Scott Davis
notificationList.append( '9376726726' ) # Gary Cornish
notificationList.append( '9376583084' ) # Kraig Wooddell
notificationList.append( '9376026782' ) # Jim Highley
notificationList.append( '9373081387' ) # Justin Bandura
notificationList.append( '9378758220' ) # Todd Davis
notificationList.append( '9375245731' ) # David Walker
#notificationList.append( '9372168721' ) # Doug Antonides


#-------------------------------------------------------------
# List of people that only receive text message notifications
# for a production release build (i.e. build number ends in 0)
#-------------------------------------------------------------
extendedNotificationList = []
extendedNotificationList.append( '9378387146' ) # Rob Ryan
extendedNotificationList.append( '9375095771' ) # Johnny Price
extendedNotificationList.append( '9374057922' ) # Debra Dyer
extendedNotificationList.append( '9374057875' ) # Dave Williams (aka Safety Dave)
extendedNotificationList.append( '9375700459' ) # Dave Fosnight
extendedNotificationList.append( '9372512871' ) # Darrell Needler
#extendedNotificationList.append( '9375701487' ) # Teresa Williams

#
# Locate gateway device with capability.
#
def locateGatewayDevice( cap ):
    gateway = ''

    # open listener
    sock = socket.socket( socket.AF_INET, socket.SOCK_DGRAM )
    sock.bind( ('', 5090) )

    # wait for discovery message
    waitTime = 120

    while (( 0 < waitTime ) and ( not gateway )):
        ready = select.select( [sock], [], [], 1 )

        if ( ready[0] ):
            data = sock.recv( 4096 )

            # found gateway, check capabilities
            if ( 'hobartGateway' in data ):
                info = string.split( data, '|' )

                if ( 2 <= len( info ) ):
                    print 'Query gateway ' + info[1]

                    try:
                        s = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
                        s.connect( (info[1], 5076) )
                        s.settimeout( 5.0 )
                        s.send( 'getCaps\n' )

                        data = s.recv( 4096 )
                        caps = string.split( string.strip( data ), '|' )

                        print caps

                        s.close()

                        if ( cap in caps ):
                            print 'FOUND!'
                            gateway = info[1]

                    except:
                        print 'ERROR'

        waitTime -= 1

    sock.close()

    return gateway

#
# Notify user of build completion
#
def notifyBuildComplete( gateway, phone, build, dirName ):
    result = False

    msg =  'Build ' + build + ' (in ' + dirName + ') is ready for System Test!\n'
    msg += '\n'
    msg += 'Please see Doug Antonides for a master.'

    try:
        s = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
        s.connect( (gateway, 5076) )
        s.settimeout( 5.0 )
        s.send( 'sendSMS|' + phone + '|' + str(len(msg)) + '|0|\n' + msg + '\n' )
        s.send( msg + '\n' )

        data = s.recv( 4096 )

        if ( 'done' in data ):
            result = True

        s.close()

    except:
        pass

    return result

# grab command line arguments
build = sys.argv[1]
dirName = sys.argv[2]

# locate gateway device with cell capability
print 'Searching for gateway...'
gateway = locateGatewayDevice( 'cell' )

if ( not gateway ):
    print 'Could not locate gateway device!'
else:

    # notify everyone
    for phone in notificationList:
        if ( not notifyBuildComplete( gateway, phone, build, dirName ) ):
            print 'Error notifying ' + phone

    # for production releases notify extended list also
    if (( 2 < len(build) ) and ( '.0' == build[-2:] )):
        for phone in extendedNotificationList:
            if ( not notifyBuildComplete( gateway, phone, build, dirName ) ):
                print 'Error notifying ' + phone
