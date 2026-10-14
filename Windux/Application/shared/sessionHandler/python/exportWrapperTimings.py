#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sqlite3
from sqlite3 import Error
import sql
import os
import sys
import glob
import csv
import re
import sys, os
import getIpAddress
from datetime import datetime, timezone

TESTDATA = False
TESTINGCSV = False
MAX_TIMINGS = 18
MAX_TRAY_ELEMENTS = 6
ZERO_START = 0


def exportWrapperTimings(filename, priVertPosition, secVertPosition, sealerBeltTemp):
    status = 0
    exempt_list = [1,11,12,13,17,18,19]                         # these are timings we do not care about. Phased out. We are missing 2P 19,20

    scale_results             = getScaleDatabase(priVertPosition, secVertPosition, sealerBeltTemp)
    wrapper_tray_results      = getWrapperTrayData()            # 6
    wrapper_timings_results   = getWrappeTimingsData()          # 190
    wrapper_audit_log_results = getWrappeAuditLogData()         # 36
    
    combined_tray_timings = []
    new_row = ''
    
    #2
    if len(wrapper_timings_results) < 1 or len(wrapper_timings_results[0]) < 1:
        print('No timings_results records found')
        status = 2
    else:
        # validate both lists of data
        for i in range(0, len(wrapper_timings_results)):
            # each loop through will add the set of Tray Size attributes with its 18 timings, as a single row in the combined_list

            row_data = wrapper_timings_results[i]           # single row of timing results 0-18
            tray_name = row_data[0]                         # tray_name = tray it belongs, eg 25H
            tray_size = row_data[1]                         # tray_size = size, 30 = 3 inch, 4 = 4 inch, etc
            timing_index = int(row_data[2])                 # timing_index = 0-18 (and hopefully up to 20)
            timing_value = row_data[3]                      # timing_value the timing at that index - these 18 values are what we want to add to 1 single row

            if( timing_index in exempt_list ):
                # means we dont display that specific timing value. It must be Deprecated

                if( timing_index == MAX_TIMINGS ):
                    # means the last value in the list is deprecated too - so lets finish off the list
                    combined_tray_timings.append(new_row)       # bc we are done adding elements
                    new_row = ''

                continue

            elif( timing_index == 0 ):
                # must have a new row of timings, so prepend the row with the tray info 
            
                # find tray_name in wrapper_tray_results. After loop, we have new row with tray_name
                for j in range(0, len(wrapper_tray_results)):
                    tray_row = wrapper_tray_results[j]
        
                    if( tray_name == tray_row[0] ):
                        tmpElem = str(tray_row[0]) + '-' + str(tray_size/10)

                        # add the tray size attributes and the first[0] wrapper timing attribute 
                        new_row = [ tmpElem, str(tray_row[1]/10.0), str(tray_row[2]/10), str(tray_row[3]/10), str(tray_row[4]/10), str(tray_row[5]/10), str(timing_value) ]
                        break

            elif( ( timing_index > 0 ) and ( timing_index <= MAX_TIMINGS )  ):
                # add latest wrapper timing attribute
                new_row.append(str(timing_value))

                if( timing_index == MAX_TIMINGS ):
                    # oh this is the last attribute to add, so add the entire row to the combined list
                    combined_tray_timings.append(new_row)
                    new_row = ''

    #3 add the audit timings to the combined list, but they will have to be cleaned up first.
    for k in range(0, len(wrapper_audit_log_results) ):             
        row_data = wrapper_audit_log_results[k]
        import datetime                                                         # see if we can move this up and out

        if( len(row_data) == 2 ): 

            epoch_time = round(row_data[0]/1000)
            pre_data = row_data[1]
            local_datetime = datetime.datetime.fromtimestamp(epoch_time)
            stripped_data = re.sub(r"[\n\t]*", "", pre_data)                    # removed newlines and tabs
            index = stripped_data.find('ORDER:')                                # FINDS the word ORDER
            if index != -1:  # Check if the word was found
                stripped_data = stripped_data[:index]                           # removed the word ORDER and the tray info that follows
            stripped_data = re.sub(r'\s+', ' ', stripped_data)                  # strips all spaces
            stripped_list = stripped_data.split('HEIGHT')                       # split on the word HEIGHT into a list of 3
            
            if( len(stripped_list) == 1 ):
                # just a description in list - no timings data - add it as is
                new_row = [ str(local_datetime), stripped_list ]
                combined_tray_timings.append(new_row)

            elif( len(stripped_list) == 3 ):
                # one single tray was modified, so one tray and 2 tray sizes like 3.0 and 6.0
                new_row = [ str(local_datetime), stripped_list[0] ]             # tray name and attributes
                combined_tray_timings.append(new_row)
                new_row = [ ' ', 'HEIGHT', stripped_list[1] ]                   # timing attributes for first tray name (3.0 inch) 
                combined_tray_timings.append(new_row)
                new_row = [ ' ', 'HEIGHT', stripped_list[2] ]                   # timing attributes for second trayn ame (6.0 inch) 
                combined_tray_timings.append(new_row)

            elif( len(stripped_list) > 3 ):
                # ok so this means we have MORE than a single tray, so probably an import happened, or reset defaults, resulting in multiple trays
                tray_list = stripped_data.split('TRAY')                         # 1: split on the trays first                               
                new_row = [ str(local_datetime), tray_list[0] ]                 # 2: first item, [0], is always a description of what happened, 
                combined_tray_timings.append(new_row)                           # 3: Film Widths were modified, so automatically just add it
                
                for m in range(1, len(tray_list) ):                             # 4: dont know how many trays we will have, so split on trays
                    height_list = tray_list[m].split('HEIGHT')                  # 5: and then split on heights under tray 
                    
                    new_row = [ str(local_datetime), "TRAY ", height_list[0] ]
                    combined_tray_timings.append(new_row)
                        
                    for n in range(1, len(height_list) ):                       # 6: bc we can have multiple heights
                        new_row = [ ' ', 'HEIGHT', height_list[1] ]
                        combined_tray_timings.append(new_row)

    #1 add passed in and dfftx values 
    if( len(scale_results) > 1):
        #print('scale_results: ' + str( scale_results ) )
        for p in range(0, len(scale_results) ):
            combined_tray_timings.append(scale_results[p])
    else:
        print('No Config results')
        
    if len(wrapper_tray_results) < 1 or len(wrapper_tray_results[0]) < 1:
        print('No tray_results records found')
        status = 1

    if len(wrapper_timings_results) < 1 or len(wrapper_timings_results[0]) < 1:
        print('No timings_results records found')
        status = 2

    if len(wrapper_audit_log_results) < 1 or len(wrapper_audit_log_results[0]) < 1:
        print('No audit log records found')
        status = 4
    
    if len(scale_results) < 1 or len(scale_results[0]) < 1:
        print('No audit log records found')
        status = 8

    writeCsvFileToDisc(filename, combined_tray_timings, True)

    return status
    
    
    
