#include "PAL_platformheader.h"

#include "properties/properties.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "HobVersion.h"
#include "CZip.h"

/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
*/

#define DBGU_CIDR_MASK          0xFFFFFFF0
#define HOBART_VENDOR_ID        0x128c
#define HOBART_APC_PRINTER_ID   0x0204

#ifdef DEFAULT_INTERNAL_CONFIG_FILE_NAME
#undef DEFAULT_INTERNAL_CONFIG_FILE_NAME
#endif
#define DEFAULT_INTERNAL_CONFIG_FILE_NAME "C:\\Program Files\\Hobart\\vmTmp\\SC\\config.dat"
#define BACKUP_INTERNAL_CONFIG_FILE_NAME "C:\\Program Files\\Hobart\\Access Scale\\config.dat"
#define APC_ZIP_SOURCE_FILE "source.bin"

int fd = 0;
char psResponse[4096];
int addrpref = 0;

int getAT91response(int fd, char *psResponse)
{
    int i;
    int curPos = 0;

    // skip initial line break
    read(fd, psResponse, 2);
    // read through response parsing out linebreak and stopping at >
    for (i=0; i<4096; i++)
    {
        read(fd, &psResponse[curPos], 1);
        if (isprint(psResponse[curPos]))
        {
            if (psResponse[curPos] == '>')
            {
                psResponse[curPos]=0;
                break;
            }
            curPos++;
        }
    }
    return curPos;
}

int parse8(char *ptr)
{
    int tmp = 0;
    sscanf(ptr, "%02x", &tmp);
    return tmp;
}

int parse16(char *ptr)
{
    int tmp = parse8(ptr)*256;
    ptr+=2;
    tmp += parse8(ptr);
    return tmp;
}

int parse_line(char *line, int *bytes, int *addr, int *num, int *code)
{
    int sum, len, cksum;
    char *ptr;

    *num = 0;
    // find SOL token
    if (line[0] != ':')
        return 0;
    if (strlen(line) < 11)
        return 0;
    ptr = line+1;

    // get line len
    len = parse8(ptr);
    ptr += 2;
    if ( strlen(line) < (size_t)(11 + (len * 2)) )
        return 0;

    *addr = addrpref + parse16(ptr);
    ptr += 4;

    *code = parse8(ptr);
    ptr += 2;

    sum = (len & 255) + ((*addr >> 8) & 255) + (*addr & 255) + (*code & 255);

    //printf("Line: length=%d Addr=0x%08x\n", len, *addr);

    while(*num != len)
    {
        bytes[*num] = parse8(ptr);
        sum += bytes[*num] & 255;

        ptr += 2;
        (*num)++;

        if (*num >= 256)
            return 0;
    }

    cksum = parse8(ptr);

    if ( ((sum & 255) + (cksum & 255)) & 255 )
        return 0; /* checksum error */

    return 1;
}

int sendFile(const int fd, const char *filename)
{
    int retVal = 0;
    FILE *file = NULL;
    char line[1000];
    char cmd[32];
    int bytes[256], status, n, addr;

    file = fopen(filename, "r");
    if (file)
    {
        while (!feof(file) && !ferror(file))
        {
            fgets(line, 1000, file);
            if ((line[strlen(line)-1] == '\n') || (line[strlen(line)-1] == '\r'))
                line[strlen(line)-1] = 0;
            if (parse_line(line, bytes, &addr, &n, &status))
            {
                switch (status)
                {
                    case 0:
                        memset(cmd, 0, 32);
                        sprintf(cmd, "S%x,%x#", addr, n);
                        write(fd, cmd, strlen(cmd));
                        write(fd, bytes, n);
                        if (getAT91response(fd, psResponse))
                            printf("%s\n", psResponse);
                        break;
                    case 1:
                        printf("Done\n");
                        break;
                    case 2:
                        printf("Got Hex start code\n");
                        break;
                    case 3:
                        printf("Got 16 bit start address\n");
                        break;
                    case 4:
                        addrpref = bytes[0] << 8;
                        addrpref |= bytes[1];
                        addrpref <<= 16;
                        printf("Got extended linear address 0x%08x\n", addrpref);
                        break;
                    case 5:
                        printf("Got 32 bit start address\n");
                        break;
                    default:
                        printf("Error reading hex file\n");
                        retVal = -1;
                        break;
                }
            }
        }
        fclose(file);
    }
    else
    {
        printf("Could not open hex file %s\n", filename);
        retVal = -1;
    }

    return retVal;
}

