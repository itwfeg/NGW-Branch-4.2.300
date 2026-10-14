#include "PAL_platformheader.h"

#include <string>
#include <iostream>
#include <fstream>

#include "win32.h"

#if defined (LINUX_FIX)
#if !defined (PAL_WIN32_RUN)
#include "../checksums/checksum.h"
#  include "../e_string/utilstring.h"
#else
#include "..\checksums\checksum.h"
#  include "..\util\e_string\utilstring.h"
#endif      // PAL_WIN32_RUN
#else
#include "..\checksums\checksum.h"
#  include "..\util\e_string\utilstring.h"
#endif      // LINUX_FIX

#ifndef NO_NAMESPACES
namespace util_platform
{
#endif


using namespace std;
#ifndef NO_NAMESPACES
using namespace util_time;
#endif


class Win32_FileSearch : public Platform::FileSearch
{
	public:
		// Types, subtypes, and friends
		friend bool Win32::findFileNext(Platform::FileSearch* fs) const;  /* (SJJC port 6/19/06) */

		// Constructors and destructor
		Win32_FileSearch(HANDLE* h, PAL_FIND_DATAA* desc); 
		virtual ~Win32_FileSearch();

		// Accessor functions
		std::string getFileName() const;
		util_time::DateTime getCreated() const;
		util_time::DateTime getModified() const;
		bool isFile() const;
		bool isDirectory() const;
		long64 getSize() const;