def getWrapperTrayData():
    # Now lets get the wrapper timings data

    if os.name == "posix":
        db_path = "/usr/local/hobart/db/diagnostics/diag.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Hobart/db/diagnostics/diag.db"
            
    import glob
    db_file = glob.glob(db_path)
    conn = ""
    results = []
    
    try:
        # WORKS on windows, fails in buildserver
        #conn = sqlite3.connect(f"file:{dbDiagFileName}?mode=ro", uri=True)
        
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)

        cursor = conn.cursor()

        selectCommand = "SELECT name, lengthMin, lengthMax, widthMin, widthMax, lip FROM wrappertrays"

        cursor.execute(selectCommand)
        results = cursor.fetchall()

    except sqlite3.OperationalError as e:
        print("Error connecting to database: {e}")
    except Exception as e:
        print("An unexpected error occurred: {e}")
    finally:
        if 'conn' in locals() and conn:
            conn.close()

    return results

def getWrappeAuditLogData():
    # Now lets get the wrapper audit log data

    if os.name == "posix":
        db_path = "/usr/local/hobart/db/diagnostics/diag.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Hobart/db/diagnostics/diag.db"
            
    import glob
    db_file = glob.glob(db_path)
    conn = ""
    results = []
    
    try:
        # WORKS on windows, fails in buildserver
        #conn = sqlite3.connect(f"file:{dbDiagFileName}?mode=ro", uri=True)
        
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)
        
        cursor = conn.cursor()

        selectCommand = "SELECT timestamp, description FROM wrapperauditlog"

        cursor.execute(selectCommand)
        results = cursor.fetchall()

    except sqlite3.OperationalError as e:
        print("Error connecting to database: {e}")
    except Exception as e:
        print("An unexpected error occurred: {e}")
    finally:
        if 'conn' in locals() and conn:
            conn.close()

    return results
 
