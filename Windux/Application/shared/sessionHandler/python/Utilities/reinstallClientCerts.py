#!/usr/bin/env python3
# -*- coding: utf-8 -*-

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script installs a file to a certain locaiton Linux/WES7 system time.
##  REQS    : All options must be provided, except for -v -h -q
######################################################


# Preprocessor directives
##########################
import sys
import os
import shellCommand
import file_io
from optparse import OptionParser
import sendDeviceEventMessage
import shutil
import time

from os import listdir
from os.path import isfile, join

##################################
#
#     G L O B A L S
#
# some easy to change vars below
#
#
#################################

def getCommonName(filePath, password):
    import ntCertificateUtils
    certInfoAllText = ntCertificateUtils.showCertContents(filePath, password)

    if certInfoAllText == "NONE":
        return certInfoAllText
    else:
        CERTID = ""
        found = False
        counter = 0;
        foundSubjectLine = False;
        for line in certInfoAllText.splitlines():
            if not found and not foundSubjectLine and line.find("Subject:") > -1:

                # found subject area
                counter = counter + 1
                foundSubjectLine = True
                subjectList = line.split(",")
                for subject in subjectList:
                    partedUPSubject = subject.split("=")
                    if partedUPSubject[0].find("CN") > -1:
                        # found the CommonName
                            CERTID = partedUPSubject[1].rstrip()
                            found = True
                            break
            elif not found and counter > 0 and counter < 6:
                    # look ahead 5 lines at most
                counter = counter + 1
                if line.find("CN=") > -1:
                    # found the common name
                    commonName = line.split("=")
                    if commonName[0].find("CN") > -1:
                        # found the CommonName
                        CERTID = commonName[1].rstrip()
                        found = True
            else:
                if not found and foundSubjectLine and counter > 0:
                    counter = counter + 1

        return CERTID

clientCertStoreDir = os.path.join("/","usr","local","hobart","db","settingswizard","CLIENTCERTSTORE")

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
p.add_option("-S", "--source",
             dest="source",
             help="Full path of source file.")
p.add_option("-D", "--desination",
             dest="destination",
             help="Full path of destination file.")
p.add_option("-P", "--password",
             dest="password",
             help="Password to allow file copy.")
p.add_option("-T", "--certType",
             dest="certType",
             help="Cert type, e.g., ca = Certificate Authority .pem format ; client = client cert and key file .p12 format.")
p.add_option("-U", "--useCert",
             dest="useCert",
             help="Whether to actually use the cert of just copy it. 0=copy only, 1=use.")
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

# an always case below
if True:
    worked = True
    ###################################
    import ntCertificateUtils

    currentCertsInfo = ntCertificateUtils.getCurrentCertsInfo()
    print(currentCertsInfo)

    for aCert in currentCertsInfo:
        #print aCert
        line = aCert.rstrip()
        keyValuePair = line.split('=')
        if keyValuePair[0] == "root":
            aCertFilePath = keyValuePair[1]
            if len(keyValuePair) == 4:
                #print "doing root"
                if keyValuePair[3] == "NONE":
                    keyValuePair[3] = ""
                try:
                    valid, answer = ntCertificateUtils.storeTrustedRootCertificate(aCertFilePath, keyValuePair[3])
                    ntCertificateUtils.addCAtoNTAuth(aCertFilePath)

                except:
                    pass
        elif keyValuePair[0] == "client":
            aCertFilePath = keyValuePair[1]
            if len(keyValuePair) == 4:
                #print "doing client"
                if keyValuePair[3] == "NONE":
                    keyValuePair[3] = ""
                try:
                    #time.sleep()
                    valid, answer = ntCertificateUtils.storeClientCertificate(aCertFilePath, keyValuePair[3])
                except:
                    pass

        #print str(len(keyValuePair))

    # read client cert dir and reinstall all
    #for cert in os.listdir(os.path.join(['C:/', 'Program Files','Hobart', 'Db','SettingsWizard','CLIENTCERTSTORE'])):
    #    print cert
    #valid, answer = ntCertificateUtils.storeClientCertificate(os.path.join(['C:/', 'Program Files','Hobart', 'Db','SettingsWizard','CLIENTCERTSTORE',options.password]))





sys.exit(exitCode)





# Use this for debugging
#########################

#print 'outputfile: %s'%options.outputFile
#print 'verbose   : %s'%options.verbose
#print 'args      : %r',args
if options.useCert == "1":
    if os.name == "posix":
        exitCode = 0
        if options.source != "None" and options.source != "":
            if options.destination:
                #with open("logA.txt", "a") as myfile:
                #    myfile.write(options.source + " AND " + options.destination + "\n")
                #myfile.close()
                if options.certType == "ca":
                    #shutil.copy2(options.source, options.destination)
                    shutil.copy2(options.source, "/usr/local/hobart/db/settingswizard/")
                    cmd = []
                    cmd.append("/bin/sync")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    if exitCode != 0:
                        exitCode = 20 # CA File not found
                elif options.certType == "client":
                    # file must be a .p12 format with client key and cert together
                    # extract the client .key and copy it
                    #
                    cmd = []
                    cmd.append("openssl")
                    cmd.append("pkcs12")
                    cmd.append("-in")
                    cmd.append(options.source)
                    cmd.append("-nodes")
                    cmd.append("-out")
                    ## XXXX ToDo FIX BELOW
                    cmd.append("/usr/local/hobart/db/settingswizard/"+ options.destination + ".key")
                    cmd.append("-nocerts")
                    cmd.append("-passin")
                    if options.password == "":
                        cmd.append("pass:")
                    else:
                        cmd.append("pass:" + options.password)
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    print(exitCode)
                    print(stdout)
                    print(stderr)

                    # extract the client.pem and copy it
                    cmd = []
                    cmd.append("openssl")
                    cmd.append("pkcs12")
                    cmd.append("-in")
                    cmd.append(options.source)
                    cmd.append("-nodes")
                    cmd.append("-out")
                    ## XXXX ToDo FIX BELOW
                    cmd.append("/usr/local/hobart/db/settingswizard/" + options.destination + ".pem")
                    cmd.append("-nokeys")
                    cmd.append("-passin")
                    if options.password == "" or options.password == None:
                        cmd.append("pass:")
                    else:
                        cmd.append("pass:" + options.password)
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    print(exitCode)
                    print(stdout)
                    print(stderr)

        sys.exit(exitCode)

    else:
        # WINDOWS BLOCK GOES HERE
        pass
sys.exit(exitCode)

