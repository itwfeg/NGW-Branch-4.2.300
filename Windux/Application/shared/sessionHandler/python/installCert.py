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

trustedRootStoreDir = os.path.join("/","usr","local","hobart","db","settingswizard","TRUSTEDROOTSTORE")
clientCertStoreDir = os.path.join("/","usr","local","hobart","db","settingswizard","CLIENTCERTSTORE")

if os.name != "posix":
    trustedRootStoreDir = os.path.join('C:/','Program Files','Hobart','db','settingswizard','TRUSTEDROOTSTORE')
    clientCertStoreDir = os.path.join('C:/','Program Files','Hobart','db','settingswizard','CLIENTCERTSTORE')


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

if options.useCert == "0":
    
    if options.certType == "ca":
        #print trustedRootStoreDir
        if not os.path.exists(trustedRootStoreDir):
            #print "Making root dir"
            os.makedirs(trustedRootStoreDir)
        try:
            #print("copying " + options.source + " to " + trustedRootStoreDir)
            shutil.copy(options.source, trustedRootStoreDir)
        except:
            pass
        if os.name == "posix":
            
            worked = True
            with open("/usr/local/hobart/db/settingswizard/TRUSTEDROOTCERTS.txt", "a") as myfi:
                answer = ""
                try:
                    #shutil.copy2(options.source, "/usr/local/hobart/db/settingswizard/")
                    cmd = []
                    cmd.append("/bin/sync")
                    exitCode, stdout, stderr = shellCommand.execute(cmd)
                    fileName = options.source.split('/')
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
        else:
            # windows
            import ntCertificateUtils
            valid, answer = ntCertificateUtils.storeTrustedRootCertificate(options.source,options.password)
            fileName = options.source.split('/')
            justFilename = fileName[len(fileName)-1]
            print(justFilename)
            ntCertificateUtils.addCAtoNTAuth(justFilename)
            if not valid:
                #print(answer)
                sys.stdout.flush()
            if valid:
                certInfoAllText = ntCertificateUtils.showCertContents(options.source, options.password)
                CERTID = getCommonName(options.source, options.password)
                #print certInfoAllText

                #print("XXXXXXXXX CERTID " + CERTID)

                #fileName = string.split(options.source,'\\')
                fileName = options.source.split('/')

                justFilename = fileName[len(fileName)-1]
                rootCertsFileContents = []
                contents = ""
                #print "A"
                if not os.path.exists('C:\Program Files\Hobart\Db\SettingsWizard\TRUSTEDROOTCERTS.txt'):
                    #print "Making root dir"
                    file = open('C:\Program Files\Hobart\Db\SettingsWizard\TRUSTEDROOTCERTS.txt', 'w')
                    file.close()

                with open('C:\Program Files\Hobart\Db\SettingsWizard\TRUSTEDROOTCERTS.txt', "r") as myfi:
                    #print "D"
                    answer = ""
                    try:
                        if os.path.getsize('C:\Program Files\Hobart\Db\SettingsWizard\TRUSTEDROOTCERTS.txt') > 0:
                            #print "E"
                            contents = myfi.read()
                    except:
                        pass
                myfi.close()

                found = False
                if len(contents) > 2:
                    for line in contents.splitlines():
                        #print line
                        line = line.rstrip()
                        keyValuePair = line.split('=')
                        if keyValuePair[0] == justFilename:
                            keyValuePair[1] = CERTID
                            keyValuePair[2] = options.password
                            found  = True
                        if keyValuePair[2] == "":
                            keyValuePair[2] = "NONE"
                        rootCertsFileContents.append(keyValuePair[0] + "=" + keyValuePair[1] + "=" + keyValuePair[2] + "\n")
                    if not found:
                        if options.password == "":
                            options.password = "NONE"
                        rootCertsFileContents.append(justFilename + "=" + CERTID + "=" + options.password + "\n")

                else:
                    if options.password == "":
                        options.password = "NONE"

                    rootCertsFileContents.append(justFilename + "=" + CERTID + "=" + options.password + "\n")
                with open('C:\Program Files\Hobart\Db\SettingsWizard\TRUSTEDROOTCERTS.txt', "w") as myfi:
                    for line in rootCertsFileContents:
                        #print "F"
                        #print line
                        myfi.write(line)
                        worked = True
                myfi.close()

    elif options.certType == "client":
        if not os.path.exists(clientCertStoreDir):
            os.makedirs(clientCertStoreDir)
        try:
            shutil.copy(options.source, clientCertStoreDir)
        except:
            pass
        worked = True
        if os.name == "posix":
            with open("/usr/local/hobart/db/settingswizard/CLIENTCERTS.txt", "a") as myfi:
                answer = ""
                try:
                    #shutil.copy2(options.source, "/usr/local/hobart/db/settingswizard/")
                    fileName = options.source.split('/')

                    answer = fileName[len(fileName)-1]
                    myfi.write(answer + "\n")
                    worked = True
                except:
                    exitCode = 21 # Client p12 file not found
                    worked = False
                    pass
            myfi.close()
            if not worked:
                sys.exit(exitCode)
        else:
            ###################################
            import ntCertificateUtils
            valid, answer = ntCertificateUtils.storeClientCertificate(options.source,options.password)
            if not valid:
                print(answer)
                sys.stdout.flush()
            if valid:
                certInfoAllText = ntCertificateUtils.showCertContents(options.source, options.password)
                CERTID = getCommonName(options.source, options.password)
                #print certInfoAllText
                if CERTID == "NONE":
                    print("Incorrect Password.")
                    sys.stdout.flush()

                #print "XXXXXXXXX CERTID " + CERTID

                #print options.source
                #fileName = string.split(options.source,'\\')
                fileName = options.source.string('/')
                justFilename = fileName[len(fileName)-1]

                rootCertsFileContents = []
                contents = ""
                print("SDSDSD-------")
                print(("justFilename is: " + justFilename))
                #print("all stringList is:")
                #print fileName
                print("A")
                if not os.path.exists('C:\Program Files\Hobart\Db\SettingsWizard\CLIENTCERTS.txt'):
                    #print "Making root dir"
                    file = open('C:\Program Files\Hobart\Db\SettingsWizard\CLIENTCERTS.txt', 'w')
                    file.close()
                with open('C:\Program Files\Hobart\Db\SettingsWizard\CLIENTCERTS.txt', "r") as myfi:
                    #print "D"
                    answer = ""
                    try:
                        if os.path.getsize('C:\Program Files\Hobart\Db\SettingsWizard\CLIENTCERTS.txt') > 0:
                            #print "E"
                            contents = myfi.read()
                    except:
                        pass
                myfi.close()

                notfound = True
                try:
                    print(("contents: " + contents))
                    if len(contents) > 7:
                        print("Length greater than 7")
                        iterator = 0
                        for line in contents.splitlines():
                            print((str(iterator + 1)))
                            if len(line) > 6:
                                print(("line: " + line))
                                line = line.rstrip()
                                keyValuePair = line.split('=')
                                if keyValuePair[0] == justFilename:
                                    print((keyValuePair[0] + " equals " + justFilename))
                                    keyValuePair[1] = CERTID
                                    keyValuePair[2] = options.password
                                    notfound  = False
                                if keyValuePair[2] == "":
                                    keyValuePair[2] = "NONE"
                                print(("1key0: " + keyValuePair[0]))
                                print(("1key1: " + keyValuePair[1]))
                                print(("1key2: " + keyValuePair[2]))
                                rootCertsFileContents.append(keyValuePair[0] + "=" + keyValuePair[1] + "=" + keyValuePair[2] + "\n")
                        print(("FOUND is: " + str(notfound)))
                        if notfound == True:
                            if options.password == "":
                                options.password = "NONE"
                            print("ok")
                            print(("QQ: " + justFilename))
                            rootCertsFileContents.append(justFilename + "=" + CERTID + "=" + options.password + "\n")
                        print(("2key0: " + keyValuePair[0]))
                        print(("2key1: " + keyValuePair[1]))
                        print(("2key2: " + keyValuePair[2]))
                    else:
                        if  options.password == "":
                            options.password = "NONE"
                        print(("Empty so will copy: " + justFilename))
                        rootCertsFileContents.append(justFilename + "=" + CERTID + "=" + options.password + "\n")
                    with open('C:\Program Files\Hobart\Db\SettingsWizard\CLIENTCERTS.txt', "w") as myfi:
                        for line in rootCertsFileContents:
                            #print "F"
                            #print line
                            myfi.write(line)
                            worked = True
                    myfi.close()
                except:
                    pass
            sys.exit(exitCode)

# Use this for debugging
#########################

print('outputfile: %s' %options.destination)
print('verbose   : %s' %options.verbose)
print('args      : %r' %args)
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
