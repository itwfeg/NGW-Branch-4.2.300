#ifndef PLATFORM_H
#define PLATFORM_H



#include <string>
#include <stdexcept>


#ifdef WIN32_OS
#  include "..\time\datetime.h"
#  include "..\thread\mutex.h"
#  include "..\types.h"
#endif
//#ifdef VXWORKS
#if defined(VXWORKS) || defined( __linux__ )
#  include "../time/datetime.h"
#  include "../thread/mutex.h"
#  include "../types.h"
#endif


#ifndef NO_NAMESPACES
namespace util_platform
{
#endif


	class Date;
	class Time;


	class Platform
	{
		public:
			// --- Types, subtypes, and friends ---
			class FileSearch
			{
				public:
					virtual ~FileSearch() {}

					virtual std::string getFileName() const = 0;
					virtual util_time::DateTime getModified() const = 0;
					virtual bool isFile() const = 0;
					virtual bool isDirectory() const = 0;
					virtual long64 getSize() const = 0;
			};

			class FileNotFound : public std::runtime_error       // Exception class
			{
				public:
					FileNotFound(const std::string& message) : std::runtime_error(message) {}
			};

			class FileAccessError : public std::runtime_error       // Exception class
			{
				public:
					FileAccessError(const std::string& message) : std::runtime_error(message) {}
			};


			// --- Constructors and destructors
			static Platform& getInstance();
			virtual ~Platform() {}


			// --- Accessor methods ---
			virtual const util_time::DateTime& getCurrentDateTime() const = 0;  /* (SJJC port 6/19/06) */
			virtual const util_time::DateTime& getCurrentDateTime(util_time::DateTime& dt) const = 0;
			virtual void setCurrentDateTime(const util_time::DateTime& dt) = 0;
			virtual const util_time::Date& getCurrentDate() const = 0;
			virtual const util_time::Date& getCurrentDate(util_time::Date& dt) const = 0;
			virtual void setCurrentDate(const util_time::Date& dt) = 0;
			virtual const util_time::Time& getCurrentTime() const = 0;
			virtual const util_time::Time& getCurrentTime(util_time::Time& tm) const = 0;
			virtual void setCurrentTime(const util_time::Time& t) = 0;


			// --- Processing methods ---
			// Filename manipulation methods
			virtual bool isAbsolutePath(const std::string& filename) const = 0;
			virtual bool isRootDirectory(const std::string& filename) const = 0;
			virtual int parseDriveName(const std::string& filename) const = 0;

			// File Attribute methods
			virtual bool fileExists(const std::string& filename) const = 0;
			virtual util_time::DateTime getFileModificationDate(const std::string& name) const = 0;
			virtual long64 fileSize(const std::string& filename) const = 0;
			virtual unsigned int fileChecksum(const std::string& filename) const = 0;

			// File search methods
			virtual FileSearch* findFileFirst(const std::string& search) const = 0 ;
			virtual bool findFileNext(FileSearch* fs) const = 0 ;
			virtual void findFileDelete(FileSearch* fs) const = 0;

			// File manipulation methods
			virtual bool copyFile(const std::string& source, const std::string& destination) const = 0;
			virtual bool appendFile(const std::string& source, const std::string& destination) const = 0;
			virtual bool moveFile(const std::string& oldname, const std::string& newname) const = 0;
			virtual bool deleteFile(const std::string& filename) const = 0;

			// Directory manipulation methods
			virtual bool createDirectory(const std::string& dirname) const = 0;
			virtual bool moveDirectory(const std::string& oldname, const std::string& newname) const = 0;
			virtual bool deleteDirectory(const std::string& dirname) const = 0;

			// File System methods
			virtual long64 fileSystemFreeSpace(const std::string& fs) const = 0;
			virtual long64 fileSystemUsedSpace(const std::string& fs) const = 0;
			virtual long64 fileSystemTotalSpace(const std::string& fs) const = 0;

			// Miscellaneous methods
        #if defined(LINUX_FIX)
            virtual bool delay(DWORD milliseconds) const = 0;
        #else
			virtual bool delay(int milliseconds) const = 0;
        #endif
			virtual char slash() const = 0;
			virtual char altSlash() const = 0;
			virtual const std::string& lineSeparator() const = 0;
			virtual const std::string& altLineSeparator() const = 0;
			virtual const std::string& allFiles() const = 0;
			virtual const char* getNFSRootDirectory() const = 0;
			virtual const char* getErrorMessage(int err) const = 0;
			virtual int getPosixErrno(int err) const { return err; }
			virtual void getLocaleString(std::string & localeString, const std::string & language,
				const std::string & countryCode) = 0;
			virtual void getLocaleString(std::string & destLocale, const std::string & srcLocale) = 0;
			virtual void setupLocale(const std::string & language, const std::string & countryCode) = 0;
			virtual void reverseLocaleString(std::string & destLocale, const std::string & srcLocale) = 0;
		protected:
			// --- Constructors and destructor ---
			Platform() {}


		private:
			// --- Data members ---
			static util_thread::Mutex<> _singletonMutex_;
	};


#ifdef VXWORKS
	// Following is a small C interface to this class' functionality.
	// This stops the function name from being mangled in the compilation of platform.cpp.
	extern "C" STATUS fileSystemFreeSpace(const char* fileSystem);
	extern "C" STATUS fileSystemUsedSpace(const char* fileSystem);
	extern "C" STATUS fileSystemTotalSpace(const char* fileSystem);
#endif


#ifndef NO_NAMESPACES
} // namespace util_platform
#endif



#endif // PLATFORM_H

