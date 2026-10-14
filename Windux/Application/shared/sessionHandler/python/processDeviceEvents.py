#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import struct
import time
import sys
import os
import select
import socket
import string
import sql
import bootSettings
import setBrightness
import setVolume
import netInterface
import netManager
import applicationTimer
import getModelName
import beep
import re
import glob
import shellCommand
import messageBox
import languageSettings


if os.name == 'nt':
    import ctypes, ctypes.wintypes


def isWindowsPrepack():
    if sys.platform == 'win32':
        #check to see if config file is present
        if os.path.exists(os.path.join("C:\Program Files", "Hobart")):
            fileIn = open(os.path.join("C:\Program Files", "Hobart", "configc.dat"), 'rb')
            fileData = fileIn.readlines()
            fileIn.close()

            p = re.compile('scaleModel=([0-9])')

            for line in fileData:
                if p.match(line) != None and p.match(line).groups()[0] == '2':
                    #print("IT is a Prepack")
                    return True

    #default to service scale
    #print("No Prepack")
    return False

def convertStr(s):
    return str(int(s) - 104)

#Create global variable for slicer-specific code
IS_SLICER = False
if(os.path.exists("/etc/hobart/slicer")):
    IS_SLICER = True
#print("Is Slicer: " + str(IS_SLICER))

## ---------- ##

DIM_BACKLIGHT_VALUE = 104
MAX_BACKLIGHT_VALUE = 204

OPERATOR_DISPLAY_DEVICE = 2
CUSTOMER_DISPLAY_DEVICE = 1
ALL_DISPLAY_DEVICES = 0

# Checks for location of file using wildcard
def check_for_files( filepath ):

    fullPath = None

    for filepath_object in glob.glob ( filepath ):
        if os.path.isfile( filepath_object ):
            fullPath = filepath_object

    return fullPath

# Locate touchscreen device
TOUCHSCREEN_INPUT_DEVICE = None
BUTTON_SOUND_DURATION = 0.05

def find_touchscreen_device():
    global TOUCHSCREEN_INPUT_DEVICE

    #Combo board (Boundary) mapping
    TOUCHSCREEN_INPUT_DEVICE = "/dev/input/touchscreen"

    #eGalax USB mapping
    device_name = "/dev/input/touchscreenusb"
    if os.path.exists( device_name ):
        TOUCHSCREEN_INPUT_DEVICE = device_name
        
    mouse0_device_name = "/dev/input/mouse0"
    mouse1_device_name = "/dev/input/mouse1"
    if (IS_SLICER and os.path.exists( mouse0_device_name ) and os.path.exists( mouse1_device_name )):
    	TOUCHSCREEN_INPUT_DEVICE = mouse1_device_name

# Locate beeper device
PIEZO_BEEPER_DEVICE = None

def find_beeper_device():
    global PIEZO_BEEPER_DEVICE

    #BCM board mapping - Default
    PIEZO_BEEPER_DEVICE = "/sys/class/backlight/buzzer.27/brightness"

    #Combo board (Boundary) mapping
    device_name = "/sys/class/leds/buzzer/brightness"
    if os.path.isfile( device_name ):
        PIEZO_BEEPER_DEVICE = device_name

    # New BCM mapping - Beeper
    # BCM board on 3.14 kernel: the 'buzzer.*' directory has a different name on scale vs. prepack
    beeper_full_path = check_for_files( "/sys/class/backlight/buzzer.*/brightness" )
    if beeper_full_path != None:
        PIEZO_BEEPER_DEVICE = beeper_full_path

    # New BCM mapping - Beeper (4.1 Kernel)
    # BCM board on 3.14 kernel: the 'buzzer.*' directory has a different name on scale vs. prepack
    beeper_full_path = check_for_files( "/sys/class/backlight/buzzer/brightness" )
    if beeper_full_path != None:
        PIEZO_BEEPER_DEVICE = beeper_full_path

# Locate operator display
OPERATOR_DISPLAY_BACKLIGHT = None
OPERATOR_MIN_BACKLIGHT_VALUE_DEV = DIM_BACKLIGHT_VALUE
OPERATOR_MAX_BACKLIGHT_VALUE_DEV = MAX_BACKLIGHT_VALUE

