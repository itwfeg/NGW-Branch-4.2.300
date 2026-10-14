import osUtils
import sys

if __name__ == '__main__':
    match = '^!define OS_TYPE \".+\"$'
    if osUtils.is64BitPython():
        nameValuePair = '!define OS_TYPE "win64"'
    else:
        nameValuePair = '!define OS_TYPE "win32"'
        
    status = osUtils.assignVariableInFile(sys.argv[1], match, nameValuePair)
