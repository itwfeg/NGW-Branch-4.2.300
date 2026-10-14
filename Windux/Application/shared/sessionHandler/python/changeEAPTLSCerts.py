#!/usr/bin/env python
# -*- coding: utf-8 -*-

# not tested with python3

######################################################
##
##       PROLOGUE
##  AUTHOR  : Todd Davis
##  COMPANY : ITW Hobart
##  DATE    : October 15, 2012
##  OVERVIEW: This script chaches EAPTLS certs.
##  REQS    : CAN ONLY BE CALLED IF ON POSIX (scaluei doesn't call this if on posix)
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
    #print "Davis"
    certInfoAllText = ntCertificateUtils.showCertContents(filePath, password)

    if certInfoAllText == "NONE":
        return certInfoAllText
    else:
        CERTID = ""
        found = False
        counter = 0;
        foundSubjectLine = False;
        #print "TODD"
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

trustedRootStoreDir = os.path.join("/","usr","local","hobart","db","settingswizard","TRUSTEDROOTSTORE")
clientCertStoreDir = os.path.join("/","usr","local","hobart","db","settingswizard","CLIENTCERTSTORE")

if os.name != "posix":
    trustedRootStoreDir = os.path.join("C:/","Program Files","Hobart","db","settingswizard","TRUSTEDROOTSTORE")
    clientCertStoreDir = os.path.join("C:/","Program Files","Hobart","db","settingswizard","CLIENTCERTSTORE")

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
p.add_option("-S", "--trustedRootCertLocation",
             dest="trustedRoot",
             help="Full path of trusted root cetificate.")
p.add_option("-C", "--clientp12CertLocation",
             dest="client",
             help="Full path of client cetificate.")
p.add_option("-D", "--desination",
             dest="destination",
             help="Full path of destination where both files reside.")
p.add_option("-P", "--password",
             dest="password",
             help="Password for client key file.")
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

currentRootText = ""
currentClientText = ""
serverFilename = ""
clientFilename = ""
serverCommonName = ""
clientCommonName = ""

if not os.path.exists(trustedRootStoreDir):
    os.makedirs(trustedRootStoreDir)

if os.name == 'posix':
    shutil.copy2(options.trustedRoot, "/usr/local/hobart/db/settingswizard/")
    cmd = []
    cmd.append("/bin/sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)
else:
    try:
        import ntCertificateUtils
        shutil.copy(options.trustedRoot, os.path.join("C:/","Program Files","Hobart","db","settingswizard"))
        #print options.trustedRoot
        ntCertificateUtils.storeTrustedRootCertificate(options.trustedRoot, options.password)
        fileName = options.trustedRoot.split('\\')
        justFilename = fileName[len(fileName)-1]
        serverFilename = justFilename

    except:
        pass
worked = True
if os.name == 'posix':
    with open("/usr/local/hobart/db/settingswizard/TRUSTEDROOTCERTS.txt", "w") as myfi:
        answer = ""
        try:
            #shutil.copy2(options.source, trustedRootStoreDir)-C "
            fileName = options.trustedRoot.split('/')
            answer = fileName[len(fileName)-1]
            myfi.write(answer + "\n")
            worked = True
        except:
            exitCode = 20 # CA file not found
            worked = False
            pass
        myfi.close()
if not worked:
    sys.exit(exitCode)

if not os.path.exists(clientCertStoreDir):
    try:
        os.makedirs(clientCertStoreDir)
    except:
        pass
worked = True

# now do client
if os.name == "posix":
    with open("/usr/local/hobart/db/settingswizard/CLIENTP12CERTS.txt", "w") as myfi:
        answer = ""
        try:
            #shutil.copy2(options.client, "/usr/local/hobart/db/settingswizard/")
            #shutil.copy2(options.source, clientCertStoreDir)
            fileName = options.client.split('/')
            answer = fileName[len(fileName)-1]
            myfi.write(answer + "\n")
            worked = True
        except:
            exitCode = 21 # Client p12 file not found
            worked = False
            pass
        myfi.close()

    cmd = []
    cmd.append("openssl")
    cmd.append("pkcs12")
    cmd.append("-in")
    cmd.append(options.client)
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

    # extract the client.pem and copy it
    cmd = []
    cmd.append("openssl")
    cmd.append("pkcs12")
    cmd.append("-in")
    cmd.append(options.client)
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

    cmd = []
    cmd.append("/bin/sync")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

else:
    #windows
    try:
        fileName = options.client.split('\\')
        clientFilename = fileName[len(fileName)-1]
        cmd = []
        cmd.append("certutil.exe")
        cmd.append("-encode")
        cmd.append(options.client)
        cmd.append(options.destination + ".pem")
        exitCode, stdout, stderr = shellCommand.execute(cmd)
        #print cmd
        #print options.client
        ntCertificateUtils.storeClientCertificate(os.path.join('C:/','Program Files','Hobart','Db','settingsWizard',options.client),options.password)
        shutil.copy2(options.client, os.path.join("C:/","Program Files","Hobart","db","settingswizard"))
        shutil.copy2(options.client, os.path.join("C:/","Program Files","Hobart","db","settingswizard",options.destination + ".pem"))
    except:
        pass

if os.name != 'posix':
    #windows current certs file management
    with open('C:\Program Files\Hobart\Db\SettingsWizard\CURRENTCERTS.txt', "w") as myfi:
        answer = ""
        try:
            #shutil.copy2(options.source, "/usr/local/hobart/db/settingswizard/")
            rootName = getCommonName(options.trustedRoot,options.password)
            clientName = getCommonName(options.client, options.password)
            myfi.write("root=" + serverFilename + "=" + rootName + "=NONE\n")
            if options.password == "":
                options.password = "NONE"
            myfi.write("client=" + clientFilename + "=" + clientName + "=" + options.password + "\n")

            worked = True
        except:
            exitCode = 20 # CA file not found
            worked = False
            pass
    myfi.close()



sys.exit(exitCode)

