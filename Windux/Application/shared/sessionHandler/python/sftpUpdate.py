#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# not tested in python3

#TWISTED_MODULES
import glob
import os
import sys
import sql
import time

from datetime import datetime
from threading import Thread
from twisted.internet import reactor
from twisted.internet.protocol import Factory
from twisted.internet.task import LoopingCall
from twisted.protocols.basic import LineReceiver

from sftpProtocol import SFTPClient, SCPError

import osUtils
import languageSettings
import messageBox
import installUpgrade
import sendDeviceEventMessage
import displayMessageBox

class UpgradeChecker(Thread):

    def __init__(self):
        Thread.__init__(self)

        self.properlyConfigured = False
        self.timer = None

        self.hostname = None
        self.datetime = None
        self.directory = None
        self.username = None
        self.password = None
        self.upgrading = False
        exitCodes = self.readConfigsFromDb()

        if exitCodes is None or 'error' in exitCodes:
            print('Could not read SFTPUpgrade Configuration Settings correct, nothing will be done until rectified.')
            print(exitCodes)
            print((self.hostname, self.datetime, self.directory, self.username, self.password))
        else:
            self.properlyConfigured = True
            self.timer = LoopingCall(self.checkTime)
            self.timer.start(30)

    def run(self, dateTime):
        self.readConfigsFromDb()
        if dateTime is not None and dateTime == 'now':
            self.executeUpgrade()
        else:
            if self.timer is None:
                self.timer = LoopingCall(self.checkTime)
            if not self.timer.running():
                self.timer.start(30)

    def readConfigsFromDb(self):
        print('Reading Configs From DB')
        exitCodes = []

        if os.name == "posix":
            dbName = glob.glob('/usr/local/hobart/db/scale/dbfiles/dbft?.db')
        else:
            import configInfo
            dbName = glob.glob(os.path.join(configInfo.getRootFolder(),'Db','Scale','DbFiles','dbft?.db'))

        if dbName is None or len(dbName) < 1:
            print('No database file could be found, can not load settings for SFTPUpgrader.')
            exitCodes.append('error')
            return exitCodes
        try:
            dbName = dbName[0]

            tableName = 'Configs'
            valueCol = 'CoValue'
            nameCol = 'CoName'

            hostnameCol = "FTPServerIPorHostName"
            self.port = 22
            usernameCol = "FTPUserName"
            passwordCol = "FTPPassword"
            directoryCol = "FTPSubDirectory"
            datetimeCol = "newFirmwareVersionSearchTime"

            selectCommandFormat = 'SELECT %s FROM %s WHERE %s = "%s"'

            exitCodes = []
            print("DB A")
            hostnameString = selectCommandFormat % (valueCol, tableName, nameCol, hostnameCol)
            status, self.hostname = sql.execute(dbName, hostnameString)
            self.hostname = str(self.hostname[0][0])
            exitCodes.append(status)
            usernameString = selectCommandFormat % (valueCol, tableName, nameCol, usernameCol)
            status, self.username = sql.execute(dbName, usernameString)
            self.username = str(self.username[0][0])
            exitCodes.append(status)
            status, self.password = sql.execute(dbName, selectCommandFormat % (valueCol, tableName,
                                                                               nameCol, passwordCol))

            self.password = str(self.password[0][0])
            exitCodes.append(status)
            status, self.directory = sql.execute(dbName, selectCommandFormat % (valueCol, tableName,
                                                                                nameCol, directoryCol))
            self.directory = str(self.directory[0][0])
            exitCodes.append(status)
            status, self.datetime = sql.execute(dbName, selectCommandFormat % (valueCol, tableName,
                                                                               nameCol, datetimeCol))
            print("DB C")

            self.datetime = str(self.datetime[0][0])
            exitCodes.append(status)
            
            if 'error' in exitCodes:
                self.properlyConfigured = False
            else:
                #print 'Successfully read configs from DB'
                #print 'Hostname:',  self.hostname
                #print 'Username:',  self.username
                #print 'Password:',  self.password
                #print 'Time:', self.datetime
                self.properlyConfigured = True
            print(exitCodes)
            return exitCodes

        except Exception as error:
            self.hostname = None
            self.username = None
            self.datetime = None
            self.password = None
            self.directory = None
            self.properlyConfigured = False
            print(("Can't load SFTPUgrader: " + str(error)))
            return None

    def changeConfigs(self, hostname = None, port = None, username = None, password = None, datetime= None, directory = None):
        if hostname is not None:
            self.hostname = hostname

        if port is not None:
            self.port = port

        if username is not None:
            self.username = username

        if password is not None:
            self.password = password

        if datetime is not None:
            if datetime == 'now':
                #print 'Doing Upgrade'
                #print 'Hostname:',  self.hostname
                #print 'User:', self.username
                #print 'Password:',  self.password
                self.executeUpgrade()
            else:
                self.datetime = datetime

        if directory is not None:
            self.directory = directory

        #print 'Configs Changed'
        #print 'Hostname:',  self.hostname
        #print 'User:', self.username
        #print 'Password:',  self.password
        self.properlyConfigured = True

    def checkTime(self):

        if not self.properlyConfigured:
            return None
        if self.datetime.lower() == 'now':
            self.executeUpgrade()
            return None

        checktime = datetime.now()
        checktime = checktime.strftime('%I:%M %p')
        print(('DB - Current Time:', checktime))
        if not self.datetime:
            return None

        #print 'Update Time:',  self.datetime
        #if checktime == self.datetime:
        #    print 'Doing Upgrade'
        #    print 'Hostname:',  self.hostname
        #    print 'User:', self.username
        #    print 'Password:',  self.password