def find_operator_display():
    global OPERATOR_DISPLAY_BACKLIGHT
    global OPERATOR_MIN_BACKLIGHT_VALUE_DEV
    global OPERATOR_MAX_BACKLIGHT_VALUE_DEV

    #BCM board mapping
    OPERATOR_DISPLAY_BACKLIGHT = "/sys/class/backlight/backlight_lvds_0.29/brightness"

    # New BCM mapping - Operator display
    # BCM board on 3.14 kernel: the 'backlight_lvds_0.*' directory has a different name on scale vs. prepack
    operator_display_backlight_full_path = check_for_files( "/sys/class/backlight/backlight_lvds_0.*/brightness" )
    if operator_display_backlight_full_path != None:
        OPERATOR_DISPLAY_BACKLIGHT = operator_display_backlight_full_path

        operator_display_max_brightness_full_path = check_for_files( "/sys/class/backlight/backlight_lvds_0.*/max_brightness" )
        if operator_display_max_brightness_full_path != None:
            file = open( operator_display_max_brightness_full_path, 'rb' )
            OPERATOR_MAX_BACKLIGHT_VALUE_DEV = file.read().decode('utf-8')
            OPERATOR_MAX_BACKLIGHT_VALUE_DEV = int(OPERATOR_MAX_BACKLIGHT_VALUE_DEV.replace( '\n', '' ))
            file.close()

            OPERATOR_MIN_BACKLIGHT_VALUE_DEV = OPERATOR_MAX_BACKLIGHT_VALUE_DEV/10
            if OPERATOR_MIN_BACKLIGHT_VALUE_DEV == 0:
                OPERATOR_MIN_BACKLIGHT_VALUE_DEV = 1

    #Combo board (Boundary) mapping
    device_name = "/sys/class/backlight/backlight_lvds.18/brightness"
    if os.path.isfile( device_name ):
        OPERATOR_DISPLAY_BACKLIGHT = device_name

        device_name = "/sys/class/backlight/backlight_lvds.18/max_brightness"
        if os.path.isfile( device_name ):
            file = open( device_name, 'rb' )
            OPERATOR_MAX_BACKLIGHT_VALUE_DEV = file.read().decode('utf-8')
            OPERATOR_MAX_BACKLIGHT_VALUE_DEV = int(OPERATOR_MAX_BACKLIGHT_VALUE_DEV.replace( '\n', '' ))
            file.close()

            OPERATOR_MIN_BACKLIGHT_VALUE_DEV = OPERATOR_MAX_BACKLIGHT_VALUE_DEV/10
            if OPERATOR_MIN_BACKLIGHT_VALUE_DEV == 0:
                OPERATOR_MIN_BACKLIGHT_VALUE_DEV = 1

    #Combo board (Boundary) mapping (4.1 kernel)
    device_name = "/sys/class/backlight/backlight_lvds/brightness"
    if os.path.isfile( device_name ):
        OPERATOR_DISPLAY_BACKLIGHT = device_name

        device_name = "/sys/class/backlight/backlight_lvds/max_brightness"
        if os.path.isfile( device_name ):
            file = open( device_name, 'rb' )
            OPERATOR_MAX_BACKLIGHT_VALUE_DEV = file.read().decode('utf-8')
            OPERATOR_MAX_BACKLIGHT_VALUE_DEV = int(OPERATOR_MAX_BACKLIGHT_VALUE_DEV.replace( '\n', '' ))
            file.close()

            OPERATOR_MIN_BACKLIGHT_VALUE_DEV = OPERATOR_MAX_BACKLIGHT_VALUE_DEV/10
            if OPERATOR_MIN_BACKLIGHT_VALUE_DEV == 0:
                OPERATOR_MIN_BACKLIGHT_VALUE_DEV = 1

    device_name = "/sys/class/backlight/backlight_lvds1/brightness"
    if os.path.isfile( device_name ):
        OPERATOR_DISPLAY_BACKLIGHT = device_name

        device_name = "/sys/class/backlight/backlight_lvds1/max_brightness"
        if os.path.isfile( device_name ):
            file = open( device_name, 'rb' )
            OPERATOR_MAX_BACKLIGHT_VALUE_DEV = file.read().decode('utf-8')
            OPERATOR_MAX_BACKLIGHT_VALUE_DEV = int(OPERATOR_MAX_BACKLIGHT_VALUE_DEV.replace( '\n', '' ))
            file.close()

            OPERATOR_MIN_BACKLIGHT_VALUE_DEV = OPERATOR_MAX_BACKLIGHT_VALUE_DEV/10
            if OPERATOR_MIN_BACKLIGHT_VALUE_DEV == 0:
                OPERATOR_MIN_BACKLIGHT_VALUE_DEV = 1

