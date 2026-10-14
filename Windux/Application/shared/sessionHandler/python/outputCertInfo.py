#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script extracts info from a certificate.
##  REQS    : All options must be provided, except for -v -h -q
######################################################


# Preprocessor directives
##########################
import sys
import os
import file_io
from optparse import OptionParser
import string
import subprocess

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-S", "--source",
             dest="source",
             help="Full path of source file.")
p.add_option("-P", "--password",
             dest="password",
             help="Password to allow file copy.")
p.add_option("-T", "--certType",
             dest="certType",
             help="Cert type, e.g., ca = Certificate Authority .pem format ; client = client cert and key file .p12 format.")
p.add_option("-v", "--verbose",
             dest="verbose", default=None, action="store_true",
             help="Print detailed status")
p.add_option("-q", "--quiet",
             dest="verbose", default=None, action="store_false",
             help="No status printing (default)")

# Parse User's CLI args
#######################
options, args = p.parse_args()
exitCode = 0

# User this for debugging
#########################

print('verbose   : %s' %options.verbose)
print('args      : %r' %args)

if os.name == "posix":
    exitCode = 0
    if options.source != "None" and options.source != "":
        if options.certType == "ca":
            if os.name == "posix":
                p1 = subprocess.Popen(["/usr/bin/openssl", "x509", "-text", "-in", options.source], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                output,err = p1.communicate()
                if len(err) > 0:
                    print("\n\n\n       Certificate information is unavailable")
                elif len(output) > 0:
                    print(output)
                sys.stdout.flush()
        elif options.certType == "client":
            # file must be a .p12 format with client key and cert together
            # extract the client .key and copy it
            try:
                passwordFlag = ""
                if options.password == "":
                    passwordFlag = "pass:"
                else:
                    passwordFlag = "pass:" + options.password
                p1 = subprocess.Popen(["/usr/bin/openssl", "pkcs12", "-in", options.source, "-nodes", "-passin", passwordFlag], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                p2 = subprocess.Popen(["/usr/bin/openssl", "x509", "-noout", "-text"], stdin = p1.stdout, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                p1.stdout.close()
                output,err = p2.communicate()
                
                if len(err) > 0:
                    print("\n\n\n       Certificate Information is unavailable")
                elif len(output) > 0:
                    print(output)
            except:
                exitCode = 30
                pass

            sys.stdout.flush()
            
    sys.exit(exitCode)

else:
    # WINDOWS BLOCK GOES HERE
    p1 = subprocess.Popen(["certutil", "-p", "", options.source], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output,err = p1.communicate()
    #print output
    #timeout in seconds needs added
    #print err
    if len(err) > 0:
        print("\n\n\n       Certificate information is unavailable")
    elif len(output) > 0:
        print(output)
    sys.stdout.flush()

    sys.exit(exitCode)
