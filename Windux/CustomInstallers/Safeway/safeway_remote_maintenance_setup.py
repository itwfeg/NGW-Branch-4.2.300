import os
import sys
import stat
import osUtils
import installUtils
import shutil
import string

TESTING = 0

PYTHON_PATH = '/usr/local/hobart/python'
sys.path.append(PYTHON_PATH)

CONFIGC = 'configc.dat'
TMPCONFIGC = 'tmpconfigc.dat'

if TESTING == 1:
    CONFIG_OVERRIDE = os.path.join("/","usr","local","hobart","clare",CONFIGC)
    TMP_CONFIG_OVERRIDE = os.path.join("/","usr","local","hobart","clare",TMPCONFIGC)
else:
    CONFIG_OVERRIDE = os.path.join("/","etc","hobart",CONFIGC)
    TMP_CONFIG_OVERRIDE = os.path.join("/","etc","hobart",TMPCONFIGC)

SAFEWAY_BASE_DIRECTORY = '/home/Hobart'
SAFEWAY_ACCESSIBLE_DIRECTORY = SAFEWAY_BASE_DIRECTORY + '/AccessScale'

DOWNLOAD_DIR_NAME = 'downloadDir'
DOWNLOAD_DIR = SAFEWAY_ACCESSIBLE_DIRECTORY + '/inbox'

DOWNLOAD_OUTPUT_DIR_NAME = 'downloadOutputDir'
DOWNLOAD_OUTPUT_DIR = SAFEWAY_ACCESSIBLE_DIRECTORY + '/outbox'

LOGGING_DIR_NAME = 'messageLoggingDirectory'
LOGGING_DIR = SAFEWAY_ACCESSIBLE_DIRECTORY

LOGGING_FILESIZE_NAME = 'messageLoggingMaxFileSize'
LOGGING_FILESIZE = '50000'

REMOTE_MAINT_ENABLED_NAME = 'remoteScaleMaintenanceEnabled'
REMOTE_MAINT_ENABLED = 'T'

REMOTE_MAINT_OUTBOX_DAYS_NAME = 'remoteScaleMaintClrOutboxDays'
REMOTE_MAINT_OUTBOX_DAYS = '14'

EQUAL_SIGN = '='

newLineChar = '\n'

def printThis(printStr):
    if TESTING == 1:
        print printStr

