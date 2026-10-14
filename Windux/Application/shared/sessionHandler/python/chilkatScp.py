#!/usr/bin/env python2
# -*- coding: utf-8 -*-

try:
    import chilkat
except Exception as err:
    print('Chilkat module not found')
    print(err)


MILLISECONDS = 1
SECONDS = 1000
MINUTES = 60 * SECONDS
HOURS   = 60 * MINUTES


class ScpClient():
    """Initializer, this Class is basically just for organization
    of Pexpect versus Chilkat based on the OS."""
    def __init__(self, hostname, username, password):
        #Class Attribute Declarations.
        self.hostname = hostname
        self.username = username
        self.password = password

        #Main Scp Client.
        self.client = chilkat.CkSFtp()
        #Not sure what this does.
        success = self.client.UnlockComponent("HOBARTSSH_Y3zVOzP79JnW")
#
        #Time outs for connections and idles.
        self.client.put_ConnectTimeoutMs(SECONDS * 15)
        self.client.put_IdleTimeoutMs(MINUTES * 15)

        self.connected = self.connect()

    def connect(self):
        #Default SSH Port.
        port = 22

        #Try and connect to the hostname
        success = self.client.Connect(self.hostname,port)
        if (success != True):
            return False
#
        #Authenticate with user and pass.
        success = self.client.AuthenticatePw(self.username,self.password)
        if (success != True):
            return False
#
        #Initialization step for Chilkat.
        success = self.client.InitializeSftp()
        if (success != True):
            return False
#
        return True

    """Downloads filepath serverFileName to filePath localFileName
    on the current machine. """
    def downloadFile(self, serverFileName, localFileName, retries=8):
        if not self.connected and not self.connect():
            if retries:
                return self.retryDownloadFile(serverFileName, localFileName, retries)
            else:
                return 'failed|Could not connect to SFTP Server|255'

        #Open the file on the server with readonly permissions.
        handle = self.client.openFile(str(serverFileName),"readOnly","openExisting")
        if (handle == None):
            if retries:
                return self.retryDownloadFile(serverFileName, localFileName, retries)
            else:
                return 'failed|' + self.client.lastErrorText() + '|6'

        #Download the file from the server to the local file path.
        success = self.client.DownloadFile(handle,str(localFileName))
        if (success != True):
            if retries:
                return self.retryDownloadFile(serverFileName, localFileName, retries)
            else:
                return 'failed|' + self.client.lastErrorText() + '|5'

        #Close the remote file object.
        success = self.client.CloseHandle(handle)
        if (success != True):
            if retries:
                return self.retryDownloadFile(serverFileName, localFileName, retries)
            else:
                return 'failed|' + self.client.lastErrorText() + '|1'

        return 'success|None|0'

    """Resume downloading filepath serverFileName to filePath localFileName
    on the current machine. """
    def resumeDownloadFile(self, serverFileName, localFileName):
        if not self.connected and not self.connect():
            return 'failed|Could not connect to SFTP Server|255'

        #Download the file from the server to the local file path.
        success = self.client.ResumeDownloadFileByName(str(serverFileName),str(localFileName))
        if (success != True):
            return 'failed|' + self.client.lastErrorText() + '|6'

        return 'success|None|0'

    """ Retry download filepath serverFileName to filePath localFileName up to 8 times,
        using the resumeDownloadFile method. """
    def retryDownloadFile(self, serverFileName, localFileName, retries):
        attempt = 0
        fileSize = 0
        result = 'Unknown error'

        while (attempt < retries):
            result = self.resumeDownloadFile(serverFileName, localFileName)

            if 'success' in result:
                break

            if not os.path.exists(localFileName) or os.path.getsize(localFilePath) <= fileSize:
                break

            attempt += 1
            fileSize = os.path.getsize(localFilePath)

        return result

    """Uploads filepath localFileName to filepath serverFileName
    on the remote machine."""
    def uploadFile(self, serverFileName, localFileName, retries=8):
        if not self.connected and not self.connect():
            return 'failed|Could not connect to SFTP Server|255'

        #Open the file on the server with overwrite permissions.
        handle = self.client.openFile(str(serverFileName),"writeOnly","createTruncate")
        if (handle == None):
            return 'failed|' + self.client.lastErrorText() + '|6'

        #Upload the local file into our newly opened file object.
        success = self.client.UploadFile(handle,str(localFileName))
        if (success != True):
            return 'failed|' + self.client.lastErrorText() + '|5'

        #Close and save the file on the remote server.
        success = self.client.CloseHandle(handle)
        if (success != True):
            return 'failed|' + self.client.lastErrorText() + '|1'

        return 'success|None|0'

    def listDir(self, dir):
        return

    """Returns list of files in a directory on the remote machine."""
    def listFiles(self, dir):
        if not self.connected and not self.connect():
            return 'failed|Could not connected to SFTP Server'

        retList = []

        try:
            # Open directory on the server
            handle = self.client.openDir(str(dir))
            if (handle == None):
                print((self.client.lastErrorText()))
                return retList

            #  Download the directory listing:
            dirListing = self.client.ReadDir(handle)
            if (dirListing == None ):
                print((self.client.lastErrorText()))
                return retList

            #  Iterate over the files.
            n = dirListing.get_NumFilesAndDirs()
            if (n == 0):
                print("No entries found in this directory.")
            else:
                for i in range(0,n):
                    fileObj = dirListing.GetFileObject(i)
                    retList.append(fileObj.filename())

            # Close handle to dir
            success = self.client.CloseHandle(handle)
            if (success != True):
                print((self.client.lastErrorText()))
                return retList
        except:
            pass

        return retList

if __name__=='__main__':
    scp = ScpClient('127.0.0.1', 'smuser', 'sm1user')
