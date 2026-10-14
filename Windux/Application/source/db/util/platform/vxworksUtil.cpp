#include <string>
#include <iostream>
#include <dirent.h>


#include "stat.h"
#include "ioLib.h"
#include "taskLib.h"
#include "sysLib.h"
#include "time.h"
#include "nfsDrv.h"
#include "iosLib.h"
#include "time.h"
#include "timers.h"


#include "vxworksUtil.h"
#include "../ptr/ptr.h"
#include "../e_string/utilstring.h"
#include "../filesys/filename.h"
#include "../checksums/checksum.h"


#ifndef NO_NAMESPACES
namespace util_platform
{
#endif


using namespace std;


#ifndef NO_NAMESPACES
using namespace util_checksums;
#endif


class VxWorks_FileSearch : public Platform::FileSearch
{
	public:
		// Types, subtypes, and friends

		// Constructors and destructor
		VxWorks_FileSearch(DIR *pDir, struct dirent *pEnt, string base, string mask);
		virtual ~VxWorks_FileSearch();

		// Accessor functions
		std::string getFileName() const;
		DateTime getModified() const;
		bool isFile() const;
		bool isDirectory() const;
		long64 getSize() const;

		DIR *getOpenDirectory() { return pOpenDir; }
		void setEntry(struct dirent* pEnt) { pEntry = pEnt; }
		const string &getBaseDir() { return baseDir; }
		const string &getFileMask() { return fileMask; }


	private:
		DIR* pOpenDir;
		struct dirent* pEntry;
		string baseDir;
		string fileMask;
};


VxWorksUtil::VxWorksUtil() 
		: ticksPerSecond_(sysClkRateGet()), datetime_(0), date_(0), time_(0)
{
}


VxWorksUtil::~VxWorksUtil()
{
	if ( datetime_ )
		delete datetime_;
	if ( date_ )
		delete date_;
	if ( time_ )
		delete time_;
}


const DateTime& VxWorksUtil::getCurrentDateTime() const
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


const DateTime& VxWorksUtil::getCurrentDateTime(DateTime& dt) const
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


void VxWorksUtil::setCurrentDateTime(const DateTime& dt)
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
		;			// Throw exception
}


const Date& VxWorksUtil::getCurrentDate() const
{
	struct tm time;
	struct timespec clock;

	if ( !date_ )
		date_ = new Date();

	if ( clock_gettime(CLOCK_REALTIME, &clock) == OK )
	{
		localtime_r(&clock.tv_sec, &time);

		date_->setDate(time.tm_year + 1900, time.tm_mon + 1, time.tm_mday);
	}

	return *date_;
}


const Date& VxWorksUtil::getCurrentDate(Date& dt) const
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


void VxWorksUtil::setCurrentDate(const Date& dt)
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
		;			// Throw exception
}


const Time& VxWorksUtil::getCurrentTime() const
{
	struct tm time;
	struct timespec clock;

	if ( !time_ )
		time_ = new Time();

	if ( clock_gettime(CLOCK_REALTIME, &clock) == OK )
	{
		localtime_r(&clock.tv_sec, &time);

		time_->setTime(time.tm_hour, time.tm_min, time.tm_sec, clock.tv_nsec / 1000000);
	}

	return *time_;
}


const Time& VxWorksUtil::getCurrentTime(Time& tm) const
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


void VxWorksUtil::setCurrentTime(const Time& t)
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
		;			// Throw exception
}


bool VxWorksUtil::isRootDirectory(const std::string& filename) const
{
	return (filename.size() == 1) && (filename[0] == slash());
}


bool VxWorksUtil::fileExists(const std::string& filename) const
{
	struct stat stats;
	return ( stat(const_cast<char *>(filename.c_str()), &stats) != ERROR );
}


DateTime VxWorksUtil::getFileModificationDate(const std::string& filename) const
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


long64 VxWorksUtil::fileSize(const std::string& filename) const
{
	struct stat stats;
	if ( stat(const_cast<char *>(filename.c_str()), &stats) != ERROR )
		return stats.st_size;

	return 0;
}


unsigned int VxWorksUtil::fileChecksum(const std::string& filename) const
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
			checksum = icrc(checksum, buffer, bytesRead);
		}
		else 
		{
			if ( 0 < bytesRead && bytesRead < blockSize )
				checksum = icrc(checksum, buffer, bytesRead);
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


Platform::FileSearch* VxWorksUtil::findFileFirst(const std::string& search) const
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
				if (comparetomask(entryName.c_str(), mask.c_str()) == 0)
				{
					return new VxWorks_FileSearch(pDir, pEntry, baseDir, mask);
				}
			}
		}
	}

	if (pDir != NULL)
		closedir (pDir);

	return NULL;
}


bool VxWorksUtil::findFileNext(Platform::FileSearch* fs) const
{
	register DIR *pDir = NULL;
	struct dirent *pEntry = NULL;
	VxWorks_FileSearch* file = dynamic_cast<VxWorks_FileSearch*>(fs);
	string entryName;
	const string &rMask = file->getFileMask();				
	
	pDir = file->getOpenDirectory();
	if (pDir != NULL)
	{
		for ( pEntry = readdir(pDir); pEntry != NULL ; pEntry = readdir(pDir) )
		{
			entryName = pEntry->d_name;
			if ( comparetomask(entryName.c_str(), rMask.c_str()) == 0 )
			{
				file->setEntry(pEntry);

				return true;
			}
		}
	}


	return false;

}