int program_atmel(const char *filename, bool bForceIt=false)
{
    int retVal = 0;
    fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
    if(fd>0)
    {
        write(fd, "V#", 2);
        if (getAT91response(fd, psResponse))
            printf("Sam-BA Version %s\n", psResponse);
        write(fd, "wFFFFF240,#", 11);
        if (getAT91response(fd, psResponse))
            printf("AT91 Version %s\n", psResponse);
        long version = strtol(&psResponse[2], NULL, 16);
        if ((version & DBGU_CIDR_MASK) == (0x270b0940 & DBGU_CIDR_MASK))
        {
            short vid, pid;
            write(fd, "h00108000,#", 11);
            if (getAT91response(fd, psResponse))
                vid = strtol(&psResponse[2], NULL, 16);
            write(fd, "h00108002,#", 11);
            if (getAT91response(fd, psResponse))
                pid = strtol(&psResponse[2], NULL, 16);
            printf("0x%04x 0x%04x \n", vid, pid);
            if (bForceIt || ((vid == HOBART_VENDOR_ID) && (pid == HOBART_APC_PRINTER_ID)))
            {
                write(fd, "wFFFFFF60,#", 11);
                if (getAT91response(fd, psResponse))
                    printf("Sam-BA Version %s\n", psResponse);

                // do printer stuff here
                if (sendFile(fd, filename))
                {
                    printf("Error loading hex file\n");
                    retVal = -1;
                }
                else
                {
                    if (!bForceIt)
                        write(fd, "G00108200#", 10);
                }
            }
        }
        else
        {
            printf("Wrong ATMEL device on ttyACM0\n");
            retVal = -1;
        }
        close(fd);
    }
    else
    {
        printf("Unable to open ATMEL device. Is it plugged in?\n");
        retVal = -1;
    }
    return retVal;
}

void cleanADir(const util_filesystem::Directory & currentDir, std::string targetToDelete)
{
    util_filesystem::Directory::const_iterator iter;
    const std::string & tmp = currentDir.getFullPath();
    std::string searchStr("");

    searchStr = tmp.substr(tmp.length() - 3);

    if (searchStr == "jar")
    {
        currentDir.select(targetToDelete, util_filesystem::Directory::SELECT_FILES);

        iter = currentDir.begin();
        while ( iter != currentDir.end() )
        {
            util_filesystem::File newFile(currentDir.getFullPath(), iter->getName());

            std::cout << "Deleting File:" << newFile.getFullPath() << std::endl << std::endl;
            if (newFile.exists())
                newFile.remove();

            ++iter;
        }
    }

    currentDir.selectAllSubdirectories();

    iter = currentDir.begin();
    while ( iter != currentDir.end() )
    {
        util_filesystem::Directory newDir("");
        newDir.appendPath(currentDir.getFullPath());

        newDir.appendPath(iter->getName());

        cleanADir(newDir, targetToDelete);

        ++iter;
    }
}

void cleanMOMCache()
{
    util_filesystem::Directory myDir("C:\\Documents and Settings");
    cleanADir(myDir, std::string("mom15.jar*.*"));
}

