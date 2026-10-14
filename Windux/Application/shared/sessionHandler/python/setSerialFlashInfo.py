#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#------------------------------------------------------------
# Gets information from the serial flash using serialFlash.py
#------------------------------------------------------------

import argparse
import sys
import os
import shellCommand

from serialFlash import *

#------------------------------------------
def getModelNameFromFlash():
    
    name = ''
    
    try:
        mfgs = mfgSection()
        name = mfgs.getScaleStyle()

    except:
        pass
        
    return name

def getModelName():
    
    name = getModelNameFromFlash()
    
    return name    
#------------------------------------------
def getManufDateFromFlash():
    
    date = ''
    
    try:
        mfgs = mfgSection()
        date = mfgs.getDateOfManufacturing()

    except:
        pass
        
    return date

def getManufDate():
    
    date = getManufDateFromFlash()
    
    return date    
#------------------------------------------
def getScaleSerialNumberFromFlash():
    
    serialNumber = ''
    
    try:
        mfgs = mfgSection()
        serialNumber = mfgs.getScaleSerialNumber()

    except:
        pass
        
    return serialNumber

def getScaleSerialNumber():
    
    serialNumber = getScaleSerialNumberFromFlash()
    
    return serialNumber    
#------------------------------------------
def getBoardAssemblyFromFlash():
    
    boardAssembly = ''
    
    try:
        mfgs = mfgSection()
        boardAssembly = mfgs.getBoardAssembly()

    except:
        pass
        
    return boardAssembly

def getBoardAssembly():
    
    boardAssembly = getBoardAssemblyFromFlash()
    
    return boardAssembly    
#------------------------------------------
def getBoardRevisonFromFlash():
    
    boardRevision = ''
    
    try:
        mfgs = mfgSection()
        boardRevision = mfgs.getBoardRevision()

    except:
        pass
        
    return boardRevision

def getBoardRevison():

    boardRevision = getBoardRevisonFromFlash()
    
    return boardRevision    
#------------------------------------------
def getMetroVerFromFlash():
    
    metroVer = ''
    
    try:
        sws = softwareSection()
        metroVer = sws.getMetroVersion()

    except:
        pass
        
    return metroVer

def getMetroVer():

    metroVer = getMetroVerFromFlash()
    
    return metroVer    
#------------------------------------------
def getFractionalEnabledFromFlash():
    
    fracEnabled = ''
    
    try:
        sws = softwareSection()
        fracEnabled = sws.getFractionalPricingEnabled()

    except:
        pass
        
    return fracEnabled

def getFractionalEnabled():

    fracEnabled = getFractionalEnabledFromFlash()
    
    return fracEnabled    
#------------------------------------------
def getPoundsForEnabledFromFlash():
    
    poundsForEnabled = ''
    
    try:
        sws = softwareSection()
        poundsForEnabled = sws.getPoundsForPricingEnabled()

    except:
        pass
        
    return poundsForEnabled

def getPoundsForEnabled():

    poundsForEnabled = getPoundsForEnabledFromFlash()
    
    return poundsForEnabled    
#------------------------------------------

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Write serial flash data.')
    parser.add_argument('--mfg', help='manufacturing data section', action='store_true')
    parser.add_argument('-scaleStyle', help='scale style', required=False, action='store')
    parser.add_argument('-boardAssembly', help='board assembly number', required=False, action='store')
    parser.add_argument('-boardRev', help='board revision', required=False, action='store')
    parser.add_argument('-dateOfManufacture', help='date of manufacture', required=False, action='store')
    parser.add_argument('-serialNumber', help='serial number', required=False, action='store')
    parser.add_argument('--sw', help='software data section', action='store_true')
    parser.add_argument('-metrologyVersion', help='metrology version', required=False, action='store')
    parser.add_argument('-fractionalPricingEnabled', help='fractional pricing enabled', required=False, action='store')
    parser.add_argument('-poundsForPricingEnabled', help='pounds for pricing enabled', required=False, action='store')

    args = parser.parse_args()
    
    if args.mfg:
        section = mfgSection()
        section.setScaleStyle( args.scaleStyle )
        section.setBoardAssembly( args.boardAssembly )
        section.setBoardRevision( args.boardRev )
        section.setDateOfManufacturing( args.dateOfManufacture )
        section.setScaleSerialNumber( args.serialNumber )

        # counter = int(section.getWriteCounter())
        # section.setWriteCounter( str(counter+1) )

        section.write()        

    if args.sw:
        section = softwareSection()
        section.setMetroVersion( args.metrologyVersion )
        section.setFractionalPricingEnabled( args.fractionalPricingEnabled )
        section.setPoundsForPricingEnabled( args.poundsForPricingEnabled )

        # counter = int(section.getWriteCounter())
        # section.setWriteCounter( str(counter+1) )

        section.write()        

    sys.exit( 0 )
