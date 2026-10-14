#ifndef FILENAME_H
#define FILENAME_H



#include <string>
#include <vector>


#ifndef NO_NAMESPACES
namespace util_filesystem
{
#endif


	class Filename
	{
		public:
			// --- Types and subtypes ---

			// --- Constructors and destructor ---
			// Call this constructor if you have a fullpath (path and filename) or only a path
			Filename(const std::string& fullname = _empty_);
			// Call this constructor if you have path and filename separately
			Filename(const std::string& path, const std::string& filename);
			// Call this constructor if your path is a Filename and you have a separate filename
			Filename(const Filename& path, const std::string& filename);
			// Copy constructor
			Filename(const Filename& fn);
			virtual ~Filename();


			// --- Getter and setter methods ---
			// Given the full-path filename 'c:\dir1\dir2\filename.more.extension',
			// getFullPath will return 'c:\dir1\dir2\filename.more.extension'.
			const std::string& getFullPath() const { return fullpath_; }
			void setFullPath(const std::string& fullPath)
					{ parsePath(fullPath, true, false); setFullPathInternal(); }
			// Given the full-path filename 'c:\dir1\dir2\filename.more.extension',
			// getPath will return 'c:\dir1\dir2'.
			const std::string& getPath() const { return path_; }
			void setPath(const std::string& path)
					{ parsePath(path, false, false); setFullPathInternal(); }
			// Given the full-path filename 'c:\dir1\dir2\filename.more.extension',
			// getDriveName will return 'c:'.
			const std::string& getDriveName() const { return drive_; }
			void setDriveName(const std::string& drive);
			// Given the full-path filename 'c:\dir1\dir2\filename.more.extension',
			// getFileName will return 'filename.more.extension'.
			const std::string& getFileName() const { return filename_; }
			void setFileName(const std::string& name)
					{ parseFilename(name); setFullPathInternal(); }
			// Given the full-path filename 'c:\dir1\dir2\filename.more.extension',
			// setFileNameBase will return 'filename.more'.
			const std::string& getFileNameBase() const { return namebase_; }
			void setFileNameBase(const std::string& name);
			// Given the full-path filename 'c:\dir1\dir2\filename.more.extension',
			// getFileNameExtension will return 'extension'.
			const std::string& getFileNameExtension() const { return extension_; }
			void setFileNameExtension(const std::string& ext);


			// --- Processing methods ---
			bool hasParent() const;
			bool isAbsolutePath() const;
			bool isRootDirectory() const;
//			void makeAbsolutePath();
			void appendPath(const std::string& relativePath)
					{ parsePath(relativePath, false, true); setFullPathInternal(); }
//			void prependPath(const std::string& path);


		private:
			mutable std::string fullpath_;
			std::string path_;
			std::string filename_;
			mutable std::string drive_;
			mutable std::string namebase_;
			mutable std::string extension_;

			void parseVolume();
			void parsePath(const std::string& path, bool includesFilename, bool appendPath);
			void parseFilename(const std::string& filename);
			void parseBaseExtension();
			void setFullPathInternal();

			static const std::string _empty_;   // Used to enable precompiled headers
	};


#ifndef NO_NAMESPACES
} // namespace util_filesystem
#endif



#endif // FILENAME_H

