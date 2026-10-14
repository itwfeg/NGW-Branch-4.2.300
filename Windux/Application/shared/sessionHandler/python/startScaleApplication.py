#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sys
import time
import deviceDiscovery
import shellCommand
import logging
import subprocess
import getBoardType
import getCanDeviceType
import serialFlash

from chooseModel import chooseModel

pythonPath = os.path.join("/","usr","local","hobart","python")
appsPath = os.path.join("/","usr","local","hobart","apps")

def determineScaleType():

    if not os.path.exists("/etc/hobart/scale-type.dat"):
        logging.debug('Before initial call to deviceDiscovery()')
        device = deviceDiscovery.atmelDevice()
        logging.debug('After initial call to deviceDiscovery()')

    #----------------------------------------------------------------------------------
    # HT-1
    # With the new Timesys 3.10 kernel, deviceDiscovery is not always successful in
    # determing the scale type, and defaults it to "unknown".  Once the scale is up
    # a longer period of time, then deviceDiscovery is able to successfully
    # determine the scale type.
    #
    # So, logic has been added here to check the contents of the scale-type.dat file,
    # and keep looping and calling deviceDiscovery until it does not contain "unknown".
    #----------------------------------------------------------------------------------
    scaleType = 'unknown'

    loopCount = 0
    done = False

    while done == False and loopCount < 30:
        try:
            with open( "/etc/hobart/scale-type.dat", 'r' ) as f:
                scaleType = f.read()

            scaleType = scaleType.replace( '\n', '' )
            logging.debug('From scale-type.dat |%s|', scaleType )

            if scaleType == 'unknown':
                # for ARK board we can check CAN bus for devices
                if getBoardType.isArkBoard():

                    model =  int(getCanDeviceType.detectModel())
                        
                    if (544 == model) or (560 == model) or (545 == model) or (592 == model):
                        #check the manufacturing section of the serial flash for blank section or incorrect scale sytle setting
                        if (545 == model):
                            flash = serialFlash.mfgSection()
                            if flash.isSectionBlank() == True:
                                flash.setScaleStyle('HTm')
                            else:
                                style = flash.getScaleStyle()
                                if style[2] != "m" and style[2] != "M":
                                    flash.setScaleStyle('HTm')                            
                        detected = 'service counter'
                    elif (576 == model) or (608 == model) or (624 == model):
                        detected = 'pre-pack'

                    if detected is not None:
                        with open( "/etc/hobart/scale-type.dat", "wb" ) as f:
                            f.write( detected.encode('utf-8'))
                            f.write( '\n'.encode('utf-8'))
                        continue

                # detect USB device
                logging.debug('scale-type.dat=unknown - Before call to deviceDiscovery(), loopCount=%s', str(loopCount))
                device = deviceDiscovery.atmelDevice()
                logging.debug('scale-type.dat=unknown - After call to deviceDiscovery()')

            else:
                done = True

        except:
            pass

        loopCount = loopCount + 1
        time.sleep( 1.0 )

    # If we are still unknown scale type even after all of the above tries lets
    # just prompt the user to pick what they are
    if scaleType == 'unknown':
        logging.debug('unkown model... prompting')
        chooseModel()