int processVersionInfo(const std::string & logEntry, int * swtype, int * maj, int * min, int * build )
{
    std::string::size_type p = logEntry.find('.');

    if (p != std::string::npos)
    {
        std::string type = logEntry.substr(0, p);
        std::string remainder = logEntry.substr(p+1);

        sscanf(type.c_str(), "%d", swtype);

        if (remainder.size() > 0)
        {
            p = remainder.find('.');

            std::string majstr;

            if (p == std::string::npos)
            {
                majstr = remainder;
                remainder = "";
            }
            else
            {
                majstr = remainder.substr(0, p);
                remainder = remainder.substr(p+1);
            }

            sscanf(majstr.c_str(), "%d", maj);

            if (remainder.size() > 0)
            {
                p = remainder.find('.');

                std::string minstr;

                if (p == std::string::npos)
                {
                    minstr = remainder;
                    remainder = "";
                }
                else
                {
                    minstr = remainder.substr(0, p);
                    remainder = remainder.substr(p+1);
                }

                sscanf(minstr.c_str(), "%d", min);

                if (remainder.size() > 0)
                {
                    sscanf(remainder.c_str(), "%d", build);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int installApcFimrware(util_env::Properties *properties)
{
    const std::string* pPath = properties->getProperty("firmwareDirectory");

    if (pPath == 0)
        return -1;

    util_filesystem::File apcSrcZip(*pPath, "APC.zip");

    if (!apcSrcZip.exists())
        return -2;

    pPath = properties->getProperty("VersionManagerTempDir");
    util_filesystem::Directory vmTmpDir("");
    vmTmpDir.appendPath(*pPath);

    Hobart::CZip::unZipSingleFile(apcSrcZip.getFullPath().c_str(), APC_ZIP_SOURCE_FILE, vmTmpDir.getFullPath().c_str());

    util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), APC_ZIP_SOURCE_FILE);

    if (firmSourceFile.exists())
    {
        // int i = 0;
        // unsigned char *pTest[10] = {0};

        std::cout << "Attempting to flash APC with new firmware from peripheral source library." << std::endl;

        if (program_atmel(firmSourceFile.getFullPath().c_str()))
        {
            printf("Error programming Atmel\n");
        }
    }
    return 0;
}

int checkLogForNewFirmware(std::string path, int targetType)
{
    std::ifstream appInstLog (path.c_str());

    if (appInstLog.rdbuf( )->is_open( ))
    {
        char entry[32];
        int bytesRead = 0;

        do
        {
            appInstLog.getline(entry, 32);

            bytesRead = appInstLog.gcount();

            if (bytesRead > 0)
            {
                std::string logEntry(entry);
                int nType = 0, maj = 0, min = 0, build = 0;

                processVersionInfo(logEntry, &nType, &maj, &min, &build);

                if (nType == targetType)
                {
                    return 1;
                }
            }
        }
        while (bytesRead > 0);
    }

    return 0;
}

int main(int argc, char* UNUSED(argv[]))
{
    int retVal = 0;

// if you want to test to atmel down load uncomment the next line
// return (program_atmel("AccessWgr_108.a79", true));

    //try to load config.dat from the new software location. if that fails load it from the old software location.
    util_env::Properties *properties = new util_env::Properties(DEFAULT_INTERNAL_CONFIG_FILE_NAME);

    const std::string *pPath;

    if ((!properties) || ( properties->getProperty("VersionManagerTempDir") == 0))
    {
        properties = new util_env::Properties(BACKUP_INTERNAL_CONFIG_FILE_NAME);

        if (!properties)
        {
            std::cout << "FAILED to find config.dat in either the new or old software" << std::endl << std::endl;
            PAL_absclass::PAL_sleep (5000);//sleep for 5s.
            return -1;
        }
    }

    pPath = properties->getProperty("VersionManagerTempDir");

    if (pPath == 0)
        return -1;

    util_filesystem::Directory vmTmpDir("");
    vmTmpDir.appendPath(*pPath);

    std::cout << "Hobart Access Scale Installer" << std::endl << std::endl;
    std::cout << "Installing new software! Please wait!" << std::endl;
    std::cout << "Scale Application will automatically start after installation is complete!" << std::endl;
    std::cout << "Please do NOT reboot or shutdown scale now!" << std::endl << std::endl << std::endl;

    util_filesystem::File appInstallerLog(vmTmpDir.getFullPath(), "appInstaller.log");

    if (appInstallerLog.exists())
    {

        if (checkLogForNewFirmware(appInstallerLog.getFullPath(), HobVersion::ApcPrinterSW))
        {
            installApcFimrware(properties);

            util_filesystem::File APCSourceFile (vmTmpDir.getFullPath(), APC_ZIP_SOURCE_FILE);

            APCSourceFile.remove();
        }


        if (checkLogForNewFirmware(appInstallerLog.getFullPath(), HobVersion::SystemControllerSW))
        {
            util_filesystem::Directory srcBaseDir("");
            srcBaseDir.appendPath(vmTmpDir.getFullPath());
            srcBaseDir.appendPath("SC");

            if (srcBaseDir.exists())
            {
                const std::string *pDestDbPath = properties->getProperty("evolutionDirectory");
                util_filesystem::Directory targetBaseDir ("");
                targetBaseDir.appendPath(*pDestDbPath);

                std::cout << "New system controller software exists" << std::endl;
                std::cout << "path :" << srcBaseDir.getFullPath() << std::endl << std::endl;
                std::cout << "cleaning Mom cache!" << std::endl << std::endl;

                cleanMOMCache();


                // TODO: disable web service

                //install new hobart drivers if they exist.

                util_filesystem::Directory srcDriversDir ("");
                srcDriversDir.appendPath(srcBaseDir.getFullPath());
                srcDriversDir.appendPath("drivers");

                if (srcDriversDir.exists())
                {
                    std::cout << "Installing new drivers ..." << std::endl;

                    util_filesystem::File targetDrvAPCInf ("C:/windows/INF", "wghUsb.inf");

                    targetDrvAPCInf.remove();

                    util_filesystem::File targetDrvAPCSys ("C:/windows/system32/drivers", "weighUsb.sys");

                    targetDrvAPCSys.remove();

                    util_filesystem::File targetDrvSmbusSys ("C:/windows/system32/drivers", "smbus.sys");

                    targetDrvSmbusSys.remove();

                    util_filesystem::File srcDrvAPCInf (srcDriversDir.getFullPath(), "wghUsb.inf");

                    targetDrvAPCInf << srcDrvAPCInf;

                    util_filesystem::File srcDrvAPCSys (srcDriversDir.getFullPath(), "weighUsb.sys");

                    targetDrvAPCSys << srcDrvAPCSys;

                    util_filesystem::File srcDrvSmbusSys (srcDriversDir.getFullPath(), "smbus.sys");

                    targetDrvSmbusSys << srcDrvSmbusSys;

                    srcDriversDir.remove(true);
                }

                {//save the current version.ini file
                    util_filesystem::File origVerIni (targetBaseDir.getFullPath(), *properties->getProperty("VersionInfoFileName"));
                    util_filesystem::File newVerIni (srcBaseDir.getFullPath(), *properties->getProperty("VersionInfoFileName"));

                    if (newVerIni.exists())
                        newVerIni.remove();

                    origVerIni.moveTo(newVerIni);
                }

                {//save the service key if one exists
                    util_filesystem::Directory sourceServiceDir("");
                    sourceServiceDir.appendPath(targetBaseDir.getFullPath());
                    sourceServiceDir.appendPath("Service");


                    if (sourceServiceDir.exists())
                    {
                        util_filesystem::Directory destinationServiceDir("");
                        sourceServiceDir.appendPath(srcBaseDir.getFullPath());
                        sourceServiceDir.appendPath("Service");

                        if (!destinationServiceDir.exists())
                            destinationServiceDir.create();

                        destinationServiceDir << sourceServiceDir;
                    }


                }

                util_filesystem::Directory newEmptyDbDir ("");
                newEmptyDbDir.appendPath(srcBaseDir.getFullPath());
                newEmptyDbDir.appendPath("dbempty");

                const std::string *pVerIndData = properties->getProperty("versionIndependentBackupFile");
                util_filesystem::File fTargetVerIndData (*pVerIndData);
                util_filesystem::File fSrcVerIndData (srcBaseDir.getFullPath(), fTargetVerIndData.getFileName());

                if (fTargetVerIndData.exists())
                {
                    if (newEmptyDbDir.exists())
                    {

                        const std::string *pDestDbPath = properties->getProperty("dbLocation");
                        util_filesystem::Directory targetWorkingDBDir("");
                        targetWorkingDBDir.appendPath(*pDestDbPath);

                        std::cout << "New db exists: " << std::endl;
                        std::cout << "Copying from " << newEmptyDbDir.getFullPath() << std::endl;
                        std::cout << "Copying to: " << targetWorkingDBDir.getFullPath() << std::endl << std::endl;


                        targetWorkingDBDir.remove(true);

                        targetWorkingDBDir.create();

                        targetWorkingDBDir << newEmptyDbDir;

                    }
                    fTargetVerIndData.moveTo(fSrcVerIndData);
                }

                std::cout << "Remove existing system controller software" << std::endl;
                std::cout << "Path :" << targetBaseDir.getFullPath() << std::endl << std::endl;

                const std::string *pTargetSystemPath = properties->getProperty("systemDir");

                util_filesystem::Directory targetSystemDir ("");
                targetSystemDir.appendPath(*pTargetSystemPath);

                targetSystemDir.remove(true);


                util_filesystem::Directory targetEmptyDbDir ("");
                targetEmptyDbDir.appendPath(targetBaseDir.getFullPath());
                targetEmptyDbDir.appendPath("dbempty");

                targetEmptyDbDir.remove(true);

                util_filesystem::Directory targetScriptsDir ("");
                targetScriptsDir.appendPath(targetBaseDir.getFullPath());
                targetScriptsDir.appendPath("scripts");

                targetScriptsDir.remove(true);

                const std::string *pTargetWebPagesPath = properties->getProperty("webPagesDir");

                util_filesystem::Directory targetWebPagesDir ("");
                targetWebPagesDir.appendPath(*pTargetWebPagesPath);

                targetWebPagesDir.removeContents(false);

                util_filesystem::Directory targetImagesDir ("");
                targetImagesDir.appendPath(targetWebPagesDir.getFullPath());
                targetImagesDir.appendPath("images");

                targetImagesDir.removeContents(false);

                util_filesystem::Directory targetEppDir ("");
                targetEppDir.appendPath(targetImagesDir.getFullPath());
                targetEppDir.appendPath("epp");

                targetEppDir.remove(true);

                util_filesystem::Directory targetEscDir ("");
                targetEscDir.appendPath(targetImagesDir.getFullPath());
                targetEscDir.appendPath("esc");

                targetEscDir.remove(true);

                util_filesystem::Directory targetWinDir ("");
                targetWinDir.appendPath(targetBaseDir.getFullPath());
                targetWinDir.appendPath("win");

                targetWinDir.remove(true);

                targetBaseDir.removeContents();

                std::cout << "Copying over new system controller software: " << std::endl;
                std::cout << "Copying from " << srcBaseDir.getFullPath() << std::endl;
                std::cout << "Copying to " << targetBaseDir.getFullPath() << std::endl << std::endl << std::endl;

                if (fSrcVerIndData.exists())
                    fSrcVerIndData.moveTo(fTargetVerIndData);

                targetBaseDir << util_filesystem::contents << util_filesystem::recursive << srcBaseDir;

                srcBaseDir.remove(true);
            }
        }

        appInstallerLog.remove();
    }

    delete properties;
    return retVal;
}
