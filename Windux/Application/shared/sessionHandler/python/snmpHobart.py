# something's broken

from pysnmp.carrier.asyncore.dispatch import AsyncoreDispatcher
from pysnmp.carrier.asyncore.dgram import udp, udp6, unix
from pyasn1.codec.ber import encoder, decoder
from pysnmp.proto import api
import time, bisect
import sys
import os
import shellCommand
import platform
import socket
import re
import signal


ITW_FEG_PEN = 51478
def _open_persistent_hobart_socket():
    persistent_socket_address = ('127.0.0.1', 6000)
    persistent_connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    persistent_connection.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    persistent_connection.settimeout(20)
    persistent_connection.connect(persistent_socket_address)
    return persistent_connection


def getScaleSettingsValue(key):
    try:
        with open('/usr/local/hobart/db/settingswizard/scalesettings') as f:
            for line in f:
                line = line.rstrip()
                if key in line:
                    tokens = line.split('=')
                    try:
                        return tokens[1].strip()
                    except:
                        pass
    except:
        pass
    return ''

def getChecksum(key):
    try:
        with open('/usr/local/hobart/checksums.txt') as f:
            for line in f:
                line = line.rstrip()
                if key in line:
                    tokens = line.split('=')
                    try:
                        return tokens[1].strip()
                    except:
                        pass
    except:
        pass
    return ''

try:
    ITW_FEG_PORT = int(getScaleSettingsValue("snmpVariablesPort"))
except:
    ITW_FEG_PORT = 161

runSNMP = False
try:
    doIt = getScaleSettingsValue("snmpEnabled")
except:
    doIt = "0"
if doIt == "1":
    runSNMP = True


############################
#### HELPER FUNCTIONS
############################

def _send_persistent_hobart_message(SOCKET, message, getReplyBack, receive):
    response = ""

    #print("SEND : " + message[6:])
    SOCKET.sendall(message if type(message) == bytes else message.encode())
    if getReplyBack:
        # allow up to 2 exceptions and 16 responses before we bail
        exceptions = 0
        iter = 0
        while receive not in response and exceptions < 2 and iter < 16:
            try:
                #print '  recv...'
                response += SOCKET.recv(1024)
                #print '  recv=' + response
            except socket.timeout as err:
                #print '  recv timeout!'
                #SOF-1688: dont return failed it will hang loop logic
                #response = "failed"
                exceptions += 1
            except:
                exceptions += 1
            iter += 1
    return response

def chobart_dTalk(SOCKET, send, receive):
    msgTokens = send.split(',')
    response_code = 'nothing'
    #register as a remote session
    msg = ''
    headlen = 6
    snDev = str(msgTokens[0]) + ','
    snEvt = str(msgTokens[1]) + ','
    snDevEvt = snDev + snEvt
    body = snDevEvt
    body += ','.join(msgTokens[2:-1])
    body = body[:-1]
    header = str(len(body) + headlen).zfill(headlen)
    msg = header + body

    response_code = _send_persistent_hobart_message(SOCKET, msg, True, receive)
    #if send in "7,106":
    #print("RAW RESPONSE:" + str(response_code))

    results = ""
    foundIt = False
    while len(response_code) > 0  and not foundIt:
        #SOF-1688: message validation
        if len(response_code) < 6:
            break
        stripped = response_code[:6].lstrip('0')
        try:
            length = int(stripped)
        except:
            length = 0

        #SOF-1688: message validation
        if len(response_code) < length:
            break
        mess = response_code[6:length]

        tokens = mess.split(',')
        if receive in mess:
            #this is the message we're looking for
            #print "XXXXXXXXXXXFOUND ITXXXXXXXXX : " + mess
            foundIt = True
            results = mess

        response_code = response_code[length:len(response_code)]

    #print("RESPONSE:" + results)
    return results

def countRowsFromDB(table):
    answer = ""
    import sqlite3
    from sqlite3 import Error
    if os.name == "posix":
        db_path = "/usr/local/hobart/db/scale/dbfiles/dbft?.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Program Files/Hobart/db/Scale/dbFiles/dbft?.db"
    import glob
    db_file = glob.glob(db_path)
    values = []
    conn = ""
    try:
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)
    except Error as e:
        print("db error")
        pass
    cur = conn.cursor()
    cur.execute("SELECT COUNT(*) AS total FROM " + table)
    rows = cur.fetchall()
    for row in rows:
        answer = str(row[0])
    return str(answer)

