#!/usr/bin/env python3
# -*- coding: utf-8 -*-

## @package recordPlayback
#  Handles recording and playback of macros.
#
#  @copyright Copyright (c) 2016 Hobart Corporation, All rights reserved
#  @author Randy Blankley
#

import argparse
import os
import shutil
import socket
import sys
import tarfile
import time
import tempfile

STATE_SUCCESS = 0
STATE_ERROR_FAILURE = -1
STATE_ERROR_PLAYBACK_IN_PROGRESS = -2
STATE_ERROR_RECORD_IN_PROGRESS = -3
STATE_ERROR_NOT_RECORDING = -4
STATE_ERROR_DAEMON_NOT_RUNNING = -5

# ==================================================================================================

## Macro record file name.
#
#  @return  name of macro file
#
def macroFile():
    return os.path.join(tempfile.gettempdir(), 'hobart_macro.py')

# ==================================================================================================

## Objects map file name.
#
#  @return  name of objects map file
#
def objectsMapFile():
    filename = ''

    # linux
    if os.name == "posix":
        return '/root/.squish/objects.map'

    # windows
    else:
        # TODO
        pass

    return filename

# ==================================================================================================

## Check if actively playing.
#
#  @return  @c True if playing, @c False otherwise
#
def playing():

    # linux
    if os.name == "posix":

        # retrieve all active pids
        pids = [pid for pid in os.listdir('/proc') if pid.isdigit()]

        # look for the squishrunner process
        for pid in pids:
            try:
                p = open(os.path.join('/proc', pid, 'cmdline'), 'rb').read()

                if (('squishrunner' in p) and ('--objectmap' in p) and ('--testcase' in p)):
                    return True

            except IOError: # proc has already terminated
                continue

    # windows
    else:
        # TODO
        pass

    return False

# ==================================================================================================

## Check if actively recording.
#
#  @return  @c True if recording, @c False otherwise
#
def recording():

    # linux
    if os.name == "posix":

        # retrieve all active pids
        pids = [pid for pid in os.listdir('/proc') if pid.isdigit()]

        # look for the squishrunner process
        for pid in pids:
            try:
                p = open(os.path.join('/proc', pid, 'cmdline'), 'rb').read()

                if (('squishrunner' in p) and ('--recordStart' in p) and ('--outfile' in p)):
                    return True

            except IOError: # proc has already terminated
                continue

    # windows
    else:
        # TODO
        pass

    return False

# ==================================================================================================

## Start recording a macro.
#
#  @return  state code
#
def startRecording():

    # make sure we are not playing or recording already
    if playing():
        print("Cannot record while a playback is in progress!")
        return STATE_ERROR_PLAYBACK_IN_PROGRESS
    elif recording():
        print("Already recording!")
        return STATE_ERROR_RECORD_IN_PROGRESS

    # tell squish exec to record
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('localhost', 16555))

        s.send('start %s\r\n' % macroFile())

        # give squish a moment to start
        time.sleep(2)

    except:
        print("Recording daemon does not appear to be running")
        return STATE_ERROR_DAEMON_NOT_RUNNING

    # make sure we are recording
    if not recording():
        print("Error initiating record!")
        return STATE_ERROR_FAILURE

    return STATE_SUCCESS

# ==================================================================================================

## Stop macro recording.
#
#  @param[in] f  macro file
#  @return  state code
#
def stopRecording(f):

    # make sure we are recording
    if not recording():
        print("Not recording!")
        return STATE_ERROR_NOT_RECORDING

    # tell squish exec to stop recording
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('localhost', 16555))

        s.send('stop\r\n')

        # give squish a moment to stop
        time.sleep(2)

    except:
        print("Recording daemon does not appear to be running")
        return STATE_ERROR_DAEMON_NOT_RUNNING

    # make sure we stopped recording
    if recording():
        print("Error stopping record!")
        return STATE_ERROR_FAILURE

    # create tarball
    try:
        t = tarfile.open(fileobj=f, mode='w:gz')
    
        # add macro and objects map
        t.add(macroFile(), './playback/test.py')
        t.add(objectsMapFile(), './playback/objects.map')

        # close file
        t.close()

    except:
        print("Error saving macro!")
        return STATE_ERROR_FAILURE

    # remove macro
    os.remove(macroFile())

    return STATE_SUCCESS

# ==================================================================================================

## Playback a macro.
#
#  @param[in] f  macro file
#  @return  state code
#
def playback(f):
    macro = os.path.join(tempfile.gettempdir(), 'playback')

    # make sure we are not playing or recording already
    if playing():
        print("Cannot playback two macros at the same time!")
        return STATE_ERROR_PLAYBACK_IN_PROGRESS
    elif recording():
        print("Cannot playback macro while recording!")
        return STATE_ERROR_RECORD_IN_PROGRESS

    # remove previous macro
    if os.path.exists(macro):
        shutil.rmtree(macro)

    # extract tarball
    try:
        t = tarfile.open(fileobj=f, mode='r:gz')

        # extract macro and objects map
        t.extractall(tempfile.gettempdir())

        # close file
        t.close()

    except:
        print("Error opening macro!")
        return STATE_ERROR_FAILURE

    # go!
    os.system("DISPLAY=:0.0 squishrunner --testcase %s --objectmap %s &" % (macro, os.path.join(macro, 'objects.map')))

    return STATE_SUCCESS

# ==================================================================================================

## Script entry point.
if __name__ == "__main__":
    exitCode = STATE_SUCCESS

    # create an argument parser with mutually exclusive arguments
    parser = argparse.ArgumentParser(description="Macro record and playback functionality.")
    g = parser.add_mutually_exclusive_group()

    g.add_argument("--start",
                   action="store_true",
                   help="start recording a macro",
                   dest="start")

    g.add_argument("--stop",
                   action="store",
                   nargs=1,
                   type=argparse.FileType('wb'),
                   help="stop recording and save macro to FILE",
                   metavar="FILE",
                   dest="recordFile")

    g.add_argument("--play",
                   action="store",
                   nargs=1,
                   type=argparse.FileType('rb'),
                   help="play macro FILE",
                   metavar="FILE",
                   dest="playbackFile")

    parser.set_defaults(start=False)

    # parse arguments!
    args = parser.parse_args()

    if args.start:
        exitCode = startRecording()
    elif args.recordFile:
        exitCode = stopRecording(args.recordFile[0])
    elif args.playbackFile:
        exitCode = playback(args.playbackFile[0])
    else:
        print("Please specify an option!")
        exitCode = STATE_ERROR_FAILURE

    sys.exit(exitCode)
