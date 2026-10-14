import bincopy
import sys
import file_io
import string

def computeChecksum(line):
    index = 2
    total = 0
    while index < len(line):
        decimalByte = int(line[index:index+2], 16)
        total = total + decimalByte
        index = index + 2

    totalHex = '%X' % total
    leastSignificantByte = totalHex[-2:]
    cksum = '%X' % (int(leastSignificantByte, 16) ^ 0xFF)
    return cksum

# print computeChecksum('S1137AF00A0A0D00000000000000000000000000')

f = bincopy.BinFile()
status,  lines = file_io.readFromFile(sys.argv[1])
if status == 'success':
    newLines = []
    for line in lines:
        line = string.strip(line)
        if line[:2] == 'S3':
            lengthOfLine = len(line)
            bytesOfData = (lengthOfLine / 2) - 6
            newLine = line[:2] + '%X' % (bytesOfData) + line[4:]
            newLine = newLine # + computeChecksum(newLine)
        else:
            newLine = line
        newLines.append(string.strip(newLine))

    status,  data = file_io.writeToFile(sys.argv[1] + 'fixed',  newLines)
    if status == 'success':
        f.add_srec_file(sys.argv[1] + 'fixed',overwrite=True)
        print(f.as_hexdump())