def selectFromDB(table, column, columnCondition):
    import sqlite3
    from sqlite3 import Error
    result = []
    if os.name == "posix":
        db_path = "/usr/local/hobart/db/scale/dbfiles/dbft?.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Program Files/Hobart/db/Scale/dbFiles/dbft?.db"
    import glob
    db_file = glob.glob(db_path)
    values = []
    conn = ""
    try:
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)
        cur = conn.cursor()
        if columnCondition == None:
            cur.execute("SELECT " + column + " FROM " + table)
        else:
            cur.execute("SELECT " + column + " FROM " + table + " where " + columnCondition)
        result = cur.fetchall()
    except Error as e:
        print("db error")
        pass
    return result

def deleteRecalls():
    import sqlite3
    from sqlite3 import Error
    result = []
    if os.name == "posix":
        db_path = "/usr/local/hobart/db/scale/dbfiles/dbft?.db"
    else:
        # WINDOWS OSs
        db_path = "C:/Program Files/Hobart/db/Scale/dbFiles/dbft?.db"
    import glob
    db_file = glob.glob(db_path)
    values = []
    conn = ""
    try:
        db_file = db_file[0]
        conn = sqlite3.connect(db_file)
        cur = conn.cursor()
        cur.execute("DELETE FROM RECALLS")
        result = cur.fetchall()
    except Error as e:
        print("db error")
        pass
    return result
#deleteRecalls()

def watchBackend():
    pass

#######################
# END HELPER FUNCTIONS
#######################

#######################
# GET RESPONSES
#######################

def dateAndTime():
    import datetime
    humanTime =  datetime.datetime.now().strftime("%m/%d/%Y %H:%M")
    return (str(humanTime))

def utcTime():
    import datetime
    utcNow = datetime.datetime.utcnow()
    machineTime = str(float(utcNow.strftime("%s.%f")))
    return (str(machineTime))

def connectionStatus():
    status = "Offline"
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getConnectionStatus.py")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    tokens = stdout.strip().split(",")
    #print(str(len(tokens)))
    if len(tokens) > 2:
        if tokens[2] == "1":
            status = "Online"
    else:
        status = "Offline"
    return status

def numOfPLUs():
    return countRowsFromDB("Prods")

def currentDept():
    result = ""
    currentDept = selectFromDB("Configs", "CoValue", "CoName='mostRecentActiveDeptNum'")
    for deptNum in currentDept:
        result += str(deptNum[0])
    return result

def hostname():
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getHostname.py")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    return stdout.strip()

def allDeptNumbers():
    result = ""
    allDeptNums = selectFromDB("Depts", "DeptNum", None)
    for deptNum in allDeptNums:
        result += str(deptNum[0]) + "/"
    return result[:-1]

def labelWidth():
    #return "80"
    # per James Meyer on Feb 9 2018 after snmp meeting, jsut report 2.25" width
    #print "-----------------------------------------------------------\nLABEL WIDTH"
    send = "7,32"
    receive = "7,41"
    answer = ''
    #register a session-type 10
    SOCKET = _open_persistent_hobart_socket()
    reg_msg = '000014100,1,10'
    SOCKET.sendall(reg_msg.encode())

    results = chobart_dTalk(SOCKET, send, receive)
    #print("RECEIVE:" + results)
    SOCKET.close()
    #print("PARSE RESULTS")
    if "," in results:
        tokens = results.split(",")
        answer = tokens[5].strip('"')
    else:
        answer = "999.999"
    #print("LABEL WIDTH:" + str(answer))
    return str(answer)

