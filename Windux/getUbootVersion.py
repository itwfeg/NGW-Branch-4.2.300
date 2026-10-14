import argparse
import file_io
import string
import re

def lineMatchesPattern(match, line):
    pattern = re.compile(match)
    if pattern.match(line):
        return line
    else:
        return None

def getUbootVersion(filename):
    match= '^EXTRAVERSION\s*=\s*[a-z, 0-9, \-, \.]+$'
    status, data = file_io.readFromFile(filename)
    if status == 'success':
        for i in xrange(0, len(data)):
            if lineMatchesPattern(match, data[i]) != None:
                # get uboot version out of line
                tokens = string.split(data[i], '-')
                if len(tokens) == 4:
                    ubootVersion = tokens[3]
                    return ubootVersion
        return 'None'
    else:
        return 'None'

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="get current version of uboot")
    parser.add_argument("filename", action="store", help="full path to uboot makefile")

    args = parser.parse_args()

    try:
        ubootVersion = getUbootVersion(args.filename)
        print ubootVersion,
    except:
        print 'None',
