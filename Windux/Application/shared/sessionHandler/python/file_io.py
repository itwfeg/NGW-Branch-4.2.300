# -*- coding: utf-8 -*-

import os
import sys
import string

try:
    import pickle as pickle
except:
    import pickle

# procedure readFileIntoBuff
# description:
#   reads the contents of the file, filename, into a buffer
# arguments:
def readFileIntoBuff(
    # filename
    filename,
    mode='r'):
    # pdl:
    # open the file filename
    # if the file can not be opened return error
    # read the contents of the file into a buffer
    # close the file
    # return success
    try:
        inFile = open(filename, mode)
        data = inFile.read()
        inFile.close()
    except IOError as exception_details:
        return('error: Can not open file: ' + filename + ', Reason:' + str(exception_details), None)

    return ('success', data)

# procedure readFromFile
# description:
#   read the contents of a file into a list of lines with
#   carriage returns optionally removed
# arguments:
def readFromFile(
    # filename
    filename,
    # remove carriage return flag
    removeCR=1):
    # pdl:
    # open the file, filename
    # read the file into a list of lines
    # remove the carriage return from end of each line if specified
    # return file contents
    # close file
    try:
        inFile = open(filename, 'r')
    except IOError as exception_details:
        return('error: Can not open file: ' + filename + ', Reason:' + str(exception_details), None)

    data = inFile.readlines()

    if removeCR:
        for i in range(0,len(data)):
            if data[i][-1:] == '\n':
                data[i] = data[i][:-1]

    inFile.close()
    return ('success', data)

# procedure writeToFile
# description:
#   writes a list of lines to a file, filename
# arguments:
def writeToFile(
    # filename
    filename,
    # list of lines to write
    data):
    # pdl:
    # open the file, filename
    # write the data list to the file with carriage returns added to each line
    # close the file
    try:
        outputFile = open(filename, 'w')
    except IOError as exception_details:
        return('error: Can not open file: ' + filename + ', Reason:' + str(exception_details), None)

    for i in range(0,len(data)):
        outputFile.write(data[i] + '\n')
    outputFile.close()

    return ('success', None)

def writeBufferToFile(
    # filename
    filename,
    data,
    mode='w'):
    # pdl:
    # open the file, filename
    # close the file
    try:
        outputFile = open(filename, mode)
    except IOError as exception_details:
        return('error: Can not open file: ' + filename + ', Reason:' + str(exception_details), None)

    if (type(data) == bytes):
        outputFile.write(str(data, 'utf-8'))
    else:
        outputFile.write(data)
        
    outputFile.close()

    return ('success', None)

def writeBinObjectToFile(
    filename,
    binData):

    try:
        outputFile = open(filename, 'wb')
    except IOError as exception_details:
        return('error: Can not open file: ' + filename + ', Reason:' + str(exception_details), None)

    pickle.dump(binData, outputFile)
    outputFile.close()
    return ('success', None)

def readBinObjectFromFile(
    filename):
    try:
        inFile = open(filename, 'rb')
        binObject = pickle.load(inFile)
        inFile.close()
    except IOError as exception_details:
        return('error: Can not open file: ' + filename + ', Reason:' + str(exception_details), None)

    return ('success', binObject)
