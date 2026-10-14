#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import subprocess
import string
import time
import struct
import socket
import shellCommand
import glob
from datetime import datetime
from optparse import OptionParser

# reordered per larry on 11/7/2013
# format of return string is:
#         with -P 0 option:   strength#,deviceType#
#                                0-5   ,  0=wire
#                                         1=wireless
#
#    without any -P option:    strength#,deviceType#,alive#
#                                0-5   ,  0=wire    , 0=not connected
#                                         1=wireless  1=connected
#
#

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-A", "--address",
             dest="userAddress",
             help="The IP Address, or domain name, of the machine to ping for network connectivity testing (result is first parameter alive=1 or not=0)")
p.add_option("-P", "--doAPing",
             dest="pinger",
             help="0 or 1 for Don't or Do a ping test")
p.add_option("-v", "--verbose",
             dest="verbose", default=None, action="store_true",
             help="Print detailed status")
p.add_option("-q", "--quiet",
             dest="verbose", default=None, action="store_false",
             help="No status printing (default)")
p.add_option("-F", "--forceAPing",
             dest="force",
             help="0 or 1 for Forcing a ping test")

# Parse User's CLI args
#######################
options, args = p.parse_args()

def get_default_gateway_linux():
    """Read the default gateway directly from /proc."""
    gateway = ""
    if os.name == "posix":
        with open("/proc/net/route") as fh:
            for line in fh:
                fields = line.strip().split()
                if fields[1] != '00000000' or not int(fields[3], 16) & 2:
                    continue
                return socket.inet_ntoa(struct.pack("<L", int(fields[2], 16)))
    else:
        import scaleSettingsInterface
        import ntNetworkUtils
        
        connectionType = scaleSettingsInterface.getValue('connectionType')
        import wmi
        if connectionType == "1":
            adapterName = "Wireless Network Connection"
        else:
            adapterName = "Local Area Connection"
        gateway = ntNetworkUtils.getGateway(adapterName)
        return str(gateway)

def isConnected(stub):
    #print stub
    if stub is None:
        return str('0')
    if os.name == "posix":
        #gateway = get_default_gateway_linux()
        #p = subprocess.Popen("ping -c1 -n $(route | grep default | cut -c 17-32) | awk 'BEGIN {found =0; t=\"0\"} $1 == \"round-trip\" {found=1;} END { print found }'", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
        import ping
        isOffline,stdout = ping.pingSystem(stub,"1","1")
        x = 0
        
        while int(isOffline) == 1 and x < 4 :
            isOffline,stdout = ping.pingSystem(stub,"1","1")
            x = x + 1
                            
        if int(isOffline) == 1:
            return str('0')

        elif int(isOffline) == 0:
            return str('1')    

    else:
        value = "0"
        import re
        import subprocess
        website = stub
        lost = 1
        try:
            value = "0"
            #ping = subprocess.Popen("ping -n 1 " + stub, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            #out, error = ping.communicate()
            
            cmd = []
            cmd.append("ping")
            cmd.append("-n")
            cmd.append("1")
            cmd.append(stub)
            exitCode, stdout, stderr = shellCommand.execute(cmd)

            if stdout:
                try:
                    #minimum = int(re.findall(r"Minimum = (\d+)", out)[0])
                    #maximum = int(re.findall(r"Maximum = (\d+)", out)[0])
                    #average = int(re.findall(r"Average = (\d+)", out)[0])
                    for line in stdout.splitlines():
                        if line.find("unreachable") > -1 or line.find("100% loss") > -1  or line.find("could not find host") > -1:
                            value = "0"
                            return value
                        else:
                            value = "1"
                    return value
                    #online = int(re.findall(r"host unreachable",out))
                    #print "wooz"
                    
                except:
                    return "0"
            else:
                return "0"
            
        except subprocess.CalledProcessError:
            return "0"
        return value    

