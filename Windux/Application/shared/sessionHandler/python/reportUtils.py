#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sql
import os
import sys
import glob
import csv
import getIpAddress

# You're using this, but not importing it.
#import scaleSettingsInterface



changesDict = {
            '0':      '',                            # all of the fields are mapped, but not all are realistic
            '1':      '9,10',                        # unit price
            '2':      '11,12',                       # tare
            '3':      '11,12,9,10',                  # tare, unitprice
            '4':      '13,14',                       # by count
            '5':      '13,14,9,10',                  # bycount, unitprice
            '6':      '13,14,11,12',                 # bycount, tare
            '7':      '13,14,11,12,9,10',            # bycount, tare, unitprice
            '128':    '15,16',                       # fixedweight
            '129':    '15,16,9,10',                  # fixedweight, unitprice
            '130':    '15,16,11,12',                 # fixedweight, tare
            '131':    '15,16,11,12,9,10',            # fixedweight,tare,unitprice
            '132':    '15,16,13,14',                 # fixedweight,bycount
            '133':    '15,16,13,14,9,10',            # fixedweight,bycount,unitprice
            '134':    '15,16,13,14,11,12',           # fixedweight,bycount,tare
            '135':    '15,16,13,14,11,12,9,10',      # fixedweight,bycount,tare,unitprice
            '512':    '7,8',                         # total price
            '513':    '7,8,9,10',                    # tp,up
            '514':    '7,8,11,12',                   # tp,tare
            '515':    '7,8,11,12,9,10',              # tp,tare,up
            '516':    '7,8,13,14',                   # tp,bc
            '517':    '7,8,13,14,9,10',              # tp,bc,up
            '518':    '7,8,13,14,11,12',             # tp,bc,tare
            '519':    '7,8,13,14,11,12,9,10',        # to,bc,tare,up
            '640':    '7,8,15,16',                   # totalprice, fixedweight
            '641':    '7,8,15,16,9,10',              # tp,fw,up
            '642':    '7,8,15,16,11,12',             # tp,fw,tare
            '643':    '7,8,15,16,11,12,9,10',        # tp,fw,tare,up
            '644':    '7,8,15,16,13,14',             # tp,fw,bc
            '645':    '7,8,15,16,13,14,9,10',        # tp,fw,bc,up
            '646':    '7,8,15,16,13,14,11,12',       # tp,fw,bc,tare
            '647':    '7,8,15,16,13,14,11,12,9,10',  # tp,fw,bc,tare,up
            '131072': '11,12',                       # Tare B
            '131073': '11,12,9,10',                  # tareB, unitprice
            '131076': '11,12,13,14',                 # tareB, bycount
            '131077': '11,12,13,14,9,10',            # tareB, bycount,unitprice
            '131200': '11,12,15,16',                 # tareB, fixedweight
            '131201': '11,12,15,16,9,10',            # tareB, fixedweight, unitprice
            '131204': '11,12,15,16,13,14',           # tareB, fixedweight, bycount
            '131205': '11,12,15,16,13,14,9,10',      # tareB, fixedweight, bycount, unitprice
            '131584': '11,12,7,8',                   # tareB, totalprice
            '131585': '11,12,7,8,9,10',              # tareB, totalprice, unitprice
            '131588': '11,12,7,8,13,14',             # tareB, totalprice, bycount
            '131589': '11,12,7,8,13,14,9,10',        # tareB, totalprice, bycount, unitprice
            '131712': '11,12,7,8,15,16',             # tareB, totalprice, fixedweight
            '131713': '11,12,7,8,15,16,9,10',        # tareB, totalprice, fixedweight
            '131714': '11,12,7,8,15,16,11,12',       # tareB, totalprice, fixedweight, unitprice
            '131716': '11,12,7,8,15,16,13,14',       # tareB, totalprice, fixedweight, bycount
            '131717': '11,12,7,8,15,16,13,14,9,10',  # tareB, totalprice, fixedweight, bycount, unitprice
            }