# Locate customer display
CUSTOMER_DISPLAY_BACKLIGHT = None
CUSTOMER_MIN_BACKLIGHT_VALUE_DEV = DIM_BACKLIGHT_VALUE
CUSTOMER_MAX_BACKLIGHT_VALUE_DEV = MAX_BACKLIGHT_VALUE

def find_customer_display():
    global CUSTOMER_DISPLAY_BACKLIGHT
    global CUSTOMER_MIN_BACKLIGHT_VALUE_DEV
    global CUSTOMER_MAX_BACKLIGHT_VALUE_DEV

    #BCM board mapping
    CUSTOMER_DISPLAY_BACKLIGHT = "/sys/class/backlight/backlight_lvds_1.30/brightness"

    # New BCM mapping - Customer display
    # BCM board on 3.14 kernel: the 'backlight_lvds_1.*' directory has a different name on scale vs. prepack
    customer_display_backlight_full_path = check_for_files( "/sys/class/backlight/backlight_lvds_1.*/brightness" )
    if customer_display_backlight_full_path != None:
        CUSTOMER_DISPLAY_BACKLIGHT = customer_display_backlight_full_path

        customer_display_max_brightness_full_path = check_for_files( "/sys/class/backlight/backlight_lvds_1.*/max_brightness" )
        if customer_display_max_brightness_full_path != None:
            file = open( customer_display_max_brightness_full_path, 'rb' )
            CUSTOMER_MAX_BACKLIGHT_VALUE_DEV = file.read().decode('utf-8')
            CUSTOMER_MAX_BACKLIGHT_VALUE_DEV = int(CUSTOMER_MAX_BACKLIGHT_VALUE_DEV.replace( '\n', '' ))
            file.close()

            CUSTOMER_MIN_BACKLIGHT_VALUE_DEV = CUSTOMER_MAX_BACKLIGHT_VALUE_DEV/10
            if CUSTOMER_MIN_BACKLIGHT_VALUE_DEV == 0:
                CUSTOMER_MIN_BACKLIGHT_VALUE_DEV = 1

    device_name = "/sys/class/backlight/backlight_lvds2.19/brightness"
    if os.path.isfile( device_name ):
        CUSTOMER_DISPLAY_BACKLIGHT = device_name

        device_name = "/sys/class/backlight/backlight_lvds2.19/max_brightness"
        if os.path.isfile( device_name ):
            file = open( device_name, 'rb' )
            CUSTOMER_MAX_BACKLIGHT_VALUE_DEV = file.read().decode('utf-8')
            CUSTOMER_MAX_BACKLIGHT_VALUE_DEV = int(CUSTOMER_MAX_BACKLIGHT_VALUE_DEV.replace( '\n', '' ))
            file.close()

            CUSTOMER_MIN_BACKLIGHT_VALUE_DEV = CUSTOMER_MAX_BACKLIGHT_VALUE_DEV/10
            if CUSTOMER_MIN_BACKLIGHT_VALUE_DEV == 0:
                CUSTOMER_MIN_BACKLIGHT_VALUE_DEV = 1

    device_name = "/sys/class/backlight/backlight_lvds2/brightness"
    if os.path.isfile( device_name ):
        CUSTOMER_DISPLAY_BACKLIGHT = device_name

        device_name = "/sys/class/backlight/backlight_lvds2/max_brightness"
        if os.path.isfile( device_name ):
            file = open( device_name, 'rb' )
            CUSTOMER_MAX_BACKLIGHT_VALUE_DEV = file.read().decode('utf-8')
            CUSTOMER_MAX_BACKLIGHT_VALUE_DEV = int(CUSTOMER_MAX_BACKLIGHT_VALUE_DEV.replace( '\n', '' ))
            file.close()

            CUSTOMER_MIN_BACKLIGHT_VALUE_DEV = CUSTOMER_MAX_BACKLIGHT_VALUE_DEV/10
            if CUSTOMER_MIN_BACKLIGHT_VALUE_DEV == 0:
                CUSTOMER_MIN_BACKLIGHT_VALUE_DEV = 1


