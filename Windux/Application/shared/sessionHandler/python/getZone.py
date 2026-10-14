#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script reports the Timezone of the scale.
##  REQS    : None
##  RETURNS : String as three characters to be mapped in Qt app
######################################################

# Sample command -> qmSettingGetTimezone.py

# Preprocessor directives
##########################
import sys, os, subprocess

from getTimezone import getTimeZone

error, tz = getTimeZone()

if error != 0:
    tz = ""
    
print(tz)