def getCounts4Restart():
    #read interval for this script's calls
    pingTimingFile = "/etc/hobart/config_linux.dat"
    pingingFreq = "0"
    try:
        pingTimingFile = open(pingTimingFile, "r")
        for line in pingTimingFile:
            keyValuePair = line.split('=')
            #if keyValuePair[0] == "networkConnectivityOfflineWaitPeriodSecs":
            if keyValuePair[0] == "continuousNetworkStatusDelay":
                intervalFull = str(keyValuePair[1])
                intervalFull = intervalFull.strip()
                intervalTokens = intervalFull.split('#')
                pingingFreq = intervalTokens[0].rstrip()
                break
        dbFile.close()
    except:
        count = "0"
        pass
    #return interval

    #read ping interval
    #sqlite
    import sqlite3
    from sqlite3 import Error
	
	# Now lets get the data
    if os.name == "posix":
		# LINUX OS
        db_path = "/usr/local/hobart/db/scale/dbfiles/dbft?.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Program Files/Hobart/db/Scale/dbFiles/dbft?.db"
        
    db_file = glob.glob(db_path)
	 
    conn = ""
    try:
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)
    except Error as e:
        pass
    cur = conn.cursor()
    cur.execute("SELECT CoValue FROM configs where CoName='pingInterval'")
    rows = cur.fetchall()
    totalCounts = 0
    for row in rows:
        intervalTokens = row[0].split(':')
        hours = intervalTokens[0]
        minutes = intervalTokens[1]
        totalCounts = int(((int(minutes) + (int(hours)*60)) * 60 ) / int(pingingFreq))
    
    return totalCounts

def getCounts4RestartRequired():
    #read interval for this script's calls
    pingTimingFile = "/etc/hobart/config_linux.dat"
    pingingFreq = "0"
    try:
        pingTimingFile = open(pingTimingFile, "r")
        for line in pingTimingFile:
            keyValuePair = line.split('=')
            #if keyValuePair[0] == "networkConnectivityOfflineWaitPeriodSecs":
            if keyValuePair[0] == "continuousNetworkStatusDelay":
                intervalFull = str(keyValuePair[1])
                intervalFull = intervalFull.strip()
                intervalTokens = intervalFull.split('#')
                #print(intervalFull)
                pingingFreq = intervalTokens[0].rstrip()
                break
        dbFile.close()
    except:
        count = "0"
        pass
    #return interval

    #read ping interval
    #sqlite
    import sqlite3
    from sqlite3 import Error
	
	# Now lets get the data
    if os.name == "posix":
		# LINUX OS
        db_path = "/usr/local/hobart/db/scale/dbfiles/dbft?.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Program Files/Hobart/db/Scale/dbFiles/dbft?.db"
        
    db_file = glob.glob(db_path)
	 
    conn = ""
    try:
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)
    except Error as e:
        pass
    cur = conn.cursor()
    cur.execute("SELECT CoValue FROM configs where CoName='pingInterval'")
    rows = cur.fetchall()
    totalCounts = 0
    for row in rows:
        intervalTokens = row[0].split(':')
        hours = intervalTokens[0]
        minutes = intervalTokens[1]
        totalCounts = int(((int(minutes) + (int(hours)*60)) * 60 ) / int(pingingFreq))
    return totalCounts

