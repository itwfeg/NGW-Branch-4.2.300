import volumeInfo
import sys
import file_io
import osUtils
import os
import getMacAddress

CSID_TAG = 'COMPUTING SYSTEM ID'


def getCSInfo(macAddress=None):
    exitCode, volumeSerialNumber = volumeInfo.getVolumeInfo("C:\\")

    if not exitCode == 0:
        print('ERROR: Can not get C: drive info!')
        return -1, ''

    if not macAddress:
        results = getMacAddress.getMacAddress('wire')

        exitCode = results[0]  # type: int
        macAddress = results[1]  # type: str

        if not exitCode == 0 or not macAddress:
            exitCode, macAddress = getMacAddress.getMacAddress('wireless')

            if not exitCode == 0 or not macAddress:
                return -1, ''

    csdata = macAddress.upper().replace(":", "") + volumeSerialNumber.upper()
    return exitCode, csdata


def extractCSInfo(csdata):
    return csdata[:12], csdata[12:]


def generateCSIDFile(filename):
    status = 'error'
    data = None

    try:
        exitCode, csdata = getCSInfo()

        if exitCode == 0:
            status,  data = file_io.writeToFile(filename=filename, data = ['%s=%s' % (CSID_TAG, csdata)])
    except:
        pass

    return status, data


def getCSIdFromFileOld(filename):
    try:
        status,  data = file_io.readFileIntoBuff(filename, 'r')
        tokens = data.split("=")
        return tokens[1].strip()
    except:
        return None


def getCSIdFromFile(filename):
    try:
        status,  data = file_io.readFromFile(filename)
        for i in xrange(0, len(data)):
            tokens = data[i].split("=")
            if len(tokens) == 2:
                if tokens[0].strip() == CSID_TAG:
                    return tokens[1].strip()
        return None
    except:
        return None


if __name__ == "__main__":
    exitCode_, csdata_ = getCSInfo()

    if exitCode_ == 0:
        print('HTe Computing System ID Generator')
        print('Please supply the COMPUTING SYSTEM ID below to Hobart for licensse key generation:')
        print('COMPUTING SYSTEM ID = %s' % csdata_)
        print('The file named csid.txt created in the current directory should be sent to HT.support@itwfeg.com')
        print('to create product licenses for this computing system')
        frozen,  applicationPath = osUtils.applicationPath()
        if not frozen:
            applicationPath = os.getcwd()

        status_, data_ = generateCSIDFile(os.path.join(applicationPath, 'csid.txt'))

        if status_ == 'success':
            print('The file csid.txt was written successfully')
    else:
        print('Can not generate COMPUTING SYSTEM ID, please contact support at HT.support@itwfeg.com for more info')

    sys.exit(exitCode_)
