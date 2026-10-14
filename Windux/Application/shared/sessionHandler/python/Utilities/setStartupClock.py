#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# WARNING:
# Do NOT put os.system() calls  in this file if you want the command to run sequentially with other commands
# os.system in startupSequence.py when run from rc.local will background shell commands so the next line
# after os.system call will immmediately execute and your nightmare will begin.
# use shellCommand.execute() call to make sure the command you want to execute is blocked on before next
# line executes
import sys
import os
import file_io
import argparse
import shellCommand
import time
import datetime
import messageBox
import languageSettings

pythonPath = os.path.join("/","usr","local","hobart","python")
firstBootFile = os.path.join("/","etc","firstBoot")
upgradeBootFile = os.path.join("/","etc","upgradeBoot")
firstTime = False
currentYear = ''

def processStartupSequence():

    # process any first boot stuff

    # if not first boot file exists which means scale has not gone
    # thru first boot installation/configuration
    if not os.path.exists(firstBootFile):

        #print 'Initializing ld library configuration'
        # needed for pyudev to install
        cmd = []
        cmd.append("/sbin/ldconfig")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # if this is not an upgrade
        if not os.path.exists(upgradeBootFile):
            cmd = []
            cmd.append("ln")
            cmd.append("-sf")
            cmd.append("/usr/share/zoneinfo/America/Detroit")
            cmd.append("/etc/localtime")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            #print ("exitCode is "+str(exitCode))

            if exitCode != 0:
                sys.exit(exitCode)

            elif exitCode == 0:
                cmd = []
                cmd.append("echo")
                cmd.append("America/Detroit")
                cmd.append(">")
                cmd.append("/etc/timezone")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

                if exitCode != 0:
                    sys.exit(exitCode)

                elif exitCode == 0:
                    os.environ['TZ'] = "America/Detroit"

                    cmd = []
                    cmd.append("export")
                    cmd.append("TZ=America/Detroit")
                    exitCode, stdout, stderr = shellCommand.execute(cmd, True)

            # NOTE NOTE NOTE NOTE
            # as of kernel 3.14 we should not need to manipulate clocks

            cmd = []
            cmd.append("/bin/date")
            cmd.append("+%Y")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            currentYear = stdout.strip()
            #print currentYear

            # need to check timezone of RTC from BCM
            if int(currentYear) < 2016:
                cmd = []
                cmd.append("/bin/date")
                cmd.append("+%Y%m%s")
                cmd.append("-s")
                cmd.append("20160101")
                exitCode, stdout, stderr = shellCommand.execute(cmd)

            cmd = []
            cmd.append("/bin/sync")
            exitCode, stdout, stderr = shellCommand.execute(cmd)

if __name__ == "__main__":
    processStartupSequence()
    sys.exit(0)