fieldnameList = {
                '7':  'TotPrice',
                '8':  'TotPrice',
                '9':  'UntPrice',
                '10': 'UntPrice',
                '11': 'Tare',
                '12': 'Tare',
                '13': 'ByCount',
                '14': 'ByCount',
                '15': 'FixdWgt',
                '16': 'FixdWgt'
                }

fieldnumList = {
               7:  512,
               8:  512,
               9:  1,
               10: 1,
               11: 2,
               12: 2,
               13: 4,
               14: 4,
               15: 128,
               16: 128
               }


TESTDATA = False
TESTINGDB = False
TESTINGCSV = False


# 2 lines between module-level functions and classes. Duh.


def printOperateModeChanges(tareDigitsStr):

    status, results = getOperateModeDatabase()
    if status != 'success' or len(results) < 1 or len(results[0]) < 1:
        returnedData = 'No records found'
        return returnedData

    convertedData = []
    returnedData = ''
    tareDigits = int(tareDigitsStr)

    for i in range(0, len(results)):
        rowData = []
        for j in range(0, len(results[i])):
            if results[i][j] == None:
                rowData.append('')
            else:
                rowData.append(str(results[i][j]))

        convertedData.append(rowData)

        if( convertedData[i][6] != "0" ):
            datefield   = convertedData[i][0]     # datetime
            datesplit = datefield.split(" ")
            date = datesplit[0].rstrip()
            time = datesplit[1].lstrip()

            trans  = convertedData[i][1]     # trans name
            trans = trans[0:4]               # Tighten it up a little for the label
            plu    = convertedData[i][3]     # plu num
            totpri = convertedData[i][17]    # final/total price
            parsedTotPriceValue = totpri.zfill(3)
            leftLen = len(parsedTotPriceValue) - 2
            parsedTotPriceValue = parsedTotPriceValue[:leftLen] + '.' + parsedTotPriceValue[1:3]
            price = parsedTotPriceValue
            chgNum = convertedData[i][6]

            if(chgNum == '0'):
                continue        #unknown transaction so start loop over

            if( chgNum not in changesDict ):
                continue

            tokens = changesDict[chgNum].split(",")
            loopCtr = 0

            for k in range(0, len(tokens) / 2):
                # then this is a single field mapping get the old and new columns
                oldnum   = int(tokens[loopCtr])
                newnum   = int(tokens[loopCtr+1])
                fieldNum = fieldnumList[oldnum]
                old = convertedData[i][oldnum]
                new = convertedData[i][newnum]
                # if (fieldNum == 2 or fieldNum == 128):
                # instead of tare or tare B, lets do this for any field
                if (len(new) == 0):
                    new = '0'
                if (len(old) == 0):
                    old = '0'

                fieldname = fieldnameList[tokens[loopCtr]]
                if( tokens[loopCtr] == '11' and  int(chgNum) >= 131072 ):
                    fieldname = 'TareB'

                # format any price or tare values
                if(fieldNum == 2 or fieldNum == 128):
                    # format tare, tareB, or fixed weight values
                    parsedTareValue = old.zfill(tareDigits + 1)
                    parsedTareValue = parsedTareValue[:1] + '.' + parsedTareValue[1:tareDigits + 1]
                    old = parsedTareValue
                    parsedTareValue = new.zfill(tareDigits + 1)
                    parsedTareValue = parsedTareValue[:1] + '.' + parsedTareValue[1:tareDigits + 1]
                    new = parsedTareValue
                elif(fieldNum == 1 or fieldNum == 512):
                    # format total price or unit price fields
                    parsedPriceValue = old.zfill(3)
                    leftLen = len(parsedPriceValue) - 2
                    parsedPriceValue = parsedPriceValue[:leftLen] + '.' + parsedPriceValue[1:3]
                    old = parsedPriceValue
                    parsedPriceValue = new.zfill(3)
                    parsedPriceValue = parsedPriceValue[:leftLen] + '.' + parsedPriceValue[1:3]
                    new = parsedPriceValue

                loopCtr += 2

                returnedData +=       date                  # datetime
                returnedData += ' ' + trans                 # transaction name
                returnedData += '  ' + plu.ljust(4)         # plu
                returnedData += ' ' + fieldname.ljust(7)    # changed field
                returnedData += ' ' + price.ljust(5)        # total price
                returnedData += ' ' + old.ljust(5)          # old original price
                returnedData += ' ' + new + '\n'            # new changed price
                returnedData += ' ' + time + '\n\n'         # time on own line bc of length


    return returnedData