def update_configc():

    errorLog = open( "/var/log/safeway-log2.txt",  "wb")

    configcExists = 0
    readyToWrite = 0
    configSet = 0

    #If configc.dat exists, rename it
    if os.path.exists(CONFIG_OVERRIDE):
        configcExists = 1
        printThis('configc.dat exists. We shall rename it and read it.')

        #Rename it so we can write our new parameters out along with whatever else is in the now 'temp' file
        printThis('Move [' + CONFIG_OVERRIDE + '] to [' + TMP_CONFIG_OVERRIDE + ']')
        shutil.move(CONFIG_OVERRIDE, TMP_CONFIG_OVERRIDE)

    #Now open our new configc.dat file
    try:
        printThis('Opening [' + CONFIG_OVERRIDE + '] for writing.')
        newFile = open(CONFIG_OVERRIDE, "w")
        readyToWrite = 1
        printThis('readyToWrite = 1')
    except IOError:
        #Could not create a temporary file. Now what do we do?
        print 'Unable to create [' + CONFIG_OVERRIDE + ']. Now what do we do?'

    if configcExists == 1 and readyToWrite == 1:
        #Read the temp file and write everything out to the new file, except for our new parameters
        try:
            with open(TMP_CONFIG_OVERRIDE) as f:
                for line in f:
                    line = string.strip(line)
                    if not line.startswith('#'):
                        keyValuePair = line.split(EQUAL_SIGN)
                        cName = string.strip(keyValuePair[0])
                        if len(keyValuePair) == 2:
                            cValue = string.strip(keyValuePair[1])
                        else:
                            cValue = ''

                        if cName != DOWNLOAD_DIR_NAME and cName != DOWNLOAD_OUTPUT_DIR_NAME and cName != LOGGING_DIR_NAME and \
                            cName != LOGGING_FILESIZE_NAME and cName != REMOTE_MAINT_ENABLED_NAME and cName != REMOTE_MAINT_OUTBOX_DAYS_NAME:
                            newFile.write(line)
                            newFile.write(newLineChar)
                            printThis('Writing: ' + line)
                    else:
                        newFile.write(line)
                        newFile.write(newLineChar)

            #Remove it. We do not need it anymore
            os.remove(TMP_CONFIG_OVERRIDE)
        except IOError:
            errorLog.write("Unable to open [" + TMP_CONFIG_OVERRIDE + "]. Now what do we do?\n")
            #print 'Unable to open [' + TMP_CONFIG_OVERRIDE + ']. Now what do we do?'

    if readyToWrite == 1:
        newLine = DOWNLOAD_DIR_NAME + EQUAL_SIGN + DOWNLOAD_DIR
        printThis('Writing: ' + newLine)
        newFile.write(newLine)
        newFile.write(newLineChar)

        newLine = DOWNLOAD_OUTPUT_DIR_NAME + EQUAL_SIGN + DOWNLOAD_OUTPUT_DIR
        printThis('Writing: ' + newLine)
        newFile.write(newLine)
        newFile.write(newLineChar)

        newLine = LOGGING_DIR_NAME + EQUAL_SIGN + LOGGING_DIR
        printThis('Writing: ' + newLine)
        newFile.write(newLine)
        newFile.write(newLineChar)

        newLine = LOGGING_FILESIZE_NAME + EQUAL_SIGN + LOGGING_FILESIZE
        printThis('Writing: ' + newLine)
        newFile.write(newLine)
        newFile.write(newLineChar)

        newLine = REMOTE_MAINT_ENABLED_NAME + EQUAL_SIGN + REMOTE_MAINT_ENABLED
        printThis('Writing: ' + newLine)
        newFile.write(newLine)
        newFile.write(newLineChar)

        newLine = REMOTE_MAINT_OUTBOX_DAYS_NAME + EQUAL_SIGN + REMOTE_MAINT_OUTBOX_DAYS
        printThis('Writing: ' + newLine)
        newFile.write(newLine)
        newFile.write(newLineChar)

        newFile.close()
        errorLog.write("Successfully set up [" + CONFIG_OVERRIDE + "].\n")
        printThis('Successfully set up [' + CONFIG_OVERRIDE + '].')
        configSet = 1
    else:
        printThis('Unable to write new parameters to [' + CONFIG_OVERRIDE + ']!')

    if configSet == 1:
        # Let us see if the folders actually exist. If not, create them.

        # /home/Hobart
        if not os.path.exists(SAFEWAY_BASE_DIRECTORY):
            printThis('Creating directory: ' + SAFEWAY_BASE_DIRECTORY)
            os.mkdir(SAFEWAY_BASE_DIRECTORY)

            if not os.path.exists(SAFEWAY_ACCESSIBLE_DIRECTORY):
                printThis('Unable to create directory: ' + SAFEWAY_ACCESSIBLE_DIRECTORY + '!!!')

        if os.path.exists(SAFEWAY_BASE_DIRECTORY):
            errorCode = installUtils.chmod(SAFEWAY_BASE_DIRECTORY, permissions='ug=rwx,o=rx')
            errorLog.write("chmod(" + SAFEWAY_BASE_DIRECTORY + ",ug=rwx,o=rx) returned: " + str(errorCode) + "\n")

        # /home/Hobart/AccessScale
        if not os.path.exists(SAFEWAY_ACCESSIBLE_DIRECTORY):
            printThis('Creating directory: ' + SAFEWAY_ACCESSIBLE_DIRECTORY)
            os.mkdir(SAFEWAY_ACCESSIBLE_DIRECTORY)

            if not os.path.exists(SAFEWAY_ACCESSIBLE_DIRECTORY):
                printThis('Unable to create directory: ' + SAFEWAY_ACCESSIBLE_DIRECTORY + '!!!')

        if os.path.exists(SAFEWAY_ACCESSIBLE_DIRECTORY):
            errorCode = installUtils.chmod(SAFEWAY_ACCESSIBLE_DIRECTORY, permissions='ug=rwx,o=rx')
            errorLog.write("chmod(" + SAFEWAY_ACCESSIBLE_DIRECTORY + ",ug=rwx,o=rx) returned: " + str(errorCode) + "\n")

        # /home/Hobart/AccessScale/inbox
        if not os.path.exists(DOWNLOAD_DIR):
            printThis('Creating directory: ' + DOWNLOAD_DIR)
            os.mkdir(DOWNLOAD_DIR)

            if not os.path.exists(DOWNLOAD_DIR):
                printThis('Unable to create directory: ' + DOWNLOAD_DIR + '!!!')

        if os.path.exists(DOWNLOAD_DIR):
            errorCode = installUtils.chmod(DOWNLOAD_DIR, permissions='ug=rwx,o=rx')
            errorLog.write("chmod(" + DOWNLOAD_DIR + ",ug=rwx,o=rx) returned: " + str(errorCode) + "\n")

        # /home/Hobart/AccessScale/outbox
        if not os.path.exists(DOWNLOAD_OUTPUT_DIR):
            printThis('Creating directory: ' + DOWNLOAD_OUTPUT_DIR)
            os.mkdir(DOWNLOAD_OUTPUT_DIR)

            if not os.path.exists(DOWNLOAD_OUTPUT_DIR):
                printThis('Unable to create directory: ' + DOWNLOAD_OUTPUT_DIR + '!!!')

        if os.path.exists(DOWNLOAD_OUTPUT_DIR):
            errorCode = installUtils.chmod(DOWNLOAD_OUTPUT_DIR, permissions='ug=rwx,o=rx')
            errorLog.write("chmod(" + DOWNLOAD_OUTPUT_DIR + ",ug=rwx,o=rx) returned: " + str(errorCode) + "\n")


if __name__ == "__main__":
    printThis('Here we go!')
    exitCode = update_configc()
    sys.exit(exitCode)
