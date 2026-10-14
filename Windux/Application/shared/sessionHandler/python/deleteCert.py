#!/usr/bin/env python2
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script deletes a cert
##  REQS    : All options must be provided, except for -v -h -q
######################################################

# Preprocessor directives
##########################
import sys
import os
import shellCommand
import file_io
from optparse import OptionParser


##################################
#
#     G L O B A L S
#
# some easy to change vars below
#
#
#################################

##################################
#
#     E N D   G L O B A L S
#
##################################

######################################

# Let's get a parser to use
###########################
p = OptionParser()

# Now add some CLI options
##########################
p.add_option("-S", "--certSourceName",
             dest="certName",
             help="Name of source file.")
p.add_option("-T", "--certType",
             dest="certType",
             help="The type of cert, either ca or client.")
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

certToDelete = ""
if os.name == "posix":
    if options.certType == "ca":
        certToDelete = os.path.join("/","usr","local","hobart","db","settingswizard","TRUSTEDROOTSTORE",options.certName)
    elif options.certType == "client":
        certToDelete = os.path.join("/","usr","local","hobart","db","settingswizard","CLIENTCERTSTORE",options.certName)

    try:
        if options.certName != "" and options.certName != None:
            os.remove(certToDelete)
    except:
        exitCode = 1
        pass


    cmd = []
    cmd.append("/bin/sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    sys.exit(exitCode)

else:
    # WINDOWS BLOCK GOES HERE
    import ntCertificateUtils
    if options.certType == "ca":
        certToDelete = os.path.join("C:/","Program Files","Hobart","Db","settingswizard","TRUSTEDROOTSTORE",options.certName)
    elif options.certType == "client":
        certToDelete = os.path.join("C:/","Program Files","Hobart","Db","settingswizard","CLIENTCERTSTORE",options.certName)

    try:
        # DELETE FROM ROOT STORE
        print("what")
        fileName = options.certName.split('\\')
        #fileName = options.source.split('/')
        justFilename = fileName[len(fileName)-1]
        print(justFilename)
        certIDName = ntCertificateUtils.getCommonNameFromFileNCertType(justFilename, options.certType)
        print("back now")
        print(certIDName)
        if options.certType == "ca":
            ntCertificateUtils.deleteTrustedRootCertificate(certIDName)
        elif options.certType == "client":
            ntCertificateUtils.deleteClientCertificate(certIDName, "")

        if options.certName != "" and options.certName != None:
            try:
                os.remove(certToDelete)
            except:
                pass
            #remove from textfile also
            try:
                ntCertificateUtils.removeCertDBFileEntry(options.certName, options.certType)
            except:
                pass
    except:
        pass

    sys.exit(exitCode)
