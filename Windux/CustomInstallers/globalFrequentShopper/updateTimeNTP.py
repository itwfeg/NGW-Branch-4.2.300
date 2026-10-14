######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script sets Linux/WES7 system time according to ntp.
##  REQS    : Must have permissions to execute the
##            OS's date/time command without password
##            prompt. All options must be provided, except for -v -h -q
######################################################

# Sample command -># updateTimeNTP.py -Y 2011 -M 10 -D 17 -H 14 -m 30 -S 20 -T "Eastern Time (US & Canada)" -t +05 -A 1 -o history.txt

# Will set to system and hardware clock to-> October 17, 2011 02:30:20 PM eastern timezone - using a 24 hour clock

# Preprocessor directives
##########################
import sys
import os
import shellCommand
import file_io
from optparse import OptionParser
import time
import sendDeviceEventMessage
import datetime

import ntplib
import string
from time import ctime

if os.name == "posix":

    # tell processDeviceEvents to stop timer since system time being changed
    exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='stopTimer')
    if exitCode == 0:

        try:
            ntpServer = "pool.ntp.org"
            frequency = "0"
            # Get NTP server address
            dbFile = open("/usr/local/hobart/db/settingswizard/scalesettings", "r")
            for line in dbFile:
                keyValuePair = line.split('=')
                if keyValuePair[0] == "ntpServer":
                    ntpServer = str(keyValuePair[1].rstrip())
                if keyValuePair[0] == "ntpFrequency":
                    frequency = str(keyValuePair[1].rstrip())
            dbFile.close()

            print(frequency)

            # See if it is daylight savings and Sunday or not
            if frequency == "2" and datetime.datetime.today().weekday() != 6:
                sys.exit(exitCode)

            print("not ntp")
            print("1")
            print(ntpServer)
            c = ntplib.NTPClient()
            response = c.request(str(ntpServer), version=3)
            ntpTime = ctime(response.tx_time)
            values = string.split(ntpTime)
            print(ntpTime)
            months = []
            months.append("JAN")
            months.append("FEB")
            months.append("MAR")
            months.append("APR")
            months.append("MAY")
            months.append("JUN")
            months.append("JUL")
            months.append("AUG")
            months.append("SEP")
            months.append("OCT")
            months.append("NOV")
            months.append("DEC")

            monthName = string.upper(values[1])

            for i in xrange(0,len(months)):
                if monthName == months[i]:
                    monthNumber = i + 1
                    break

            timeValues = string.split(values[3],':')

            systemTime = '%02d %s %s %s %s %s' % (monthNumber, values[2], values[4], timeValues[0], timeValues[1], timeValues[2])

            exitCode = 0

            date = '%s%02d%s' % (values[4], monthNumber, values[2])
            print(date)
            dateFormat = "+%Y%m%d"

            dateOptions = "-s " + date

            # Set the date
            #######################
            cmd = []
            cmd.append("date")
            cmd.append(dateFormat)
            cmd.append("-s")
            cmd.append(date)
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            if exitCode != 0:
                sys.exit(exitCode)
            elif exitCode == 0:

                fileDate = date

                timew = timeValues[0] + ":" + timeValues[1] + ":" + timeValues[2]
                timeFormat = "+%H:%M:%S"
                print(timew)

                # Set the Time
                #######################
                cmd = []
                cmd.append("date")
                cmd.append(timeFormat)
                cmd.append("-s")
                cmd.append(timew)

                exitCode, stdout, stderr = shellCommand.execute(cmd)
                if exitCode != 0:
                    sys.exit(exitCode)
                elif exitCode == 0:
                    cmd = []
                    cmd.append("hwclock")
                    cmd.append("--utc")
                    cmd.append("--systohc")

                    exitCode, stdout, stderr = shellCommand.execute(cmd)


                    cmd = []
                    cmd.append("date")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    print("DATE    is : " + stdout)
                    cmd = []
                    cmd.append("hwclock")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    print("HWCLOCK is : " + stdout)
                    if exitCode != 0:
                        time.sleep(2)
                        sys.exit(exitCode)
                    elif exitCode == 0:
                        time.sleep(5)
                        exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='startTimer')
        except:
            print "failed"
            exitCode = 1
        #print 'at end exit code = %d' % (exitCode)
        cmd = []
        cmd.append("/usr/bin/openbox")
        cmd.append("--restart")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

    sys.exit(exitCode)

else:
    # WINDOWS BLOCK GOES HERE

    #These commands must be run in command.exe as administrator
    dateCommand = "date"
    timeCommand = "time"
    timeZoneCommand = "tzutil" #tzutil available in Win7 and above

    #Disable Windows Time Synchronization
    # requires admin privileges to test
    cmd = []
    cmd.append("net")
    cmd.append("stop")
    cmd.append("w32time")

    exitCode, stdout, stderr = shellCommand.execute(cmd)

    if exitCode != 0:
        sys.exit(exitCode)

    #Format the date
    fileDate = str(options.month).zfill(2) + "/" + str(options.day).zfill(2) + "/" + str(options.year).zfill(2)

    #Change the date
    cmd = []
    cmd.append(dateCommand)
    cmd.append(fileDate)

    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        sys.exit(exitCode)

    #Format the time
    fileTime = str(options.hour).zfill(2) + ":" + str(options.minute).zfill(2) + ":" + str(options.second).zfill(2)

    cmd = []
    cmd.append(timeCommand)
    cmd.append(fileTime)

    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        sys.exit(exitCode)

    #Prepare the astronomical flag
    fileNonAstronomical = (str(options.nonAstronomical) == "0")

    #if 24-hour clock, change the registry to display 24-hour clock format
    #REBOOT REQUIRED
    if fileNonAstronomical:
        cmd = []
        cmd.append("reg")
        cmd.append("add")
        cmd.append("HKCU\\Control Panel\\International\\")
        cmd.append("/v")
        cmd.append("sShortTime")
        cmd.append("/d")
        cmd.append("HH:mm")
        cmd.append("/f")

        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode != 0:
            sys.exit(exitCode)

        cmd = []
        cmd.append("reg")
        cmd.append("add")
        cmd.append("HKCU\\Control Panel\\International\\")
        cmd.append("/v")
        cmd.append("sTimeFormat")
        cmd.append("/d")
        cmd.append("HH:mm:ss")
        cmd.append("/f")

        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode != 0:
            sys.exit(exitCode)

        #os.system("reg add \"HKCU\Control Panel\International\" /v sShortTime /d HH:mm /f")
        #os.system("reg add \"HKCU\Control Panel\International\" /v sTimeFormat /d HH:mm:ss /f")

    #Store timeZone
    fileTimeZone = str(options.timeZone)

    #Prepare the timeZone
    fileTimeZoneText = str(options.timeZoneText)

    timeZoneOptions = "/s"

    cmd = []
    cmd.append(timeZoneCommand)
    cmd.append(timeZoneOptions)
    cmd.append(fileTimeZoneText)

    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        sys.exit(exitCode)

    # Save this action to a file for Logistics purposes. Overwrite existing file using provided filename or else default to overwriting Global Filename defined above.
    ################################
    #if options.outputFile:

    #    filename = options.outputFile

    #    fileUser = options.user

    #    line = (str(fileNonAstronomical) + "," + fileNow + "," + str(fileUser) + "," + str(fileTimeZone) + "," + str(fileTimeZoneText) + "," +str(fileDate) + "," + str(fileTime))

    #    status, data = file_io.writeTofile(filename,[line])

    sys.exit(exitCode)