def printSupervisorModeChanges(tareDigitsStr):

    status, results = getSupervisorModeDatabase()
    if status != 'success' or len(results) < 1 or len(results[0]) < 1:
        returnedData = 'No records found'
        return returnedData

    convertedData = []
    returnedData = ''
    tareDigits = int(tareDigitsStr)
    if tareDigitsStr == '':
        tareDigitsInt = 2
    else:
        tareDigitsInt = int(tareDigitsStr)

    for i in range(0, len(results)):
        rowData = []
        for j in range(0, len(results[i])):
            if results[i][j] == None:
                rowData.append('')
            else:
                rowData.append(str(results[i][j]))

        convertedData.append(rowData)

        if( convertedData[i][6] != "0" ):
            datefield   = convertedData[i][0]     # datetime
            datesplit = datefield.split(" ")
            date = datesplit[0].rstrip()
            time = datesplit[1].lstrip()
            plu    = convertedData[i][3]     # plu num
            totpri = convertedData[i][17]    # final/total price
            parsedTotPriceValue = totpri.zfill(3)
            leftLen = len(parsedTotPriceValue) - 2
            parsedTotPriceValue = parsedTotPriceValue[:leftLen] + '.' + parsedTotPriceValue[1:3]
            price = parsedTotPriceValue
            chgNum = convertedData[i][6]

            if(chgNum == '0'):
                continue        #unknown transaction so start loop over

            if( chgNum not in changesDict ):
                continue

            tokens = changesDict[chgNum].split(",")
            loopCtr = 0

            for k in range(0, len(tokens) / 2):
                # then this is a single field mapping get the old and new columns
                oldnum   = int(tokens[loopCtr])
                newnum   = int(tokens[loopCtr+1])
                fieldNum = fieldnumList[oldnum]
                old = convertedData[i][oldnum]
                new = convertedData[i][newnum]
                # if (fieldNum == 2 or fieldNum == 128):
                # instead of tare or tare B, lets do this for any field
                if ( len(new) == 0 ):
                    new = '0'
                if( len(old)== 0 ):
                    old = '0'

                fieldname = fieldnameList[tokens[loopCtr]]
                if( tokens[loopCtr] == '11' and  int(chgNum) >= 131072 ):
                    fieldname = 'TareB'

                if(fieldNum == 2 or fieldNum == 128):
                    # format tare, tareB, or fixed weight values
                    old = translateDecimalPlacesWeight( old, tareDigitsInt)
                    new = translateDecimalPlacesWeight( new, tareDigitsInt)
                elif(fieldNum == 1 or fieldNum == 512):
                    # format total price or unit price fields
                    old = translateDecimalPlacesPrice( old, 2 )     # value, numOfDigits - might have to be dynamic in future
                    new = translateDecimalPlacesPrice(new, 2)  # value, numOfDigits - might have to be dynamic in future

                loopCtr += 2

                returnedData +=       date                  # datetime
                returnedData += ' ' + plu.ljust(4)          # plu
                returnedData += ' ' + fieldname.ljust(7)    # changed field
                returnedData += ' ' + price                 # total price
                returnedData += ' ' + old                   # old original price
                returnedData += ' ' + new + '\n'            # new changed price
                returnedData += ' ' + time + '\n\n'         # time on own line bc of length

    return returnedData


def translateDecimalPlacesPrice(value, numDecimals=2):
    if value == '':
        return value

    try:
        value = int(value)
    except Exception as error:
        print(('could not translate decimals places' + str(error)))
        return

    formatString = "{:." + str(numDecimals) + "f}"
    parsedPriceValue = formatString.format(value / 10.0 ** numDecimals)
    return parsedPriceValue


