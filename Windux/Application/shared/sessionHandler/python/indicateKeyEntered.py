#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os

#keyMask bit mask values
mapKeyTypeToBit = {}
mapKeyTypeToBit["SUPERVISOR"] = 1
mapKeyTypeToBit["SERVICE"] = 4
mapKeyTypeToBit["SELFSERVICE"] = 8
mapKeyTypeToBit["SYSTEMTEST"] = 16
mapKeyTypeToBit["MANUFACTURING"] = 32

#bit value is 2^(bit_index)
def keyMaskCheck(keyMask, bit):
    return ((keyMask & bit) != 0)

#check if the bit for the corresponding key is true. 
## Create a file if true
## Try to delete the file if false
def modifyKeyFile(keyMask, keyType):
    if keyMaskCheck(keyMask, mapKeyTypeToBit[keyType]):
        createSingleKeyFile(keyType)
    else:
        deleteSingleKeyFile(keyType)

#create a temp file for a valid key entered
def createSingleKeyFile(fileName):
    fileString = "/tmp/%s" % fileName
    F = open(fileString, "w")
#    print "\tCreated: " + fileString

#if file exists, delete it
def deleteSingleKeyFile(fileName):
    fileString = "/tmp/%s" % fileName
    try:
        os.remove(fileString)
    except OSError as e:
        pass
#    print "\tDeleted: " + fileString

#generate/delete file(s) in /tmp/ corresponding to the keyMask values
def modifyKeyFiles(keyMask):
    for key in mapKeyTypeToBit:
        modifyKeyFile(keyMask, key)
