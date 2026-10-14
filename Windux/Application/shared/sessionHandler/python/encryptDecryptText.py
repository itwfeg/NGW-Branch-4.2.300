#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# not tested in python3

import os
import sys
import encryptDecrypt

from optparse import OptionParser

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-T", "--TYPE",
             dest="type",
             help="The type of ciphering to perform. E=encrypt, D=decrypt")
p.add_option("-P", "--PHRASE",
             dest="text",
             help="The text that is to be encrypted or decrypted")
p.add_option("-Z", "--PASSPHRASE",
             dest="passphrase",
             help="The passphrase to use to encrypt or decrypt the text phrase")

# Parse User's CLI args
#######################
options, args = p.parse_args()

if __name__ == "__main__":
    exitCode = 0
    answer = ""

    if options.type == "D":
        print("decrypting")
        answer = encryptDecrypt.decrypt(options.text, options.passphrase)
    elif options.type == "E":
        print("encrypting")
        answer = encryptDecrypt.encrypt(options.text, options.passphrase)
    print(answer)
    sys.stdout.flush()
    
    sys.exit(exitCode)
