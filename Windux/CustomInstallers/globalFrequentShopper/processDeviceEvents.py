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

TOUCHSCREEN_INPUT_DEVICE = "/dev/input/touchscreen"
PIEZO_BEEPER_DEVICE = "/sys/class/backlight/buzzer.27/brightness"
BUTTON_SOUND_DURATION = 0.05
DIM_BACKLIGHT_VALUE = 104
MAX_BACKLIGHT_VALUE = 204

#DISPLAY_DEVICE_MASK = "/sys/class/backlight/pwm-backlight.%d/brightness"
OPERATOR_DISPLAY_BACKLIGHT = "/sys/class/backlight/backlight_lvds_0.29/brightness"
CUSTOMER_DISPLAY_BACKLIGHT = "/sys/class/backlight/backlight_lvds_1.30/brightness"
OPERATOR_DISPLAY_DEVICE = 2
CUSTOMER_DISPLAY_DEVICE = 1
ALL_DISPLAY_DEVICES = 0

displayIsDim = False

def convertStr(s):
    return str(int(s) - 104)

def setDisplayBrightness(device,value):
    global displayIsDim
    if value == DIM_BACKLIGHT_VALUE:
        displayIsDim = True
    else:
        displayIsDim = False

    if device == ALL_DISPLAY_DEVICES:
        start_device = CUSTOMER_DISPLAY_DEVICE
        end_device = OPERATOR_DISPLAY_DEVICE+1
    else:
        start_device = device
        end_device = device+1

    if os.name == 'posix':
        for i in xrange(start_device,end_device):
            if device == CUSTOMER_DISPLAY_DEVICE:
                if not os.path.exists(os.path.join(os.sep,"tmp","gcdidle.lock")):
                    out_file = open(CUSTOMER_DISPLAY_BACKLIGHT,"w")
                    out_file.write(str(value))
                    out_file.close()
            else:
                out_file = open(OPERATOR_DISPLAY_BACKLIGHT,"w")
                out_file.write(str(value))
                out_file.close()
    else:
        setBrightness.ntSetBacklight(convertStr(value), str(device))

dimDisplayTimeout = 10 * 60 # 10 minutes

configSettings = bootSettings.getBootSettings()
beeperVolume = int(configSettings['oVolume'])
operatorDisplayBrightness=int(configSettings['cBrightness'])
customerDisplayBrightness=int(configSettings['oBrightness'])
dimDisplayTimeout=int(configSettings['dimDisplayTimeout']) * 60
connectionType=int(configSettings['connectionType'])
isStatic=int(configSettings['isStatic'])
#print configSettings

# set interfaces to initial state based on configuration settings
'''if connectionType == 0:
    netInterface.setInterfaceState(netInterface.wirelessDevice,"down")
    netInterface.setInterfaceState(netInterface.wiredDevice,"up")
    netManager.initialize(netInterface.wiredDevice, isStatic)

elif connectionType == 1:
    #netInterface.setInterfaceState(netInterface.wiredDevice,"down")
    #if isStatic == 1:
    #    netInterface.setInterfaceState(netInterface.wirelessDevice,"up")
    netManager.initialize(netInterface.wirelessDevice, isStatic)

else:
    netInterface.setInterfaceState(netInterface.wiredDevice,"down")
    netInterface.setInterfaceState(netInterface.wirelessDevice,"down")
'''
if os.name == 'posix':
    setBrightness.linuxSetBacklight(value=configSettings['oBrightness'], device=OPERATOR_DISPLAY_DEVICE)
    setBrightness.linuxSetBacklight(value=configSettings['cBrightness'], device=CUSTOMER_DISPLAY_DEVICE)
else:
    setBrightness.ntSetBacklight(duty=convertStr(configSettings['oBrightness']), device=str(OPERATOR_DISPLAY_DEVICE))
    setBrightness.ntSetBacklight(duty=convertStr(configSettings['cBrightness']), device=str(CUSTOMER_DISPLAY_DEVICE))

setVolume.volumeSetting(configSettings['cVolume'])

