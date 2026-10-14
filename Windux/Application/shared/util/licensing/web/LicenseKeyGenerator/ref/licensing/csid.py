import volumeInfo
import getMacAddress
import sys
import string
import file_io
import osUtils
import os

CSID_TAG = 'COMPUTING SYSTEM ID'

def getCSInfo():
    csdata = None
    exitCode, volumeSerialNumber = volumeInfo.getVolumeInfo("C:\\")
    if exitCode == 0:
        exitCode, macAddress = getMacAddress.getMacAddress('wire')
        if exitCode == 0 and macAddress:
            csdata = '%s%s' % (string.upper(string.replace(macAddress, ':', '')), string.upper(volumeSerialNumber))
        else:
            exitCode, macAddress = getMacAddress.getMacAddress('wireless')
            if exitCode == 0 and macAddress:
                csdata = '%s%s' % (string.upper(string.replace(macAddress, ':', '')), string.upper(volumeSerialNumber))
            else:
                print 'Error: Cannot find a network interface device, cannot generate CSID'
    else:
        print 'ERROR: Can not get C: drive info'
    return exitCode,  csdata

def extractCSInfo(csdata):
    return (csdata[:12], csdata[12:])

def generateCsIdFile(filename):
    status = 'error'
    data = None

    try:
        exitCode, csdata = getCSInfo()
        if exitCode == 0:
            status,  data = file_io.writeToFile(filename=filename, data = ['%s=%s' % (CSID_TAG, csdata)])
    except:
        pass

    return (status, data)

def getCSIdFromFileOld(filename):
    try:
        status,  data = file_io.readFileIntoBuff(filename, 'r')
        tokens = string.split(data, "=")
        return string.strip(tokens[1])
    except:
        return None

def getCSIdFromFile(filename):
    try:
        status,  data = file_io.readFromFile(filename)
        for i in xrange(0, len(data)):
            tokens = string.split(data[i], "=")
            if len(tokens) == 2:
                if string.strip(tokens[0]) == CSID_TAG:
                    return string.strip(tokens[1])
        return None
    except:
        return None

if __name__ == "__main__":
    exitCode, csdata = getCSInfo()
    if exitCode == 0:
        print 'HTe Computing System ID Generator'
        print 'Please supply the COMPUTING SYSTEM ID below to Hobart for licensse key generation:'
        print 'COMPUTING SYSTEM ID = %s' % (csdata)
        print 'The file named csid.txt created in the current directory should be sent to HT.support@itwfeg.com'
        print 'to create product licenses for this computing system'
        frozen,  applicationPath = osUtils.applicationPath()
        if not frozen:
            applicationPath = os.getcwd()
        status,  data = generateCsIdFile(os.path.join(applicationPath, 'csid.txt'))
        if status == 'success':
            print 'The file csid.txt written successfully'
    else:
        print 'Can not generate COMPUTING SYSTEM ID, please contact support at HT.support@itwfeg.com for more info'

    #macaddress,  serialnumber = extractCSInfo(csdata)
    #print macaddress
    #print serialnumber

    sys.exit(exitCode)
