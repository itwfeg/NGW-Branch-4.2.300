#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os

try:
    import pexpect
except Exception as err:
    print('Pexpect Installation not found in Python Path')
    print(err)

timeout_minute = 60
timeout_hour = timeout_minute * 60
timeout_day = timeout_hour * 24

SCP_DOWNLOAD = 'scp -o ConnectTimeout=30 -o StrictHostKeyChecking=no -v {user}@{host}:{remote} {local}'
SCP_UPLOAD   = 'scp -o ConnectTimeout=30 -o StrictHostKeyChecking=no -v {local} {user}@{host}:{remote}'
SFTP_DOWNLOAD = 'sftp -o ConnectTimeout=30 -o StrictHostKeyChecking=no -v {user}@{host}:{remote} {local}'
SFTP_DOWNLOAD_RETRY = 'sftp -a -o ConnectTimeout=30 -o StrictHostKeyChecking=no -v {user}@{host}:{remote} {local}'
SFTP_UPLOAD = '/bin/ash -c "printf \'put {local} {remote}\nquit\' | sftp -o ConnectTimeout=30 -o StrictHostKeyChecking=no -v {user}@{host}"'
SFTP_UPLOAD_RETRY = '/bin/ash -c "printf \'put {local} {remote}\nquit\' | sftp -a -o ConnectTimeout=30 -o StrictHostKeyChecking=no -v {user}@{host}"'


class ScpError(Exception):
    def __init__(self, data, exit_code):
        self.data = data
        self.exit_code = exit_code

    def __str__(self):
        return self.data

    def __repr__(self):
        return str(self)


