#ifndef FILE_H
#define FILE_H

#include "PAL_platformheader.h"

#include <string>
#include <vector>


#include "filename.h"
//#ifdef VXWORKS
#if defined(VXWORKS) || defined( __linux__ )
#  include "../types.h"
#  include "../time/datetime.h"
#endif
#ifdef WIN32_OS
#  include "..\types.h"
#  include "..\time\datetime.h"
#endif


#ifndef NO_NAMESPACES
namespace util_filesystem
{
#endif


#ifndef NO_NAMESPACES
	using namespace util_time;
#endif


	class File
	{
		public:
			// --- Types and subtypes ---

			// --- Constructors and destructor ---
			// Call this constructor if you have a fullname (path and filename)
			File(const std::string& fullname = _empty_) : filename_(fullname) {}
			// Call this constructor if you have only a path ( use "" for the filename),
			// or path and filename separately
			File(const std::string& path, const std::string& filename)
					: filename_(path, filename) {}
			// Call this constructor if you have a Filename
			File(const Filename& filename) : filename_(filename) {}
			// Copy constructor
			File(const File& fn) : filename_(fn.filename_) {}
			virtual ~File() {}


			// --- Getter and setter methods ---
			const std::string& getFullPath() const { return filename_.getFullPath(); }
			void setFullPath(const std::string& fullPath) { filename_.setFullPath(fullPath); }
			const std::string& getPath() const { return filename_.getPath(); }
			void setPath(const std::string& path) { filename_.setPath(path); }
			const std::string& getDriveName() const { return filename_.getDriveName(); }
			void setDriveName(const std::string& drive) { filename_.setDriveName(drive); }
			const std::string& getFileName() const { return filename_.getFileName(); }
			void setFileName(const std::string& name) { filename_.setFileName(name); }
			const std::string& getFileNameBase() const { return filename_.getFileNameBase(); }
			void setFileNameBase(const std::string& name) { filename_.setFileNameBase(name); }
			const std::string& getFileNameExtension() const
					{ return filename_.getFileNameExtension(); }
			void setFileNameExtension(const std::string& ext)
					{ filename_.setFileNameExtension(ext); }

			DateTime getModified() const;


			// --- Processing methods ---
			bool exists() const;
			long64 size() const;
			unsigned int checksum() const;
			bool remove();
			bool operator <<(const File& oldfile);   // Copy "oldfile" onto "this" file

			bool copyTo(const File& destination) { return copyTo(destination.getFullPath()); }
			bool copyTo(const std::string& destination);
			bool moveTo(const File& destination) { return moveTo(destination.getFullPath()); }
			bool moveTo(const std::string& destination);
			bool appendTo(const File& destination) { return appendTo(destination.getFullPath()); }
			bool appendTo(const std::string& destination);


		private:
			Filename filename_;

			static const std::string _empty_;    // Used to enable precompiled headers
	};


#ifndef NO_NAMESPACES
} // namespace util_filesystem
#endif



#endif // FILE_H

