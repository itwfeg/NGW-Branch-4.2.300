#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import tarfile

def extractall(tar_file, extractdir):
    tar = tarfile.open(tar_file, 'r')
    try:
        tar.extractall(extractdir)
    except:
        pass
    finally:
        tar.close()

if __name__ == '__main__':
    if len(sys.argv) == 3:
        extractall(tar_file=sys.argv[1], extractdir=sys.argv[2])