def getCountsForTotalRestart():
    #read interval for this script's calls
    pingTimingFile = "/etc/hobart/config_linux.dat"
    pingingFreq = "0"
    try:
        pingTimingFile = open(pingTimingFile, "r")
        for line in pingTimingFile:
            keyValuePair = line.split('=')
            if keyValuePair[0] == "continuousNetworkStatusDelay":
                intervalFull = str(keyValuePair[1])
                intervalFull = intervalFull.strip()
                intervalTokens = intervalFull.split('#')
                pingingFreq = intervalTokens[0].rstrip()
                break
        dbFile.close()
    except:
        count = "0"
        pass
    #return interval

    #read ping interval
    #sqlite
    import sqlite3
    from sqlite3 import Error
	
	# Now lets get the data
    if os.name == "posix":
		# LINUX OS
        db_path = "/usr/local/hobart/db/scale/dbfiles/dbft?.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Program Files/Hobart/db/Scale/dbFiles/dbft?.db"
        
    #print db_path
    db_file = glob.glob(db_path)
	 
    conn = ""
    try:
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)
    except Error as e:
        pass
    cur = conn.cursor()
    cur.execute("SELECT CoValue FROM configs where CoName='pingInterval'")
    rows = cur.fetchall()
    totalCounts = 0
    for row in rows:
        intervalTokens = row[0].split(':')
        hours = intervalTokens[0]
        minutes = intervalTokens[1]
        totalCounts = int(((int(minutes) + (int(hours)*60)) * 60 ) / int(pingingFreq))
    
    return totalCounts

def getContinuousNetworkStatusDelay():
    dbFileName = "/etc/hobart/config_linux.dat"
    delay = "30"
    try:
        dbFile = open(dbFileName, "r")
        for line in dbFile:
            if line.startswith("continuousNetworkStatusDelay="):
                keyValuePair = line.split('=')
                if keyValuePair[0] == "continuousNetworkStatusDelay":
                    delay = str(keyValuePair[1]).strip()
                    break
        dbFile.close()
    except:
        delay = "30"
    if " " in delay.strip():
        delay = delay.strip().split(" ")[0]
    return str(delay)

def getCurrentCount():
    dbFileName = "/usr/local/hobart/db/settingswizard/offlineCount"
    count = 0
    try:
        dbFile = open(dbFileName, "r")
        for line in dbFile:
            keyValuePair = line.split('=')
            if keyValuePair[0] == "count":
                count = str(keyValuePair[1]).strip()
                break
        dbFile.close()
    except:
        count = "0"
    count = count
    return str(count)

def getUserGateway():
    dbFileName = "/usr/local/hobart/db/settingswizard/scalesettings"
    gateway = "0.0.0.0"
    try:
        dbFile = open(dbFileName, "r")
        for line in dbFile:
            keyValuePair = line.split('=')
            if keyValuePair[0] == "gateway":
                gateway = str(keyValuePair[1])
                break
        dbFile.close()
    except:
        gateway = "0.0.0.0"
    return str(gateway)


def adjustOfflineCount(decision):
    dbFileName = "C:\Program Files\Hobart\db\settingswizard\offlineCount"
    if os.name == "posix":
        dbFileName = "/usr/local/hobart/db/settingswizard/offlineCount"
    count = "0"
    if decision == "reset":
        try:
            import fileinput
            for line in fileinput.FileInput(dbFileName, inplace=1):
                if line.startswith('count='):
                    line = 'count=1'
                success = True
                print((line.rstrip()))
        except:
            count = "0"
            pass
    elif decision == "increment":
        try:
            import fileinput
            for line in fileinput.FileInput(dbFileName, inplace=1):
                if line.startswith('count='):
                    keyValuePair = line.split("=")
                    localCount = keyValuePair[1].strip()
                    line = 'count=' + str(int(localCount) + 1)
                    print((str(line)))
                    success = True
                break
        except:
            count = "0"
            pass