def labelLength():

    #------------------------------------------------------------------
    # Shamelessly stolen from Printer.h and PrinterStockMetrics.cpp ...
    #------------------------------------------------------------------

    # LabelLengthLines
    UnknownLabelLengthLines = 0
    Length1_000_Lines = 203
    Length1_500_Lines = 304
    Length1_750_Lines = 355
    Length2_000_Lines = 406
    Length2_375_Lines = 482
    Length2_500_Lines = 508
    Length3_000_Lines = 609
    Length3_500_Lines = 711
    Length4_000_Lines = 812
    Length4_500_Lines = 914
    Length5_000_Lines = 1016
    Length5_500_Lines = 1118
    Length6_000_Lines = 1219
    Length6_500_Lines = 1320
    Length7_000_Lines = 1422
    Length7_500_Lines = 1524
    Length8_000_Lines = 1625
    Length8_500_Lines = 1727
    Length9_000_Lines = 1829
    Length9_500_Lines = 1930
    Length10_000_Lines = 2032
    LengthContinuous_Lines = 4999

    # LabelLength
    UnknownLabelLength = "0"
    Length1_000 = "1.000"
    Length1_500 = "1.500"
    Length1_750 = "1.750"
    Length2_000 = "2.000"
    Length2_375 = "2.375"
    Length2_500 = "2.500"
    Length3_000 = "3.000"
    Length3_500 = "3.500"
    Length4_000 = "4.000"
    Length4_500 = "4.500"
    Length5_000 = "5.000"
    Length5_500 = "5.500"
    Length6_000 = "6.000"
    Length6_500 = "6.500"
    Length7_000 = "7.000"
    Length7_500 = "7.500"
    Length8_000 = "8.000"
    Length8_500 = "8.500"
    Length9_000 = "9.000"
    Length9_500 = "9.500"
    Length10_000 = "10.000"

    result = ""
    rows = selectFromDB("Configs", "CoValue", "CoName='printer1LabelLength'")

    for row in rows:
        result += row[0]

    numLines = int( result )
    labelLengthResult = UnknownLabelLength

    if ((0 < numLines) and (numLines <= Length1_000_Lines)):
        labelLengthResult = Length1_000

    elif ((Length1_000_Lines < numLines) and (numLines <= Length1_500_Lines)):
        labelLengthResult = Length1_500

    elif ((Length1_500_Lines < numLines) and (numLines <= Length1_750_Lines)):
        labelLengthResult = Length1_750

    elif ((Length1_750_Lines < numLines) and (numLines <= Length2_000_Lines)):
        labelLengthResult = Length2_000

    elif ((Length2_000_Lines < numLines) and (numLines <= Length2_375_Lines)):
        labelLengthResult = Length2_375

    elif ((Length2_375_Lines < numLines) and (numLines <= Length2_500_Lines)):
        labelLengthResult = Length2_500

    elif ((Length2_500_Lines < numLines) and (numLines <= Length3_000_Lines)):
        labelLengthResult = Length3_000

    elif ((Length3_000_Lines < numLines) and (numLines <= Length3_500_Lines)):
        labelLengthResult = Length3_500

    elif ((Length3_500_Lines < numLines) and (numLines <= Length4_000_Lines)):
        labelLengthResult = Length4_000

    elif ((Length4_000_Lines < numLines) and (numLines <= Length4_500_Lines)):
        labelLengthResult = Length4_500

    elif ((Length4_500_Lines < numLines) and (numLines <= Length5_000_Lines)):
        labelLengthResult = Length5_000

    elif ((Length5_000_Lines < numLines) and (numLines <= Length5_500_Lines)):
        labelLengthResult = Length5_500

    elif ((Length5_500_Lines < numLines) and (numLines <= Length6_000_Lines)):
        labelLengthResult = Length6_000

    elif ((Length6_000_Lines < numLines) and (numLines <= Length6_500_Lines)):
        labelLengthResult = Length6_500

    elif ((Length6_500_Lines < numLines) and (numLines <= Length7_000_Lines)):
        labelLengthResult = Length7_000

    elif ((Length7_000_Lines < numLines) and (numLines <= Length7_500_Lines)):
        labelLengthResult = Length7_500

    elif ((Length7_500_Lines < numLines) and (numLines <= Length8_000_Lines)):
        labelLengthResult = Length8_000

    elif ((Length8_000_Lines < numLines) and (numLines <= Length8_500_Lines)):
        labelLengthResult = Length8_500

    elif ((Length8_500_Lines < numLines) and (numLines <= Length9_000_Lines)):
        labelLengthResult = Length9_000

    elif ((Length9_000_Lines < numLines) and (numLines <= Length9_500_Lines)):
        labelLengthResult = Length9_500

    elif ((Length9_500_Lines < numLines) and (numLines <= Length10_000_Lines)):
        labelLengthResult = Length10_000

    elif ( LengthContinuous_Lines <= numLines ):
        labelLengthResult = "continuous"

    return labelLengthResult

def labelType():

    result = "n/a"
    LengthContinuous_Lines = 4999

    rows = selectFromDB("Configs", "CoValue", "CoName='printer1LabelLength'")
    for row in rows:
        result = str(row[0])

    lines = result

    if int( lines ) >= LengthContinuous_Lines:
        result = "continuous"
    else:
        result = "die-cut"

    return result


def testing():
    try:
        print("VV")
        #doesUserPingDefaultGateway = selectFromDB("configs", "CoValue"
        gateway = selectFromDB("Configs", "CoValue", "CoName='pingTargetIPAddress'")
        print(gateway)
    except:
        print("ERROR : Scale Status")
        pass

def gateway():
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getGateway.py")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    return stdout.strip()

