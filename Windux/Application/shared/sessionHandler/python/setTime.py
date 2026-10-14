#!/usr/bin/env python3
# -*- coding: utf-8 -*-

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

import datetime
import file_io
import os
import sendDeviceEventMessage
import shellCommand
import sys
import time
import getBoardType
import subprocess

from optparse import OptionParser

# Enter relative path & file for save 
filename = "HobartQmSettings-DATETIME.txt"
mainCommand = "date"
dbName = ""

##################################
#
# local time offset
#  
##################################
def local_time_offset(t=None):
    """Return offset of local zone from GMT, either at present or at time t."""
    # python2.3 localtime() can't take None
    if t is None:
        t = time.time()

    if time.localtime(t).tm_isdst and time.daylight:
        return -time.altzone

    return -time.timezone

##################################
#
# set time from options
#  
# The jist is to set system time to what you want the rtc to be then
# copy the sytem time to the hwclock
# Now set the system time to what you want the system time to be and
# don't set the rtc
#
##################################
def set_time_from_options(options):

    allTestsPassed = True

    #---------------------------------------------
    # SOF-3521 - Additional checks for ARK hwclock
    #---------------------------------------------

    # This needs to be done first thing... if hwclock gets set the flags will
    # be zeroed out on the part and it will report okay rtc

    if getBoardType.isArkBoard():

        # check for bad battery
        cmd01 = 'i2cget -f -y 0 0x51 0x01 b'
        p = subprocess.Popen(cmd01, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        reg01, err = p.communicate()

        reg01 = reg01.strip()
        #print reg01

        if (4 == len(reg01)) and ('0x' == reg01[0:2]):
            reg01_val = int(reg01, 16)
            #print reg01_val

            flag_blf = 4

            if (reg01_val & flag_blf):
                print('low voltage detected, check/replace RTC battery')
                allTestsPassed = False

        # check for stopped oscillator
        cmd02 = 'i2cget -f -y 0 0x51 0x02 b'
        p = subprocess.Popen(cmd02, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        reg02, err = p.communicate()

        reg02 = reg02.strip()
        #print reg02

        if (4 == len(reg02)) and ('0x' == reg02[0:2]):
            reg02_val = int(reg02, 16)
            #print reg02_val

            flag_osf = 128

            if (reg02_val & flag_osf):
                print('oscillator stop detected, date/time is not reliable')
                allTestsPassed = False

    # Preparing TimeZone
    #######################
    fileTimeZoneText = options.timeZoneText

    cmd = []
    cmd.append("ln")
    cmd.append("-sf")
    cmd.append("/usr/share/zoneinfo/%s" % (str(options.timeZoneText)))
    cmd.append("/etc/localtime")

    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        #print('1')
        allTestsPassed = False

    #print('-1A-')
    cmd = []
    cmd.append("echo")
    cmd.append("%s" % (str(options.timeZoneText)))
    cmd.append(">")
    cmd.append("/etc/timezone")

    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        #print('2')
        allTestsPassed = False

    #print('-2A-')
    os.environ['TZ'] = str(options.timeZoneText)

    cmd = []
    cmd.append("export")
    cmd.append("TZ=" + str(options.timeZoneText))

    exitCode, stdout, stderr = shellCommand.execute(cmd, True)
    if exitCode != 0:
        #print('3')
        allTestsPassed = False

    #print('-3A-')

    # Preparing the Date
    #######################

    date = str(options.year).zfill(4) + str(options.month).zfill(2) + str(options.day).zfill(2)

    dateFormat = "+%Y%m%d"

    dateOptions = "-s " + date

    #print ("\n\nXXX\nSet date\n command: " + command + "\nXXX\n\n")

    # Set the date
    #######################
    cmd = []
    cmd.append("date")
    cmd.append(dateFormat)
    cmd.append("-s")
    cmd.append(date)
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        #print('4')
        allTestsPassed = False

    #print('-4A-')

    fileDate = date

    # Preparing the Time
    #######################

    timew = str(options.hour).zfill(2) + ":" + str(options.minute).zfill(2) + ":00"
    timeFormat = "+%H:%M:%S"

    #print ("\n\nXXX\nSet time\n command: " + command + "\nXXX\n\n")

    # Set the Time
    #######################
    cmd = []
    cmd.append("date")
    cmd.append(timeFormat)
    cmd.append("-s")
    cmd.append(timew)

    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        #print('5')
        allTestsPassed = False

    #print('-5A-')

    if ( not allTestsPassed ):
        return False

    # Set the hardware clock
    #######################
    cmd = []
    cmd.append("hwclock")
    cmd.append("--utc")
    cmd.append("--systohc")

    exitCode, stdout, stderr = shellCommand.execute(cmd)
    if exitCode != 0:
        #print('6')
        return False

    #print('-6A-')

    # Validate hardware clock
    #######################

    cmd = []
    cmd.append("hwclock")
    cmd.append("--utc")

    exitCode, stdout, stderr = shellCommand.execute(cmd)

    if exitCode != 0:
        #print('7')
        return False
    elif len(stdout)<32:
        #print('8')
        return False

    hwday = stdout[8:10]
    hwyear = stdout[20:24]

    #print "From hwclock --utc:"
    #print "stdout=" + stdout,
    #print(hwday)
    #print(options.day)
    #print(hwyear)
    #print(options.year)

    if ((int(options.year) != int(hwyear)) or (int(options.day) != int(hwday))):
        #print('9')
        return False

    #print('-8A-')

    # Validate i2c device
    #######################

    if getBoardType.isArkBoard():
        rtc = os.path.join('/','sys','bus','i2c','devices','0-0051','rtc','rtc0','date')
    else:
        rtc = os.path.join('/','sys','bus','i2c','devices','0-0068','rtc','rtc0','date')
    
    # check device exists
    if not os.path.isfile(rtc):
        #print('9')
        return False

    cmd = []
    cmd.append("cat")
    cmd.append(rtc)

    exitCode, stdout, stderr = shellCommand.execute(cmd)

    hwday = stdout[8:10]
    hwyear = stdout[0:4]

    #if getBoardType.isArkBoard():
    #    print "From /sys/bus/i2c/devices/0-0051/rtc/rtc0/date:"
    #else:
    #    print "From /sys/bus/i2c/devices/0-0068/rtc/rtc0/date:"
    #print "stdout=" + stdout,
    #print(hwday)
    #print(options.day)
    #print(hwyear)
    #print(options.year)

    #if ((int(options.year) != int(hwyear)) or (int(options.day) != int(hwday))):
    # SOF-1751: The 'date' file does take timezone into account, so just make sure the file is not empty
    if not hwday or not hwyear:
        #print('10')
        return False

    #---------------------------------------------
    # SOF-3506 - Validate the hwclock is advancing
    #---------------------------------------------
    if getBoardType.isArkBoard():
        rtc = os.path.join('/','sys','bus','i2c','devices','0-0051','rtc','rtc0','time')
    else:
        rtc = os.path.join('/','sys','bus','i2c','devices','0-0068','rtc','rtc0','time')
        
    # Check device exists
    if not os.path.isfile(rtc):
        #print('11')
        return False

    cmd = []
    cmd.append("cat")
    cmd.append(rtc)

    exitCode, time1, stderr = shellCommand.execute(cmd)
    
    # Wait, then capture the time again and use to compare
    time.sleep( 2.0 )

    exitCode, time2, stderr = shellCommand.execute(cmd)
    
    #print "time1=" + time1
    #print "time2=" + time2
    
    hour_ = time1[0:2]
    minute_ = time1[3:5]
    seconds_ = time1[6:8]
    
    #print(hour_)
    #print(minute_)
    #print(seconds_)

    timeCheck1 = datetime.time( int(hour_), int(minute_), int(seconds_) )

    hour_ = time2[0:2]
    minute_ = time2[3:5]
    seconds_ = time2[6:8]

    #print(hour_)
    #print(minute_)
    #print(seconds_)

    timeCheck2 = datetime.time( int(hour_), int(minute_), int(seconds_) )
    
    #print "timeCheck1=" + str(timeCheck1)
    #print "timeCheck2=" + str(timeCheck2)
    
    # Make sure the time has advanced
    if( timeCheck1 >= timeCheck2 ):
        #print('12')
        return False


    #print ('set_time_from_options() - SUCCESS')
   
    return True


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

if os.name == "posix":

    # We have to kill openbox and restart it after a time change. Look at bottom of posix block
    # define this in case the offsetSign flag is not given on the commandline
    exitCode = 0

    # tell processDeviceEvents to stop timer since system time being changed
    if options.offsetSign != "99":
        exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='stopTimer')

    #print(exitCode)
    if exitCode == 0:
        if not set_time_from_options(options):
            #print('set time failed!')
            sys.exit(1)

        if options.offsetSign != "99":
            exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='startTimer')

    if options.doReboot == "1":
        # do a reboot : Note manufacturing should not do this
        os.system("/sbin/reboot")

    sys.exit(0)

