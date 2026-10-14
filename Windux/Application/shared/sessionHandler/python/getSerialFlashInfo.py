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
    parser = argparse.ArgumentParser(description='Retrieve serial flash data.')
    parser.add_argument('--raw', help='dump entire serial data section', action='store_true')
    parser.add_argument('--mfg', help='dump serial manufacturing data section', action='store_true')
    parser.add_argument('--sw', help='dump serial software data section', action='store_true')
    
    args = parser.parse_args()
    
    if args.mfg:
        if args.raw:
            section = mfgSection()
        
            print((section.getRawPartition()))
        
        else:
            manufDate = getManufDate()
            manufDate = manufDate.strip( '"' )
        
            serialNumber = getScaleSerialNumber()
            serialNumber = serialNumber.strip( '"' )
        
            scaleStyle = getModelName()
            scaleStyle = scaleStyle.strip( '"' )
        
            boardAssemblyNumber = getBoardAssembly()
            boardAssemblyNumber = boardAssemblyNumber.strip( '"' )
        
            boardRevisionNumber = getBoardRevison()
            boardRevisionNumber = boardRevisionNumber.strip( '"' )
        
            print((manufDate + ',' + serialNumber + ',' + scaleStyle + ',' + boardAssemblyNumber + ',' + boardRevisionNumber))

    if args.sw:
        if args.raw:
            section = softwareSection()
        
            print((section.getRawPartition()))
        
        else:
            metroVer = getMetroVer()
            metroVer = metroVer.strip( '"' )
        
            fractionalEnabled = getFractionalEnabled()
            fractionalEnabled = fractionalEnabled.strip( '"' )
        
            poundsForEnabled = getPoundsForEnabled()
            poundsForEnabled = poundsForEnabled.strip( '"' )
        
            print((metroVer + ',' + fractionalEnabled + ',' + poundsForEnabled))

    sys.exit( 0 )