def getWrappeTimingsData():
    # Now lets get the wrapper timings data

    if os.name == "posix":
        db_path = "/usr/local/hobart/db/diagnostics/diag.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Hobart/db/diagnostics/diag.db"
            
    import glob
    db_file = glob.glob(db_path)
    conn = ""
    results = []
    
    try:
        # WORKS on windows, fails in buildserver
        #conn = sqlite3.connect(f"file:{dbDiagFileName}?mode=ro", uri=True)
        
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)
        
        cursor = conn.cursor()

        selectCommand = "SELECT name, height, param, paramValue FROM wrappertimings"

        cursor.execute(selectCommand)
        results = cursor.fetchall()

    except sqlite3.OperationalError as e:
        print("Error connecting to database: {e}")
    except Exception as e:
        print("An unexpected error occurred: {e}")
    finally:
        if 'conn' in locals() and conn:
            conn.close()

    return results

def writeCsvFileToDisc(filename, timings_data, append_ip_address=True):
    csvPath  = ''
    extType = '.csv'
        
    try:
        exit, ipAddress = getIpAddress.getIpAddress()
        if exit:
            raise Exception
        #print(("IP: " + ipAddress))
    except:
        print("Failed to get IP")
        pass

    if append_ip_address:
        ipAddress = ipAddress.replace('.', '_')
        csvPath = filename + '_' + ipAddress + extType

    else:
        csvPath = filename + extType

    headerList = []
    headerList.append( ['Tray Name', 'Length Minimum', 'Length Maximum', 'Width Minimum', 'Weight Maximum', 'Lip Maximum', 'Side to side pre-stretch', 'Side to side wrap tension', 'Pkg position on elevator', 'Secondary label offset', 'Front to back wrap tension', 'Primary label offset', 'Wrap speed', 'Discharge speed', 'Slide folder travel', 'Gripper relax', 'Film width', 'Read Underfolder travel', 'Film pull length', '(MIA) Secondary label offset' ] )

    with open(csvPath, "w+") as csvFile:
        csvwriter = csv.writer(csvFile)
        csvwriter.writerows(headerList)
        csvwriter.writerows(timings_data)
    
