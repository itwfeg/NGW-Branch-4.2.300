#!/usr/bin/env python3
# -*- coding: utf-8 -*-


import threading
import shellCommand
import configInfo
import osUtils
import re
import string
import os
import subprocess
import glob

def storeTrustedRootCertificate(filePath, password):
    #print "ROOT NOW"
    exitCode, stdout, stderr = osUtils.winRunElevated(['certutil.exe', '-f', '-addstore', 'ROOT', filePath])
    #print stdout
    answer = ""
    valid = True
    for line in stdout.splitlines():
        if line.find("added to store") > -1:
            valid  = True
            answer = "worked"
        elif line.find("cannot find the file") > -1:
            valid = False
            answer = "no file"
        elif line.find("data is invalid") > -1:
            valid = False
            answer = "invalid password"
    return (valid, answer)

def emptyClientCertificatesFromStore():
    cmd = []
    cmd.append("certutil")
    cmd.append("-user")
    cmd.append("-store")
    cmd.append("my")
    exitCode, stdout, stderr = shellCommand.execute(cmd)

    print(stdout)
    for line in stdout.splitlines():
        line = line.rstrip()
        if line.find("Subject:") > -1:
            commaSplits = line.split(',')
            certIDNames = commanSplits[0].split('=')
            certIDName = certIDNames[1].rstrip()

            print("MMMMMMMMMMMMMMMMMMMMMMMMMMM")
            print(certIDName)
            print("000000000MMMMMMMMMMMM000000")

def storeClientCertificate(filePath, password, connecting=False):

    #print "CLIENT NOW"

    answer = ""
    valid = True

    if connecting == True:
        print("connecting")
        emptyClientCertificatesFromStore()
        exitCode, stdout, stderr = osUtils.winRunElevated(['certutil.exe', '-f', '-user', '-p', password, '-importPFX', filePath, "NoRoot,NoProtect"])

        #print stdout
        for line in stdout.splitlines():
            if line.find("added to store") > -1:
                valid = True
                answer = "worked"
            elif line.find("cannot find the file") > -1:
                valid = False
                answer = "no file"
            elif line.find("data is invalid") > -1:
                valid = False
                answer = "invalid password"
    return (valid, answer)

def storeClientCertificateTrusted(filePath, password):
    #print "CLIENT NOW"
    exitCode, stdout, stderr = osUtils.winRunElevated(['certutil.exe', 'addstore', '-p', password, '-importPFX', filePath])
    answer = ""
    valid = True

    #print stdout
    for line in stdout.splitlines():
        if line.find("added to store") > -1:
            valid = True
            answer = "worked"
        elif line.find("cannot find the file") > -1:
            valid = False
            answer = "no file"
        elif line.find("data is invalid") > -1:
            valid = False
            answer = "invalid password"
    return (valid, answer)

def deleteTrustedRootCertificate(certIDName):
    osUtils.winRunElevated(['certutil', '-delstore', 'ROOT', certIDName])
    certmgr = os.path.join('C:/','Program Files','Hobart','bin','CertMgr.exe')
    osUtils.winRunElevated([certmgr,'-del','-c','-n',certIDName,'-s','CA'])


def deleteClientCertificate(certIDName, password):
    answer = osUtils.winRunElevated(['certutil', '-user', '-delstore', 'My', certIDName])
    certmgr = os.path.join('C:/','Program Files','Hobart','bin','CertMgr.exe')
    osUtils.winRunElevated([certmgr,'-del','-c','-n',certIDName,'-s','CA'])