#            if not self.upgrading:
#                self.executeUpgrade()

    def updateImageSubDirs(self, dirLocation, dirFilePaths):

        def downloadFile(fileName, localFilePath):
            osUtils.mkDestPath(os.path.dirname(localFilePath))
            print(('Downloading %s to %s' % (dirLocation + '/' + fileName, localFilePath)))

            try:
                self.SFTPClient.downloadFile(dirLocation + '/' + fileName, localFilePath)
            except Exception as error:
                if 'Permission Denied' in error:
                    print(('Could not access file %s' % dirLocation + '/' + fileName))


        imageSubDirs = ('categories', 'planograms', 'products', 'selfServe', 'softkeys', 'specials', 'system', 'undesignated')
        dirName = 'images'

        for fileName in dirFilePaths:
            print(('fileName: ' + fileName))

            if os.name == 'posix':
                localFilePath = '/usr/local/hobart/' + '/'.join([dirName, fileName])
            elif os.name == 'nt':
                localFilePath = os.path.join('c:\\updates',dirName, fileName)

            print(('localFilePath: ' + localFilePath))

            if fileName in imageSubDirs:
                for subdir_file in self.SFTPClient.listFiles(dirLocation + '/' + fileName):
                    subDirFilePath = os.path.join(localFilePath, subdir_file)
                    downloadFile(os.path.join(fileName, subdir_file), subDirFilePath)
            else:
                # then it must be a regular image in the images directory, so just send it
                downloadFile(fileName, localFilePath)

    def executeUpgrade(self):
        if self.properlyConfigured:
            self.upgrading = True
            self.hostname = self.hostname.strip('"')
            self.username = self.username.strip('"')
            self.password = self.password.strip('"')
            self.directory = self.directory.strip('"')
            self.SFTPClient = SFTPClient(self.username, self.password, self.hostname)
            connectResult = self.SFTPClient.connect()


            if not connectResult == "success" and not connectResult == "alreadyConnected":
                pass
                #print 'Failed to connect to SFTPClient@%s with user:%s pass:%s' %(self.hostname,
                #                                                                  self.username, self.password)

            if not 'hobart' in self.SFTPClient.listFiles(self.directory):
                print('No hobart directory in list, not upgrading.')
                # send message to backend to check User's connection entries
                sendDeviceEventMessage.sendMessage(server="localhost", port=6000, message="RTDO" + chr(0x1F) + "TNUpgradeSFTPBadAuthentication" + chr(0x1E))
                return None

            upgradeFolders = self.SFTPClient.listFiles(os.path.join(self.directory, 'hobart'))
            print(('upgradeFolders: ' + str(upgradeFolders)))

            dirFilePaths = []
            firmwarePaths = []
            licensePaths = []

            for upgradeFolderPath in upgradeFolders:
                dirLocation = '/'.join([self.directory, 'hobart', upgradeFolderPath])
                print(('dirLocation: ' + dirLocation))

                if 'firmware' not in upgradeFolderPath and 'license' not in upgradeFolderPath:
                    dirFilePaths = self.SFTPClient.listFiles(dirLocation)
                    print(('dirFilePaths: ', dirFilePaths))

                    if len(dirFilePaths) > 0:

                        if upgradeFolderPath == 'images':
                            self.updateImageSubDirs(dirLocation, dirFilePaths)
                            continue

                        language = languageSettings.currentLanguage()
                        if language == 'spanish':
                            message = "La Balance se está actualizando, espere por favor..."
                        elif language == 'french':
                            message = "Mise à jour de la Balanza, prière d’attendre..."
                        else:
                            message = "The Scale is updating, please wait..."

                        if os.name == 'posix':
                            exitCode,  mbId = messageBox.display(message=message)
                        elif os.name == 'nt':
                            print("SPECIAL MESSAGE BOX NOW")
                            displayMessageBox.showBox(message)

                        for dirFilePath in dirFilePaths:
                            fileName = dirFilePath
                            dirName = upgradeFolderPath
                            print(('fileName: ' + fileName))
                            print(('dirName:  ' + dirName))


                            if os.name == 'posix':
                                localFilePath = '/usr/local/hobart/' + '/'.join([dirName, fileName])
                            elif os.name == 'nt':
                                # Need to determine local file storage location and valid extensions



                                localFilePath = os.path.join('c:\\Program Files\\Hobart',dirName, fileName)

                            print(('localFilePath: ' + localFilePath))


                            print(('localFilePath: ' + localFilePath))

                            osUtils.mkDestPath(os.path.dirname(localFilePath))
                            print(('Downloading %s to %s' % (dirLocation + '/' + dirFilePath, localFilePath)))

                            try:
                                self.SFTPClient.downloadFile(dirLocation + '/' + dirFilePath, localFilePath)
                            except Exception as error:
                                if 'Permission Denied' in error:
                                    print(('Could not access file %s' % dirLocation + '/' + dirFilePath))

                        if os.name == 'posix':
                            messageBox.destroy(mbId)
                        elif os.name == 'nt':
                            print("DESTROYING SPECIAL MESSAGE BOX")
                            displayMessageBox.destroy()

                    else:
                        print(('No Files found in directory:',  dirLocation))

                elif 'license' in upgradeFolderPath:
                    licensePaths.append(dirLocation)

                else:
                    firmwarePaths.append(dirLocation)

            if len(firmwarePaths) < 1:
                print('No Firmware upgrade was detected')
                print(firmwarePaths)

            if len(licensePaths) < 1:
                print('No license upgrade was detected')
                print(licensePaths)

            if len(firmwarePaths) < 1 and len(licensePaths) < 1:
                print('No firmware and no license files to upgrade')
                print('so nothing to install')
                return

            if len(firmwarePaths) >= 1 or len(licensePaths) >= 1:
                print(firmwarePaths)
                print(licensePaths)
                language = languageSettings.currentLanguage()
                if language == 'spanish':
                    message = "La Balance se está actualizando, espere por favor..."
                elif language == 'french':
                    message = "Mise à jour de la Balanza, prière d’attendre."
                else:
                    message = "The Scale is updating, please wait..."
                exitCode,  mbId = messageBox.display(message=message)

            if os.name == 'posix':
                for licensePath in licensePaths:
                    dirLocation = licensePath
                    licenseFilePaths = self.SFTPClient.listFiles(dirLocation)
                    for licenseFilePath in licenseFilePaths:
                        fileName = licenseFilePath
                        if '.lic' not in fileName:
                            print(fileName)
                            continue
                        dirName = 'license'
                        localPath = '/usr/local/hobart/' + '/'.join([dirName, fileName])
                        osUtils.mkDestPath(os.path.dirname(localPath))

                    print(('Downloading License Upgrade from %s to %s' % (dirLocation + '/' + fileName, localPath)))

                    try:
                        self.SFTPClient.downloadFile(dirLocation + '/' + fileName, localPath)
                    except SCPError as err:
                        num_retries = 5
                        for num_retry in range(0, num_retries):
                            try:
                                self.SFTPClient.downloadFile(dirLocation + '/' + fileName, localPath)
                            except SCPError:
                                minute = 60
                                time.sleep(minute * 5)
                            else:
                                break
                        else:
                            print(err)

                    except Exception as err:
                        print(err)

                    print(('Upgrading licensing using update script %s and file %s' % (installUpgrade.__file__, localPath)))

                    messageBox.destroy(mbId)
                    installUpgrade.installUpgrade(upgradeFilename=localPath, serverHost=self.hostname, username=self.username, password=self.password, uploadFolder=dirLocation + '/' + fileName, removeUpgrade=True)

            # get the MD5 files first
            for firmwarePath in firmwarePaths:
                dirLocation = firmwarePath
                firmwareFilePaths = self.SFTPClient.listFiles(dirLocation)
                for firmwareFilePath in firmwareFilePaths:
                    fileName = firmwareFilePath

                    if '.md5' not in fileName:
                        # firmware folder must only have .tgz or .md5 files or we ignore
                        continue

                    dirName = 'firmware'
                    if os.name == 'posix':
                        localPath = '/usr/local/hobart/' + '/'.join([dirName, fileName])
                    elif os.name == 'nt':
                        localPath = os.path.join('c:\\updates',dirName, fileName)
                    osUtils.mkDestPath(os.path.dirname(localPath))

                    print(('Downloading Firmware Upgrade from %s to %s' % (dirLocation + '/' + fileName, localPath)))
                    try:
                        self.SFTPClient.downloadFile(dirLocation + '/' + fileName, localPath)
                    except SCPError as err:
                        num_retries = 5
                        for num_retry in range(0, num_retries):
                            try:
                                self.SFTPClient.downloadFile(dirLocation + '/' + fileName, localPath)
                            except SCPError:
                                minute = 60
                                time.sleep(minute * 5)
                            else:
                                break
                        else:
                            print(err)
                    except Exception as err:
                        print(err)

            for firmwarePath in firmwarePaths:
                dirLocation = firmwarePath
                firmwareFilePaths = self.SFTPClient.listFiles(dirLocation)
                for firmwareFilePath in firmwareFilePaths:
                    fileName = firmwareFilePath

                    if '.tgz' not in fileName:
                        # firmware folder must only have .tgz or .md5 files or we ignore
                        continue

                    dirName = 'firmware'
                    if os.name == 'posix':
                        localPath = '/usr/local/hobart/' + '/'.join([dirName, fileName])
                    elif os.name == 'nt':
                        localPath = os.path.join('c:\\updates',dirName, fileName)
                    osUtils.mkDestPath(os.path.dirname(localPath))

                    print(('Downloading Firmware Upgrade from %s to %s' % (dirLocation + '/' + fileName, localPath)))
                    try:
                        self.SFTPClient.downloadFile(dirLocation + '/' + fileName, localPath)
                    except SCPError:
                        num_retries = 5
                        for num_retry in range(0, num_retries):
                            try:
                                self.SFTPClient.downloadFile(dirLocation + '/' + fileName, localPath)
                            except SCPError as err:
                                minute = 60
                                time.sleep(minute * 5)
                            else:
                                break
                        else:
                            print(err)

                    except Exception as err:
                        print(err)


                    print(('Upgrading Firmware using update script %s and file %s' % (installUpgrade.__file__, localPath)))
                    messageBox.destroy(mbId)
                    installUpgrade.installUpgrade(upgradeFilename=localPath, removeUpgrade=True)

            self.upgrading = False

