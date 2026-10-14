import os
if os.name == 'nt':
    import win32api
import sys

def getVolumeInfo(drive="C:\\"):
    if os.name == 'nt':
        hexser = None
        try:
            x = win32api.GetVolumeInformation(drive)
            volser = int(x[1])
            if volser < 0:
                volser = 4294967296 - abs(volser)
            hexser = hex(volser).strip('L').upper()[2:]
            while len(hexser) < 8:
                hexser = '0' + hexser
            exitCode = 0
        except:
            exitCode = 1
        return (exitCode, hexser)
    else:
        # TODO:
        # Need linux equivalent
        return(1, None)

if __name__ == "__main__":
    exitCode, serialNumber = getVolumeInfo(sys.argv[1])
    if exitCode == 0:
        print('Serial # for drive %s is %s' % (sys.argv[1], serialNumber))
    else:
        print('Error finding serial number')
    sys.exit(exitCode)