# Need to find all devices
loopCount = 0
done = False

while done == False and loopCount < 60:

    find_touchscreen_device()
    find_beeper_device()
    find_operator_display()
    find_customer_display()

    if ((os.path.exists(TOUCHSCREEN_INPUT_DEVICE)) and # touch may be symlink so use exists
        (os.path.isfile(PIEZO_BEEPER_DEVICE)) and
        (os.path.isfile(OPERATOR_DISPLAY_BACKLIGHT)) and
        (os.path.isfile(CUSTOMER_DISPLAY_BACKLIGHT))):

        done = True
    else:

        time.sleep( 0.5 )
        print("waiting for devices...")

    loopCount = loopCount + 1

print("Touchscreen input: " + TOUCHSCREEN_INPUT_DEVICE)
print("Beeper: " + PIEZO_BEEPER_DEVICE)
print("Oper Backlight: " + OPERATOR_DISPLAY_BACKLIGHT)
print("Cust Backlight: " + CUSTOMER_DISPLAY_BACKLIGHT)

## ---------- ##

displayIsDim = False

def setDisplayBrightness(device,value):
    print("Am I getting called?")
    global displayIsDim

    if value == DIM_BACKLIGHT_VALUE:
        displayIsDim = True
    else:
        displayIsDim = False

    if os.name == 'posix':
        if CUSTOMER_DISPLAY_DEVICE == device or ALL_DISPLAY_DEVICES == device:
            if os.path.exists(os.path.join(os.sep,"tmp","gcdidle.lock")):
                print('customer display is not idle')
            elif not os.path.isfile(CUSTOMER_DISPLAY_BACKLIGHT):
                print('customer display backlight does not exist')
            else:
                if displayIsDim:
                    value = CUSTOMER_MIN_BACKLIGHT_VALUE_DEV
                else:
                    value = int(value)
                    value *= CUSTOMER_MAX_BACKLIGHT_VALUE_DEV
                    value //= MAX_BACKLIGHT_VALUE

                if value < CUSTOMER_MIN_BACKLIGHT_VALUE_DEV:
                    value = CUSTOMER_MIN_BACKLIGHT_VALUE_DEV
                elif CUSTOMER_MAX_BACKLIGHT_VALUE_DEV < value:
                    value = CUSTOMER_MAX_BACKLIGHT_VALUE_DEV

                value = str(int(value))
                print('setting customer brightness: ' + value)

                with open(CUSTOMER_DISPLAY_BACKLIGHT,"w") as out_file:
                    out_file.write(value)

        if OPERATOR_DISPLAY_DEVICE == device or ALL_DISPLAY_DEVICES == device:
            if not os.path.isfile(OPERATOR_DISPLAY_BACKLIGHT):
                print('operator display backlight does not exist')
            else:
                if displayIsDim:
                    value = OPERATOR_MIN_BACKLIGHT_VALUE_DEV
                else:
                    value = int(value)
                    value *= OPERATOR_MAX_BACKLIGHT_VALUE_DEV
                    value //= MAX_BACKLIGHT_VALUE

                if value < OPERATOR_MIN_BACKLIGHT_VALUE_DEV:
                    value = OPERATOR_MIN_BACKLIGHT_VALUE_DEV
                elif OPERATOR_MAX_BACKLIGHT_VALUE_DEV < value:
                    value = OPERATOR_MAX_BACKLIGHT_VALUE_DEV

                value = str(int(value))
                print('setting operator brightness: ' + value)

                out_file = open(OPERATOR_DISPLAY_BACKLIGHT,"w")
                out_file.write(value)
                out_file.close()

    else:
        setBrightness.ntSetBacklight(convertStr(value), str(device))

dimDisplayTimeout = 10 * 60 # 10 minutes