def translateDecimalPlacesWeight(value, numDecimals=2):
    if not value:
        return ''

    try:
        value = int(value)
    except Exception as error:
        print(('could not translate decimals places, ' + str(error)))
        return

    formatString = "{:.3f}"
    parsedValue = formatString.format(value / 10.0 ** 3)

    if numDecimals == 2:
        parsedValue = "{:.2f}".format(float(parsedValue))

    return parsedValue


def writeCsvOperateModeChanges(tareDigitsStr, filename, appendIpAddress=True):
    if TESTINGCSV:
        ipAddress = "127.0.0.1"     # testing writing csv file
    else:
        #ipAddress = scaleSettingsInterface.getValue('ipAddress')
        try:
            exit, ipAddress = getIpAddress.getIpAddress()
            if exit:
                raise Exception
            print(("IP: " + ipAddress))
        except:
            print("Failed to get IP")
            pass

    if appendIpAddress:
        ipAddress = ipAddress.replace('.', '_')
        csvPath = filename + '_' + ipAddress + '.csv'
    else:
        csvPath = filename

    status, results = getOperateModeDatabase()
    if status != 'success':
        raise ValueError("Could not load operator database!")

    convertedData = []
    returnedData = []
    tareDigits = int(tareDigitsStr)

    for i in range(0, len(results)):
        rowData = []
        for j in range(0, len(results[i])):
            if results[i][j] == None:
                rowData.append('')
            else:
                rowData.append(str(results[i][j]))

        convertedData.append(rowData)

        if convertedData[i][6] != "0":
            date = convertedData[i][0]       # datetime
            trans = convertedData[i][1]      # trans name
            plu = convertedData[i][3]        # plu num
            totpri = convertedData[i][17]    # final/total price
            parsedTotPriceValue = totpri.zfill(3)
            leftLen = len(parsedTotPriceValue) - 2
            parsedTotPriceValue = parsedTotPriceValue[:leftLen] + '.' + parsedTotPriceValue[1:3]
            price = parsedTotPriceValue
            chgNum = convertedData[i][6]

            if chgNum == '0':
                continue        # unknown transaction so start loop over

            if chgNum not in changesDict:
                continue

            tokens = changesDict[chgNum].split(",")
            loopCtr = 0

            for k in range(0, len(tokens) / 2):
                # then this is a single field mapping get the old and new columns
                oldnum   = int(tokens[loopCtr])
                newnum   = int(tokens[loopCtr+1])
                fieldNum = fieldnumList[oldnum]
                old = convertedData[i][oldnum]
                new = convertedData[i][newnum]
                # if (fieldNum == 2 or fieldNum == 128):
                # instead of tare or tare B, lets do this for any field
                if (len(new) == 0):
                    new = '0'
                if (len(old) == 0):
                    old = '0'

                fieldname = fieldnameList[tokens[loopCtr]]
                if( tokens[loopCtr] == '11' and  int(chgNum) >= 131072 ):
                    fieldname = 'TareB'

                # format any price or tare values
                if(fieldNum == 2 or fieldNum == 128):
                    # format tare, tareB, or fixed weight values
                    parsedTareValue = old.zfill(tareDigits + 1)
                    parsedTareValue = parsedTareValue[:1] + '.' + parsedTareValue[1:tareDigits + 1]
                    old = parsedTareValue
                    parsedTareValue = new.zfill(tareDigits + 1)
                    parsedTareValue = parsedTareValue[:1] + '.' + parsedTareValue[1:tareDigits + 1]
                    new = parsedTareValue
                elif(fieldNum == 1 or fieldNum == 512):
                    # format total price or unit price fields
                    parsedPriceValue = old.zfill(3)
                    leftLen = len(parsedPriceValue) - 2
                    parsedPriceValue = parsedPriceValue[:leftLen] + '.' + parsedPriceValue[1:3]
                    old = parsedPriceValue
                    parsedPriceValue = new.zfill(3)
                    parsedPriceValue = parsedPriceValue[:leftLen] + '.' + parsedPriceValue[1:3]
                    new = parsedPriceValue

                loopCtr += 2
                returnedData.append([date, trans, plu, fieldname, price, old, new])

    headerList = []
    headerList.append(['Date Time', 'Transaction Name', 'PLU #', 'Changed Field', 'Total Price', 'Original Value', 'New Value'])

    with open(csvPath, "w+") as csvFile:
        csvwriter = csv.writer(csvFile)
        csvwriter.writerows(headerList)
        csvwriter.writerows(returnedData)


