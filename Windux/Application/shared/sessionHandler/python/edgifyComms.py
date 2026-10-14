#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import datetime
import json
import os
import requests
import serial
import subprocess
import sys
import time

def logLine(t, text):
    print(str(datetime.datetime.now()) + ' ' + t + ' ' + text)

def error(text):
    logLine('ERR ', text)

def warn(text):
    logLine('WARN', text)

def info(text):
    logLine('INFO', text)

# ==================================================

predictionUrl = 'http://127.0.0.1:8090/api/v1/prediction'
setGroundTruthUrl = 'http://127.0.0.1:8090/api/v1/predictions'

images = {}
uuids = []

def prediction():
    info('reqesting edgify prediction...')

    try:
        # make request to edgify
        r = requests.get(url=predictionUrl)

        # give response to caller
        resp = json.loads(r.text)

        # Note:
        #
        # Here we go looking for encoded raw jpeg image. The image sizes (compressed) are running
        # in the 80K range which is a lot to send over a UART connection. Even at 115200 baud this
        # is 6 seconds or so to XMIT the full payload.
        #
        # To resolve this time deficiency, we will save off any raw jpeg image into memory. Each
        # image has its own unique uuid so we use this to store it off. We then remove the raw
        # image from the response.
        if ('prediction' in resp):
            prediction = resp['prediction']

            if ('image' in prediction):
                image = prediction['image']

                if (('uuid' in image) and ('image' in image)):
                    global images
                    global uuids

                    uuid = image['uuid']
                    jpeg = image['image']

                    info('stripping uuid ' + str(uuid))

                    # strip out raw jpeg image
                    image['image'] = None

                    # store raw jpeg image for later
                    images[uuid] = jpeg

                    uuids.append(uuid)

                    info('cache size ' + str(len(uuids)))

                    # only keep last 8 images
                    if (8 < len(uuids)):
                        uuid = uuids[0]

                        if (uuid in images):
                            info('remove cache uuid ' + str(uuid))
                            images.pop(uuid)

                        else:
                            warn('uuid not found in cache' + str(uuid))

                        uuids.remove(uuid)

                    # give to caller
                    prediction['image'] = image
                    resp['prediction'] = prediction

        info('give prediction to caller...')
        sendCommand('PRED', json.dumps(resp))

        info('done!')

    except KeyboardInterrupt:
        raise

    except Exception as e:
        error('exception during edgify prediction: ' + str(e))
        pass

    except:
        error('unhandled exception during edgify prediction')
        pass

def setGroundTruth(truth):
    info('setting ground truth...')

    try:
        resp = json.loads(truth)

        # Note:
        #
        # Here we go looking to restore the encoded raw jpeg image. See explanation above.
        #
        # Look for the image uuid in our cache and put it back into the JSON document.
        if ('ground_truth' in resp):
            ground_truth = resp['ground_truth']

            if ('prediction' in ground_truth):
                prediction = ground_truth['prediction']

                if ('image' in prediction):
                    image = prediction['image']

                    if (('uuid' in image) and ('image' in image)):
                        global images
                        global uuids

                        uuid = image['uuid']

                        # put back in the raw jpeg image
                        if (uuid in images):
                            info('found uuid in cache ' + str(uuid))
                            image['image'] = images[uuid]

                        else:
                            warn('uuid not found in cache' + str(uuid))

                        # give to caller
                        prediction['image'] = image
                        ground_truth['prediction'] = prediction
                        resp['ground_truth'] = ground_truth

        # make request to edgify
        info('setting edgify ground truth...')
        r = requests.post(url=setGroundTruthUrl, data=json.dumps(resp))

        info('done!')

    except KeyboardInterrupt:
        raise

    except Exception as e:
        error('exception during edgify sgt: ' + str(e))
        pass

    except:
        error('unhandled exception during edgify sgt')
        pass