def getScaleDatabase(priVertPosition, secVertPosition, sealerBeltTemp):
    # Now lets get the data

    results = []

    if os.name == "posix":
		# LINUX OS
        db_path = "/usr/local/hobart/db/scale/dbfiles/dbft?.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Hobart/db/scale/dbfiles/dbft?.db"
    db_file = glob.glob(db_path)

    horizLabelSkew1 = '0'
    horizLabelSkew2 = '0'
    defaultFilmWidth = '0'
    defaultFilmTension = '0'
    defaultFilmTensionFrontToBack = '0'
    defaultFilmTensionLeftToRight = '0'
    defaultIntakeMode = '0'
    defaultLabelPlacement = '0'
    secondaryDefaultLabelPlacement = "0"
    defaultApplierRotation = '0'
    secondaryDefaultApplierRotation = "0"

    conn = ""
    try:
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='horizLabelSkew1'")
        rows = cur.fetchall() 
        for row in rows:
            horizLabelSkew1 = str(row[0])

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='horizLabelSkew2'")
        rows = cur.fetchall()  
        for row in rows:
            horizLabelSkew2 = str(row[0])

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='defaultFilmWidth'")
        rows = cur.fetchall()
        for row in rows:
            defaultFilmWidth = str(row[0])

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='defaultFilmTension'")
        rows = cur.fetchall()
        for row in rows:
            defaultFilmTension = str(row[0])

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='defaultFilmTensionFrontToBack'")
        rows = cur.fetchall()
        for row in rows:
            defaultFilmTensionFrontToBack = str(row[0])

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='defaultFilmTensionLeftToRight'")
        rows = cur.fetchall()
        for row in rows:
            defaultFilmTensionLeftToRight = str(row[0])

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='defaultIntakeMode'")
        rows = cur.fetchall()
        for row in rows:
            defaultIntakeMode = str(row[0])

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='defaultLabelPlacement'")
        rows = cur.fetchall()
        for row in rows:
            defaultLabelPlacement = str(row[0])

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='secondary defaultLabelPlacement'")
        rows = cur.fetchall()
        for row in rows:
            secondaryDefaultLabelPlacement = str(row[0])

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='defaultApplierRotation'")
        rows = cur.fetchall()
        if row in rows:
            defaultApplierRotation = str(row[0])

        cur = conn.cursor()
        cur.execute("SELECT CoValue FROM configs where CoName='secondary defaultApplierRotation'")
        rows = cur.fetchall()
        if row in rows:
            secondaryDefaultApplierRotation = str(row[0])

        #print('horizLabelSkew1: ' + horizLabelSkew1)
        #print('horizLabelSkew2: ' + horizLabelSkew2)
        #print('defaultFilmWidth: ' + defaultFilmWidth)
        #print('defaultFilmTension: ' + defaultFilmTension)
        #print('defaultFilmTensionFrontToBack: ' + defaultFilmTensionFrontToBack)
        #print('defaultFilmTensionLeftToRight: ' + defaultFilmTensionLeftToRight)
        #print('defaultIntakeMode: ' + defaultIntakeMode)
        #print('defaultLabelPlacement: ' + defaultLabelPlacement)
        #print('secondaryDefaultLabelPlacement: ' + secondaryDefaultLabelPlacement)
        #print('defaultApplierRotation: ' + defaultApplierRotation)
        #print('secondaryDefaultApplierRotation: ' + secondaryDefaultApplierRotation)
        #print('priVertPosition: ' + priVertPosition)
        #print('secVertPosition: ' + secVertPosition)
        #print('sealerBeltTemp: ' + sealerBeltTemp)

    except sqlite3.OperationalError as e:
        print("Error connecting to database: {e}")
    except Exception as e:
        print("An unexpected error occurred: {e}")
    finally:
        if 'conn' in locals() and conn:
            conn.close()

    intake_dict = {
        "1": "Gentle",
        "2": "Normal"
    }

    tension_dict = {
        "0": "Low",
        "1": "Medium"
    }

    placement_dict = {
        "0": "Upper Right",
        "1": "Upper Left",
        "2": "None",
        "3": "Lower Right",
        "4": "Lower Left",
        "5": "Right of Primary",
        "6": "Left of Primary",
        "7": "Above Primary",
        "8": "Below Primary"
    }

    defaultIntakeMode               = intake_dict.get(defaultIntakeMode)
    defaultFilmTension              = tension_dict.get(defaultFilmTension)
    defaultLabelPlacement           = placement_dict.get(defaultLabelPlacement)
    secondaryDefaultLabelPlacement  = placement_dict.get(secondaryDefaultLabelPlacement)

    #print('Updated defaultIntakeMode: ' + defaultIntakeMode)
    #print('Updated defaultFilmTension: ' + defaultFilmTension)
    #print('Updated defaultLabelPlacement: ' + defaultLabelPlacement)
    #print('Updated secondaryDefaultLabelPlacement: ' + secondaryDefaultLabelPlacement)

    new_row = [ "#############################", "#############################", "#############################", "#############################", "#############################", "#############################", "#############################", "#############################" ]
    results.append(new_row)
    new_row = [ "WRAPPER", "SETTINGS" ]
    results.append(new_row)
    new_row = [ "Primary horizontal label placement", horizLabelSkew1 ]
    results.append(new_row)
    new_row = [ "Secondary horizontal label placement", horizLabelSkew2 ]
    results.append(new_row)
    new_row = [ "Primary vertical label placement", priVertPosition ]
    results.append(new_row)
    new_row = [ "Primary vertical label placement", secVertPosition ]
    results.append(new_row)
    new_row = [ "Default film width", defaultFilmWidth ]
    results.append(new_row)
    new_row = [ "Default film tension", defaultFilmTension ]
    results.append(new_row)
    new_row = [ "Default film tension front to back", defaultFilmTensionFrontToBack ]
    results.append(new_row)
    new_row = [ "Default film tension left to right", defaultFilmTensionLeftToRight ]
    results.append(new_row)
    new_row = [ "Default intake mode", defaultIntakeMode ]
    results.append(new_row)
    new_row = [ "Primary default label placement", defaultLabelPlacement ]
    results.append(new_row)
    new_row = [ "Secondary default label placement", secondaryDefaultLabelPlacement ]
    results.append(new_row)
    new_row = [ "Primary default label applier rotation", defaultApplierRotation ]
    results.append(new_row)
    new_row = [ "Secondary default label applier totation", secondaryDefaultApplierRotation ]
    results.append(new_row)
    new_row = [ "Sealer belt temperature", sealerBeltTemp ]
    results.append(new_row)
    
    return results


if __name__=='__main__':

    exitCode = "0"
    
    inputFileName = ""
    inputPriVertPosition = "0"
    inputSecVertPosition = "0"
    inputSealerBeltTemp = "0"

    argv1 = sys.argv[1:]

    if len(argv1) >= 4:
       inputFileName        = argv1[0]
       inputPriVertPosition = argv1[1]
       inputSecVertPosition = argv1[2]
       inputSealerBeltTemp  = argv1[3]
       
    status = exportWrapperTimings(inputFileName, inputPriVertPosition, inputSecVertPosition, inputSealerBeltTemp)

    sys.exit(exitCode)