def main( ):

    logging.basicConfig(filename='/tmp/startScaleApplication.log', filemode='w', level=logging.DEBUG, format='%(asctime)s %(message)s', datefmt='%m/%d/%Y %I:%M:%S %p')
    logging.debug('Beginning of startScaleApplication logging')

    squishEnabled = False
    recordPlaybackEnabled = False

    # check for squish
    if os.path.isfile("/usr/bin/squishserver"):
        os.environ['SQUISH_PREFIX'] = '/usr'
        squishEnabled = True
        
        # check if record playback functionality enabled
        if os.path.isfile("/usr/bin/squishexecutive"):
            recordPlaybackEnabled = os.path.exists("/usr/local/hobart/features/record_playback")

    # check for java
    if os.path.exists("/opt/java"):
        os.environ['JAVA_HOME'] = '/opt/java'
        os.environ['PATH'] += os.pathsep + '/opt/java/bin'

    # need to determine our scale type
    if (not os.path.exists(os.path.join("/", "etc", "hobart", "blackbox")) and
            not os.path.exists(os.path.join("/", "etc", "hobart", "coronaPsu")) and
            not os.path.exists(os.path.join("/", "etc", "hobart", "tdcc")) and
            not os.path.exists(os.path.join("/", "etc", "hobart", "testsrv")) and
            not os.path.exists(os.path.join("/", "etc", "hobart", "masterblaster")) and
            not os.path.exists(os.path.join("/", "etc", "hobart", "hflbProgrammer")) and
            not os.path.exists(os.path.join("/", "etc", "hobart", "slicer"))):
        determineScaleType()

    # startup app daemon processes
    logging.debug('Before start appsStartup')
    os.system("DISPLAY=:0 /usr/bin/python %s --folder %s &" % (os.path.join(pythonPath,"appsStartup.py"), appsPath))
    logging.debug('After start appsStartup')
    
    if squishEnabled:
        os.system('/usr/bin/squishserver &')

    # Start GCD ... only if Service Counter
    try:
        scaleType = 'unknown'
        displayMode = 'unknown'

        if os.path.isfile( "/etc/hobart/scale-type.dat" ):
            file = open( "/etc/hobart/scale-type.dat", 'r' )
            scaleType = file.read()
            scaleType = scaleType.replace( '\n', '' )
            file.close()

        if os.path.isfile( "/sys/class/graphics/fb2/modes" ):
            file = open( "/sys/class/graphics/fb2/modes", 'r' )
            displayMode = file.read()
            displayMode = displayMode.replace( '\n', '' )
            file.close()

        logging.debug( 'scale type: %s' % (scaleType) )
        logging.debug( 'display mode: %s' % (displayMode) )

        if (( scaleType == 'service counter' ) and ( displayMode != 'unknown' )):

            # blank framebuffer
            cmd = []
            cmd.append("echo")
            cmd.append("1")
            cmd.append(">")
            cmd.append("/sys/class/graphics/fb2/blank")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            # unblank framebuffer
            cmd = []
            cmd.append("echo")
            cmd.append("0")
            cmd.append(">")
            cmd.append("/sys/class/graphics/fb2/blank")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            # enable gcd framebuffer
            cmd = []
            cmd.append("echo")
            cmd.append(displayMode)
            cmd.append(">")
            cmd.append("/sys/class/graphics/fb2/mode")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            # start gcd application
            # unless i'm a slicer
            if(not os.path.exists("/etc/hobart/slicer")):
                 os.system('DISPLAY=:0 nice -n -5 /usr/bin/gcd -platform linuxfb:fb=/dev/fb2 &')

            # turn on gcd backlight
            cmd = []
            cmd.append("echo")
            cmd.append("1")
            cmd.append(">")
            cmd.append("/usr/local/gpio/gcd_backlight_enable/value")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        elif(os.path.exists("/etc/hobart/slicer")):
            # turn on gcd backlight
            cmd = []
            cmd.append("echo")
            cmd.append("1")
            cmd.append(">")
            cmd.append("/usr/local/gpio/gcd_backlight_enable/value")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

        else:

            # turn off gcd backlight
            cmd = []
            cmd.append("echo")
            cmd.append("0")
            cmd.append(">")
            cmd.append("/usr/local/gpio/gcd_backlight_enable/value")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

    except:
        pass
    
    if ( os.path.exists(os.path.join("/","etc","hobart","online_ordering"))):
        # script that alerts manager to overdue orders in the OO system
        os.system("nice -n 15 python3 -u /usr/local/hobart/python/alert_overdue_orders.py > /tmp/alert_overdue_orders.log &")

    # Blackbox (Now Serving)
    if ( os.path.exists(os.path.join("/","etc","hobart","blackbox")) or os.path.exists(os.path.join("/","etc","hobart","enable_mysql")) ):

        # check for HD display
        if (0 == os.system('DISPLAY=:0 xrandr |grep 1920x1080')):
            os.system('DISPLAY=:0 /usr/bin/xrandr -s 1920x1080')

        if( os.path.exists(os.path.join("/","etc","hobart","enable_mysql") ) ):
            # Online Ordering mode
            logging.debug('Before start nowServing')
            os.system('cd /usr/local/hobart/nowServing && DISPLAY=:0 nowServing -oo &')
            logging.debug('After start nowServing')
        else:
            logging.debug('Before start nowServing')
            os.system('cd /usr/local/hobart/nowServing && DISPLAY=:0 nowServing -tv &')
            logging.debug('After start nowServing')

        # Telit Gateway
        if (os.path.exists(os.path.join("/","etc","hobart","telit"))):

            # try to establish connection do devicewise before launching app
            try:
                loops = 0
                dwapi = 0
                dwm2m = 0

                # this is kind of tricky...
                # Telit alternates between a few different ip addresses and the DNS switches
                # up every 15-30 seconds.. so if the first ping succeeds and we continue on we risk
                # it changing before telit starts up which will cause a failure in devicewise, so
                # here we attampt to resolv a few different ip addresses before moving on
                while loops < 30 and dwapi < 8 and dwm2m < 8:
                    response = subprocess.call( ['ping', '-c', '1', '-W', '1', 'api.devicewise.com' ] )
                    if 0 == response:
                        dwapi = dwapi + 1

                    response = subprocess.call( ['ping', '-c', '1', '-W', '1', 'm2m.devicewise.com' ] )
                    if 0 == response:
                        dwm2m = dwm2m + 1

                    time.sleep( 10 )
                    loops = loops + 1
            except:
                pass

            # Telit DeviceWise
            if os.path.exists(os.path.join("/","opt","dw","devicewise")):
                logging.debug('Before start devicewise')
                os.system('/opt/dw/devicewise start')
                logging.debug('After start devicewise')

                # Let it startup and start listening for UDP packets
                time.sleep( 10 )

            # UDP Discovery script
            logging.debug('Before start gateway')
            os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"gatewayDiscovery.py")))
            logging.debug('After start gateway')

            # UDP Broadcast script
            logging.debug('Before start gateway server')
            os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"gatewayServer.py")))
            logging.debug('After start gateway server')

    # Portion Slicer
    elif (os.path.exists(os.path.join("/","etc","hobart","slicer"))):

        logging.debug('Before start slicerui')
        os.system('DISPLAY=:0 /usr/bin/xrandr -o right')
        os.system('DISPLAY=:0 /usr/bin/slicerui &')
        logging.debug('After start slicerui')

    elif os.path.exists(os.path.join("/","etc","hobart","coronaPsu")):
        pass

    elif os.path.exists(os.path.join("/","etc","hobart","tdcc")):
        pass

    # Hillsboro Test Server
    # E-MEK Test Server
    elif (os.path.exists(os.path.join("/","etc","hobart","testsrv"))):

        # Telit Gateway
        if (os.path.exists(os.path.join("/","etc","hobart","telit"))):

            # Telit DeviceWise
            if os.path.exists(os.path.join("/","opt","dw","devicewise")):
                logging.debug('Before start devicewise')
                os.system('/opt/dw/devicewise start')
                logging.debug('After start devicewise')

    elif (os.path.exists(os.path.join("/","etc","hobart","masterblaster"))):
        # keep other ui's from appearing
        # masterblaster UI starts at the end of startupsequence.py
        logging.debug('Before masterblaster')
        logging.debug("After MasterBlaster App")

    elif os.path.exists(os.path.join("/","etc","hobart","hflbProgrammer")):
        # keep other ui's from appearing
        # HFLB Programmer UI starts at the end of startupsequence.py
        logging.debug('Before hflbProgrammer App')
        logging.debug("After hflbProgrammer App")

    # Hillsboro Manual Accelerometer Calibration Tool
    elif (os.path.exists(os.path.join("/","etc","hobart","weighercalib"))):

        startApplication = False
        
        if startApplication:
        
            # Hobart Daemon
            logging.debug('Before start chobart_d')
            os.system('/usr/bin/chobart_d &')
            logging.debug('After start chobart_d')

            # Scale UI ... Decided to start scaleui along with weighercalib ...
            logging.debug('Before start scaleui')
            os.system('DISPLAY=:0 /usr/bin/scaleui &')
            logging.debug('After start scaleui')

        # weigher calibration tool
        logging.debug('Before start weighercalib')
        os.system('DISPLAY=:0 /usr/bin/weighercalib &')
        logging.debug('After start weighercalib')

	# Hillsboro Auto Rig Accelerometer Calibration Tool
    elif (os.path.exists(os.path.join("/","etc","hobart","autocalibrationrig"))):

        startApplication = False
        
        if startApplication:

            # Hobart Daemon
            logging.debug('Before start chobart_d')
            os.system('/usr/bin/chobart_d &')
            logging.debug('After start chobart_d')

            # Scale UI ... Decided to start scaleui along with autocalibrationrig ...
            logging.debug('Before start scaleui')
            os.system('DISPLAY=:0 /usr/bin/scaleui &')
            logging.debug('After start scaleui')

        # autocalibrationrig weigher calibration tool
        logging.debug('Before start autocalibrationrig')
        os.system('DISPLAY=:0 /usr/bin/autocalibrationrig &')
        logging.debug('After start autocalibrationrig')
    
    # Manufacturing GCD display test fixture
    elif (os.path.exists(os.path.join("/","etc","hobart","boardsitelvds"))):

        # Hobart Daemon
        logging.debug('Before start chobart_d')
        os.system('/usr/bin/chobart_d &')
        logging.debug('After start chobart_d')

        # Test UI
        logging.debug('Before start lvdsTestUI')
        os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"lvdsTestUI.py")))
        logging.debug('After start lvdsTestUI')


    # Service Counter or Access
    else:

        # Enable the HUB
        cmd = []
        cmd.append("echo")
        cmd.append("1")
        cmd.append(">")
        cmd.append("/usr/local/gpio/hub1_enable/value")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # SOF-3827
        # Make sure loopback is operational
        cmd = []
        cmd.append("ifconfig")
        cmd.append("lo")
        cmd.append("up")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # Hobart Daemon
        logging.debug('nice -n -10 /usr/bin/chobart_d &')
        os.system('nice -n -10 /usr/bin/chobart_d &')
        logging.debug('After start chobart_d')

        # Scale UI
        logging.debug('Before start scaleui')
        os.system('DISPLAY=:0 nice -n -5 /usr/bin/scaleui &')
        logging.debug('After start scaleui')

        # Process Monitor
        logging.debug('Before start processMonitor')
        os.system("nice -n 15 /usr/bin/python %s &" % (os.path.join(pythonPath,"processMonitor.py")))
        logging.debug('After start processMonitor')

    # crond
    logging.debug('Before start crond')
    os.system('/usr/sbin/crond &')
    logging.debug('After start crond')

    if recordPlaybackEnabled:
        os.system('cd /root/.squish && DISPLAY=:0 /usr/bin/squishexecutive &')

    logging.debug('End of startScaleApplication logging')
    
    if os.path.exists(os.path.join("/", "etc", "hobart", "blackbox")):
        os.system("/usr/bin/python %s smooth 3" % (os.path.join(pythonPath,"blackboxBeeps.py")))


if __name__ == "__main__":
    main()
    sys.exit(0)
