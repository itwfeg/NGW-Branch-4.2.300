import os
import sys
import time
from datetime import datetime

kilobyte=1024
numFiles = 20

output=""
for c in xrange(0,kilobyte):
    output = "%s%d" % (output,c)
output = "%s\n" % (output)

for d in xrange(0, 1000):

    totalTime = 0
    for i in xrange(1,numFiles):
        fileSize = i * kilobyte
        filename = "data%d.txt" % i
        #fileStartTime = time.time()
        fileStartTime = datetime.now()
        f = open(filename, "w")
        f.write(fileSize * output)
        f.close()
        #fileWriteTime = time.time() - fileStartTime
        fileWriteTime = datetime.now() - fileStartTime
        totalTime = totalTime + fileWriteTime.microseconds
        print "File %d of size %d write time elapsed is %d us" % (i, fileSize, fileWriteTime.microseconds)

    print "Time to write %d files with file step of 1K bytes per file is %d us" % (numFiles, totalTime)

    totalTime = 0
    for i in xrange(1,numFiles):
        fileSize = i * kilobyte
        filename = "data%d.txt" % i
        #fileStartTime = time.time()
        fileStartTime = datetime.now()
        f = open(filename, "r")
        f.readlines()
        f.close()
        fileReadTime = datetime.now() - fileStartTime
        totalTime = totalTime + fileReadTime.microseconds
        print "File %d of size %d read time elapsed is %d us" % (i, fileSize, fileReadTime.microseconds)

    print "Time to read %d files with file step of 1K bytes per file is %d us" % (numFiles, totalTime)

    print "removing created files"
    totalRemoveTime = 0
    for i in xrange(1,numFiles):
        filename = "data%d.txt" % i
        #removeStartTime = time.time()
        removeStartTime = datetime.now()
        os.remove(filename)
        removeTime = datetime.now() - removeStartTime
        totalRemoveTime = totalRemoveTime + removeTime.microseconds

    print "Time to remove files %d us" % (totalRemoveTime)
