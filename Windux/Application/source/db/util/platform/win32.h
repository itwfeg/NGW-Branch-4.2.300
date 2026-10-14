#ifndef WIN32_H
#define WIN32_H



#include <string>
#include <map>

#include "platform.h"
#include "../time/datetime.h"


#ifndef NO_NAMESPACES
namespace util_platform
{
#endif


	class Win32 : public Platform
	{
		public:
			// --- Constructors and destructor ---
			Win32();
			~Win32();

			typedef std::map<std::string, std::string> languageContainer;
			typedef std::map<std::string, std::string> countryContainer;

			// --- Accessor methods ---
			const util_time::DateTime& getCurrentDateTime() const;
			const util_time::DateTime& getCurrentDateTime(util_time::DateTime& dt) const;
			void setCurrentDateTime(const util_time::DateTime& dt);
			const util_time::Date& getCurrentDate() const;
			const util_time::Date& getCurrentDate(util_time::Date& dt) const;
			void setCurrentDate(const util_time::Date& dt);
			const util_time::Time& getCurrentTime() const;
			const util_time::Time& getCurrentTime(util_time::Time& tm) const;
			void setCurrentTime(const util_time::Time& t);


			// --- Processing methods ---
			// Filename manipulation methods
			bool isAbsolutePath(const std::string& filename) const;
			bool isRootDirectory(const std::string& filename) const;
			int parseDriveName(const std::string& filename) const;

			// File Attribute methods
			bool fileExists(const std::string& filename) const;
			util_time::DateTime getFileModificationDate(const std::string& filename) const;
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
        #if defined(LINUX_FIX)
            bool delay(DWORD milliseconds) const;
        #else
			bool delay(int milliseconds) const;
        #endif
			char slash() const { return '\\'; }
			char altSlash() const { return '/'; }
			const std::string& lineSeparator() const { return _lineSeparator_; }
			const std::string& altLineSeparator() const { return _altLineSeparator_; }
			const std::string& allFiles() const { return _allFiles_; }
			const char* getNFSRootDirectory() const { return 0; }
			const char* getErrorMessage(int /*err*/) const { return 0; }  /* (SJJC port 6/19/06) */
			void getLocaleString(std::string & localeString, const std::string & language,
				const std::string & countryCode);
			void getLocaleString(std::string & destLocale, const std::string & srcLocale);
			void setupLocale(const std::string & language, const std::string & countryCode);
			void reverseLocaleString(std::string & destLocale, const std::string & srcLocale);


		private:
			mutable util_time::DateTime* datetime_;
			mutable util_time::Date* date_;
			mutable util_time::Time* time_;

			static const std::string _lineSeparator_;
			static const std::string _altLineSeparator_;
			static const std::string _allFiles_;

			languageContainer _languages_;
			countryContainer _countries_;

			void initializeLocaleMaps();
	};


#ifndef NO_NAMESPACES
} // namespace util_platform
#endif



#endif // WIN32_H