def writeCsvSupervisorModeChanges(tareDigitsStr, filename, appendIpAddress=True):
    if TESTINGCSV:
        ipAddress = "127.0.0.1"     # testing writing csv file
    else:
        # ipAddress = scaleSettingsInterface.getValue('ipAddress')
        try:
            exit, ipAddress = getIpAddress.getIpAddress()
            if exit:
                raise Exception
            print(("IP: " + ipAddress))
        except:
            print("Failed to get IP")
            pass

    if appendIpAddress:
        ipAddress = ipAddress.replace('.', '_')
        csvPath = filename + '_' + ipAddress + '.csv'
    else:
        csvPath = filename

    status, results = getSupervisorModeDatabase()
    if status != 'success' or len(results) < 1 or len(results[0]) < 1:
        raise ValueError("Could not load operator database!")

    convertedData = []
    returnedData = []
    tareDigits = int(tareDigitsStr)

    # datetime,transtype,plu,totalprice,changedfield,originalvalue,newvalue

    for i in range(0, len(results)):
        rowData = []
        for j in range(0, len(results[i])):
            if results[i][j] == None:
                rowData.append('')
            else:
                rowData.append(str(results[i][j]))

        convertedData.append(rowData)

        if( convertedData[i][6] != "0" ):
            date   = convertedData[i][0]     # datetime
            #trans  = convertedData[i][1]     # trans name
            plu    = convertedData[i][3]     # plu num
            totpri = convertedData[i][17]    # final/total price
            parsedTotPriceValue = totpri.zfill(3)
            leftLen = len(parsedTotPriceValue) - 2
            parsedTotPriceValue = parsedTotPriceValue[:leftLen] + '.' + parsedTotPriceValue[1:3]
            price = parsedTotPriceValue
            chgNum = convertedData[i][6]

            if(chgNum == '0'):
                continue        #unknown transaction so start loop over

            tokens = changesDict[chgNum].split(",")
            loopCtr = 0

            for k in range(0, len(tokens) / 2):
                # then this is a single field mapping get the old and new columns
                oldnum   = int(tokens[loopCtr])
                newnum   = int(tokens[loopCtr+1])
                fieldNum = fieldnumList[oldnum]
                old = convertedData[i][oldnum]
                new = convertedData[i][newnum]

                #if (fieldNum == 2 or fieldNum == 128):
                # instead of tare or tare B, lets do this for any field
                if (len(new) == 0):
                    new = '0'
                if (len(old) == 0):
                    old = '0'

                fieldname = fieldnameList[tokens[loopCtr]]
                if( tokens[loopCtr] == '11' and  int(chgNum) >= 131072 ):
                    fieldname = 'TareB'

                # format any price or tare values
                if(fieldNum == 2 or fieldNum == 128):
                    # format tare, tareB, or fixed weight values
                    parsedTareValue = old.zfill(tareDigits + 1)
                    parsedTareValue = parsedTareValue[:1] + '.' + parsedTareValue[1:tareDigits + 1]
                    old = parsedTareValue
                    parsedTareValue = new.zfill(tareDigits + 1)
                    parsedTareValue = parsedTareValue[:1] + '.' + parsedTareValue[1:tareDigits + 1]
                    new = parsedTareValue
                elif(fieldNum == 1 or fieldNum == 512):
                    # format total price or unit price fields
                    parsedPriceValue = old.zfill(3)
                    leftLen = len(parsedPriceValue) - 2
                    parsedPriceValue = parsedPriceValue[:leftLen] + '.' + parsedPriceValue[1:3]
                    old = parsedPriceValue
                    parsedPriceValue = new.zfill(3)
                    parsedPriceValue = parsedPriceValue[:leftLen] + '.' + parsedPriceValue[1:3]
                    new = parsedPriceValue

                loopCtr += 2
                returnedData.append([date, plu, fieldname, price, old, new])

    headerList = []
    headerList.append(['Date Time', 'PLU #', 'Changed Field', 'Total Price', 'Original Value', 'New Value'])
    with open(csvPath, "w+") as csvFile:
        csvwriter = csv.writer(csvFile)
        csvwriter.writerows(headerList)
        csvwriter.writerows(returnedData)