class ScpClient():
    """Initializer, this Class is basically just for organization
    of Pexpect versus Chilkat based on the OS."""
    def __init__(self, hostname, username, password):
        #Class Attribute Declarations.
        self.hostname = hostname
        self.username = username
        self.password = password

    def getTimeout(self, type='Day'):
        return timeout_day

    def runScp(self, command, timeout=None):
        if not timeout:
            timeout = self.getTimeout('Day')

        scpProc = pexpect.spawn(command, timeout=timeout, encoding='utf-8')
        scpProc.timeout = timeout

        # Waits for the process to prompt for password and inputs it.
        index = scpProc.expect(['assword:', 'yes/no', 'y/n', 'onnection timed out', 'o route to host', 'nvalid argument', pexpect.EOF, pexpect.TIMEOUT], timeout=15)

        if index == 0:
            scpProc.sendline(self.password)
        elif index == 1:
            scpProc.sendline('yes')
            scpProc.expect('assword')
            scpProc.sendline(self.password)
        elif index == 2:
            scpProc.sendline('y')
            scpProc.expect('assword')
            scpProc.sendline(self.password)
        elif index == 3 or index == 4:
            scpProc.close()
            raise ScpError('ssh: connect to host {0} port 22: Connection timed out'.format(self.hostname), 4)
        elif index == 5:
            scpProc.close()
            raise ScpError('ssh: connect to host {0} port 22: Invalid argument'.format(self.hostname), 4)
        elif index == 6:
            data = scpProc.before
            scpProc.close()
            raise ScpError(data, 1)
        elif index == 7:
            scpProc.close()
            raise ScpError('SCP command timed out', 5)

        # note: timeout here is for the file download
        #       allow 15m (900s)
        index = scpProc.expect(['assword:' ,'onnection timed out', 'onnection closed', 'not found', 'O SUCH file or directory', pexpect.EOF, pexpect.TIMEOUT], timeout=900)

        if index == 0:
            scpProc.close()
            raise ScpError('Incorrect username or password', 10)
        elif index == 1:
            scpProc.close()
            raise ScpError('ssh: connect to host {0} port 22: Connection timed out'.format(self.hostname), 4)
        elif index == 2:
            scpProc.close()
            raise ScpError('ssh: connect to host {0} port 22: Connection closed'.format(self.hostname), 5)
        elif index == 3:
            scpProc.close()
            raise ScpError('File not found on remote server', 6)
        elif index == 4:
            scpProc.close()
            raise ScpError('File not found on remote server', 6)
        elif index == 5:
            data = scpProc.before
        elif index == 6:
            scpProc.close()
            raise ScpError('SCP connection timed out', 5)

        scpProc.close()

        try:
            exitCode = scpProc.signalstatus
        except AttributeError as err:
            exitCode = scpProc.exitstatus

        if not exitCode:
            exitCode = scpProc.exitstatus

        error = self.checkForErrors(process=scpProc, data=data)

        if error:
            if 'onnection timed out' in data.lower():
                raise ScpError('ssh: connect to host {0} port 22: Connection timed out'.format(self.hostname), exitCode)
            else:
                raise ScpError(data, exitCode)
        else:
            exitCode = 0

        return data, exitCode

    def checkForErrors(self, process, data):
        success_message = 'Exit status 0'
        lines = data.split('\n')[:-1]

        if any(success_message in line.strip() for line in lines):
            return False

        return True

    def quotedFilename(self, filepath):
        dirname = os.path.dirname(filepath)
        filename = os.path.basename(filepath)
        filename = filename.replace(' ', '\\\\\\ ')
        quoted_filename = '"{0}"'.format(filename)
        return '{0}/{1}'.format(dirname, filename)

    """Downloads filepath serverFileName to filePath localFileName
    on the current machine."""
    def downloadFile(self, serverFileName, localFileName, retries=8):
        data = ''
        serverFileName = self.quotedFilename(serverFileName)
        localFileName = self.quotedFilename(localFileName)
        command = SFTP_DOWNLOAD.format(user=self.username, host=self.hostname, remote=serverFileName, local=localFileName)

        try:
            status = 'success'
            data, exit_code = self.runScp(command=command)
        except ScpError as err:
            status = 'failed'
            data = err.data
            exit_code = err.exit_code

        if status == 'failed' and retries:
            if not exit_code == 4 and not '0%' in data:
                status, data, exit_code = self.retryDownload(SFTP_DOWNLOAD_RETRY, serverFileName, localFileName, retries)

        return '{status}|{data}|{exitCode}'.format(data=data, exitCode=exit_code, status=status)

    """Uploads filepath localFileName to filepath serverFileName
    on the remote machine."""
    def uploadFile(self, serverFileName, localFileName, retries=8):
        data = ''
        serverFileName = self.quotedFilename(serverFileName)
        localFileName = self.quotedFilename(localFileName)
        command = SFTP_UPLOAD.format(user=self.username, host=self.hostname, remote=serverFileName, local=localFileName)

        try:
            data, exit_code = self.runScp(command=command)
            status = 'success'
        except ScpError as err:
            status = 'failed'
            data = err.data
            exit_code = err.exit_code

        return '{status}|{data}|{exitCode}'.format(data=data, exitCode=exit_code, status=status)

    def retryDownload(self, command_template, serverFileName, localFileName, retries=8):
        data = ''
        command = command_template.format(user=self.username,
                                           host=self.hostname,
                                           remote=serverFileName,
                                           local=localFileName)

        fileSize = 0
        attempt = 0

        while (attempt < retries):
            try:
                data, exit_code = self.runScp(command=command)
                status = 'success'
                break
            except ScpError as err:
                status = 'failed'
                data = err.data
                exit_code = err.exit_code

            # check progression
            if ((not os.path.exists(localFileName)) or (os.path.getsize(localFileName) <= fileSize)):
                break;

            attempt += 1
            fileSize = os.path.getsize(localFileName)

        return status, data, exit_code


def test_download(client):
    dl_results = client.downloadFile('./projectDirs.txt', 'projectDirs.txt')
    tokens = dl_results.split('|')

    print(dl_results)

    if tokens[0] != 'success':
        print('Download Test Failed')
    else:
        print('Download Test Successful')

    print('------------------------------')


def test_upload(client):
    ul_results = client.uploadFile('./projectDirs.txt', './projectDirs.txt')
    tokens = ul_results.split('|')

    print(ul_results)

    if tokens[0] != 'success':
        print('Upload Test Failed')
    else:
        print('Upload Test Successful')

    print('------------------------------')


def run_tests():
    client = ScpClient('10.3.177.95', 'smuser', 'sm1user')
    test_download(client)
    test_upload(client)


if __name__=='__main__':
    run_tests()
