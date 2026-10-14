#ifndef VXWORKSUTIL_H
#define VXWORKSUTIL_H



#include <string>


#include "platform.h"
#include "../time/datetime.h"


#ifndef NO_NAMESPACES
namespace util_platform
{
#endif


#ifndef NO_NAMESPACES
using namespace util_time;
#endif


class VxWorksUtil : public Platform
{
	public:
		// --- Constructors and destructor ---
		VxWorksUtil();
		~VxWorksUtil();


		// --- Accessor methods ---
		const DateTime& getCurrentDateTime() const;
		const DateTime& getCurrentDateTime(DateTime& dt) const;
		void setCurrentDateTime(const DateTime& dt);
		const Date& getCurrentDate() const;
		const Date& getCurrentDate(Date& dt) const;
		void setCurrentDate(const Date& dt);
		const Time& getCurrentTime() const;
		const Time& getCurrentTime(Time& tm) const;
		void setCurrentTime(const Time& t);


		// --- Processing methods ---
		// Filename manipulation methods
		bool isAbsolutePath(const std::string& filename) const
				{ return filename.size() == 1 && filename[0] == slash(); }
		bool isRootDirectory(const std::string& filename) const;
		int parseDriveName(const std::string& ) const { return 0; }

		// File Attribute methods
		bool fileExists(const std::string& filename) const;
		DateTime getFileModificationDate(const std::string& filename) const;
		long64 fileSize(const std::string& filename) const;
		unsigned int fileChecksum(const std::string& filename) const;

		// File search methods
		Platform::FileSearch* findFileFirst(const std::string& search) const;
		bool findFileNext(Platform::FileSearch* fs) const;
		void findFileDelete(Platform::FileSearch* fs) const;

		// File manipulation methods
		bool copyFile(const std::string& source, const std::string& destination) const;
		bool appendFile(const std::string& source, const std::string& destination) const;
		bool moveFile(const std::string& oldname, const std::string& newname) const;
		bool deleteFile(const std::string& filename) const;

		// Directory manipulation methods
		bool createDirectory(const std::string& dirname) const;
		bool moveDirectory(const std::string& oldname, const std::string& newname) const;
		bool deleteDirectory(const std::string& dirname) const;

		// File System methods
		long64 fileSystemFreeSpace(const std::string& fs) const;
		long64 fileSystemUsedSpace(const std::string& fs) const;
		long64 fileSystemTotalSpace(const std::string& fs) const;

		// Miscellaneous methods
		bool delay(int milliseconds) const;
		char slash () const { return '/'; }
		char altSlash() const { return '\\'; }
		const std::string& lineSeparator() const { return _lineSeparator_; }
		const std::string& altLineSeparator() const { return _altLineSeparator_; }
		const std::string& allFiles() const { return _allFiles_; }
		const char* getNFSRootDirectory() const;
		const char* getErrorMessage(int err) const;
		int getPosixErrno(int err) const;
		void getLocaleString(std::string & localeString, const std::string & language,
			const std::string & countryCode);
		void getLocaleString(std::string & destLocale, const std::string & srcLocale);
		void setupLocale(const std::string & language, const std::string & countryCode) {}
		void reverseLocaleString(std::string & destLocale, const std::string & srcLocale);

	private:
		int ticksPerSecond_;
		mutable DateTime* datetime_;
		mutable Date* date_;
		mutable Time* time_;

		int getTicks(int milliseconds) const { return (milliseconds ? 1 : 0) + (milliseconds * ticksPerSecond_) / 1000; }

		static const std::string _lineSeparator_;
		static const std::string _altLineSeparator_;
		static const std::string _allFiles_;
};


#ifndef NO_NAMESPACES
} // namespace util_platform
#endif



#endif // VXWORKSUTIL_H