def iPAddress():
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getIpAddress.py")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    return stdout.strip()

def wiredMACAddress():
    result = "n/a"
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getMacAddress.py")
    exitCode,stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.strip()
    tokens = stdout.split("|")
    if len(tokens[0]):
        result = str(tokens[0])
    return result

def wifiMACAddress():
    result = "n/a"
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getMacAddress.py")
    exitCode,stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.strip()
    tokens = stdout.split("|")
    if len(tokens[1]):
        result = str(tokens[1])
    return result

def mfgDate():
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getSerialFlashInfo.py")
    cmd.append("--mfg")
    exitCode,stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.strip()
    tokens = stdout.split(",")
    return str(tokens[0].strip())

def serialNumber():
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getSerialFlashInfo.py")
    cmd.append("--mfg")
    exitCode,stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.strip()
    tokens = stdout.split(",")
    return str(tokens[1].strip())

def model():
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getSerialFlashInfo.py")
    cmd.append("--mfg")
    exitCode,stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.strip()
    tokens = stdout.split(",")
    return str(tokens[2].strip())

def systemControllerVersion():
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getSerialFlashInfo.py")
    cmd.append("--mfg")
    exitCode,stdout, stderr = shellCommand.execute(cmd)
    stdout = stdout.strip()
    tokens = stdout.split(",")
    return str(tokens[4].strip())

def smBackendVersion():
    cmd = []
    cmd.append("/usr/bin/python")
    cmd.append("/usr/local/hobart/python/getSmBackendVersion.py")
    cmd.append("-X") # purposely an incorrent path to get local location
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    return stdout.strip()

def softwareVersion():
    result = "n/a"
    try:
        fileHandle = '/SOFTWARE_VERSION_INFO'
        with open(fileHandle, 'r') as f:
            lines = f.readlines()
        for line in lines:
            if 'Release ID:' in line:
                lineTokens = line.split(':')
                result = lineTokens[1].strip()

    except:
        result = "n/a"

    return str(result)

def signalStrength():

    strength = 'n/a'
    nic = getScaleSettingsValue('connectionType')
    if nic == '1':
        try:
            cmd = []
            cmd.append('iwconfig')
            cmd.append('wlan0')
            exitCode, stdout, stderr = shellCommand.execute(cmd)
            for line in stdout.splitlines():
                if line.find("Link Quality") > -1:
                    strength = line.lstrip()
                    strength = strength.rstrip()
        except:
            pass
    return strength

def bandwidth():
    cmd = []
    cmd.append('/usr/bin/python')
    cmd.append('/usr/local/hobart/python/getNetworkInfo.py')
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    tokens = stdout.split(',')
    return str(tokens[6].strip())

def dnsServers():
    dns1 = getScaleSettingsValue('dns1')
    dns2 = getScaleSettingsValue('dns2')
    dns3 = getScaleSettingsValue('dns3')
    result = ""
    if len(dns1) > 3:
        result += dns1
    if len(dns2) > 3:
        result += " " + dns2
    if len(dns3) > 3:
        result += " " + dns3
    return str(result)

def lastCommOccurred():
    result = ""
    lastCommOccurred = selectFromDB("Configs", "CoValue", "CoName='communicationHistory'")
    for event in lastCommOccurred:
        result += str(event[0])
    return result

def nicType():
    nic = 'n/a'
    try:
        nic = getScaleSettingsValue('connectionType')
        if nic == '1':
            nice = 'wireless'
        elif nic == '0':
            nic = 'wired'
    except:
        pass
    return str(nic)

def lastPLUReceived():
    result = "n/a"
    lastCommOccurred = selectFromDB("Configs", "CoValue", "CoName='communicationItemReceived'")
    for event in lastCommOccurred:
        result = str(event[0])
    if len(result) < 7:
        result = "never"
    return str(result)

def peripheralControllerVersion():
    result = "n/a"
    rows = selectFromDB("Configs", "CoValue", "CoName='FPCVersion'")
    for row in rows:
        result = str(row[0])
    return str(result)

def osVersion():
    return platform.uname()[2]

def dhcpServerIP():
    result = "n/a"
    isStatic = getScaleSettingsValue('isStatic')
    if isStatic == '0':
        import subprocess
        cmd = subprocess.Popen('/sbin/dhcpcd -T', shell=True, stdout=subprocess.PIPE)
        for line in cmd.stdout:
            line = line.rstrip()
            if "new_dhcp_server_identifier" in line:
                tokens = line.split('=')
                return str(tokens[1]).replace("'", "")
    else:
        return result

