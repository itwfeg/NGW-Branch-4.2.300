######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script calls the Touchscreen Calibration Utility
##  REQS    : Must have permissions to execute the
##            OS's calibration tool command without password
##            prompt. All options must be provided, except for -v -h -q
######################################################

# Sample command -># qmSettingsCalibrateTouchscreen.pyc

# Will launch the touchscreen utility and close

# Preprocessor directives
##########################
import sys, os

##################################
#
#     G L O B A L S
#
# some easy to change vars below
#
# 
#################################

mainPosixCommand = "/usr/bin/xinput_calibrator"
mainWES7Command = "TBD but...installDir/xAuto4PtsCal.exe"

# No CLI arguments to pass

if os.name == "posix":
    # i.MX6
    os.system(mainPosixCommand)
    #os.system("/usr/bin/xemacs")
else:
    # TBD Windows
    os.system(mainWES7Command)