def updateOfflineCount(stub, counts4Restart):
    dbFileName = "C:\Program Files\Hobart\db\settingswizard\offlineCount"
    if os.name == "posix":
        dbFileName = "/usr/local/hobart/db/settingswizard/offlineCount"
    count = "0"
    if stub == "reset":
        try:
            import fileinput
            for line in fileinput.FileInput(dbFileName, inplace=1):
                if line.startswith('count='):
                    line = 'count=1'
                success = True
                print((line.rstrip()))
        except:
            count = "0"
            pass
    elif stub == "increment":
        try:
            import fileinput
            for line in fileinput.FileInput(dbFileName, inplace=1):
                if line.startswith('count='):
                    keyValuePair = line.split("=")
                    localCount = keyValuePair[1].strip()
                    #print("localCount is: " + localCount)
                    line = 'count=' + str(int(localCount) + 1)
                    print((str(line)))
                    success = True
                break
        except:
            count = "0"
            pass

    else:
        try:
            dbFile = open(dbFileName, "r")
            for line in dbFile:
                keyValuePair = line.split('=')
                if keyValuePair[0] == "count":
                    count = str(keyValuePair[1])
                    break
            dbFile.close()
        except:
            count = "0"
            pass
        count = str(count).rstrip()
        if ((int(counts4Restart) > 0) and (int(count) >= int(counts4Restart))):
            #restart networking
            #reset count
            try:
                #Read the contents and replace inline
                import fileinput
                for line in fileinput.FileInput(dbFileName, inplace=1):
                    if line.startswith('count='):
                        line = 'count=1'
                        count = 1
                        success = True
                    print((line.rstrip()))
                    #break
            except:
                success = False
            #restart networking
            pythonPath = os.path.join("/","usr","local","hobart","python")
            #os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"connectNetworkStartupOnce.py")))
            #write event to a logfile now
            import datetime
            now = datetime.datetime.now()
            logTime = now.strftime("%m-%d-%Y at %H:%M %p")
            with open("/usr/local/hobart/info.log", "a") as f:
                f.write("NETWORK - Stabilized on " + logTime + "\n")
           
        else:
            try:
                #Read the contents and replace inline
                import fileinput
                for line in fileinput.FileInput(dbFileName, inplace=1):
                    if line.startswith('count='):
                        line = 'count=' + str(int(count) + 1)
                        print((str(line)))
                        count = count + 1
                        success = True
                    print((line.rstrip()))
                    break
            except:
                success = False
    return (int(count))
    

def isWireless(stub):
    dbFileName = "C:\Program Files\Hobart\db\settingswizard\scalesettings"
    if os.name == "posix":
        dbFileName = "/usr/local/hobart/db/settingswizard/scalesettings"
    answer = "0"    
    try:
        dbFile = open(dbFileName, "r")
        for line in dbFile:
            keyValuePair = line.split('=')
            if keyValuePair[0] == "connectionType":
                # 0 = wire      1 = wifi
                answer = str(keyValuePair[1])
                break
        dbFile.close()
    except:
        answer = "0"
        pass
    return answer.strip()
    
