#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# not tested in python3

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script sets Linux/WES7 system time.
##  REQS    : Must have permissions to execute the
##            OS's date/time command without password
##            prompt. All options must be provided, except for -v -h -q
######################################################

# Sample command -># setTime.py -Y 2011 -M 10 -D 17 -H 14 -m 30 -S 20 -T "Eastern Time (US & Canada)" -t +05 -A 1 -o history.txt

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

##################################
#
#     G L O B A L S
#
# some easy to change vars below
#
#
#################################

# file format-> timestampOfEvent,user,timezone,dateAndTime,is12HourClock
# Enter relative path & file for save
filename = "HobartQmSettings-DATETIME.txt"
mainCommand = "date"
dbName = ""

##################################
#
#     E N D   G L O B A L S
#
##################################

# First thing, get a timestamp for this
# executable run
######################################
#now = time.localtime()
#fileNow = str("%d%02d%02d%02d%02d%02d" % (now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec))

#fileTimezone = "-05"

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-U", "--user",
             dest="user",
             help="The UserID of the person setting the DateTime (ex. 002)")
p.add_option("-Y", "--year",
             dest="year",
             help="The year component as 4 digits (ex.1964)")
p.add_option("-M", "--month",
             dest="month",
             help="The month component as 2 digits (ex. 06 is June)")
p.add_option("-D", "--day",
             dest="day",
             help="The day component as 2 digits (ex. 06 is the 6th)")
p.add_option("-H", "--hour",
             dest="hour",
             help="The hour component with 2 digits given in astronomical time (ex. 18 is 6:00 PM)")
p.add_option("-m", "--minute",
             dest="minute",
             help="The minute component with 2 digits (ex. 03 is 3 mintues)")
p.add_option("-S", "--second",
             dest="second",
             help="The second component with 2 digits (ex. 08 is 8 seconds)")
p.add_option("-T", "--offsetHours",
             dest="offsetHours",
             help="The timezone offset in hours as a digit/s, e.g., 6 or 10")
p.add_option("-t", "--timeZoneText",
             dest="timeZoneText",
             help="The timezone text component (ex. US/Eastern)")
p.add_option("-A", "--nonAstronomical",
             dest="nonAstronomical",
             help="The 24 hour clock component with 1 digit (ex. non-zero = use 12 hour clock or 0 = use 24 hour clock ")
p.add_option("-o", "--output",
             dest="outputFile",
             help="Results of setting Hobart Time/Date written to this file")
p.add_option("-R", "--reboot",
             dest="doReboot",
             help="Setting Hobart Time/Date should reboot, 0 = no, 1 = yes")
p.add_option("-O", "--offsetSign",
             dest="offsetSign",
             help="Setting Hobart Time/Date should reboot, 0 = no, 1 = yes")
p.add_option("-v", "--verbose",
             dest="verbose", default=None, action="store_true",
             help="Print detailed status")
p.add_option("-q", "--quiet",
             dest="verbose", default=None, action="store_false",
             help="No status printing (default)")

# Parse User's CLI args
#######################
options, args = p.parse_args()

# User this for debugging
#########################
#print 'Year           : %s'%options.year
#print 'Month          : %s'%options.month
#print 'Day            : %s'%options.day
#print 'Hour           : %s'%options.hour
#print 'Minute         : %s'%options.minute
#print 'Second         : %s'%options.second
#print 'Timezone       : %s'%options.timeZone
#print 'Timezone Text  : %s'%options.timeZoneText
#print 'nonAstronomical: %s'%options.astronomical

#print 'outputfile: %s'%options.outputFile
#print 'verbose   : %s'%options.verbose
#print 'args      : %r',args