configSettings = bootSettings.getBootSettings()
beeperVolume = int(configSettings['oVolume'])
operatorDisplayBrightness = int(configSettings['cBrightness'])
customerDisplayBrightness = int(configSettings['oBrightness'])
dimDisplayTimeout = int(configSettings['dimDisplayTimeout']) * 60
if(dimDisplayTimeout == 0 and 'dimDisplayTimeout' not in configSettings ):
    dimDisplayTimeout = 10 * 60 #default to 10 minutes if zero
# Set default dimeDisplayTimeout to 10 in scalesettings folder
connectionType = int(configSettings['connectionType'])
isStatic = int(configSettings['isStatic'])

if os.name == 'posix':
    setDisplayBrightness( OPERATOR_DISPLAY_DEVICE, configSettings['oBrightness'] )
    setDisplayBrightness( CUSTOMER_DISPLAY_DEVICE, configSettings['cBrightness'] )
else:

    if isWindowsPrepack():
        #print("changing displayPorts")
        CUSTOMER_DISPLAY_DEVICE = 2
        OPERATOR_DISPLAY_DEVICE = 1

        setBrightness.ntSetBacklight(duty=convertStr(configSettings['oBrightness']), device=str(OPERATOR_DISPLAY_DEVICE))

    else:
        setBrightness.ntSetBacklight(duty=convertStr(configSettings['oBrightness']), device=str(OPERATOR_DISPLAY_DEVICE))
        setBrightness.ntSetBacklight(duty=convertStr(configSettings['cBrightness']), device=str(CUSTOMER_DISPLAY_DEVICE))

setVolume.volumeSetting(configSettings['cVolume'])


## ---------- ##

FORMAT = 'llHHI'
EVENT_SIZE = struct.calcsize(FORMAT)

if dimDisplayTimeout != 0:
    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, ALL_DISPLAY_DEVICES, DIM_BACKLIGHT_VALUE)

if os.name != 'posix':
    setBeepExitCode = beep.ntSetBeep(beeperVolume > 0)
    while setBeepExitCode != 0:
        try:
            # attempt to set beep on or off
            setBeepExitCode = beep.ntSetBeep(beeperVolume > 0)
        except:
            time.sleep(2)

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_address = ('localhost', 5077)
print('processDeviceEvents starting up on %s port %s' % server_address, str(sys.stderr))
server.bind(server_address)
server.listen(5)

MSG_EXITCODE = 0
MSG_PID = 0

#Send a popup message if touchscreen gets disconnected
#Only execute the message if it hasn't been sent yet
def handleTouchscreenDisconnected():
    global MSG_PID
    
    if(MSG_PID == 0):
        msg = "Touchscreen not detected. Please contact\nyour local Hobart-authorized service office."
        msg += "\n\nL'ecran tactile non detecte. Contactez\nle bureau de service Hobart autorise local."
        MSG_EXITCODE, MSG_PID = messageBox.display(msg)
        print("\nTouchscreen not connected: " + str(MSG_EXITCODE) + " " + str(MSG_PID))

#If there's an existing popup message, kill it
def handleTouchscreenReconnected():
    global MSG_PID
    
    if(MSG_PID != 0):
        messageBox.destroy(MSG_PID)
        MSG_PID = 0
        print( "Touchscreen reconnected!" )

#If slicer, check on startup to make sure the Galaxy touchscreen or a service mouse is connected
#Send a message on screen if it isn't
if((IS_SLICER) and (TOUCHSCREEN_INPUT_DEVICE == "/dev/input/touchscreen")):
    print( "Startup: Touchscreen not connected \n" )
    time.sleep(5)
    handleTouchscreenDisconnected()
    while (TOUCHSCREEN_INPUT_DEVICE == "/dev/input/touchscreen"):
        time.sleep(1) #Keep checking every second until the correct touchscreen is found
        find_touchscreen_device()
    handleTouchscreenReconnected()

event_input_device_found = False
event_input_device = None

shutdown = False