if os.name == 'nt':
    # For Windows/FBWF, we must apply the timezone and NTP settings on each boot
    import configInfo
    import subprocess

    # Are we using NTP or Manual
    timeKeeper = str(configSettings['timeKeeper'])
    if timeKeeper == "Manual":
        # Set timezone
        timezone = str(configSettings['timezone'])
        cmd = []
        cmd.append(os.path.join(configInfo.getPythonInstallPath(), "pythonw"))
        cmd.append(os.path.join(configInfo.getRootFolder(), "python", "setTimezone.py"))
        cmd.append("-t")
        cmd.append(timezone)
        subprocess.Popen(cmd)

        # Turn NTP off
        cmd = []
        cmd.append(os.path.join(configInfo.getPythonInstallPath(), "pythonw"))
        cmd.append(os.path.join(configInfo.getRootFolder(), "python", "setNTP.py"))
        cmd.append("-U")
        cmd.append("0")
        subprocess.Popen(cmd)

    else:
        ntpServer = str(configSettings['ntpServer'])
        ntpFrequency = str(configSettings['ntpFrequency'])
        ntpDayOfWeek = str(configSettings['ntpDayOfWeek'])
        ntpTimeOfDay = str(configSettings['ntpTimeOfDay'])
        ntpHour=''
        ntpMinute=''
        tokens = string.split(ntpTimeOfDay,':')
        try:
            ntpHour = tokens[0]
            ntpMinute = tokens[1]
        except:
            pass
        ntpTimeZone = str(configSettings['ntpTimeZone'])

        # Set timezone
        cmd = []
        cmd.append(os.path.join(configInfo.getPythonInstallPath(), "pythonw"))
        cmd.append(os.path.join(configInfo.getRootFolder(), "python", "setTimezone.py"))
        cmd.append("-t")
        cmd.append(ntpTimeZone)
        subprocess.Popen(cmd)

        cmd = []
        cmd.append(os.path.join(configInfo.getPythonInstallPath(), "pythonw"))
        cmd.append(os.path.join(configInfo.getRootFolder(), "python", "setNTP.py"))
        cmd.append("-U")
        cmd.append("1")
        cmd.append("-s")
        cmd.append(ntpServer)
        cmd.append("-F")
        cmd.append(ntpFrequency)
        cmd.append("-D")
        cmd.append(ntpDayOfWeek)
        cmd.append("-H")
        cmd.append(ntpHour)
        cmd.append("-M")
        cmd.append(ntpMinute)
        cmd.append("-T")
        cmd.append(ntpTimeZone)
        subprocess.Popen(cmd)

# debug line
#dimDisplayTimeout = 104

FORMAT = 'llHHI'
EVENT_SIZE = struct.calcsize(FORMAT)

if dimDisplayTimeout != 0:
    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, ALL_DISPLAY_DEVICES, DIM_BACKLIGHT_VALUE)

event_input_device = None

if os.name == 'posix':
    while event_input_device == None:
        try:
            # attempt to get touch screen device
            # it is not guaranteed that udev has created line for device
            # need to try and open
            event_input_device = open(TOUCHSCREEN_INPUT_DEVICE, "rb")
        except:
            # go idle before checking if device mapped yet
            time.sleep(2)
else:
    setBeepExitCode = beep.ntSetBeep(beeperVolume > 0)
    while setBeepExitCode != 0:
        try:
            # attempt to set beep on or off
            setBeepExitCode = beep.ntSetBeep(beeperVolume > 0)
        except:
            time.sleep(2)

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

server_address = ('localhost', 5077)
print >>sys.stderr, 'processDeviceEvents starting up on %s port %s' % server_address
server.bind(server_address)
server.listen(5)

if event_input_device != None:
    inputs = [event_input_device, server]
else:
    inputs = [server]

outputs = []

shutdown = False

