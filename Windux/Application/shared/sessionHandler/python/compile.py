#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# compiles file given on the command line

import sys, py_compile, os, stat

def main(argv1):
    print ("\n\n                Hobart Compiler\n")
    print ("        The following python script was compiled:\n")
    py_compile.compile(argv1 + '.py')
    os.chmod(argv1 + '.pyc',stat.S_IRWXU)
    print(("          " + argv1 + ".pyc\n\n"))

if __name__=='__main__':
    main(sys.argv[1])

