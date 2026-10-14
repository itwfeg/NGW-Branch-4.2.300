import osUtils
import sys

if __name__ == '__main__':
    if osUtils.is64BitOs():
        sys.exit(0)
    else:
        sys.exit(1)