while not shutdown:

    # block on event coming in
    readable, writable, exceptional = select.select(inputs, outputs, inputs)

    type = 0
    code = 0
    value = 0

    # event occurred either socket connection or touchscreen event
    for s in readable:

        # socket message coming in to process
        if s is server:

            #print 'socket message event'

            # accept network connection
            connection, client_address = s.accept()
            data = connection.recv(1024)
            connection.close()

            if data == 'weightOnPlatter':

                # stop any timer currently enabled
                rt.stop()

                # if display is dim
                if displayIsDim:

                    # set the display brightness of the operator and customer display
                    setDisplayBrightness(OPERATOR_DISPLAY_DEVICE,operatorDisplayBrightness)
                    setDisplayBrightness(CUSTOMER_DISPLAY_DEVICE,customerDisplayBrightness)

            elif data == 'weightOffPlatter':

                # weight of platter so start new timer if timeout > 0
                if dimDisplayTimeout != 0:
                    # multiple stops are allowed so stop any pending which there should not
                    # be but try anyway
                    rt.stop()

                    # start new timer
                    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, ALL_DISPLAY_DEVICES, DIM_BACKLIGHT_VALUE)

            elif data == 'shutdown':
                if dimDisplayTimeout != 0:
                    rt.stop()
                shutdown = True

            elif data[:len("beeperVolume")] == 'beeperVolume':
                tokens = string.split(data,'=')
                try:
                    beeperVolume = int(tokens[1])
                except:
                    print 'invalid beeperVolume specified'
                    beeperVolume = 128

                if os.name == 'nt':
                    # HTx can not adjust beeper volume but it can be muted
                    beep.ntSetBeep(beeperVolume > 0)

            elif data[:len("operatorDisplayBrightness")] == "operatorDisplayBrightness":
                tokens = string.split(data,'=')
                try:
                    operatorDisplayBrightness = int(tokens[1])
                except:
                    print 'invalid operatorDisplayBrightness specified'
                    operatorDisplayBrightness = MAX_BACKLIGHT_VALUE

            elif data[:len("customerDisplayBrightness")] == "customerDisplayBrightness":
                tokens = string.split(data,'=')
                try:
                    customerDisplayBrightness = int(tokens[1])
                except:
                    print 'invalid customerDisplayBrightness specified'
                    customerDisplayBrightness = MAX_BACKLIGHT_VALUE

            elif data[:len("stopTimer")] == "stopTimer":
                if dimDisplayTimeout != 0:
                    rt.stop()

            elif data[:len("startTimer")] == "startTimer":
                if dimDisplayTimeout != 0:
                    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, ALL_DISPLAY_DEVICES, DIM_BACKLIGHT_VALUE)

            elif data[:len("resetTimer")] == "resetTimer":
                if dimDisplayTimeout != 0:
                    rt.stop()
                    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, ALL_DISPLAY_DEVICES, DIM_BACKLIGHT_VALUE)

            elif data[:len("dimDisplayTimeout")] == "dimDisplayTimeout":
                tokens = string.split(data,'=')
                try:
                    # current display timeout
                    prevDimDisplayTimeout = dimDisplayTimeout

                    # dim display timeout just got via message sent
                    dimDisplayTimeout = int(tokens[1]) * 60

                    # if current display timeout before new timeout value was sent is not 0
                    if prevDimDisplayTimeout != 0:
                        # stop current timer
                        rt.stop()

                    # if new timeout value is not 0
                    if dimDisplayTimeout != 0:
                        # start new timer with value just received
                        rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, ALL_DISPLAY_DEVICES, DIM_BACKLIGHT_VALUE)

                except:
                    print 'invalid dimDisplayTimeout specified'
                    dimDisplayTimeout = 10 * 60

        # else touchscreen event occurred
        elif event_input_device != None:

            #print 'touchscreen event'

            event = event_input_device.read(EVENT_SIZE)
            (tv_sec, tv_usec, type, code, value) = struct.unpack(FORMAT, event)

            #print code
            #print value
            #print type

            # if this was event due to someone touching screen
            # was code == 227
            if (type == 1 and code == 272 and value == 1):

                # restart timeout from this touch screen event
                # if timeout not 0
                if dimDisplayTimeout != 0:
                    rt.stop()
                    rt = applicationTimer.RepeatedTimer(dimDisplayTimeout, setDisplayBrightness, ALL_DISPLAY_DEVICES, DIM_BACKLIGHT_VALUE)

                # if display is dim
                if displayIsDim:

                    # set the display brightness of the operator and customer display
                    setDisplayBrightness(OPERATOR_DISPLAY_DEVICE,operatorDisplayBrightness)
                    setDisplayBrightness(CUSTOMER_DISPLAY_DEVICE,customerDisplayBrightness)

                # if beeper volume on
                if beeperVolume > 0:

                    if os.name == 'posix':
                        # if beeper volume on then sound beeper
                        out_file = open(PIEZO_BEEPER_DEVICE,"w")
                        out_file.write(str(beeperVolume))
                        out_file.close()
                        time.sleep(BUTTON_SOUND_DURATION)
                        out_file = open(PIEZO_BEEPER_DEVICE,"w")
                        out_file.write("0")
                        out_file.close()

event_input_device.close()


server.close()
