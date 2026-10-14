//
// File : LinuxUtil.cpp
//
//

#include <string>
#include <iostream>
#include <dirent.h>
//#include <fstream>

#include <sys/vfs.h>
#include <sys/ioctl.h>

#include "LinuxUtil.h"
#include "../checksums/checksum.h"
#include "../e_string/utilstring.h"
#include "../filesys/filename.h"

#include "../../../util/Debug.h"

//#include "../ptr/prt.h"



#ifndef NO_NAMESPACES
namespace util_platform
{
#endif

using namespace std;

#ifndef NO_NAMESPACES
using namespace util_time;
#endif

class Linux_FileSearch : public Platform::FileSearch
{
public:
    // Types, subtypes, and friends
//    friend bool Win32::findFileNext(Platform::FileSearch* fs) const;  /* (SJJC port 6/19/06) */   (win32 only ?)

    // Constructors and destructor
    Linux_FileSearch(DIR *pDir, struct dirent *pEnt, string base, string mask);
    virtual ~Linux_FileSearch();

    // Accessor functions
    std::string getFileName() const;
    //util_time::DateTime getCreated() const;   (win32 only ?)
    util_time::DateTime getModified() const;
    bool isFile() const;
    bool isDirectory() const;
    long64 getSize() const;

    DIR *getOpenDirectory()             { return pOpenDir; }
    void setEntry(struct dirent* pEnt)  { pEntry = pEnt; }
    const string &getBaseDir()          { return baseDir; }
    const string &getFileMask()         { return fileMask; }

private:
    DIR*    pOpenDir;
    struct  dirent* pEntry;
    string  baseDir;
    string  fileMask;
};


LinuxUtil::LinuxUtil()
    : datetime_(0), date_(0), time_(0)
{
    initializeLocaleMaps();
}

LinuxUtil::~LinuxUtil()
{
    if ( datetime_ )
        delete datetime_;
    if ( date_ )
        delete date_;
    if ( time_ )
        delete time_;
}

void LinuxUtil::initializeLocaleMaps()
{
    _languages_.insert(make_pair(std::string("en"), std::string("English")));
    _languages_.insert(make_pair(std::string("es"), std::string("Spanish")));
    _languages_.insert(make_pair(std::string("fr"), std::string("French")));

    _countries_.insert(make_pair(std::string("US"), std::string("United States")));
    _countries_.insert(make_pair(std::string("MX"), std::string("Mexico")));
    _countries_.insert(make_pair(std::string("CA"), std::string("Canada")));
}

const util_time::DateTime& LinuxUtil::getCurrentDateTime() const
{
    struct tm time;
    struct timespec clock;

    if ( !datetime_ )
        datetime_ = new DateTime();

    if ( clock_gettime(CLOCK_REALTIME, &clock) == OK )
    {
        localtime_r(&clock.tv_sec, &time);

        datetime_->setDate(time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
        datetime_->setTime(time.tm_hour, time.tm_min, time.tm_sec, clock.tv_nsec / 1000000);
    }

    return *datetime_;
}

const util_time::DateTime& LinuxUtil::getCurrentDateTime(util_time::DateTime& dt) const
{
    struct tm time;
    struct timespec clock;

    if ( clock_gettime(CLOCK_REALTIME, &clock) == OK )
    {
        localtime_r(&clock.tv_sec, &time);

        dt.setDate(time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
        dt.setTime(time.tm_hour, time.tm_min, time.tm_sec, clock.tv_nsec / 1000000);
    }

    return dt;
}

void LinuxUtil::setCurrentDateTime(const util_time::DateTime& dt)
{
    struct tm time;
    struct timespec clock;

    time.tm_year = dt.getYear() - 1900;
    time.tm_mon = dt.getMonth() - 1;
    time.tm_mday = dt.getMonthDay();
    time.tm_hour = dt.getHour();
    time.tm_min = dt.getMinute();
    time.tm_sec = dt.getSecond();

    clock.tv_sec = mktime(&time);
    clock.tv_nsec = dt.getMillisecond() * 1000000;

    if ( clock.tv_sec == (time_t)-1 || clock_settime(CLOCK_REALTIME, &clock) != OK )
    {
        ;           // Throw exception
    }
}

const util_time::Date& LinuxUtil::getCurrentDate() const
{
    if ( !date_ )
        date_ = new util_time::Date();

    getCurrentDate( *date_ );

    return *date_;
}

const util_time::Date& LinuxUtil::getCurrentDate(util_time::Date& dt) const
{
    struct tm time;
    struct timespec clock;

    if ( clock_gettime(CLOCK_REALTIME, &clock) == OK )
    {
        localtime_r(&clock.tv_sec, &time);

        dt.setDate(time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
    }

    return dt;
}

void LinuxUtil::setCurrentDate(const util_time::Date& dt)
{
    struct tm time;
    struct timespec clock;

    getCurrentDateTime();

    time.tm_year = dt.getYear() - 1900;
    time.tm_mon = dt.getMonth() - 1;
    time.tm_mday = dt.getMonthDay();
    time.tm_hour = datetime_->getHour();
    time.tm_min = datetime_->getMinute();
    time.tm_sec = datetime_->getSecond();

    clock.tv_sec = mktime(&time);
    clock.tv_nsec = datetime_->getMillisecond() * 1000000;

    if ( clock.tv_sec == (time_t)-1 || clock_settime(CLOCK_REALTIME, &clock) != OK )
    {
        ;           // Throw exception
    }
}

const util_time::Time& LinuxUtil::getCurrentTime() const
{
    struct tm time;
    struct timespec clock;

    if ( !time_ )
        time_ = new util_time::Time();

    if ( clock_gettime(CLOCK_REALTIME, &clock) == OK )
    {
        localtime_r(&clock.tv_sec, &time);

        time_->setTime(time.tm_hour, time.tm_min, time.tm_sec, clock.tv_nsec / 1000000);
    }

    return *time_;
}

const util_time::Time& LinuxUtil::getCurrentTime(util_time::Time& tm) const
{
    struct tm time;
    struct timespec clock;

    if ( clock_gettime(CLOCK_REALTIME, &clock) == OK )
    {
        localtime_r(&clock.tv_sec, &time);

        tm.setTime(time.tm_hour, time.tm_min, time.tm_sec, clock.tv_nsec / 1000000);
    }

    return tm;
}

void LinuxUtil::setCurrentTime(const util_time::Time& t)
{
    struct tm time;
    struct timespec clock;

    getCurrentDateTime();

    time.tm_year = datetime_->getYear() - 1900;
    time.tm_mon = datetime_->getMonth() - 1;
    time.tm_mday = datetime_->getMonthDay();
    time.tm_hour = t.getHour();
    time.tm_min = t.getMinute();
    time.tm_sec = t.getSecond();

    clock.tv_sec = mktime(&time);
    clock.tv_nsec = t.getMillisecond() * 1000000;

    if ( clock.tv_sec == (time_t)-1 || clock_settime(CLOCK_REALTIME, &clock) != OK )
    {
        ;           // Throw exception
    }
}

bool LinuxUtil::isRootDirectory(const std::string& filename) const
{
    return (filename.size() == 1) && (filename[0] == slash());
}

bool LinuxUtil::fileExists(const std::string& filename) const
{
    struct stat stats;
    return ( stat(const_cast<char *>(filename.c_str()), &stats) != ERROR );
}

DateTime LinuxUtil::getFileModificationDate(const string& filename) const
{
    struct stat stats;
    if ( stat(const_cast<char *>(filename.c_str()), &stats) != ERROR )
    {
        struct tm dt;
        localtime_r(&stats.st_mtime, &dt);

        DateTime date(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
        date.setTime(dt.tm_hour, dt.tm_min, dt.tm_sec);

        return date;
    }

    throw Platform::FileNotFound(filename);
}


long64 LinuxUtil::fileSize(const std::string& filename) const
{
    struct stat stats;
    if ( stat(const_cast<char *>(filename.c_str()), &stats) != ERROR )
        return stats.st_size;

    return 0;
}

unsigned int LinuxUtil::fileChecksum(const std::string& filename) const
{
    int blockSize = 0;
    unsigned int checksum = 0;

    // Open source file
    int sfd = open(filename.c_str(), O_RDONLY, 0666);
    if ( sfd == ERROR )
        throw Platform::FileNotFound(filename);

    // Determine read buffer size
    struct statfs stats;
    if ( fstatfs(sfd, &stats) == OK )
        blockSize = 4 * stats.f_bsize;
    else
        blockSize = 512;

    // Proceed to read the source file and calculate its checksum
    bool done = false;
    unsigned char* buffer = new unsigned char[blockSize];
    while ( !done )
    {
        int bytesRead = read(sfd, (char*)buffer, blockSize);
        if ( bytesRead == blockSize )
        {
            checksum = util_checksums::icrc(checksum, buffer, bytesRead);
        }
        else
        {
            if ( 0 < bytesRead && bytesRead < blockSize )
                checksum = util_checksums::icrc(checksum, buffer, bytesRead);
            else if ( bytesRead )
                throw Platform::FileAccessError("reading " + filename);

            done = true;
        }
    }

    delete[] buffer;

    // Close source file
    close(sfd);

    return checksum;
}

Platform::FileSearch* LinuxUtil::findFileFirst(const std::string& search) const
{
    string baseDir;
    string mask;
    string entryName;
    string::size_type index;
    register DIR *pDir = NULL;
    struct dirent *pEntry = NULL;

    index = search.rfind ('/');

    if (index != string::npos)
    {
        baseDir = search.substr(0, index+1);
        mask = search.substr(index+1);

        pDir = opendir(const_cast<char *>(baseDir.c_str()));
        if (pDir != NULL)
        {
            for( pEntry = readdir(pDir); pEntry != NULL ;
                pEntry = readdir(pDir))
            {
                entryName = pEntry->d_name;
                if ( util_string::comparetomask(entryName.c_str(), mask.c_str()) == 0 )
                {
                    return new Linux_FileSearch(pDir, pEntry, baseDir, mask);
                }
            }
        }
    }

    if (pDir != NULL)
        closedir (pDir);

    return NULL;
}

bool LinuxUtil::findFileNext(Platform::FileSearch* fs) const
{
    register DIR *pDir = NULL;
    struct dirent *pEntry = NULL;
    Linux_FileSearch* file = dynamic_cast<Linux_FileSearch*>(fs);
    string entryName;
    const string &rMask = file->getFileMask();

    pDir = file->getOpenDirectory();
    if (pDir != NULL)
    {
        for ( pEntry = readdir(pDir); pEntry != NULL ; pEntry = readdir(pDir) )
        {
            entryName = pEntry->d_name;
            if ( util_string::comparetomask(entryName.c_str(), rMask.c_str()) == 0 )
            {
                file->setEntry(pEntry);
                return true;
            }
        }
    }

    return false;
}

void LinuxUtil::findFileDelete(Platform::FileSearch* fs) const
{
    delete fs;
}


bool LinuxUtil::copyFile(const string& source, const string& destination) const
{
    bool failed = false;

    // Open source file
    int sfd = open(source.c_str(), O_RDONLY, 0666);
    if ( sfd == ERROR )
        return false;

    // Open destination file
    int dfd = open(destination.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if ( dfd == ERROR )
    {
        close(sfd);
        return false;
    }

    // Determine read buffer size
    struct statfs stats;
    if ( fstatfs(sfd, &stats) == ERROR )
        failed = true;

    // Proceed to copy the file
    if ( !failed )
    {
        int bytesRead, bytesWritten;
        int blockSize = 4 * stats.f_bsize;
        char* buffer = new char[blockSize];

        // Proceed to read the source file and write the destination file
        bool done = false;
        while ( !done )
        {
            bytesRead = read(sfd, buffer, blockSize);
            if ( 0 <= bytesRead && bytesRead <= blockSize )
            {
                if ( bytesRead )
                {
                    bytesWritten = write(dfd, buffer, bytesRead);
                    if ( bytesWritten != bytesRead )
                        failed = true;
                }
            }
            else
                failed = true;

            if ( failed )
                break;

            if ( 0 <= bytesRead && bytesRead < blockSize )
                done = true;
        }

        delete[] buffer;
    }

    // If we failed to copy the file, truncate the destination file
    if ( failed )
    {
        int result = ftruncate(dfd, 0);

		if (result == -1)
			Debug::trace("LinuxUtil", "ftruncate failed!");
    }

    close(sfd);
    close(dfd);

    // If we failed, attempt also to delete the file (if it was a file what we opened)
    if ( failed )
    {
        deleteFile(destination);
        return false;
    }

    return true;
}

bool LinuxUtil::appendFile(const std::string& source, const std::string& destination) const
{
    int blockSize = 512, destinationSize = -1;


    // Open source file
    int sfd = open(source.c_str(), O_RDONLY, 0666);
    if ( sfd == ERROR )
        return false;


    // Open destination file
    //int dfd = open(destination.c_str(), O_APPEND | O_CREAT, 0666);
    int dfd = open(destination.c_str(), O_WRONLY | O_CREAT, 0666);
    if ( dfd == ERROR )
    {
        close(sfd);
        return false;
    }
    else
    {
        struct stat stats;
        if ( fstat(dfd, &stats) == OK )
            destinationSize = stats.st_size;

        // We place the pointer at the end of the file because the 0_APPEND attribute
        // does not work (not supported) in VxWorks
        lseek(dfd, 0, SEEK_END);
    }


    // Determine read buffer size
    struct statfs stats;
    if ( fstatfs(sfd, &stats) == OK )
        blockSize = 4 * stats.f_bsize;


    // Proceed to read the source file and write the destination file
    char* buffer = new char[blockSize];
    bool done = false, failed = false;
    while ( !done && !failed )
    {
        int bytesRead = read(sfd, buffer, blockSize);
        if ( 0 <= bytesRead && bytesRead <= blockSize )
        {
            // If you read something, append it.
            if ( bytesRead )
            {
                int bytesWritten = write(dfd, buffer, bytesRead);
                if ( bytesWritten != bytesRead )
                    failed = true;
            }

            if ( bytesRead < blockSize )
                done = true;
        }
        else
            failed = true;
    }
    delete[] buffer;


    // If we failed to append the file, truncate the destination file to its original size
    if ( failed && destinationSize != -1 )
    {
        int result = ftruncate(dfd, destinationSize);
    
		if (result == -1)
			Debug::trace("LinuxUtil", "ftruncate failed!");
	}

    close(sfd);
    close(dfd);

    return !failed;
}

bool LinuxUtil::moveFile(const string& oldname, const string& newname) const
{
    return rename(oldname.c_str(), newname.c_str()) == OK;
}

bool LinuxUtil::deleteFile(const string& filename) const
{
    return unlink(const_cast<char *>(filename.c_str())) == OK;
}

bool LinuxUtil::createDirectory(const std::string& dirname) const
{
    return mkdir(const_cast<char *>(dirname.c_str()), DIR_CREATE_MODE) == OK;
}

bool LinuxUtil::moveDirectory(const std::string& oldname, const std::string& newname) const
{
    return rename(oldname.c_str(), newname.c_str()) == OK;
}

bool LinuxUtil::deleteDirectory(const std::string& dirname) const
{
    return rmdir(const_cast<char *>(dirname.c_str())) == OK;
}

long64 LinuxUtil::fileSystemFreeSpace(const std::string& fs) const
{
    long64 space = (long64)-1;
    int fd = open(fs.c_str(), O_RDONLY, 0666);
    if ( fd != ERROR )
    {
        struct statfs stats;
        if ( fstatfs(fd, &stats) != ERROR )
            space = stats.f_bsize * (long64)stats.f_bfree;

        close(fd);
    }

    return space;
}

long64 LinuxUtil::fileSystemUsedSpace(const std::string& fs) const
{
    long64 space = (long64)-1;
    int fd = open(fs.c_str(), O_RDONLY, 0666);
    if ( fd != ERROR )
    {
        struct statfs stats;
        if ( fstatfs(fd, &stats) != ERROR )
            space = stats.f_bsize * (long64)(stats.f_blocks - stats.f_bfree);

        close(fd);
    }

    return space;
}

long64 LinuxUtil::fileSystemTotalSpace(const std::string& fs) const
{
    long64 space = (long64)-1;
    int fd = open(fs.c_str(), O_RDONLY, 0666);
    if ( fd != ERROR )
    {
        struct statfs stats;
        if ( fstatfs(fd, &stats) != ERROR )
            space = stats.f_bsize * (long64)stats.f_blocks;

        close(fd);
    }

    return space;
}

#if 0   // Used by db/cb/access/fileiowp.cpp, but it is not compiled.
const char* LinuxUtil::getErrorMessage(int UNUSED(err)) const
{
    return "UNKNOWN ERRNO";
}

int LinuxUtil::getPosixErrno(int err) const
{
    return err;
}
#endif

bool LinuxUtil::delay(DWORD milliseconds) const
{
    return (usleep( milliseconds * 1000 ) == OK);
}

void LinuxUtil::getLocaleString(std::string & localeString, const std::string & language, const std::string & countryCode)
{
	localeString = "";	
	
	if ( strcmp(language.c_str(),"xx") && strcmp(countryCode.c_str(), "XX") )	
		localeString = language + "_" + countryCode + ".utf8";    
}
	
void LinuxUtil::getLocaleString(std::string & destLocale, const std::string & srcLocale)
{
	destLocale = "";

	std::string::size_type begPos = srcLocale.find("_");
		
	if ( begPos == std::string::npos )
        return;    
	
	if (strcmp(srcLocale.c_str(), "xx_XX"))	
		destLocale = srcLocale + ".utf8";
}

void LinuxUtil::setupLocale(const std::string & language, const std::string & countryCode)
{
	string localeStr = "";	
	
	//build the locale string from the passed in arguments	
	getLocaleString(localeStr, language, countryCode);

	locale currentLocale = locale(localeStr.c_str());
	locale::global(currentLocale);	
}

void LinuxUtil::reverseLocaleString(std::string & destLocale, const std::string & srcLocale)
{
    std::string::size_type begPos = srcLocale.find("_");
    if ( begPos == std::string::npos )
        return;


    std::string tmpLang(srcLocale.substr(0,begPos));
    std::string tmpCountry(srcLocale.substr(begPos+1));

    std::string::size_type secondPos = tmpCountry.find(".");
    if ( begPos != std::string::npos )
        tmpCountry = tmpCountry.substr(0, secondPos);

	destLocale = tmpLang + "_" + tmpCountry;	
}

//-----------------------------------------------------------------------------

Linux_FileSearch::Linux_FileSearch(DIR *pDir, struct dirent *pEnt, string base, string mask)
    : pOpenDir(pDir), pEntry(pEnt), baseDir(base), fileMask(mask)
{
}

Linux_FileSearch::~Linux_FileSearch()
{
    if (pOpenDir != NULL)
        closedir (pOpenDir);
}

string Linux_FileSearch::getFileName() const
{
    return pEntry->d_name;
}

#if 0   // From Win32.  It's good seems no one use this function
util_time::DateTime Linux_FileSearch::getCreated() const
{
    // First use FileTimeToLocalFileTime to convert the UTC time to local time
    PAL_FILETIME localTime;
    if ( PAL_absclass::PAL_fileTimeToLocalFileTime(&descriptor->ftCreationTime, &localTime) )
    {
        // Now use FileTimeToSystemTime to convert the local PAL_FILETIME to PAL_TIME_STRUCT structure
        PAL_TIME_STRUCT systemTime;
        if ( PAL_absclass::PAL_fileTimeToSystemTime(&localTime, &systemTime) )
        {
            util_time::DateTime date(systemTime.wYear, systemTime.wMonth - 1, systemTime.wDay);

            date.setTime(systemTime.wHour, systemTime.wMinute, systemTime.wSecond,
                    systemTime.wMilliseconds);

            return date;
        }
    }

    throw util_time::Date::InvalidDate("Unable to obtain a date");
}
#endif

using namespace util_filesystem;

DateTime Linux_FileSearch::getModified() const
{
    struct stat stats;
    Filename name(baseDir, pEntry->d_name);

    if ( stat(const_cast<char *>(name.getFullPath().c_str()), &stats) == OK )
    {
        struct tm dt;
        localtime_r(&stats.st_mtime, &dt);

        DateTime date(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday);
        date.setTime(dt.tm_hour, dt.tm_min, dt.tm_sec);

        return date;
    }

    throw Platform::FileNotFound(name.getFullPath());
}

bool Linux_FileSearch::isFile() const
{
    string entryPath;
    struct stat dirEntStat;

    entryPath = baseDir + pEntry->d_name;
    if (stat(const_cast<char *>(entryPath.c_str()), &dirEntStat) == OK)
    {
        return S_ISREG(dirEntStat.st_mode);
    }
    return false;
}

bool Linux_FileSearch::isDirectory() const
{
    string entryPath;
    struct stat dirEntStat;

    entryPath = baseDir + pEntry->d_name;
    if (stat(const_cast<char *>(entryPath.c_str()), &dirEntStat) == OK)
    {
        return S_ISDIR(dirEntStat.st_mode);
    }
    return false;
}

long64 Linux_FileSearch::getSize() const
{
    struct stat stats;
    Filename name(baseDir, pEntry->d_name);

    if ( stat(const_cast<char *>(name.getFullPath().c_str()), &stats) == OK )
    {
        return stats.st_size;
    }

    return 0;
}


const std::string LinuxUtil::_lineSeparator_ = "\x0a";
const std::string LinuxUtil::_altLineSeparator_ = "\x0d\x0a";
const std::string LinuxUtil::_allFiles_ = "*";


#ifndef NO_NAMESPACES
} // namespace util_platform
#endif