else:    
    # WINDOWS BLOCK
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
        
    import datetime
    from datetime import timedelta
     
    usersDateChoice = datetime.datetime(int(str(options.year)), 
        int(str(options.month)),  
        int(str(options.day)),
        int(str(options.hour)), 
        int(str(options.minute)), 
        0, 
        0)
        
    import time
    
    readyToConvertToUTC = datetime.datetime.fromtimestamp(time.mktime(usersDateChoice.utctimetuple()))
    offset = local_time_offset(time.mktime(usersDateChoice.utctimetuple())) / 3600
    
    userChoiceTimeInUTC = usersDateChoice - timedelta(hours=offset)
    
    #These commands must be run in command.exe as administrator
    dateCommand = "date"
    timeCommand = "time"
    timeZoneCommand = "tzutil" #tzutil available in Win7 and above

    #Disable Windows Time Synchronization
    # requires admin privileges to test
    
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
    'America/Detroit':'US Eastern Standard Time',
    'America/Chicago':'Central Standard Time',
    'America/Phoenix':'US Mountain Standard Time',
    'America/Denver':'Mountain Standard Time',
    'America/Los_Angeles':'Pacific Standard Time',
    'America/Anchorage':'Alaskan Standard Time',
    'Pacific/Honolulu':'Hawaiian Standard Time',
    'Pacific/Midway':'UTC-11 Standard Time',
    'Pacific/Midway2':'Dateline Standard Time'
    }
 
    ###########################
    NTPScheduledTaskName = 'SyncTimeFromNTP'
    
    cmd = []
    cmd.append('schtasks')
    cmd.append('/delete')
    cmd.append('/tn')
    cmd.append(NTPScheduledTaskName)
    cmd.append('/f')
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    
    cmd = []
    cmd.append("net")
    cmd.append("stop")
    cmd.append("w32time")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
 
    cmd = []
    cmd.append('w32tm')
    cmd.append('/unregister')
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    #Format the date
    fileDate = str(options.month).zfill(2) + "-" + str(options.day).zfill(2) + "-" + str(options.year)[-2:]
    dateDesired = datetime.datetime(int(options.year),  int(options.month),  int(options.day))
    
    #################
    try:
        import win32api
        
    except ImportError:
        sys.exit(1)
    timeTuple = win32api.GetSystemTime()
    
    newTimeTuple = userChoiceTimeInUTC.timetuple()
    # Need permissions to execute these
    win32api.SetSystemTime(newTimeTuple.tm_year, 
        newTimeTuple.tm_mon , 
        newTimeTuple.tm_wday , 
        newTimeTuple.tm_mday , 
        newTimeTuple.tm_hour , 
        newTimeTuple.tm_min , 
        newTimeTuple.tm_sec , 0 )
    
    sys.exit(exitCode)
sys.exit(1)
