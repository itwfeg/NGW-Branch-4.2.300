#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import time

class boardTest:
    def __init__(self, mountPoint,  testName):
        self.errorLog   = None
        self.logLocation = '/tmp'
        self.testName = testName
        self.testData = ""
        self.mountPoint = mountPoint
        self.testFilename =  os.path.join(self.mountPoint, "newFile")
        kilobyte=1024
        for c in range(0,kilobyte):
            self.testData = "%s%d" % (self.testData,c)
        self.testData = "%s\n" % (self.testData)
        self.initializeLog()

    def writeTestFile(self, fname):
        try:
            f = open(fname, "w")
            f.write(self.testData)
            f.close()
            return True
        except:
            return False

    def readTestFile(self, fname):
        try:
            f = open(fname, "r")
            data = f.read()
            f.close()
            return data
        except:
            return ""

    def compareTestFiles(self, fname):
        data = self.readTestFile(fname)
        if data == self.testData:
            return True
        else:
            return False

    def touch(self, fname):
        if os.path.exists(fname):
            os.utime(fname,  None)
        else:
            open(fname, 'a').close()

    def removeFile(self, fname):
        try:
            os.remove(fname)
            exitCode = True
        except:
            exitCode = False
        return exitCode
        
    def log(self, s):
        if (self.errorLog is not None):
            self.errorLog.write(s.encode('utf-8'))

    def initializeLog(self):
        if sys.platform.startswith("linux"):
            #create debug log file
            self.errorLog = open(os.path.join(self.logLocation,  self.testName  + "-log.txt"),  "wb")
            self.log("log opened: " + time.asctime( time.localtime(time.time()) ) + "\n")
            return True
        else:
            self.log("OS type not supported: " +  sys.platform)
            self.log("log closed\n")
            return False

    def performReadWriteTest(self):
        if sys.platform.startswith("linux"):
            if os.path.exists(self.mountPoint):
                if not os.path.exists(self.testFilename):
                    self.touch(self.testFilename)
                    if os.path.exists(self.testFilename):
                        self.log("SATA drive file newFile created.\n")
                        exitCode = self.removeFile(self.testFilename)
                        if not os.path.exists(self.testFilename):
                            self.log("SATA drive file newFile removed.\n")
                            exitCode = exitCode and self.writeTestFile(os.path.join(os.sep, self.mountPoint,"test1.dat"))
                            exitCode = exitCode and self.compareTestFiles(os.path.join(os.sep, self.mountPoint, "test1.dat"))
                            exitCode = exitCode and self.removeFile(os.path.join(os.sep, self.mountPoint, "test1.dat"))
                            return exitCode
                        else:
                            self.log("SATA drive file newFile could not be removed.\n")
                            return False
                    else:
                        self.log("SATA drive file newFile could not be created.\n")
                        return False
                else:
                    self.log("SATA drive file newFile already exists.\n")
                    return False
            else:
                self.log("SATA drive not mounted.\n")
                return False
        else:
            self.log("OS type not supported: " +  sys.platform)
            self.log("log closed\n")
            return False

    def stop(self):
        self.errorLog.close()
        return True

def main():
    exitCode1 = 0
    exitCode2 = 0
    exitCode3 = 0
	
    result = ''

    #------------------------------------------------------------------------------------
    # SOF-896: Added SSD tab back into Board Tester ... only Second uSD slot, not SATADOM
    #------------------------------------------------------------------------------------
    # boardSataTest = boardTest(os.path.join(os.sep, "media", "sda1"),  'satatest')
    # exitCode1 = boardSataTest.performReadWriteTest()
    # exitCode1 = exitCode1 and boardSataTest.stop()

    # if not exitCode1:
        # exitCode1 = 1
        # result += 'FAIL'
    # else:
        # exitCode1 = 0
        # result += 'PASS'

    # result += '|'

    boardPrimaryUsdTest = boardTest(os.path.join(os.sep, "media", "mmcblk0p1"), 'primaryusdtest')
    exitCode2 = boardPrimaryUsdTest.performReadWriteTest()
    exitCode2 = exitCode2 and boardPrimaryUsdTest.stop()

    if not exitCode2:
        exitCode2 = 1
        result += 'FAIL'
    else:
        exitCode2 = 0
        result += 'PASS'

    result += '|'

    boardSecondUsdTest = boardTest(os.path.join(os.sep, "media", "mmcblk1p1"), 'secondusdtest')
    exitCode3 = boardSecondUsdTest.performReadWriteTest()
    exitCode3 = exitCode3 and boardSecondUsdTest.stop()

    if not exitCode3:
        exitCode3 = 1
        result += 'FAIL'
    else:
        exitCode3 = 0
        result += 'PASS'

    print(result)

    if not exitCode1 and not exitCode2 and not exitCode3:
        sys.exit(0)
    else:
        sys.exit(1)

if __name__ == "__main__":
    main()