def getOperateModeDatabase():
    # Now lets get the data
    if os.name == "posix":
        # LINUX OS
        dbPathFileName = '/usr/local/hobart/db/scale/dbfiles/dbft?.db'
    else:
        # WINDOWS OSs
        if TESTINGDB:
            dbPathFileName = 'C:/hobart/db/scale/super_changes_report/dbft?.db'
        else:
            dbPathFileName = 'C:/Program Files/Hobart/db/Scale/dbFiles/dbft?.db'

    selectCommand = 'select trdttm, trtypes.ttname1, tropname, trprnum, trfnlntwt, traprdt, tropchg, trottvl, trttval, trounpr, trunpr, trotare, trtare, trobycount, trbycount, trofxwt, trfxwt, trfnlpr '
    selectCommand += 'from trans, trtypes where trvoid=0 and trrdclr<2 and trans.ttnum = trtypes.ttnum and trmodechange=0'
    dbFile = glob.glob(dbPathFileName)

    status, results = sql.execute(dbFile[0], selectCommand)

    return status, results


def getSupervisorModeDatabase():
    # Now lets get the data
    if os.name == "posix":
        # LINUX OS
        dbPathFileName = '/usr/local/hobart/db/scale/dbfiles/dbft?.db'
    else:
        # WINDOWS OSs
        if TESTINGDB:
            dbPathFileName = 'C:/hobart/db/scale/super_changes_report/dbft?.db'
        else:
            dbPathFileName = 'C:/Program Files/Hobart/db/Scale/dbFiles/dbft?.db'


    selectCommand = 'select trdttm, trtypes.ttname1, tropname, trprnum, trfnlntwt, traprdt, tropchg, trottvl, trttval, trounpr, trunpr, trotare, trtare, trobycount, trbycount, trofxwt, trfxwt, trfnlpr '
    selectCommand += 'from trans left join trtypes on trans.ttnum = trtypes.ttnum '
    selectCommand += 'where trvoid=0 and trrdclr<2 and trmodechange=1 '
    dbFile = glob.glob(dbPathFileName)

    status, results = sql.execute(dbFile[0], selectCommand)

    return status, results


if __name__=='__main__':

    exitCode = "0"

    argv1 = sys.argv[1:]

    if len(argv1) >= 1:
       inputReportAction = argv1[0]
    if  len(argv1) >= 2:
       inputTareDigits = argv1[1]
    if len(argv1) >= 1:
       inputFileName = argv1[2]

    if TESTDATA:
        inputReportAction = '2'
        inputTareDigits = '3'
        inputFileName = 'none'

    if inputReportAction:
        if( inputReportAction == '1' ):
           print((printOperateModeChanges(inputTareDigits)))
        elif( inputReportAction == '2' ):
           print((printSupervisorModeChanges(inputTareDigits)))
        elif( inputReportAction == '3' ):
           writeCsvOperateModeChanges(inputTareDigits, inputFileName)
        elif( inputReportAction == '4' ):
            writeCsvSupervisorModeChanges(inputTareDigits, inputFileName)
        else:
            print('Else nothing')


    sys.exit(exitCode)