def sendStatus():
    info('sending status...')

    try:
        # retrieve current date/time
        data = subprocess.run("date", capture_output=True, shell=True)

        now = data.stdout.decode()
        now = now.strip()

        # retrieve version information
        data = subprocess.run("rpm -q edgify-micro-smbackend", capture_output=True, shell=True)

        versionInfo = data.stdout.decode()
        versionInfo = versionInfo.strip()

        # check camera plugged in
        cameraDev = os.path.join( '/', 'dev', 'video0' )
        cameraMounted = False

        if os.path.exists(cameraDev):
            cameraMounted = True

        # check agent docker image running
        dockerActive = False

        data = subprocess.run("docker ps", capture_output=True, shell=True)
        data = data.stdout.decode()

        if 'edgify-agent' in data:
            dockerActive=True

        # check edgify http port
        portActive3000 = False

        data = subprocess.run("netstat -plan | grep :3000", capture_output=True, shell=True)
        data = data.stdout.decode()

        if 'LISTEN' in data:
            if 'edgify-agent' in data:
                portActive3000=True

        # check edgify http port
        portActive8080 = False

        data = subprocess.run("netstat -plan | grep :8080", capture_output=True, shell=True)
        data = data.stdout.decode()

        if 'LISTEN' in data:
            if 'edgify-agent' in data:
                portActive8080=True

        # check edgify api port
        portActive8090 = False

        data = subprocess.run("netstat -plan | grep :8090", capture_output=True, shell=True)
        data = data.stdout.decode()

        if 'LISTEN' in data:
            if 'edgify-agent' in data:
                portActive8090=True

        # number of sample images
        samplesPath = os.path.join( '/', 'edgify_agent', 'shared', 'samples' )
        numSamples = 0

        if os.path.exists(samplesPath):
            for f in os.listdir(samplesPath):
                if os.path.isfile(os.path.join(samplesPath, f)):
                    numSamples += 1

        # stats information
        result = str(now) + '|' + str(versionInfo) + '|' + str(cameraMounted) + '|' + \
                 str(dockerActive) + '|' + str(portActive3000) + '|' + str(portActive8080) + '|' + str(portActive8090) + '|' + str(numSamples)

        sendCommand('SINF', result)

    except:
        error('unhandled exception during send status')
        pass

def executeCommand(cmd):
    info("execute command '" + cmd + "'...")

    try:
        # execute
        data = subprocess.run(cmd, capture_output=True, shell=True)

        stdout = '### stdout ###\n' + data.stdout.decode()
        stderr = '### stderr ###\n' + data.stderr.decode()

        # command result
        result = str(stdout) + "\n" + str(stderr)
        sendCommand('CINF', result)

    except:
        error('unhandled exception while setting date')
        pass

def setSystemDate(d):
    info("setting date to '" + d + "'...")

    try:
        # set current date/time
        data = subprocess.run("date -s '" + d + "'", capture_output=True, shell=True)

    except:
        error('unhandled exception while setting date')
        pass

def shutdownSystem():
    info("shutting down...")

    try:
        # sync and shutdown
        data = subprocess.run("sync", capture_output=True, shell=True)
        data = subprocess.run("shutdown -h now", capture_output=True, shell=True)

    except:
        error('unhandled exception while shutting down')
        pass

# ==================================================

ser = serial.Serial()
serErrors = 0

def sendCommand(cmd, cmdData = ''):
    global ser

    # command must be 4 characters
    if (4 != len(cmd)):
        error('protocol error!')
        return

    # buffer length is 6 digits
    s = str(4+len(cmdData))

    while (len(s) < 6):
        s = '0' + s

    s = s + cmd
    s = s + cmdData

    # send!
    # string in form "000123XXXX...." where
    #   000123 is length of command and command data
    #   XXXX   is the command
    #   ....   is the command data (optional)
    ser.write(s.encode('utf-8'))

def processCommand(cmd, cmdData):
    global serErrors

    info('process command ' + cmd + ' (' + str(len(cmdData)) + ')')

    # ping/pong handling
    if ('PING' == cmd):
        sendCommand('PONG')
    elif ('PONG' == cmd):
        serErrors = serErrors - 1
        if (serErrors < 0):
            serErrors = 0

    # edgify prediction
    elif ('WHAT' == cmd):
        prediction()

    # edgify set ground truth
    elif ('TRUE' == cmd):
        setGroundTruth(cmdData)

    # stats
    elif ('STAT' == cmd):
        sendStatus()

    # execute command
    elif ('CMD ' == cmd):
        executeCommand(cmdData)

    # set system date
    elif ('DATE' == cmd):
        setSystemDate(cmdData)

    # shutdown system
    elif ('HALT' == cmd):
        shutdownSystem()

    # unknown
    else:
        warn('unknown command ' + cmd)