class clientFactory(Factory):
    def __init__(self):
        self.upgradeChecker = UpgradeChecker()

    # Overridden to create custom line receiver protocol and pass it the settings dictionary.
    def buildProtocol(self, addr):
        self.transPortConnection = addr.host
        print(('[DEBUG]: Client Connected@' + addr.host))
        return commandHandler(self.pushCommand)

    def stopFactory(self):
        print('[DEBUG]: Closing Listener and Releasing all Resources')
        Factory.stopFactory(self)

    def clientConnectionLost(self, connector, reason):
        print(("[DEBUG]: Connection@" + connector.getDestination() + " Lost Unexpectedly."))

    def clientConnectionFailed(self, connector, reason):
        print(("[DEBUG]: Failed to Connect@" + connector.getDestination() + '-' + reason.getErrorMessage()))

    def pushCommand(self,  command):
        argList = command.decode().split('|')
        if command == 'sftpUpgradeConfigChanged':
            self.upgradeChecker.readConfigsFromDb()
            if len(argList) > 1:
                self.upgradeChecker.changeConfigs(datetime=argList[1])

        elif command == 'sftpUpgradeNow':
            self.upgradeChecker.changeConfigs(datetime='now')


# Begin Command Receiver Protocol, Called by Reactor whenever a whole line of data is received from a Protocol.
class commandHandler(LineReceiver):
    # Specifies what the Protocol will see as a new line.
    delimiter = b'\n'

    def __init__(self, lineReceived_callback):
        self.lineReceived_callback = lineReceived_callback

    def connectionLost(self, reason):
        print(("[DEBUG]: Connection@" + self.connectionHost.host + " Closed \n"))

    def connectionMade(self):
        self.connectionHost = self.transport.getPeer()

    # Event Handler, called by twisted reactor loop whenever a full line of data is received from a connected client.
    # TODO: --lineReceived--
    # Remove Print statement after debugging stage.
    def lineReceived(self, line):
        args = line.decode().split('|')
        print(('[DEBUG]: ' + "'" + args[0] + "'" + ' command received from:' + self.connectionHost.host))
        self.lineReceived_callback(line)

# Instantiate the Server and Begin Event Listener Reactor
if __name__ == "__main__":
    def main():
        # Reads all Variables from Config File into memory.
        factory = clientFactory()
        factory.protocol = commandHandler
        reactor.listenTCP(5078, factory, interface='127.0.0.1')
        print("[DEBUG]: Listening...")
        reactor.run()
        sys.exit(0)

    main()