def ipAddressType():
    result = "n/a"
    isStatic = getScaleSettingsValue('isStatic')
    if isStatic == '0':
        return "DHCP"
    elif isStatic == '1':
        return "Static"
    else:
        return result

def ntpServerIP():
    result = "n/a"
    answer = getScaleSettingsValue('timeKeeper')
    if answer == 'NTP':
        ntpserverip = getScaleSettingsValue('ntpServer')
        return ntpserverip
    else:
        return answer

def freeMemory():
    import subprocess
    cmd = subprocess.Popen('/usr/bin/free -h', shell=True, stdout=subprocess.PIPE)
    for line in cmd.stdout:
        line = line.rstrip()
        if "Mem:" in line:
            #print line
            tokens = re.split(r' +',line)

            return str(tokens[3]).strip()

def printerStatus():
    HOST = '127.0.0.1'  # smBackend server's hostname or IP address. localhost in this case.
    PORT = 5076         # smBackend port

    # Create a TCP socket
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        logFunc('Failed to create socket: {}'.format(e))

    # Connect to the local smBackend socket
    try:
        sock.connect((HOST, PORT))
    except socket.error as e:
        logFunc('Failed to connect to socket: {}'.format(e))

    # Send a getPrinterStatus command to smBackend
    try:
        message = 'getPrinterStatus|\n'
        sock.sendall(message.encode())
    except socket.error as e:
        logFunc('Failed to send data: {}'.format(e))

    # Wait for a response from the smBackend to return data.
    try:
        returnedData = sock.recv(1024)
    except socket.error as e:
        logFunc('Failed to receive data: {}'.format(e))

    # Close the socket
    sock.close()

    try:
        printerStatus = returnedData.decode().strip().split(':')[-1]
    except Exception as e:
        logFunc('Failed to parse printer status: {}'.format(e))
        printerStatus = 'Error parsing printer status'

    return str(printerStatus)



def generalDiscoveryName():
    return 'HOBART HT Series Device / HTi scale'

def checksumConfig():
    result = ""
    result = currentDept()
    result += "|" + getChecksum('download/configuration_data.ht')
    return str(result)

def checksumLabels():
    result = ""
    result = currentDept()
    result += "|" + getChecksum('download/label_data.ht')
    return str(result)

def checksumFlashkeys():
    result = ""
    result = currentDept()
    result += "|" + getChecksum('download/flashkey_data.ht')
    return str(result)

def checksumOperators():
    result = ""
    result = currentDept()
    result += "|" + getChecksum('download/operator_data.ht')
    return str(result)

def checksumStore():
    result = ""
    result = currentDept()
    result += "|" + getChecksum('download/store_data.ht')
    return str(result)

def checksumSynchronization():
    result = ""
    result = currentDept()
    result += "|" + getChecksum('download/synchronization_data.ht')
    return str(result)

def checksumPlaylists():
    result = ""
    result = currentDept()
    result += "|" + getChecksum('playlists/playlist.txt')
    return str(result)

def checksumPLUs():
    result = ""
    result = currentDept()
    result += "|" + getChecksum('download/plu_data.ht')
    return str(result)

def secondCheckDigit():
    result = "unchecked"
    rows = selectFromDB("Configs", "CoValue", "CoName='secondCheckDigit'")
    for row in rows:
        result = str(row[0])
    if result == "0":
        result = "unchecked"
    elif result == "1":
        result = "checked"
    return result

def WeigherConfig():
    result = ""

    rows = selectFromDB("Configs","CoValue","CoName='XXX'")
    for row in rows:
        result = str(row[0])
    return result

def scaleMode():
    result = ""
    rows = selectFromDB("Configs","CoValue","CoName='mostRecentRunMode'")
    for row in rows:
        result = str(row[0])
    if result == "5":
        result += "|Manual"
    elif result == "13":
        result += "|Self Service"
    elif result == "14":
        result += "|Prepack"
    return result

def currentWeight():
    #print "-----------------------------------------------------------\nCURRENT WEIGHT"

    send = "1,35"
    receive = "1,4"
    answer = ''
    #register a session-type 10
    SOCKET = _open_persistent_hobart_socket()
    reg_msg = '000014100,1,10'
    SOCKET.sendall(reg_msg.encode())

    results = chobart_dTalk(SOCKET, send, receive)
    SOCKET.close()
    #print("RECEIVE: " + str(results))
    if "," in results:
        tokens = results.split(",")
        answer = tokens[2].strip('"')
    else:
        answer = "999.999"
    #print(str(answer))
    #print("CURRENT WEIGHT:" + str(answer))
    return str(answer)