def bringUpWire(stub):
    if os.name == 'posix':
        #answer = 'down'
        if str(stub) == '0':
            return str('5')

        else:
            # Wire
            p = subprocess.Popen("ifconfig eth0 up", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
            p = subprocess.Popen("cat /sys/class/net/eth0/operstate", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
            answer, _ = p.communicate()
            answer = answer.strip()
            if answer == 'up':
                p = subprocess.Popen("ifup eth0", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
            return str(answer)
    

def getConnectionName(stub):
    if os.name == 'posix':
        dbFile = open("/usr/local/hobart/db/settingswizard/scalesettings", "r")
        for line in dbFile:
            keyValuePair = line.split('=')
            if keyValuePair[0] == "connectionName":
                return str(keyValuePair[1])
        dbFile.close()
    else:
        return 'Windows ToDo'

def getCountsForASingleDivision():
    if os.name == 'posix':
        dbFile = open("/usr/local/hobart/db/settingswizard/offlineDivision", "r")
        for line in dbFile:
            keyValuePair = line.split('=')
            if keyValuePair[0] == "division":
                return str(keyValuePair[1]).strip()
        dbFile.close()
    else:
        return '7'

def isAssociatedFunc():
    answer = "1"
    cmd = []
    cmd.append("iwconfig")
    cmd.append("wlan0")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    if stdout:
        try:
            for line in stdout.splitlines():
                if line.find("Not-Associated") > -1:
                    answer = "0"
                    return answer
                elif line.find("ESSID:off/any") > -1:
                    answer = "0"
                    return answer
                else:
                    answer = "1"
        except:
            pass
    return answer

def strength(stub):
    if os.name == 'posix':
        answer = '0'
        if str(stub) == '0':
            return str('5')

        else:
            # Wireless
            p = subprocess.Popen("cat /proc/net/wireless | awk '/wlan0/ {print $3}'", shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
            answer, _ = p.communicate()
            answer = answer.strip()
            if not answer:
                return "0"
            try:
                if answer != "0":
                    answer = str(int((float(answer) / 14) + 1))
                else:
                    answer = '0'
                if float(answer) > 5:
                    answer = '5'
                return str(answer)
            except:
                return "0"
    else:
        #windows
        import ntNetworkUtils
        answer = '0'
        if str(stub) == '1':
            #wireless
            answer = ntNetworkUtils.getWlanAdapterSignalStrength(adapterName = "*")
            # in windows, it's going to have a percentage symbol so remove
            if answer:
                bars = int(answer) /  20 + 1
                if bars > 5:
                    bars = 5
                answer = str(bars)
            else:
                answer = 0
        else:
            #wire
            pass
        return answer

def getNormalOpsNetworkUISettings():
    #sqlite
    import sqlite3
    from sqlite3 import Error
	
	# Now lets get the data
    if os.name == "posix":
		# LINUX OS
        db_path = "/usr/local/hobart/db/scale/dbfiles/dbft?.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Program Files/Hobart/db/Scale/dbFiles/dbft?.db"
        
    db_file = glob.glob(db_path)
    pingPrimary = "0"
    pingGateway = "1"
    pingTarget = "0"
    pingTargetIPAddress = "0.0.0.0"
    pingPrimaryScaleIPAddress = "0.0.0.0"
    checkingNetworkConnectivityValue = "1"
	 
    conn = ""
    try:
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)
    except Error as e:
        pass
    cur = conn.cursor()
    cur.execute("SELECT CoValue FROM configs where CoName='pingPrimary'")
    rows = cur.fetchall()    
    for row in rows:
        pingPrimary = str(row[0])

    cur = conn.cursor()
    cur.execute("SELECT CoValue FROM configs where CoName='pingGateway'")
    rows = cur.fetchall()    
    for row in rows:
        pingGateway = str(row[0])

    cur = conn.cursor()
    cur.execute("SELECT CoValue FROM configs where CoName='pingTarget'")
    rows = cur.fetchall()    
    for row in rows:
        pingTarget = str(row[0])

    cur = conn.cursor()
    cur.execute("SELECT CoValue FROM configs where CoName='pingTargetIPAddress'")
    rows = cur.fetchall()    
    for row in rows:
        pingTargetIPAddress = str(row[0])

    cur = conn.cursor()
    cur.execute("SELECT CoValue FROM configs where CoName='primaryScale'")
    rows = cur.fetchall()    
    for row in rows:
        if row[0]:
            pingPrimaryScaleIPAddress = str(row[0])
        else:
            pingPrimaryScaleIPAdddress = "0.0.0.0"


    cur = conn.cursor()
    cur.execute("SELECT CoValue FROM configs where CoName='checkingNetworkConnectivity'")
    rows = cur.fetchall()    
    for row in rows:
        if row[0]:
            checkingNetworkConnectivityValue = str(row[0])
        else:
            checkingNetworkConnectivityValue = "0"

    return pingPrimary,pingGateway,pingTarget,pingTargetIPAddress,pingPrimaryScaleIPAddress,checkingNetworkConnectivityValue


def networkRestart():
    pythonPath = os.path.join("/","usr","local","hobart","python")
    os.system("/usr/bin/python %s &" % (os.path.join(pythonPath,"connectNetworkStartupOnce.py")))
    #write event to a logfile now
    import datetime
    now = datetime.datetime.now()
    logTime = now.strftime("%m-%d-%Y at %H:%M %p")
    with open("/usr/local/hobart/info.log", "a") as f:
        f.write("NETWORK-REFRESH occurred " + logTime + "\n")

if __name__ == "__main__":
    exitCode = 1
    result = ''
    alive = '0'
    wireless = '2'
    strength1 = '0'
    halt = False

    wireless = isWireless('')
    alive = "1"
    doIsConnected = True

    pingPrimary,pingGateway,pingTarget,pingTargetIPAddress,pingPrimaryScaleIPAddress,checkingNetworkConnectivityValue = getNormalOpsNetworkUISettings()

    #print("pingPrimary" + str(pingPrimary))
    #print("pingTarget" + str(pingTarget))
    #print("pingGateway" + str(pingGateway))
    #print("pingPrimaryScaleIPAddress" + str(pingPrimaryScaleIPAddress))
    #print("pingTargetIPAddress" + str(pingTargetIPAddress))

    '''
    try:
        logger = open("/usr/local/hobart/db/settingswizard/NSLog.txt", 'a+') 
        logger.write("START : " + str(datetime.now().strftime('%H:%M:%S')) + "\nIP    : " + str(options.userAddress) + "\nDoPing: " + str(options.pinger) + "\n")
        logger.write(str(sys.argv[1:]))
        logger.write("\n")
        logger.close()
    except:
        pass
    '''

    doIsConnected = True

    if str(options.force) == "1":
        checkingNetworkConnectivityValue = "1"

    if checkingNetworkConnectivityValue == "0":
        doIsConnected = True
        alive = '1'

    # this ping will report whether machine can ping
    # the configured machine in Normal Operations screen
  
    IPtoUse = "0.0.0.0"
    if int(pingGateway) == 1:
        localIP = get_default_gateway_linux()
        if localIP:
            IPtoUse = localIP
        else:
            IPtoUse = "0.0.0.0"
    elif int(pingTarget) == 1:
        IPtoUse = pingTargetIPAddress
    elif int(pingPrimary) == 1:
        IPtoUse = pingPrimaryScaleIPAddress
    elif str(options.force) == "1":
        localIP = get_default_gateway_linux()
        if localIP:
            IPtoUse = localIP
        else:
            IPtoUse = "0.0.0.0"

    if doIsConnected:
        #print("IPtoUse:" + str(IPtoUse))
        if not options.userAddress:
            if checkingNetworkConnectivityValue == "0":
                alive = '1'
            elif IPtoUse == "0.0.0.0" or IPtoUse == "":
                alive = '0'
            else:
                alive = str(isConnected(IPtoUse))
        else:
            alive = str(isConnected(options.userAddress))

    if str(alive) == '0':
        if str(wireless.strip()) == '0':
        # bring up the dead eth0
            alive = '0'
            halt = True
            exit
            
            strength1 = '0'
        else:
            #wireless
            alive = '0'
            strength1 = str(strength('1'))
            halt = True
            #if str(bringUpWire('')) == 'up':
            #    alive = '1'
            #    halt = False
            #    strength1 = '5'
            #else:
            #    alive = '0'
            #    halt = True
            #    strength1 = '0'

    if not halt:
        if str(wireless.strip()) == '0' and str(alive.strip()) == '1':
            strength1 = '5'

        elif str(wireless.strip()) == '0' and str(alive.strip()) == '0':
            strength1 = '0'

        elif str(wireless.strip()) == "1":
            strength1 = str(strength('1'))

    #OLD CODE
    #result = alive + str(',') + str(strength1) + str(',') + str(wireless)
 
    #PJR:109

    #############################################################
    #############################################################
    #=== New for James per 3.1.0 autoRestart =======

    #Defaults
    countsForUIEnteredPingInterval = "30"   #15 minutes at 30 second ping interval
    countsForADivision = "7"                #1/4 time do association check then restart if failed
    

    #if wireless.strip() == "1":
    if str(alive).strip() == "0" and wireless.strip() == "1":
        #print "should be here for wireless only"
        countsForUIEnteredPingInterval = getCountsForTotalRestart()
        portionForADivision = getCountsForASingleDivision()

        
        currentOfflineCount = getCurrentCount()
         
        #NEW

        #determine how many divisions
        wholeNumOfDivisions = 4        

        wholeNum = 1 // float(portionForADivision)
        remainder = 1 % float(portionForADivision)

        try:
            if remainder > 0:
                wholeNumOfDivisions = wholeNum + 1
            else:
                wholeNumOfDivisions = wholeNum
        except:
            pass
       
        countsForASingleDivision = int(countsForUIEnteredPingInterval / wholeNumOfDivisions)

        secondsForEveryPing = int(getContinuousNetworkStatusDelay())
        if countsForASingleDivision * secondsForEveryPing < 120:
            countsForASingleDivision = ((60.0 // float(secondsForEveryPing)) * 2.0) + 1.0
        
        for x in range(1,int(wholeNumOfDivisions)+2):
            isAssociatedText = isAssociatedFunc()
            isAssociated = False
            if isAssociatedText == "1":
                isAssociated = True
            else:
                isAssociated = False

            if int(currentOfflineCount) < int(countsForASingleDivision*x):
                #just increment
                updateOfflineCount("increment", countsForUIEnteredPingInterval)        
                #print "not on a division"
                break
            elif int(currentOfflineCount) == int(countsForASingleDivision*x):
                userGateway = getUserGateway()
                hasAGateway = True
                if len(userGateway) > 6 and userGateway != "0.0.0.0" and userGateway != "64641964":
                    hasAGateway = True
                else:
                    hasAGateway = False 
                
                if not hasAGateway:
                    adjustOfflineCount("increment")
                    networkRestart()
                else:
                    adjustOfflineCount("reset")
                    networkRestart()

                '''
                if isAssociated or not hasAGateway:
                    adjustOfflineCount("increment")
                elif hasAGateway and isAssociated:
                    adjustOfflineCount("reset")
                    networkRestart()
                elif not isAssociated and hasAGateway:
                    adjustOfflineCount("reset")
                    networkRestart()    
                '''

                break
            elif int(currentOfflineCount) == int(countsForUIEnteredPingInterval):
                hasGateway = False
                localGateway = get_default_gateway_linux()
                if localGateway:
                    if len(localGateway):
                        hasGateway = True
                    else:
                        hasGateway = False
                else:
                    hasGateway = False


                if hasGateway and isAssociated:
                    #do we really need to restart after a ping tothe gateway?
                    #maybe someone fat-fingered the target IP
                    # or maybe the primary is not reachable
                    localAlive = str(isConnected(localGateway))
                    if localAlive == "0":                
                        networkRestart()
                else:
                    networkRestart()

                adjustOfflineCount("reset")    
                break
            elif int(currentOfflineCount) > int(countsForUIEnteredPingInterval):
                adjustOfflineCount("reset")
                networkRestart()
                break
            #print("################################")
             

        ###########   END NEW CODE ##########################
        #####################################################
        #####################################################
    elif str(alive).strip() == "1" and wireless.strip() == "1":
        currentCount = getCurrentCount()
        #print "resetting please" + str(currentCount)
        if int(currentCount.strip()) > 1:
            #reset the counts to zero
            #NEW
            updateOfflineCount("reset", countsForUIEnteredPingInterval) 
    elif wireless.strip() == "0":
        #print "WIRED!!!"
        if getCurrentCount().strip() != "1":
            updateOfflineCount("reset", countsForUIEnteredPingInterval)
       
    #END PJR:109

    if doIsConnected:
        result = str(strength1) + str(',') + str(wireless.strip()) + str(',') + str(alive.rstrip())
    else:
        result = str(strength1) + str(',') + str(wireless.rstrip())
    print((str(result).rstrip()))
    '''
    try:
        logger = open("/usr/local/hobart/db/settingswizard/NSLog.txt", 'a') 
        logger.write(str(result).rstrip() + "\n-------------\n")
        logger.write("\n")
        logger.close()
    except:
        print "failed B"
        pass
    '''

    sys.exit(0)


