#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script sets Linux/WES7 NTP cronjob.
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

cronjobFile = "/usr/local/hobart/python/cronjobs"

##################################
#
#     G L O B A L S
#
# some easy to change vars below
#
#
#################################


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

#options.append("-S"); //ntp Server (text of servier ip or domainname)
#options.append(ui->ntpServer->text());
#//options.append(QString("%1").arg(this->pickedDate->year() ,4,));
#options.append("-F"); // ntp frequency 0=weekly 1=monthly 2=daylightSaving and Standard Time
#//options.append(QString::number(this->ui->dateEdit->selectedDate().month()));
#options.append(QString::number(ui->ntpFrequencyCombo->currentIndex()));
#options.append("-D"); //ntp dayOfWeek 0=Sunday 1=Monday 2=Tuesday 3=Wednesday 4=Thursday 5=Friday 6=Saturday
#options.append(QString::number(ui->ntpDayOfWeekCombo->currentIndex()));

#options.append("-H"); // ntp timeOfDay hour
#options.append(QString("%1").arg(this->ui->ntpTimeEdit->time().hour(),2,10, QChar('0')).toUpper());
#options.append("-M"); // ntp timeOfDay minute
#options.append(QString("%1").arg(this->ui->ntpTimeEdit->time().minute(),2,10, QChar('0')).toUpper());
#options.append("-S"); // ntp timeOfDay second
#options.append(QString("%1").arg(this->ui->ntpTimeEdit->time().second(),2,10, QChar('0')).toUpper());

#options.append("-T"); // NTP text of timezone e.g., America/Detroit
#options.append(QString("%1").arg(this->ui->ntpTimeZoneCombo->itemData(this->ui->ntpTimeZoneCombo->currentIndex()).toString()));

p.add_option("-s", "--ntpserver",
             dest="server",
             help="The IP address or domain name of the ntp server")
p.add_option("-F", "--frequency",
             dest="frequency",
             help="The frequency of ntp updates (ex. 0=weekly 1=monthly 2=daylightsaving)")
p.add_option("-D", "--dayOfWeek",
             dest="day",
             help="The day component as 1 digit (ex. 6=Saturday 0=Sunday 1=Monday 2=Tuesday etc.)")
p.add_option("-H", "--hour",
             dest="hour",
             help="The hour component with 2 digits given in astronomical time (ex. 18 is 6:00 PM)")
p.add_option("-M", "--minute",
             dest="minute",
             help="The minute component with 2 digits (ex. 03 is 3 minutes)")
p.add_option("-S", "--second",
             dest="second",
             help="The second component with 2 digits (ex. 08 is 8 seconds)")
p.add_option("-T", "--timezone",
             dest="timezone",
             help="The timezone (ex. America/Detroit")
p.add_option("-U", "--useNTP",
             dest="useNTP",
             help="Whether to use NTP or not (ex. 0=NO 1=YES)")
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

crontabFile

if os.name == "posix":

    if options.useNTP == "0":
        trial = 0
        '''try:
            cmd = []
            cmd.append("crontab")
            cmd.append("-d")
            cmd.append("root")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
        except:
            pass

        if exitCode == 0:
            cmd = []
            cmd.append("sync")
            exitCode, stdout, stderr = shellCommand.execute(cmd)
        '''
        # remove line from cronjobs file


    else:

        # remove old crontab
        cmd = []
        cmd.append("crontab")
        cmd.append("-d")
        cmd.append("root")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        try:
            os.remove("/usr/local/hobart/db/settingswizard/ntpcron")
        except:
            pass
        cmd = []
        cmd.append("sync")
        exitCode, stdout, stderr = shellCommand.execute(cmd)

        # write temp file for crontab
        try:
            cronString = ""
            cronString2 = ""
            f = open("/usr/local/hobart/db/settingswizard/ntpcron", "a")

            # WEEKLY
            if len(options.frequency) > 0 and options.frequency != "" and options.frequency == "0":

                if len(options.minute) > 0 and options.minute != "":
                    cronString = cronString + str(options.minute) + " "
                else:
                    cronString = cronString + "0 "

                if len(options.hour) > 0 and options.hour != "":
                    cronString = cronString + str(options.hour) + " "
                else:
                    cronString = cronString + "0 "

                cronString = cronString + " * * "

                if len(options.day) > 0 and day != "":
                    cronString = cronString + str(options.day) + " "
                else:
                    cronString = cronString + "0 "

            # MONTHLY
            elif len(options.frequency) > 0 and options.frequency != "" and options.frequency == "1":

                if len(options.minute) > 0 and options.minute != "":
                    cronString = cronString + str(options.minute) + " "
                else:
                    cronString = cronString + "0 "

                if len(options.hour) > 0 and options.hour != "":
                    cronString = cronString + str(options.hour) + " "
                else:
                    cronString = cronString + "0 "

                cronString = cronString + "1 * * "

            # DAYLIGHT SAVINGS
            # 2 lines needed
            # Second Sunday in March and First Sunday in November

            elif len(options.frequency) > 0 and options.frequency != "" and options.frequency == "2":

                if len(options.minute) > 0 and options.minute != "":
                    if options.hour == "2":
                        cronString = cronString + "30 "
                        cronString2 = cronString2 + "30 "
                    else:
                        cronString = cronString + str(options.minute) + " "
                        cronString2 = cronString2 + str(options.minute) + " "
                else:
                    cronString = cronString + "0 "
                    cronString2 = cronString2 + "0 "

                if len(options.hour) > 0 and options.hour != "":
                    if options.hour == "2":
                        cronString = cronString + "3 "
                        cronString2 = cronString2 + "3 "
                    else:
                        cronString = cronString + str(options.hour) + " "
                        cronString2 = cronString2 + str(options.hour) + " "


                cronString = cronString + "8-15 3 0"
                cronString2 = cronString2 + "1-7 11 0"

            f.close()
        except:
            pass
        cmd = []
        cmd.append("sync")
        exitCode, stdout, stderr = shellCommand.execute(cmd)


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
            fileTimeZoneText = options.timezone
            cmd = []
            cmd.append("ln")
            cmd.append("-sf")
            cmd.append("/usr/share/zoneinfo/%s" % (str(options.timezone)))
            cmd.append("/etc/localtime")

            exitCode, stdout, stderr = shellCommand.execute(cmd)
            if exitCode != 0:
                #print('1')
                sys.exit(exitCode)
            elif exitCode == 0:
                #print('-1A-')
                cmd = []
                cmd.append("echo")
                cmd.append("%s" % (str(options.timezone)))
                cmd.append(">")
                cmd.append("/etc/timezone")

                exitCode, stdout, stderr = shellCommand.execute(cmd)
                if exitCode != 0:
                    #print('2')
                    sys.exit(exitCode)
                elif exitCode == 0:
                    #print('-2A-')
                    os.environ['TZ'] = str(options.timezone)

                    cmd = []
                    cmd.append("export")
                    cmd.append("TZ=" + str(options.timezone))

                    exitCode, stdout, stderr = shellCommand.execute(cmd, True)
                    if exitCode != 0:
                        #print('3')
                        sys.exit(exitCode)

            exitCode = sendDeviceEventMessage.sendMessage(server='localhost',port=5077,message='startTimer')
    #print 'at end exit code = %d' % (exitCode)
    #if options.doReboot == "1":
        # do a reboot : Note manufacturing should not do this
    #    os.system("/sbin/reboot")
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