def remoteAccessEnabled():
    result = ""
    rows = selectFromDB("Configs","CoValue","CoName='remoteAccessPermitted'")
    for row in rows:
        result = str(row[0])
    if result == "0":
        result += "|disabled"
    elif result == "1":
        result += "|enabled"
    return result


#########################
# END GET RESPONSES
#########################

def logFunc(f):
    shouldPrint = False
    if (shouldPrint):
        print(f)

class sysDescr(object):
    name = (1, 3, 6, 1, 2, 1, 1, 1, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('sysDescr')
        return api.protoModules[protoVer].OctetString(
            'ITW Food Equipment Group - HT Device' + model() + ' - ' + osVersion()
        )

class SystemDescription(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 1, 1, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('SystemDescription')
        return api.protoModules[protoVer].OctetString(
            'HT device'
        )

class ConnectionStatus(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 1, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('ConnectionStatus')
        return api.protoModules[protoVer].OctetString(
            str(connectionStatus())
        )

class Hostname(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 2, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('Hostname')
        return api.protoModules[protoVer].OctetString(
            str(hostname())
        )

class WiredMACAddress(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 3, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('WiredMACAddress')
        return api.protoModules[protoVer].OctetString(
            str(wiredMACAddress())
        )

class WifiMACAddress(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 4, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('WifiMACAddress')
        return api.protoModules[protoVer].OctetString(
            str(wifiMACAddress())
        )

class IPAddress(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 5, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('IPAddress')
        return api.protoModules[protoVer].OctetString(
            str(iPAddress())
        )

class Gateway(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 6, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('Gateway')
        return api.protoModules[protoVer].OctetString(
            str(gateway())
        )

class SignalStrength(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 7, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('SignalStrength')
        return api.protoModules[protoVer].OctetString(
            str(signalStrength())
        )

class DNSServers(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 8, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('DNSServers')
        return api.protoModules[protoVer].OctetString(
            str(dnsServers())
        )

class NICType(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 9, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('NICType')
        return api.protoModules[protoVer].OctetString(
            str(nicType())
        )

class DHCPServerIP(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 10, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('DHCPServerIP')
        return api.protoModules[protoVer].OctetString(
            str(dhcpServerIP())
        )

class IPAddressType(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 11, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('IPAddressType')
        return api.protoModules[protoVer].OctetString(
            str(ipAddressType())
        )

class NTPServerIP(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 12, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('NTPServerIP')
        return api.protoModules[protoVer].OctetString(
            str(ntpServerIP())
        )

class Bandwidth(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 2, 13, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('Bandwidth')
        return api.protoModules[protoVer].OctetString(
            str(bandwidth())
        )

class DateAndTime(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 3, 1, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('DateAndTime')
        return api.protoModules[protoVer].OctetString(
            str(dateAndTime())
        )

class UTCTime(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 3, 2, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('UTCTime')
        return api.protoModules[protoVer].OctetString(
            str(utcTime())
        )

class MfgDate(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 3, 3, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('MfgDate')
        return api.protoModules[protoVer].OctetString(
            str(mfgDate())
        )

class SerialNumber(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 3, 4, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('SerialNumber')
        return api.protoModules[protoVer].OctetString(
            str(serialNumber())
        )

class Model(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 3, 5, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('Model')
        return api.protoModules[protoVer].OctetString(
            str(model())
        )

class SystemControllerVersion(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 3, 6, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('SystemControllerVersion')
        return api.protoModules[protoVer].OctetString(
            str(systemControllerVersion())
        )

class SMBackendVersion(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 3, 7, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('SMBackendVersion')
        return api.protoModules[protoVer].OctetString(
            str(smBackendVersion())
        )

class SoftwareVersion(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 3, 8, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('SoftwareVersion')
        return api.protoModules[protoVer].OctetString(
            str(softwareVersion())
        )

class PeripheralControllerVersion(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 3, 9, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('PeripheralControllerVersion')
        return api.protoModules[protoVer].OctetString(
            str(peripheralControllerVersion())
        )

class OSVersion(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 3, 10, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('OSVersion')
        return api.protoModules[protoVer].OctetString(
            str(osVersion())
        )


class SNMPUptime(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 1, 0)
    birthday = time.time()

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('SNMPUptime')
        return api.protoModules[protoVer].TimeTicks(
            (time.time() - self.birthday) * 100
        )

class LastCommunicationOccurred(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 2, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('LastCommunicationOccurred')
        return api.protoModules[protoVer].OctetString(
            str(lastCommOccurred())
        )

class LastPLUReceived(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 3, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('LastPLUReceived')
        return api.protoModules[protoVer].OctetString(
            str(lastPLUReceived())
        )

class FreeMemory(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 4, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('FreeMemory')
        return api.protoModules[protoVer].OctetString(
            str(freeMemory())
        )

class PrinterStatus(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 5, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('PrinterStatus')
        return api.protoModules[protoVer].OctetString(
            str(printerStatus())
        )

class ChecksumConfig(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 6, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('ChecksumConfig')
        return api.protoModules[protoVer].OctetString(
            str(checksumConfig())
        )

class ChecksumLabels(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 7, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('ChecksumLabels')
        return api.protoModules[protoVer].OctetString(
            str(checksumLabels())
        )

class ChecksumFlashkeys(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 8, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('ChecksumFlashkeys')
        return api.protoModules[protoVer].OctetString(
            str(checksumFlashkeys())
        )

class ChecksumOperators(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 9, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('ChecksumOperators')
        return api.protoModules[protoVer].OctetString(
            str(checksumOperators())
        )

class ChecksumStore(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 10, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('ChecksumStore')
        return api.protoModules[protoVer].OctetString(
            str(checksumStore())
        )

class ChecksumSynchronization(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 11, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('ChecksumSynchronization')
        return api.protoModules[protoVer].OctetString(
            str(checksumSynchronization())
        )

class ChecksumPlaylists(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 12, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('ChecksumPlaylists')
        return api.protoModules[protoVer].OctetString(
            str(checksumPlaylists())
        )

class ChecksumPLUs(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 13, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('ChecksumPLUs')
        return api.protoModules[protoVer].OctetString(
            str(checksumPLUs())
        )

class SecondCheckDigit(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 14, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('SecondCheckDigit')
        return api.protoModules[protoVer].OctetString(
            str(secondCheckDigit())
        )

class ScaleMode(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 15, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('ScaleMode')
        return api.protoModules[protoVer].OctetString(
            str(scaleMode())
        )

class CurrentWeight(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 16, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('CurrentWeight')
        return api.protoModules[protoVer].OctetString(
            str(currentWeight())
        )

class RemoteAccessEnabled(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 4, 17, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('RemoteAccessEnabled')
        return api.protoModules[protoVer].OctetString(
            str(remoteAccessEnabled())
        )

class TotalPLUs(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 5, 1, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('TotalPLUs')
        return api.protoModules[protoVer].Integer(
            str(numOfPLUs())
        )

class AllDepts(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 6, 1, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('AllDepts')
        return api.protoModules[protoVer].OctetString(
            allDeptNumbers()
        )

class CurrentDept(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 6, 2, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('CurrentDept')
        return api.protoModules[protoVer].Integer(
            str(currentDept())
        )

class LabelType(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 7, 1, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('LabelType')
        return api.protoModules[protoVer].OctetString(
            str(labelType())
        )

class LabelLength(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 7, 2, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('LabelLength')
        return api.protoModules[protoVer].OctetString(
            str(labelLength())
        )

class LabelWidth(object):
    name = (1, 3, 6, 1, 4, 1, ITW_FEG_PEN, 7, 3, 0)

    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('LabelWidth')
        return api.protoModules[protoVer].OctetString(
            str(labelWidth())
        )

class GeneralDiscoveryName(object):
    name = (1, 3, 6, 1, 2, 1, 1, 1, 0)
    def __eq__(self, other): return self.name == other

    def __ne__(self, other): return self.name != other

    def __lt__(self, other): return self.name < other

    def __le__(self, other): return self.name <= other

    def __gt__(self, other): return self.name > other

    def __ge__(self, other): return self.name >= other

    def __call__(self, protoVer):
        logFunc('GeneralDiscoveryName')
        return api.protoModules[protoVer].OctetString(
            str(generalDiscoveryName())
        )


mibInstr = (
    SystemDescription(),
    ConnectionStatus(), Hostname(), WiredMACAddress(), WifiMACAddress(), IPAddress(), Gateway(), SignalStrength(), DNSServers(), NICType(), DHCPServerIP(), IPAddressType(), NTPServerIP(), Bandwidth(),
    DateAndTime(), UTCTime(), MfgDate(), SerialNumber(), Model(), SystemControllerVersion(), SMBackendVersion(), SoftwareVersion(), PeripheralControllerVersion(), OSVersion(), CurrentWeight(), RemoteAccessEnabled(),
    SNMPUptime(), LastCommunicationOccurred(), LastPLUReceived(), FreeMemory(), PrinterStatus(), ChecksumConfig(), ChecksumLabels(), ChecksumFlashkeys(), ChecksumOperators(), ChecksumStore(), ChecksumSynchronization(), ChecksumPlaylists(), ChecksumPLUs(), SecondCheckDigit(), ScaleMode(),CurrentWeight(),
    TotalPLUs(),
    AllDepts(), CurrentDept(),
    LabelType(), LabelLength(), LabelWidth(),
    GeneralDiscoveryName()
    #ScaleConnected()
 # sorted by OID
)

mibInstrIdx = {}
for mibVar in mibInstr:
    mibInstrIdx[mibVar.name] = mibVar

def cbFun(transportDispatcher, transportDomain, transportAddress, wholeMsg):
    # prevent high CPU usage
    if not wholeMsg:
        sleep(0.1)
        return wholeMsg
    # process request(s)
    while wholeMsg:
        msgVer = api.decodeMessageVersion(wholeMsg)
        if msgVer in api.protoModules:
            pMod = api.protoModules[msgVer]
        else:
            print(('Unsupported SNMP version %s' % msgVer))
            return
        reqMsg, wholeMsg = decoder.decode(
            wholeMsg, asn1Spec=pMod.Message(),
        )
        rspMsg = pMod.apiMessage.getResponse(reqMsg)
        rspPDU = pMod.apiMessage.getPDU(rspMsg)
        reqPDU = pMod.apiMessage.getPDU(reqMsg)
        varBinds = []
        pendingErrors = []
        errorIndex = 0
        # GETNEXT PDU
        if reqPDU.isSameTypeWith(pMod.GetNextRequestPDU()):
            # Produce response var-binds
            for oid, val in pMod.apiPDU.getVarBinds(reqPDU):
                errorIndex = errorIndex + 1
                # Search next OID to report
                nextIdx = bisect.bisect(mibInstr, oid)
                if nextIdx == len(mibInstr):
                    # Out of MIB
                    varBinds.append((oid, val))
                    pendingErrors.append(
                        (pMod.apiPDU.setEndOfMibError, errorIndex)
                    )
                else:
                    # Report value if OID is found
                    varBinds.append(
                        (mibInstr[nextIdx].name, mibInstr[nextIdx](msgVer))
                    )
        elif reqPDU.isSameTypeWith(pMod.GetRequestPDU()):
            for oid, val in pMod.apiPDU.getVarBinds(reqPDU):
                if oid in mibInstrIdx:
                    varBinds.append((oid, mibInstrIdx[oid](msgVer)))
                else:
                    # No such instance
                    varBinds.append((oid, val))
                    pendingErrors.append(
                        (pMod.apiPDU.setNoSuchInstanceError, errorIndex)
                    )
                    break
        else:
            # Report unsupported request type
            pMod.apiPDU.setErrorStatus(rspPDU, 'genErr')
        pMod.apiPDU.setVarBinds(rspPDU, varBinds)
        # Commit possible error indices to response PDU
        for f, i in pendingErrors:
            f(rspPDU, i)
        transportDispatcher.sendMessage(
            encoder.encode(rspMsg), transportDomain, transportAddress
        )
    return wholeMsg

while runSNMP:
    print("SNMP Agent Running")
    transportDispatcher = AsyncoreDispatcher()
    transportDispatcher.registerRecvCbFun(cbFun)
    # UDP/IPv4
    transportDispatcher.registerTransport(
         udp.domainName, udp.UdpSocketTransport().openServerMode((iPAddress(), ITW_FEG_PORT))
    )
    # UDP/IPv6
    transportDispatcher.registerTransport(
        udp6.domainName, udp6.Udp6SocketTransport().openServerMode(('::1', ITW_FEG_PORT))
    )
    ## Local domain socket
    # transportDispatcher.registerTransport(
    #    unix.domainName, unix.UnixSocketTransport().openServerMode('/tmp/snmp-agent')
    # )
    transportDispatcher.jobStarted(1)
    try:
        # Dispatcher will never finish as job#1 never reaches zero
        transportDispatcher.runDispatcher()
    except:
        print("SNMP Agent Exception!")
        import traceback
        traceback.print_exc()
    finally:
        #SOCKET.close()
        transportDispatcher.jobFinished(1)
        #transportDispatcher.unregisterRecvCbFun(recevId=None)
        transportDispatcher.unregisterRecvCbFun()
        transportDispatcher.unregisterTransport(udp.domainName)
        transportDispatcher.unregisterTransport(udp6.domainName)
        transportDispatcher.closeDispatcher()