while not shutdown:
    
    # check for bad event input
    if event_input_device is not None:
        if event_input_device.closed or not os.path.exists( TOUCHSCREEN_INPUT_DEVICE ):

            print('have bad touchscreen')
            event_input_device.close()
            event_input_device = None

    # check for new event input
    if event_input_device is None:
        
        if not event_input_device_found:
            find_touchscreen_device()
        
        if os.path.exists( TOUCHSCREEN_INPUT_DEVICE ):
            
            try:
                # attempt to get touch screen device
                # it is not guaranteed that udev has created line for device
                # need to try and open
                event_input_device = open(TOUCHSCREEN_INPUT_DEVICE, "rb")
                print( "Read worked" )
                
                #If touchscreen is plugged in, kill disconnected message
                #if((IS_SLICER) and (TOUCHSCREEN_INPUT_DEVICE != "/dev/input/touchscreen")):
                #    handleTouchscreenReconnected()

                print('found touchscreen device')
                event_input_device_found = True

            except:
                # no touchscreen device
                event_input_device = None

    # block on event coming in
    if event_input_device != None:
        inputs = [event_input_device, server]
    else:
        inputs = [server]

    outputs = []

    readable, writable, exceptional = select.select(inputs, outputs, inputs, 2.0)

    #HTA-86 Operator dimmer is inop
    # event occurred either socket connection or touchscreen event
    for s in readable:

        # socket message coming in to process
        if s is server:

            print('socket message event')

            # accept network connection
            connection, client_address = s.accept()
            data = connection.recv(1024).decode()
            connection.close()

            #print(str(data))
            
            if data == 'weightOnPlatter':
                #print("weightOnPlatter detected")
                # stop any timer currently enabled
                rt.stop()

                # if display is dim
                if displayIsDim:

                    # set the display brightness of the operator and customer display
                    setDisplayBrightness(OPERATOR_DISPLAY_DEVICE,operatorDisplayBrightness)
                    setDisplayBrightness(CUSTOMER_DISPLAY_DEVICE,customerDisplayBrightness)

            elif data == 'weightOffPlatter':
                #print("weightOffPlatter detected")
                # weight of platter so start new timer if timeout > 0
                if dimDisplayTimeout != 0:
                    # multiple stops are allowed so stop any pending which there should not
                    # be but try anyway
                    rt.stop()

                    # start new timer
                    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, ALL_DISPLAY_DEVICES, DIM_BACKLIGHT_VALUE)

            elif data == 'shutdown':
                #print("shutdown detected")
                if dimDisplayTimeout != 0:
                    rt.stop()
                shutdown = True

            elif data[:len("beeperVolume")] == 'beeperVolume':
                #print("beeperVolume detected")
                tokens = data.split('=')
                try:
                    beeperVolume = int(tokens[1])
                except:
                    #print('invalid beeperVolume specified')
                    beeperVolume = 128

                if os.name == 'nt':
                    # HTx can not adjust beeper volume but it can be muted
                    beep.ntSetBeep(beeperVolume > 0)

            elif data[:len("operatorDisplayBrightness")] == "operatorDisplayBrightness":
                #print("operatorDisplayBrightness detected")
                tokens = data.split('=')
                try:
                    operatorDisplayBrightness = int(tokens[1])
                except:
                    print('invalid operatorDisplayBrightness specified')
                    operatorDisplayBrightness = MAX_BACKLIGHT_VALUE

                setDisplayBrightness( OPERATOR_DISPLAY_DEVICE, operatorDisplayBrightness )

            elif data[:len("customerDisplayBrightness")] == "customerDisplayBrightness":
                #print("customerDisplayBrightness detected")
                tokens = data.split('=')
                try:
                    customerDisplayBrightness = int(tokens[1])
                except:
                    print('invalid customerDisplayBrightness specified')
                    customerDisplayBrightness = MAX_BACKLIGHT_VALUE

                setDisplayBrightness( CUSTOMER_DISPLAY_DEVICE, customerDisplayBrightness )

            elif data[:len("stopTimer")] == "stopTimer":
                #print("stop timer detected")
                if dimDisplayTimeout != 0:
                    rt.stop()

            elif data[:len("startTimer")] == "startTimer":
                #print("start timer detected")
                if dimDisplayTimeout != 0:
                    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, OPERATOR_DISPLAY_DEVICE, DIM_BACKLIGHT_VALUE)

            elif data[:len("resetTimer")] == "resetTimer":
                #print("reset timer detected")
                if dimDisplayTimeout != 0:
                    rt.stop()
                    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, OPERATOR_DISPLAY_DEVICE, DIM_BACKLIGHT_VALUE)

            elif data[:len("restoreBrightness")] == "restoreBrightness":
                if dimDisplayTimeout != 0:
                    #print("Idle")
                    rt.stop()
                    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, OPERATOR_DISPLAY_DEVICE, DIM_BACKLIGHT_VALUE)

                # if display is dim
                if displayIsDim:
                    #print("Idle")
                    # set the display brightness of the operator and customer display
                    setDisplayBrightness(OPERATOR_DISPLAY_DEVICE,operatorDisplayBrightness)
                    setDisplayBrightness(CUSTOMER_DISPLAY_DEVICE,customerDisplayBrightness)

            elif data[:len("dimDisplayTimeout")] == "dimDisplayTimeout":
                #print("dimDisplayTimeout detected")
                tokens = data.split('=')
                #print(tokens)
                try:
                    # current display timeout
                    #print("1")
                    prevDimDisplayTimeout = dimDisplayTimeout

                    # dim display timeout just got via message sent
                    #print("2")
                    #print(str(tokens[1]))
                    dimDisplayTimeout = int(tokens[1]) * 60

                    # if current display timeout before new timeout value was sent is not 0
                    if prevDimDisplayTimeout != 0:
                        # stop current timer
                        #print("3")
                        rt.stop()

                    # if new timeout value is not 0
                    if dimDisplayTimeout != 0:
                        # start new timer with value just received
                        #print("Active")
                        # NEW code below
                        rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, OPERATOR_DISPLAY_DEVICE, DIM_BACKLIGHT_VALUE)
                        # OLD code below
                        #rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, ALL_DISPLAY_DEVICES, DIM_BACKLIGHT_VALUE)

                except:
                    print('invalid dimDisplayTimeout specified')
                    dimDisplayTimeout = 10 * 60

        # else touchscreen event occurred
        elif event_input_device != None:

            #print('touchscreen event')
            type = 0
            code = 0
            value = 0

            try:
                event = event_input_device.read(EVENT_SIZE)
                (tv_sec, tv_usec, type, code, value) = struct.unpack(FORMAT, event)

            except:
                print('have exception reading from touchscreen')
                event_input_device.close()
                event_input_device = None

            #print("code : " + str(code))
            #print("value: " + str(value))
            #print("type : " + str(type))

            # if this was event due to someone touching screen
            # was code == 227
            if (type == 1 and (code == 272 or code == 330) and value == 1):

                # restart timeout from this touch screen event
                # if timeout not 0
                if dimDisplayTimeout != 0:
                    #print("dimDisplay != 0")
                    rt.stop()
                    
                    if(IS_SLICER):
                    	rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, OPERATOR_DISPLAY_DEVICE, DIM_BACKLIGHT_VALUE)
                    
                    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, ALL_DISPLAY_DEVICES, DIM_BACKLIGHT_VALUE)

                # if display is dim
                if displayIsDim:
                    #print("dimDisplay : " + str(displayIsDim))
                    # set the display brightness of the operator and customer display
                    setDisplayBrightness(OPERATOR_DISPLAY_DEVICE,operatorDisplayBrightness)
                    setDisplayBrightness(CUSTOMER_DISPLAY_DEVICE,customerDisplayBrightness)

                # if beeper volume on
                if beeperVolume > 0:

                    if os.name == 'posix':
                        # if beeper volume on then sound beeper
                        if not os.path.isfile(PIEZO_BEEPER_DEVICE):
                            print('piezo beeper does not exist')
                        else:
                            out_file = open(PIEZO_BEEPER_DEVICE,"w")
                            out_file.write(str(beeperVolume))
                            out_file.close()
                            time.sleep(BUTTON_SOUND_DURATION)
                            out_file = open(PIEZO_BEEPER_DEVICE,"w")
                            out_file.write("0")
                            out_file.close()

    # error or exception occurred either socket connection or touchscreen event
    for s in exceptional:

        if s is event_input_device:

            print('have exception case for touchscreen')
            event_input_device.close()
            event_input_device = None


## ---------- ##

if event_input_device != None:
    event_input_device.close()

server.close()

