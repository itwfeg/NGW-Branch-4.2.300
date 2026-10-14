import sys
import os
import zipfile
import glob
from datetime import datetime
from optparse import OptionParser
from getSerialFlashInfo import getScaleSerialNumber

# collects all logs and zips them into destination
def logCollector(destination):
    serialNumber = getScaleSerialNumber()

    now = datetime.now()
    timestamp = now.strftime("%m%d%Y_%H%M%S")

    zipname = timestamp + "-" + serialNumber + "-logDump.zip"
    if destination is not None:
        if destination[-1] == "/":
            zf = zipfile.ZipFile(destination + zipname, mode="w")
        else:
            zf = zipfile.ZipFile(destination + "/" + zipname, mode="w")
    else:
        zf = zipfile.ZipFile("/usr/local/hobart/" + zipname, mode="w")

    try:
        # add files to zip
        logs = glob.glob("/usr/local/hobart/*.log")
        logs += glob.glob("/tmp/*.log")
        logs += glob.glob("/tmp/*.txt")

        for i in logs:
            zf.write(i)
    finally:
        zf.close()

if __name__ == "__main__":
    p = OptionParser()

    p.add_option("-d", "--destination",
                 dest="destination",
                 help="Full path of directory where to save zip file of logs")
    
    options, args = p.parse_args()
    exitCode = 0
    logCollector(options.destination)