#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import traceback
import time

paramiko = None

SCP_EXIT_CODES = {0:'SUCCESS',
                  1:'UNDETERMINED_ERROR',
                  3:'DESTINATION_NOT_DIR',
                  4:'CONNECTION_FAILED',
                  5:'CONNECTION_LOST',
                  6:'FILE_DOESNT_EXIST',
                  7:'NO_PERMISSIONS',
                  8:'SSHFILEXFER_ERROR',
                  9:'PROTOCOL_MISMATCH',
                  10:'INVALID_LOGIN',
                  255:'SSH_ERROR',
                  }

class SCPError(Exception):
    UNDETERMINED_ERROR = 'Undetermined error occured during SCP'
    DESTINATION_NOT_DIR = 'Expected a directory, but was given something else'
    CONNECTION_FAILED = 'Could not connect to given host'
    CONNECTION_LOST = 'Lost connection to host during file transfer'
    FILE_DOESNT_EXIST = 'The specified file does not exist on the sftp server'
    NO_PERMISSIONS = 'Insufficient permissions to do file transfer'
    SSHFILEXFER_ERROR = 'Error occured during SSH file transfer'
    PROTOCOL_MISMATCH = 'SSH Protocol mismatch'
    SSH_ERROR = 'Unknown error occured during SSH connection'
    INVALID_LOGIN = 'Incorrect username or password'
    UNKNOWN = 'Unknown error occured: {error}'

    def __init__(self, error, data):
        self.error = error
        self.data = data

    def __str__(self):
        return self.error

    def __repr__(self):
        return 'SCPError(error={error})'.format(error=self.error)


class SFTPClient():
    def __init__(self,  username, password, host, listDirSupport=True):
        self.listDirSupport = listDirSupport

        if os.name == 'posix':
            from pexpectScp import ScpClient as scpClient
            # NOTE:
            # Must have paramiko on linux OS to support listing directories over SFTP where
            # pexpect has issues.  Also we do not need sftp list directory support for hobarttv
            # and asura cprnt especially since cprnt angstrom linux does not support pycrypto required for scpClient
            if self.listDirSupport:
                global paramiko
                paramiko = __import__('paramiko')
        elif os.name != 'posix':
            from chilkatScp import ScpClient as scpClient

        # setup logging
        #paramiko.util.log_to_file('sftp_log.log')

        # get self.hostname
        self.username = username
        self.password = password

        #Port constant, only used by paramiko for listdir.
        self.port = 22

        self.hostname = str(host)
        self.scpClient = scpClient(self.hostname, self.username, self.password)

    def connect(self):
        return 'success'

    def disconnect(self):
        pass

    def listFiles(self, dir):
        if os.name == 'posix' and self.listDirSupport:
            try:
                transport = paramiko.Transport((self.hostname, self.port))
            except NameError:
                return self.scpClient.listDir(dir)

            try:
                transport.connect(None, self.username, self.password)
                sftp = paramiko.SFTPClient.from_transport(transport)
            except RuntimeError:
                #Happens if we're already connected to SFTP
                pass
            except Exception as e:
                print(('*** Caught exception: %s: %s' % (e.__class__, e)))
                traceback.print_exc()
                try:
                    transport.close()
                except:
                    pass
                return "failure"

            dir = sftp.listdir(dir)
            transport.close()
            return dir

        elif os.name == 'nt' and self.listDirSupport:
            dirList = []
            dirList = self.scpClient.listFiles(dir)

            return dirList

        else:
            print('Warning: List Directory not supported based on object initialization')
            return None

    def getIpAddress(self):
        return self.hostname

    def wait_and_retry(self, method, server_file_name, local_file_name, num_retries=5):
        for num_retry in range(0, num_retries):
            results = method(server_file_name, local_file_name)
            status, data, exit_code = results.split('|')

            if status == 'success' and exit_code == '0':
                return results
            else:
                print(('ERROR: ' + data))
                time.sleep(60)

        self._handle_error(exit_code, data)

    def parseResults(self, results):
        """ Given results from the scp client, parse the status, data, and exit code.

            :param results: The string result message to parse.
            :return: status, data, exitCode.

            Format of results:
            "status|{data}|exitCode"
        """
        args = results.split('|')
        status = args[0]
        exitCode = args[-1]
        data = '|'.join(args[1:-1])

        return status, data, exitCode

    def downloadFile(self, serverFileName, localFileName):
        #Get archive ServerFileName and store it in LocalFileName
        results = self.scpClient.downloadFile(serverFileName, localFileName)
        status, data, exitCode = self.parseResults(results)

        if status == 'success' and exitCode == '0':
            return results
        else:
            self._handle_error(exitCode, data)

    def resumeDownloadFile(self, serverFileName, localFileName):
        #Get archive ServerFileName and store it in LocalFileName
        results = self.scpClient.resumeDownloadFile(serverFileName, localFileName)
        status, data, exitCode = self.parseResults(results)

        if status == 'success' and exitCode == '0':
            return results
        else:
            self._handle_error(exitCode, data)

    def uploadFile(self, serverFileName, localFileName):
        #Put archive localFileName on server in serverFileName
        results = self.scpClient.uploadFile(serverFileName, localFileName)
        status, data, exitCode = self.parseResults(results)

        if status == 'success' and exitCode == '0':
            return results
        else:
            self._handle_error(exitCode, data)

    def _handle_error(self, exitCode, data):
        try:
            exit_code_str = SCP_EXIT_CODES[int(exitCode)]
        except KeyError:
            exit_code_str = 'UNKNOWN'
#
        try:
            message = getattr(SCPError, exit_code_str)
        except AttributeError:
            message = SCPError.UNKNOWN.format(error=data)
#
        raise SCPError(message, data)

