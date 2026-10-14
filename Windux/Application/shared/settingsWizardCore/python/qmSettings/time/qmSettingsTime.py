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

# Sample command -># qmSettingsTime.pyc -Y 2011 -M 10 -D 17 -H 14 -m 30 -S 20 -T "Eastern Time (US & Canada)" -t +05 -A 1 -o history.txt

# Will set to system and hardware clock to-> October 17, 2011 02:30:20 PM eastern timezone - using a 24 hour clock

# Preprocessor directives
##########################
import sys, os
from optparse import OptionParser
import time

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
mainCommand = "sudo date"
dbName = ""

##################################
#
#     E N D   G L O B A L S
#  
##################################

# First thing, get a timestamp for this
# executable run
######################################
now = time.localtime()
fileNow = str("%d%02d%02d%02d%02d%02d" % (now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec))

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
p.add_option("-T", "--timeZone",
             dest="timeZone",
             help="The timezone component with +|- and 2 digits from GMT (ex.+05 = GMT+5 hours or -11 = GMT-11 hours)")
p.add_option("-t", "--timeZoneText",
             dest="timeZoneText",
             help="The timezone text component (ex. Eastern Time (US & Canada)")
p.add_option("-A", "--nonAstronomical",
             dest="nonAstronomical",
             help="The 24 hour clock component with 1 digit (ex. non-zero = use 12 hour clock or 0 = use 24 hour clock ")
p.add_option("-o", "--output",
             dest="outputFile",
             help="Results of setting Hobart Time/Date written to this file")
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
    # Preparing the Date
    #######################

    date = str(options.year) + str(options.month) + str(options.day)

    dateFormat = "+%Y%m%d"

    dateOptions = "-s " + date

    command = mainCommand + " " + dateFormat + " " + dateOptions

    #print ("\n\nXXX\nSet date\n command: " + command + "\nXXX\n\n")

    # Set the date
    #######################
    os.system(command + " > /dev/null") 
    fileDate = date

    # Preparing the Time
    #######################
    time = str(options.hour) + ":" + str(options.minute) + ":" + str(options.second)
    timeOptions = "-s \"" + time + "\""
    timeFormat = "+%H:%M:%S"

    command = mainCommand + " " + timeFormat + " " + timeOptions
    #print ("\n\nXXX\nSet time\n command: " + command + "\nXXX\n\n")

    # Set the Time
    #######################
    os.system(command + " > /dev/null")
    fileTime = str(options.hour) + str(options.minute) + str(options.second)

    # Preparing TimeZone
    #######################
    # Prep timezone for file save
    fileTimeZoneText = options.timeZoneText

    # Set Timezone
    #######################
    command = "ln -sf /usr/share/zoneinfo/" + str(options.timeZoneText) + " /etc/localtime"

    os.system(command)
    #print("\n\ntimezone command: " + command + "\n\n")

    os.environ['TZ'] = str(options.timeZoneText)
    command = "export TZ=" + str(options.timeZoneText)
    os.system(command)

    fileTimeZone = options.timeZone

    # Just an example of timeZoneText "Eastern Time (US & Canada)"

    # Prepare 24 hour clock
    ########################

    # This setting is for display purposes within our app.
    # System time is always kept in 24 hour format. Other
    # formats will be derived from this.
    # This setting will be kept in a file for the scale
    # to use to display the time in either 12 or 24 hour
    # format.

    # Var below is used to fail-safe the setting to the
    # most common format, which is 12 hour clock. It requires
    # the value to be only zero.
    #
    # If value is typed, or mis-typed, to any non-zero number, the
    # 12 hour clock will show. Change this default if target customer
    # is military.

    fileNonAstronomical = True

    if options.nonAstronomical == "0":
        fileNonAstronomical = False

    # fileNonAstronomical var is ready for file write below.
    # Apps will have to refer to it to align their clock display
    # with User's choice, 12 or 24 hour.
    ######################################################

    # Set the hardware clock to the newly set system clock
    #######################################################
    os.system('hwclock --systohc > /dev/null' )

    # Save this action to a file for Logistics purposes. Overwrite existing file using provided filename or else default to overwriting Global Filename defined above.
    ################################
    if options.outputFile:
        filename = options.outputFile

    fileIO = open(filename, 'w')

    fileUser = options.user

    line = (str(fileNonAstronomical) + "," + fileNow + "," + str(fileUser) + "," + str(fileTimeZone) + "," + str(fileTimeZoneText) + "," +str(fileDate) + str(fileTime))

    fileIO.write(line)
    fileIO.close()

else:
    # WINDOWS BLOCK GOES HERE
    print("Windows")