def parseBuffer(buffer):
    if (len(buffer) < 6):
        return buffer

    # read in buffer length
    s = buffer[:6]

    if (not s.isdigit()):
        error('protocol error!')
        return ''

    bufferLen = int(s)

    if (bufferLen < 4):
        error('buffer length error!')
        return ''

    # not enough data yet...
    if (len(buffer) < 6+bufferLen):
        return buffer

    # read in command (and command data)
    cmd = buffer[6:10]
    cmdData = buffer[10:6+bufferLen]

    # process!
    processCommand(cmd, cmdData)

    return buffer[6+bufferLen:]

def runEdgifyServer(uart):
    global ser
    global serErrors

    ser.port = uart
    ser.baudrate = 38400

    # open uart for read/write

    try:
        info('opening UART...')
        ser.open()

        if (not ser.isOpen()):
            error('UART ' + uart + ' is not open!')
            quit()

    except Exception as e:
        error('error opening UART ' + uart + ': ' + str(e))
        quit()

    # --------------------------------------------------

    buffer = ''

    pingEnabled = False
    pingTime = datetime.datetime.now()

    recvTime = datetime.datetime.now()

    resetConnection = True
    forceQuit = False

    try:
        info('===== processing loop =====')

        # processing loop
        while True:
        
            # reset connection!
            if (resetConnection):
                info('reset connection...')
                
                buffer = ''

                pingEnabled = False

                # flush i/o buffers, discarding all its contents
                ser.flushInput()
                ser.flushOutput()
                
                serErrors = 0

                resetConnection = False

            stamp = datetime.datetime.now()

            # read pending data
            pending = ser.inWaiting()

            if (0 < pending):
                info('reading ' + str(pending) + ' bytes...')
                buffer += ser.read( pending ).decode('utf-8')

                recvTime = stamp

                # somebody is sending data, enable pings
                if (not pingEnabled):
                    pingEnabled = True
                    pingTime = stamp

            # check for connection reset string
            if ('^CON^RESET^CON^RESET^' in buffer):
                warn('connection reset received!')
                resetConnection = True

                continue

            # check for too many errors
            elif (4 <= serErrors):
                error('too many errors')
                resetConnection = True

                continue

            # process buffer
            if (0 < len(buffer)):

                # handle new data added to buffer
                if (0 < pending):

                    while True:
                        bufferLen = len(buffer)

                        # parse!
                        info('processsing buffer of size ' + str(bufferLen) + '...')
                        buffer = parseBuffer(buffer)

                        bufferLenAfter = len(buffer)

                        # buffer is empty or it did not change after processing
                        if ((0 == bufferLenAfter) or (bufferLen == bufferLenAfter)):
                            break

                    continue

                # buffer has not changed for 16 seconds, lets reset it
                elif (16 <= (stamp-recvTime).total_seconds()):
                    error('protocol error!')
                    buffer = ''

                    continue

            # send ping every 8 seconds
            if ((pingEnabled) and (8 <= (stamp-pingTime).total_seconds())):
                info('sending ping...')
                sendCommand('PING')

                pingTime = stamp
                
                serErrors = serErrors + 1

            # wait for more data...
            time.sleep(0.25)

    except KeyboardInterrupt:
        info('shutting down...')

        forceQuit = True
        pass

    except Exception as e:
        error('exception during processing of UART ' + uart + ': ' + str(e))
        pass

    except:
        error('unhandled exception during processing')
        pass

    info('closing UART...')
    ser.close()

    if forceQuit:
        quit()

# ==================================================

if __name__ == "__main__":

    # they need to specify UART as first argument
    if (len(sys.argv) < 2):
        error('please specify a UART')
        quit()

    fname = str(sys.argv[1])
    info('using UART ' + fname + '...')

    # make sure UART exists
    if (not os.path.exists(fname)):
        error('UART ' + fname + ' does not appear to exist')
        quit()

    # go!!
    while True:
        runEdgifyServer(fname)
