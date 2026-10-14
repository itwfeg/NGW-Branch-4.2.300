#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# is this used anywhere? seems to have issues

'''
	The traceroute.py file has been modified so much by the \branch-advanced-service-diagnostics\ project
	that it scares me a little bit since it is a complete rewrite of something that use to work just fine.  So
	I am not going to just merge the \branch-advanced-service-diagnostics\ code. I'm going to copy over the
	new \branch-advanced-service-diagnostics\ code and paste it here in this file.  And then at the bottom I am
	going to include the original traceroute.py code in it's full form, commented out, for past reference and testing.
'''
### NEW CODE from \branch-advanced-service-diagnostics\ project ###

import sys
import os
import shellCommand
import argparse
import subprocess

if os.name == 'posix':
    WAIT_TIMEOUT = 5
else:
    WAIT_TIMEOUT = 5000

def trace(host, wait=WAIT_TIMEOUT,  hops=30):
    cmd = []

    if os.name == "posix":
        cmd.append("traceroute")
        cmd.append("-w")
        cmd.append(str(wait))
        cmd.append("-m")
        cmd.append(str(hops))
        cmd.append(host)
    else:
        # WINDOWS BLOCK GOES HERE
        cmd.append("tracert")
        cmd.append('-d')
        cmd.append("-w")
        cmd.append(str(wait))
        cmd.append("-h")
        cmd.append(str(hops))
        cmd.append(host)

#    poll_period = 1
#
#    proc = subprocess.Popen(cmd,
#                            stdin=subprocess.PIPE,
#                            stdout=subprocess.PIPE,
#                            stderr=subprocess.STDOUT)
#
#    exitcode = proc.poll()
#    while exitcode is not None:
#        time.sleep(poll_period)
#        exitcode = proc.poll()
#
#    stdout = ''
#    stderr = ''
#    if proc.stdout:
#        stdout = proc.stdout.read()
#    if proc.stderr:
#        stderr = proc.stderr.read()
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    return (exitCode,  stdout)

def traceroute(host):
    cmd = []
    stdout = ""
    if os.name == "posix":
        #cmd.append("traceroute")
        #cmd.append("-n")
        #cmd.append("15")
        #cmd.append(host)
        #exitCode, stdout, stderr = shellCommand.execute(cmd)
        cmd = "traceroute -n -m 15 " + host

        p = subprocess.Popen(cmd.split(), stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        for line in iter(p.stdout.readline, ''):
            if line != b'':
                print(line.decode('utf-8'))
            sys.stdout.flush()
        '''stdout = []
        while True:
            line = p.stdout.readline()
            #stdout.append(line)
            print line
            sys.stdout.flush()
            if line == '' and p.poll() != None:
                break'''

    else:
        # WINDOWS BLOCK GOES HERE
        TBD = "windows"
    return (stdout)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="traceroute host or ipaddress")

    parser.add_argument("host",
                        action="store",
                        help="hostname or ip address of computing system to traceroute")

    args = parser.parse_args()

    stdout = traceroute(args.host)
    #print stdout
    sys.stdout.flush()
    sys.exit(0)