	private:
		HANDLE * handle;
		PAL_FIND_DATAA * descriptor;
};

Win32::Win32() : datetime_(0), date_(0), time_(0)  /* (SJJC port 6/19/06) */
{
	initializeLocaleMaps();
}


Win32::~Win32()
{
	_languages_.clear();
	_countries_.clear();

	if ( datetime_ )
		delete datetime_;
	if ( date_ )  /* (SJJC port 6/19/06) */
		delete date_;
	if ( time_ )
		delete time_;
}

void Win32::initializeLocaleMaps()
{
	_languages_.insert(make_pair(std::string("en"), std::string("English")));
	_languages_.insert(make_pair(std::string("es"), std::string("Spanish")));
	_languages_.insert(make_pair(std::string("fr"), std::string("French")));

	_countries_.insert(make_pair(std::string("US"), std::string("United States")));
	_countries_.insert(make_pair(std::string("MX"), std::string("Mexico")));
	_countries_.insert(make_pair(std::string("CA"), std::string("Canada")));
}

const util_time::DateTime& Win32::getCurrentDateTime() const
{
	PAL_TIME_STRUCT time;
        PAL_absclass::PAL_getLocalTime(&time);

	if ( !datetime_ )
		datetime_ = new util_time::DateTime();

	datetime_->setDate(time.wYear, time.wMonth, time.wDay);
	datetime_->setTime(time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	return *datetime_;
}


const util_time::DateTime& Win32::getCurrentDateTime(util_time::DateTime& dt) const
{
	PAL_TIME_STRUCT time;
	PAL_absclass::PAL_getLocalTime(&time);

	dt.setDate(time.wYear, time.wMonth, time.wDay);
	dt.setTime(time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	return dt;
}


void Win32::setCurrentDateTime(const util_time::DateTime& /*dt*/)
{
}


const util_time::Date& Win32::getCurrentDate() const
{
	PAL_TIME_STRUCT time;
	PAL_absclass::PAL_getLocalTime(&time);

	if ( !date_ )
		date_ = new util_time::Date();

	date_->setDate(time.wYear, time.wMonth, time.wDay);

	return *date_;
}


const util_time::Date& Win32::getCurrentDate(util_time::Date& dt) const
{
	PAL_TIME_STRUCT time;
	PAL_absclass::PAL_getLocalTime(&time);

	dt.setDate(time.wYear, time.wMonth, time.wDay);

	return dt;
}


void Win32::setCurrentDate(const util_time::Date& /*dt*/)
{
}


const util_time::Time& Win32::getCurrentTime() const
{
	PAL_TIME_STRUCT time;
	PAL_absclass::PAL_getLocalTime(&time);

	if ( !time_ )
		time_ = new util_time::Time();

	time_->setTime(time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	return *time_;
}


const util_time::Time& Win32::getCurrentTime(util_time::Time& tm) const
{
	PAL_TIME_STRUCT time;
	PAL_absclass::PAL_getLocalTime(&time);

	tm.setTime(time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

	return tm;
}


void Win32::setCurrentTime(const util_time::Time& /*t*/)
{
}


bool Win32::isAbsolutePath(const std::string& filename) const
{
	string::size_type size = filename.size();
	if ( size > 0 && filename[0] == slash() )
		return true;
	if ( size > 2 && filename[1] == ':' )
	{
		if ( 'A' <= toupper(filename[0]) && toupper(filename[0]) <= 'Z'
				&& filename[2] == slash() )
			return true;
	}

	return false;
}


bool Win32::isRootDirectory(const std::string& filename) const
{
	string::size_type drive = parseDriveName(filename);

	return (filename.size() == drive + 1) && (filename[drive] == slash());
}


int Win32::parseDriveName(const std::string& filename) const
{
	if ( filename.size() > 1 )
	{
		if ( 'A' <= toupper(filename[0]) && toupper(filename[0]) <= 'Z' && ':' == filename[1] )
			return 2;
	}

	return 0;
}


bool Win32::fileExists(const std::string& filename) const
{
#if !defined (PAL_WIN32_RUN)
    // a portable method of determining if a file exists
    struct stat stFileInfo;
    int intStat;

    // Attempt to get the file attributes
    intStat = stat(filename.c_str(),&stFileInfo);
    if(intStat == 0) {
        // We were able to get the file attributes
        // so the file obviously exists.
        return true;
    } else {
        // We were not able to get the file attributes.
        // This may mean that we don't have permission to
        // access the folder which contains this file. If you
        // need to do that level of checking, lookup the
        // return values of stat which will give you
        // more details on why stat failed.
        return false;
    }
#else
    return ( GetFileAttributesA(filename.c_str()) != 0xFFFFFFFF );
#endif
}


DateTime Win32::getFileModificationDate(const string& UNUSED(filename)) const
{
#if defined (PAL_WIN32_RUN)
	WIN32_FILE_ATTRIBUTE_DATA attributes;
	if ( GetFileAttributesExA(filename.c_str(), GetFileExInfoStandard, &attributes) )
	{
		// Now use FileTimeToSystemTime to convert the PAL_FILETIME to PAL_TIME_STRUCT structure
		PAL_TIME_STRUCT systemTime;
		if ( FileTimeToSystemTime(&attributes.ftLastWriteTime, &systemTime) )
		{
			util_time::DateTime date(systemTime.wYear, systemTime.wMonth - 1, systemTime.wDay);

			date.setTime(systemTime.wHour, systemTime.wMinute, systemTime.wSecond,
					systemTime.wMilliseconds);

			return date;
		}
	}

	throw util_time::Date::InvalidDate("Unable to obtain a date");
#else
        // TODO implement this later
        util_time::DateTime date(0, 0, 0);
        return date;
#endif      // PAL_WIN32_RUN
}


long64 Win32::fileSize(const std::string& UNUSED(filename)) const
{
	long64 size = 0;
#if defined (PAL_WIN32_RUN)
	HANDLE* searchHandle = (HANDLE *)malloc(sizeof(HANDLE));
	WIN32_FIND_DATAA* descriptor = (WIN32_FIND_DATAA *)malloc(sizeof(WIN32_FIND_DATAA));

	*searchHandle = FindFirstFileA(filename.c_str(), descriptor);
	if ( *searchHandle != INVALID_HANDLE_VALUE )
	{
		size = descriptor->nFileSizeHigh * MAXDWORD + descriptor->nFileSizeLow;
		FindClose(*searchHandle);
	}

	free(searchHandle);
	free(descriptor);
#endif      // PAL_WIN32_RUN
	return size;
}


// This function has not been tested at all.
unsigned int Win32::fileChecksum(const std::string& filename) const
{
	static const int blockSize = 512;
	char buffer[blockSize];
	unsigned short checksum = 0;

	// Open source file
	ifstream sourceFile(filename.c_str(), ios::in|ios::binary);
	if ( !sourceFile )
		throw Platform::FileNotFound(filename);

	bool done = false;
	while ( !done )
	{
		sourceFile.read(buffer, blockSize);
		if ( sourceFile || sourceFile.eof() )
		{
			const std::streamsize bytesRead = sourceFile.gcount();
			if ( bytesRead )
			{
				checksum = util_checksums::icrc(checksum, (unsigned char*)buffer, (size_t) bytesRead);

				if ( bytesRead != blockSize )
					done = true;
			}
			else
				done = true;
		}
		else
				throw Platform::FileAccessError("reading " + filename);
	}

	return checksum;
}


Platform::FileSearch* Win32::findFileFirst(const std::string& UNUSED(search)) const
{
#if defined (PAL_WIN32_RUN)
	/* Initialize the Win32 structures. */
	HANDLE* searchHandle = (HANDLE *)malloc(sizeof(HANDLE));
	WIN32_FIND_DATAA* descriptor = (WIN32_FIND_DATAA *)malloc(sizeof(WIN32_FIND_DATAA));


	*searchHandle = FindFirstFileA(search.c_str(), descriptor);
	if ( *searchHandle != INVALID_HANDLE_VALUE )
		return new Win32_FileSearch(searchHandle, descriptor);

	// These are deleted here only in the case we did not instanciate the Win32_FileSearch
	// class. Otherwise, the destructor of the object takes care of doing so.
	free(searchHandle);
	free(descriptor);
#endif      // PAL_WIN32_RUN
	return 0;
}


bool Win32::findFileNext(Platform::FileSearch* UNUSED(fs)) const
{
#if defined (PAL_WIN32_RUN)
	Win32_FileSearch* file = dynamic_cast<Win32_FileSearch*>(fs);

	if ( FindNextFileA(*file->handle, file->descriptor) )
		return true;
#endif      // PAL_WIN32_RUN
	return false;
}


void Win32::findFileDelete(Platform::FileSearch* fs) const
{
	delete fs;
}


bool Win32::copyFile(const string& UNUSED(source), const string& UNUSED(destination)) const
{
#if defined (PAL_WIN32_RUN)
	return CopyFileA(source.c_str(), destination.c_str(), false) != 0;  /* (SJJC port 6/19/06) */
#else
        return false;
#endif      // PAL_WIN32_RUN
}


// This function has not been tested at all.
bool Win32::appendFile(const std::string& source, const std::string& destination) const
{
	static const int blockSize = 512;
	char buffer[blockSize];

	// Open source file
	ifstream sourceFile(source.c_str(), ios::in|ios::binary);
	if ( !sourceFile )
		return false;

	// Open destination file
	ofstream destinationFile(destination.c_str(), ios::out|ios::binary|ios::app);
	if ( !destinationFile )
		return false;

	bool done = false, failed = false;
	while ( !done && !failed )
	{
		sourceFile.read(buffer, blockSize);
		if ( sourceFile )
		{
			const std::streamsize bytesRead = sourceFile.gcount();
			if ( bytesRead > 0)
			{
				destinationFile.write(buffer, bytesRead);
				if ( !destinationFile )
					failed = true;
			}

			if ( bytesRead < blockSize )
				done = true;
		}
		else
			failed = true;
	}
	
	return !failed;
}


bool Win32::moveFile(const string& UNUSED(oldname), const string& UNUSED(newname)) const
{
#if defined (PAL_WIN32_RUN)
	return MoveFileA(oldname.c_str(), newname.c_str()) != 0;	 /* (SJJC port 6/19/06) */
#else
        return false;
#endif      // PAL_WIN32_RUN
}


bool Win32::deleteFile(const string& UNUSED(filename)) const
{
#if defined (PAL_WIN32_RUN)
	DWORD attribs = GetFileAttributesA(filename.c_str());
	if (attribs & FILE_ATTRIBUTE_READONLY)
	{
		attribs &= (~FILE_ATTRIBUTE_READONLY);
		SetFileAttributesA(filename.c_str(), attribs);
	}
	
	return DeleteFileA(filename.c_str()) != 0;  /* (SJJC port 6/19/06) */
#else
        return false;
#endif      // PAL_WIN32_RUN
}


bool Win32::createDirectory(const std::string& UNUSED(dirname)) const
{
#if defined (PAL_WIN32_RUN)
	return CreateDirectoryA(dirname.c_str(), NULL) != 0;	 /* (SJJC port 6/19/06) */
#else
        return false;
#endif      // PAL_WIN32_RUN
}


bool Win32::moveDirectory(const std::string& UNUSED(oldname), const std::string& UNUSED(newname)) const
{
#if defined (PAL_WIN32_RUN)
	return MoveFileA(oldname.c_str(), newname.c_str()) != 0;	 /* (SJJC port 6/19/06) */
#else
        return false;
#endif      // PAL_WIN32_RUN
}


bool Win32::deleteDirectory(const std::string& UNUSED(dirname)) const
{
#if defined (PAL_WIN32_RUN)
	return RemoveDirectoryA(dirname.c_str()) != 0;  /* (SJJC port 6/19/06) */
#else
        return false;
#endif      // PAL_WIN32_RUN
}


long64 Win32::fileSystemFreeSpace(const std::string& UNUSED(fs)) const
{
#if defined (PAL_WIN32_RUN)
	ULARGE_INTEGER userAvailable, totalSpace, freeSpace;
	if ( GetDiskFreeSpaceExA(fs.c_str(), &userAvailable, &totalSpace, &freeSpace) )
		return *(long64*)&freeSpace;
	else
#endif      // PAL_WIN32_RUN
		return (long64)-1;
}


long64 Win32::fileSystemUsedSpace(const std::string& UNUSED(fs)) const
{
#if defined (PAL_WIN32_RUN)
	ULARGE_INTEGER userAvailable, totalSpace, freeSpace;
	if ( GetDiskFreeSpaceExA(fs.c_str(), &userAvailable, &totalSpace, &freeSpace) )
		return *(long64*)&totalSpace - *(long64*)&freeSpace;
	else
#endif      // PAL_WIN32_RUN
		return (long64)-1;
}


long64 Win32::fileSystemTotalSpace(const std::string& UNUSED(fs)) const
{
#if defined (PAL_WIN32_RUN)
	ULARGE_INTEGER userAvailable, totalSpace, freeSpace;
	if ( GetDiskFreeSpaceExA(fs.c_str(), &userAvailable, &totalSpace, &freeSpace) )
		return *(long64*)&totalSpace;
	else
#endif      // PAL_WIN32_RUN
		return (long64)-1;
}

#if defined(LINUX_FIX)
bool Win32::delay(DWORD milliseconds) const
#else
bool Win32::delay(int milliseconds) const
#endif
{
    PAL_absclass::PAL_sleep(milliseconds);

	return true;
}


void Win32::getLocaleString(std::string & localeString, const std::string & language,
				const std::string & countryCode)
{
	std::string tmpLang(language);
	std::string tmpCountry(countryCode);

	localeString = "";

	util_string::toupper(tmpCountry);

	languageContainer::iterator iterLang = _languages_.find(tmpLang); 
	if ( iterLang != _languages_.end() )
	{
		countryContainer::iterator iterCountry = _countries_.find(tmpCountry);
		if ( iterCountry != _countries_.end() )
		{
			localeString = iterLang->second + "_" + iterCountry->second;
		}
	}
}


void Win32::getLocaleString(std::string & UNUSED(destLocale), const std::string & UNUSED(srcLocale))
{
#ifdef PAL_WIN32_RUN
	destLocale = "";

	std::string::size_type begPos = srcLocale.find("_");
	if ( begPos == std::string::npos )
		return;


	std::string tmpLang(srcLocale.substr(0,begPos));
	std::string tmpCountry(srcLocale.substr(begPos+1));

	util_string::tolower(tmpLang);
	util_string::toupper(tmpCountry);

	languageContainer::iterator iterLang = _languages_.find(tmpLang); 
	if ( iterLang != _languages_.end() )
	{
		countryContainer::iterator iterCountry = _countries_.find(tmpCountry);
		if ( iterCountry != _countries_.end() )
		{
			destLocale = iterLang->second + "_" + iterCountry->second;
		}
	}
#endif      // PAL_WIN32_RUN
}

void Win32::setupLocale(const std::string & UNUSED(language), const std::string & UNUSED(countryCode))
{
#if defined (PAL_WIN32_RUN)
	std::string localeStr("");
	getLocaleString(localeStr, language, countryCode);
	locale currentLocale = locale(localeStr.c_str());
	locale::global(currentLocale);
	
	//Debug::trace("Win32", HobString("Changed Session's locale to be ") + currentLocale.name() +
	//		HobString(" from a languageNumber of ") + Integer::toString(cls) + HobString(" and a language code of ") + localeStr);
#endif      // PAL_WIN32_RUN
}

void Win32::reverseLocaleString(std::string & UNUSED(destLocale), const std::string & UNUSED(srcLocale))
{
#if defined (PAL_WIN32_RUN)
	destLocale = "";

	std::string::size_type begPos = srcLocale.find("_");
	if ( begPos == std::string::npos )
		return;
		

	std::string tmpLang(srcLocale.substr(0,begPos));
	std::string tmpCountry(srcLocale.substr(begPos+1));

	std::string::size_type secondPos = tmpCountry.find(".");
	if ( begPos != std::string::npos )
		tmpCountry = tmpCountry.substr(0, secondPos);

	countryContainer::iterator iterCountry;
	languageContainer::iterator iterLang;

	for (iterLang = _languages_.begin(); iterLang != _languages_.end(); iterLang++)
	{
		if ( tmpLang == iterLang->second )
			break;
	}

	for (iterCountry = _countries_.begin(); iterCountry != _countries_.end(); iterCountry++)
	{
		if (tmpCountry == iterCountry->second)
			break;
	}

	if ((iterLang != _languages_.end()) && (iterCountry != _countries_.end()))
	{
		destLocale = iterLang->first + "_" + iterCountry->first;
	}
#endif      // PAL_WIN32_RUN
}



const std::string Win32::_lineSeparator_ = "\x0d\x0a";
const std::string Win32::_altLineSeparator_ = "\x0a";
const std::string Win32::_allFiles_ = "*.*";



Win32_FileSearch::Win32_FileSearch(HANDLE* h, PAL_FIND_DATAA* desc)
		: handle(h), descriptor(desc)
{
}


Win32_FileSearch::~Win32_FileSearch()
{
	// If *handle is valid, it needs to be closed.
	if ( *handle != INVALID_HANDLE_VALUE )
        FindClose(*handle);

	delete handle;
	delete descriptor;
}


string Win32_FileSearch::getFileName() const
{
	// The "descriptor" contains the filename without path information (just the name)
	return descriptor->cFileName;
}


util_time::DateTime Win32_FileSearch::getCreated() const
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


util_time::DateTime Win32_FileSearch::getModified() const
{
	// First use FileTimeToLocalFileTime to convert the UTC time to local time
	FILETIME localTime;
	if ( FileTimeToLocalFileTime(&descriptor->ftLastWriteTime, &localTime) )
	{
		// Now use FileTimeToSystemTime to convert the local FILETIME to SYSTEMTIME structure
		SYSTEMTIME systemTime;
		if ( FileTimeToSystemTime(&localTime, &systemTime) )
		{
			util_time::DateTime date(systemTime.wYear, systemTime.wMonth - 1, systemTime.wDay);

			date.setTime(systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

			return date;
		}
	}
    return util_time::DateTime();
}


bool Win32_FileSearch::isFile() const
{
#if defined ( _WIN32 )
	return !(descriptor->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
#else
        return false;
#endif
}


bool Win32_FileSearch::isDirectory() const
{
#if defined ( _WIN32 )
	return (descriptor->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;	/* (SJJC port 6/19/06) */
#else
        return false;
#endif
}


long64 Win32_FileSearch::getSize() const
{
#if defined ( _WIN32 )
	return (long64)descriptor->nFileSizeHigh * (long64)MAXDWORD + descriptor->nFileSizeLow;
#else
    return 0;
#endif
}



#ifndef NO_NAMESPACES
} // namespace util_platform
#endif