void VxWorksUtil::findFileDelete(Platform::FileSearch* fs) const
{
	delete fs;
}


bool VxWorksUtil::copyFile(const std::string& source, const std::string& destination) const
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
		ioctl(dfd, FIOTRUNC, 0);

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


bool VxWorksUtil::appendFile(const std::string& source, const std::string& destination) const
{
	int blockSize = 512, destinationSize = -1;


	// Open source file
	int sfd = open(source.c_str(), O_RDONLY, 0666);
	if ( sfd == ERROR )
		return false;


	// Open destination file
//	int dfd = open(destination.c_str(), O_APPEND | O_CREAT, 0666);
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
		ioctl(dfd, FIOTRUNC, destinationSize);

	close(sfd);
	close(dfd);

	return !failed;
}


bool VxWorksUtil::moveFile(const std::string& oldname, const std::string& newname) const
{
	// Possible optimization: use "rename" if they are in the same directory
	// [use ioctl(, FIORENAME, newname.c_str())]
//	if ( copyFile(oldname, newname) )
//		return deleteFile(oldname);

//	return false;

	// It looks that rename should do the job
	return rename(oldname.c_str(), newname.c_str()) == OK;
}


bool VxWorksUtil::deleteFile(const std::string& filename) const
{
	return unlink(const_cast<char *>(filename.c_str())) == OK;
}


bool VxWorksUtil::createDirectory(const std::string& dirname) const
{
	return mkdir(const_cast<char *>(dirname.c_str())) == OK;
}


bool VxWorksUtil::moveDirectory(const std::string& oldname, const std::string& newname) const
{
	return rename(oldname.c_str(), newname.c_str()) == OK;
}


bool VxWorksUtil::deleteDirectory(const std::string& dirname) const
{
	return rmdir(const_cast<char *>(dirname.c_str())) == OK;
}


long64 VxWorksUtil::fileSystemFreeSpace(const std::string& fs) const
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


long64 VxWorksUtil::fileSystemUsedSpace(const std::string& fs) const
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


long64 VxWorksUtil::fileSystemTotalSpace(const std::string& fs) const
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


const char* VxWorksUtil::getNFSRootDirectory() const
{
	static char nfsRootDirectory[3] = "\0";

	if ( !*nfsRootDirectory )
	{
		int nfsDrvNum = nfsDrvNumGet();
		char device[6] = "/null";
		char *pTail;
		DEV_HDR* pHeader = iosDevFind(device, &pTail);
		while ( pHeader && pHeader->drvNum != nfsDrvNum )
			pHeader = iosNextDevGet(pHeader);

		if ( pHeader && pHeader->drvNum == nfsDrvNum )
		{
			strncpy(nfsRootDirectory, pHeader->name, 2);
			nfsRootDirectory[2] = 0;
		}
	}

	return nfsRootDirectory;
}


const char* VxWorksUtil::getErrorMessage(int err) const
{
	switch ( err )
	{
		case 0x000D0004:
			return "S_iosLib_TOO_MANY_OPEN_FILES";
		case 0x00380003:
			return "S_dosFsLib_FILE_NOT_FOUND";
		default:
			return "UNKNOWN_ERRNO";
	}
}


int VxWorksUtil::getPosixErrno(int err) const
{
	switch ( err )
	{
		case 0x000D0004:
			return EMFILE;      // "S_iosLib_TOO_MANY_OPEN_FILES";
		case 0x00380003:
			return ENOENT;      // "S_dosFsLib_FILE_NOT_FOUND";
		default:
			return err;
	}
}


bool VxWorksUtil::delay(int milliseconds) const
{
	return taskDelay(getTicks(milliseconds)) == OK;
}

bool VxWorksUtil::getLocaleString(std::string & localeString, const std::string & language,
				const std::string & countryCode)
{
	localeString = language + "_" + countryCode;
}

void VxWorksUtil::getLocaleString(std::string & destLocale, const std::string & srcLocale)
{
	destLocale = srcLocale;
}

void VxWorksUtil::reverseLocaleString(std::string & destLocale, const std::string & srcLocale)
{
	destLocale = srcLocale;
}

VxWorks_FileSearch::VxWorks_FileSearch(DIR *pDir, struct dirent *pEnt, string base, string mask)
	: pOpenDir(pDir), pEntry(pEnt), baseDir(base), fileMask(mask)
{
}


VxWorks_FileSearch::~VxWorks_FileSearch()
{
	if (pOpenDir != NULL)
		closedir (pOpenDir);
}


string VxWorks_FileSearch::getFileName() const
{
	return pEntry->d_name;
}


DateTime VxWorks_FileSearch::getModified() const
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


bool VxWorks_FileSearch::isFile() const
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


bool VxWorks_FileSearch::isDirectory() const
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


long64 VxWorks_FileSearch::getSize() const
{
	struct stat stats;
	Filename name(baseDir, pEntry->d_name);

	if ( stat(const_cast<char *>(name.getFullPath().c_str()), &stats) == OK )
	{
		return stats.st_size;
	}

	return 0;
}


const std::string VxWorksUtil::_lineSeparator_ = "\x0a";
const std::string VxWorksUtil::_altLineSeparator_ = "\x0d\x0a";
const std::string VxWorksUtil::_allFiles_ = "*";



#ifndef NO_NAMESPACES
} // namespace util_platform
#endif