def showCertContents(filePath, password):
    p1 = subprocess.Popen(["certutil", "-p", password, filePath], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    output,err = p1.communicate()
    #print "XXXXXXXX BEGIN"
    worked = False
    for line in output.splitlines():
        if line.find("successfully") > -1:
            worked = True
        elif line.find("password is not correct") > -1:
            worked = False
        else:
            worked = False
    if len(err) > 0:
        output = "NONE"
    if not worked:
        output = "NONE"
    return output

def getCommonName(filePath, password):
    certInfoAllText = showCertContents(filePath, password)
    #print "Wooz"
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
                subjectList = string.split(line, ",")
                for subject in subjectList:
                    partedUPSubject = string.split(subject, "=")
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
                    commonName = string.split(line, "=")
                    if commonName[0].find("CN") > -1:
                        # found the CommonName
                        CERTID = commonName[1].rstrip()
                        found = True
            else:
                if not found and foundSubjectLine and counter > 0:
                    counter = counter + 1

        return CERTID

def getCommonNameFromFileNCertType(fileNameOnly, certType):
    #certType must be "ca" or "client"
    answer = "NONE"
    try:
        if certType == "ca":
            dbFile = 'C:\Program Files\Hobart\Db\SettingsWizard\TRUSTEDROOTCERTS.txt'
        elif certType == "client":
            dbFile = 'C:\Program Files\Hobart\Db\SettingsWizard\CLIENTCERTS.txt'

        with open(dbFile, "r") as myfi:
            print("D")
            answer = ""
            try:
                if os.path.getsize(dbFile) > 0:
                    print("E")
                    contents = myfi.read()
            except:
                pass
        myfi.close()

        found = False
        if len(contents) > 2:
            for line in contents.splitlines():
                print(line)
                line = line.rstrip()
                keyValuePair = line.split('=')
                print("WOOZIE")
                print((fileNameOnly + "XXX"))

                print(("key0: " + keyValuePair[0]))
                print(("key1: " + keyValuePair[1]))
                print(("key2: " + keyValuePair[2]))
                if keyValuePair[0] == fileNameOnly:
                    print(("foundit" + keyValuePair[1]))
                    answer = keyValuePair[1]
                else:
                    print((keyValuePair[0] + " does not equal " + fileNameOnly))

        myfi.close()
    except:
        pass
    print(("answer is: " + answer))
    return answer

def getCurrentCertsInfo():
    contents = ""
    currentCertsFile = os.path.join('C:/','Program Files','Hobart','Db','SettingsWizard','CURRENTCERTS.txt')
    #print currentCertsFile
    with open(currentCertsFile, "r") as myfi:
        #print "D"
        answer = ""
        try:
            if os.path.getsize(currentCertsFile) > 0:
                #print "E"
                contents = myfi.read()
        except:
            # print "uh oh"
            pass
    myfi.close()
    files = contents.splitlines()
    return files

def getClientCertsInfo():
    #certType must be "ca" or "client"
    answer = "NONE"

    #print "TODD"
    try:
        #clientCertDir = os.path.join('C:/','Program Files','Hobart','Db','SettingsWizard','CLIENTCERTSTORE')

        #allFiles = [name for name in glob.glob(os.path.join(clientCertDir,'*.*')) if os.path.isfile(os.path.join(clientCertDir,name))]
        #return allFiles
        clientCertsFile = os.path.join('C:/','Program Files','Hobart','Db','SettingsWizard','CLIENTCERTS.txt')
        #print clientCertsFile
        with open(clientCertsFile, "r") as myfi:
            #print "D"
            answer = ""
            try:
                if os.path.getsize(clientCertsFile) > 0:
                    #print "E"
                    contents = myfi.read()
            except:
                #print "uh oh"
                pass
        myfi.close()
        files = contents.splitlines()
        return files

        '''found = False
        if len(contents) > 2:
            for line in contents.splitlines():
                #print line
                line = line.rstrip()
                keyValuePair = line.split('=')
                if keyValuePair[0] == "client":
                    #print "foundit"
                    return (keyValuePair[1]),keyValuePair[2],keyValuePair[3])
                else:
                    return ("NONE","NONE"

        myfi.close()
        '''
    except:
        pass
    return answer


def addCAtoNTAuth(filenameOnly):
    # add the CA cert to the NTAuth store
    fullFilePath = os.path.join('C:/','Program Files','Hobart','db','settingswizard','TRUSTEDROOTSTORE',filenameOnly)
    cmd = []
    cmd.append("certutil")
    cmd.append("-f")
    cmd.append("-enterprise")
    cmd.append("-addstore")
    cmd.append("NTAuth")
    cmd.append(fullFilePath)
    exitCode, stdout, stderr = shellCommand.execute(cmd)
    #print exitCode
    #print stdout
    #print stderr
    #return osUtils.winRunElevated(['certutil', '-enterprise', '-addstore', 'NTAuth', fullFilePath])

def emptyClientCertificates():
    dbFile = 'C:\Program Files\Hobart\Db\SettingsWizard\CLIENTCERTS.txt'
    try:
        with open(dbFile, "r") as myfi:
            answer = ""
            try:
                if os.path.getsize(dbFile) > 0:
                    #print "E"
                    contents = myfi.read()
            except:
                pass
            myfi.close()
        CERTID = ""
        found = False
        rootCertsFileContents = []
        if len(contents) > 2:
            for line in contents.splitlines():
                line = line.rstrip()
                keyValuePair = line.split('=')
                if len(keyValuePair[1]) > 0:
                    deleteClientCertificate(keyValuePair[1], "")
    except:
        pass

def removeCertDBFileEntry(fileNameOnly, certType):
    if certType == "ca":
        dbFile = 'C:\Program Files\Hobart\Db\SettingsWizard\TRUSTEDROOTCERTS.txt'
    elif certType == "client":
        dbFile = 'C:\Program Files\Hobart\Db\SettingsWizard\CLIENTCERTS.txt'
    with open(dbFile, "r") as myfi:
        answer = ""
        try:
            if os.path.getsize(dbFile) > 0:
                #print "E"
                contents = myfi.read()
        except:
            pass
    myfi.close()
    CERTID = ""
    found = False
    rootCertsFileContents = []
    if len(contents) > 2:
        for line in contents.splitlines():
            line = line.rstrip()
            keyValuePair = line.split('=')
            if keyValuePair[0] != fileNameOnly:
                rootCertsFileContents.append(keyValuePair[0] + "=" + keyValuePair[1] + "=" + keyValuePair[2] + "\n")

        with open(dbFile, "w") as myfi:
            for line in rootCertsFileContents:
                #print "F"
                #print line
                myfi.write(line)
                worked = True
        myfi.close()

