######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script sets the Brightness of the Display
##  REQS    : Must have permissions to execute the
##            OS's brightness tool command without password
##            prompt. All options must be provided, except for -v -h -q
######################################################

# Sample command -># qmSettingsSetBrightness.pyc -B 225 -D 1

# Will set the brightness of Operator Display to 225

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


# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-B", "--brightness",
             dest="brightness",
             help="The BRIGHTNESS you want to set the display to as 1-3 digits in range 50-255 (ex. 234)")
p.add_option("-D", "--display",
             dest="display",
             help="The Display you want to set the brightness of as 1 digit (ex. 0=both, 1=Operator, 2=Customer)")
p.add_option("-v", "--verbose",
             dest="verbose", default=None, action="store_true",
             help="Print detailed status")
p.add_option("-q", "--quiet",
             dest="verbose", default=None, action="store_false",
             help="No status printing (default)")

# Parse User's CLI args
#######################
options, args = p.parse_args()

# Use this for debugging
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
####################################################

if os.name == "posix":
    # i.MX6
    #os.system(mainPosixCommand)

    operatorCommand = "echo -n " + str(options.brightness) + " > /sys/class/backlight/pwm-backlight.3/brightness"
    customerCommand = "echo -n " + str(options.brightness) + " > /sys/class/backlight/pwm-backlight.0/brightness"
    if options.display == "1":
        # Operator only
        os.system(operatorCommand)
    if options.display == "2":
        # Customer only
        #TBD 11/8/2012 Electrical doesn't know which pwm to use yet change below when known
        os.system(customerCommand)
    if options.display == "0":
        # Set both displays
        # set the operator display
        os.system(operatorCommand)
        # set the customer display
        os.system(customerCommand)
else:
    # TBD Windows
    # What are Windows calls to go here?

    operatorCommand = "TBD"
    customerCommand = "TBD"
    if options.display == "1":
        # Operator only
        os.system(operatorCommand)
    if options.display == "2":
        # Customer only
        #TBD 11/8/2012 Electrical doesn't know which pwm to use yet change below when known
        os.system(customerCommand)
    if options.display == "0":
        # Set both displays
        # set the operator display
        os.system(operatorCommand)
        # set the customer display
        os.system(customerCommand)

