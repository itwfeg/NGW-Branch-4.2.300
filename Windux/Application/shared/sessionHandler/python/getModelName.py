#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script reports the model name of the scale.
##  REQS    : None
##  RETURNS : Qmx or Qmt based on os.name
######################################################

# Preprocessor directives
##########################
import sys
import os

from serialFlash import *


def getSystemStyleFromFlash():
    name = ''

    mfgs = mfgSection()
    name = mfgs.getSystemStyle()

    return name


def getModelNameFromFlash():
    name = ''

    mfgs = mfgSection()
    name = mfgs.getScaleStyle()

    return name


def getFullModelName():
    # try to read model name from flash first
    name = getModelNameFromFlash()

    if name and name != '0':
        return name

    if os.name == "posix":
        return "HTi"

    else:
        # WINDOWS OS
        # TODO needs changed once marketing decides on name
        return "HTx"


def getModelName():
    # get full model name
    name = getFullModelName()

    # TODO:
    # Why in the world would we do this below?
    # I am removing this
    # truncate down to 3 letters
    #if name and len(name) > 3:
    #    name = name[:3]

    return name


if __name__=='__main__':
    print((getFullModelName()))
    sys.exit(0)