exitCode = 0
if os.name == "posix":

    # tell processDeviceEvents to stop timer since system time being changed
    exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='stopTimer')
    #print(exitCode)
    if exitCode == 0:
        # Preparing TimeZone
        #######################
        # Prep timezone for file save

        #This is a hack because of limited hwclock and no monotonic timer use in Qt

        # The jist is to set system time to what you want the rtc to be then
        # copy the sytem time to the hwclock
        # Now set the system time to what you want the system time to be and
        # don't set the rtc

        time.sleep(2)
        fileTimeZoneText = options.timeZoneText
        cmd = []
        cmd.append("ln")
        cmd.append("-sf")
        cmd.append("/usr/share/zoneinfo/%s" % (str(options.timeZoneText)))
        cmd.append("/etc/localtime")

        exitCode, stdout, stderr = shellCommand.execute(cmd)
        if exitCode != 0:
            #print('1')
            sys.exit(exitCode)
        elif exitCode == 0:
            #print('-1A-')
            cmd = []
            cmd.append("echo")
            cmd.append("%s" % (str(options.timeZoneText)))
            cmd.append(">")
            cmd.append("/etc/timezone")

            exitCode, stdout, stderr = shellCommand.execute(cmd)
            if exitCode != 0:
                #print('2')
                sys.exit(exitCode)
            elif exitCode == 0:
                #print('-2A-')
                os.environ['TZ'] = str(options.timeZoneText)

                cmd = []
                cmd.append("export")
                cmd.append("TZ=" + str(options.timeZoneText))

                exitCode, stdout, stderr = shellCommand.execute(cmd, True)

    if options.doReboot == "1":
        # do a reboot : Note manufacturing should not do this
        os.system("/sbin/reboot")
    sys.exit(exitCode)

else:
    # WINDOWS BLOCK GOES HERE
    # this all needs to be run admin re run elevated if necessary
    from win32com.shell import shell
    if not shell.IsUserAnAdmin():
        exitCode = 1
        try:
            import osUtils
            import inspect
            cmd = []
            cmd.append('C:\\Python27\\python.exe')
            cmd.append(inspect.getfile(inspect.currentframe()))
            cmd.extend(sys.argv[1:])
            exitCode, stdout, stderr = osUtils.winRunElevated(cmd)
        except:
            pass
        sys.exit(exitCode)

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
    print((str(exitCode)))
    print((str(stderr)))
    print((str(stdout)))
    if (exitCode == 0) or (exitCode == 2) or (stdout.startswith('The Windows Time service is not started.')):     
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
    #fileTimeZone = str(options.timeZone)

    #Prepare the timeZone
    ntTimezones = {
    'Europe/London': 'GMT Standard Time',
    'Europe/Paris':'Romance Standard Time',
    'Africa/Cairo':'Egypt Standard Time',  
    'Europe/Moscow':'Kaliningrad Standard Time',
    'Asia/Muscat':'Arabian Standard Time',
    'Asia/Karachi':'Pakistan Standard Time',
    'Asia/Dhaka':'Bangladesh Standard Time',
    'Asia/Jakarta':'SE Asia Standard Time',
    'Asia/Taipei':'Taipei Standard Time',
    'Asia/Tokyo':'Tokyo Standard Time',
    'Australia/Brisbane':'E. Australia Standard Time',
    'Pacific/Noumea':'Central Pacific Standard Time',
    'Pacific/Auckland':'New Zealand Standard Time',
    'Atlantic/Cape_Verde':'Cape Verde Standard Time',
    'Atlantic/Noronha':'Mid-Atlantic Standard Time',
    'America/Godthab':'Montevideo Standard Time',
    'America/Halifax':'Atlantic Standard Time',
    'America/Detroit':'Eastern Standard Time',
    'America/Chicago':'Central Standard Time',
    'America/Phoenix':'US Mountain Standard Time',
    'America/Denver':'Mountain Standard Time',
    'America/Los_Angeles':'Pacific Standard Time',
    'America/Anchorage':'Alaskan Standard Time',
    'Pacific/Honolulu':'Hawaiian Standard Time',
    'Pacific/Midway':'UTC-11 Standard Time',
    'Pacific/Midway2':'Dateline Standard Time'
    }
 
    fileTimeZoneText = str(str(ntTimezones.get(options.timeZoneText)))

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
