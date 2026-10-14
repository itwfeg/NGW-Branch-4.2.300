#ifndef DIRECTORY_H
#define DIRECTORY_H

#ifdef __cplusplus

#include <string>
#include <set>
#include <stdexcept>
#include <map>

#include "file.h"
#ifdef WIN32_OS
#  include "..\types.h"
#  include "..\platform\platform.h"
#  include "..\time\datetime.h"
#endif
//#ifdef VXWORKS
#if defined(VXWORKS) || defined( __linux__ )
#  include "../types.h"
#  include "../platform/platform.h"
#  include "../time/datetime.h"
#endif


#ifndef NO_NAMESPACES
namespace util_filesystem
{
#endif


#ifndef NO_NAMESPACES
	using namespace util_platform;
	using namespace util_time;
#endif

	class Directory
	{
		public:
			// --- Types and subtypes ---
			enum Selection
					{ SELECT_FILES, SELECT_DIRS, SELECT_FILES_DIRS, SELECT_INVALID };
			enum Sortby
					{ NO_SORT, NAME_SORT, MODIFIED_SORT, SIZE_SORT, INVALID_SORT };
			class DirectoryEntry
			{
				public:
					enum EntryType
							{ DIR_ENTRY, FILE_ENTRY, INVALID_ENTRY };

					DirectoryEntry(Platform::FileSearch* fs, int order = 0)
							: type_(fs->isDirectory()? DIR_ENTRY : FILE_ENTRY),
							name_(fs->getFileName()), modified_(fs->getModified()),
							order_(order), size_(fs->getSize()) { }
					~DirectoryEntry() {}

					EntryType getType() const { return type_; }
					const std::string& getName() const { return name_; }
					const DateTime& getModifiedDate() const { return modified_; }
					int getOrder() const { return order_; }
					long64 getSize() const { return size_; }
					bool isDirectory() const
					{
						if (getType() == DIR_ENTRY)
							return true;
						else
							return false;
					};
					bool isFile() const
					{
						if (getType() == FILE_ENTRY)
							return true;
						else
							return false;
					};

				private:
					EntryType type_;
					std::string name_;
					DateTime modified_;
					int order_;
					long64 size_;
			};
			struct Sorter
			{
				Sorter(Sortby s = NAME_SORT) : sort_(s) {}
				bool operator()(const DirectoryEntry& e1, const DirectoryEntry& e2) const;
				Sortby sort_;
			};
			typedef std::set<DirectoryEntry, Sorter> container_type;
			typedef container_type::size_type size_type;
//			typedef container_type::iterator iterator;
			typedef container_type::const_iterator const_iterator;
			typedef container_type::const_reverse_iterator const_reverse_iterator;

			const static int NFS_ROOTDIRECTORY_SIZE = 3;

			struct EntryNavigator
			{
				virtual bool operator()(const Directory& dir, const DirectoryEntry& entry) = 0;
			};

			class CannotMoveDirectory : public std::invalid_argument
			{
				public:
					CannotMoveDirectory(const std::string& what) : std::invalid_argument(what) {}
			};


			// --- Constructors and destructor ---
			Directory(const std::string& dir) : path_(dir), files_(0), sort_(INVALID_SORT)
					{ setDefaultFlags(); }
			Directory(const Filename& file) : path_(file), files_(0), sort_(INVALID_SORT)
					{ setDefaultFlags(); }
			Directory(const Directory& dir) : path_(dir.path_), files_(0), sort_(INVALID_SORT)
					{ setDefaultFlags(); }
			virtual ~Directory() { removeFiles(); }


			// --- Getter and setter methods ---
			const std::string& getFullPath() const { return path_.getFullPath(); }
			void setFullPath(const std::string& path)
					{ path_.setFullPath(path); removeFiles(); }
			const std::string& getDirectoryName() const { return path_.getFileName(); }
			void setDirectoryName(const std::string& name)
					{ path_.setFileName(name); removeFiles(); }
			const std::string& getDriveName() const { return path_.getDriveName(); }
			void setDriveName(const std::string& drive)
					{ path_.setDriveName(drive); removeFiles(); }
			const std::string& getDirectoryNameBase() const { return path_.getFileNameBase(); }
			void setDirectoryNameBase(const std::string& base)
					{ path_.setFileNameBase(base); removeFiles(); }
			const std::string& getDirectoryNameExtension() const
					{ return path_.getFileNameExtension(); }
			void setDirectoryNameExtension(const std::string& extension)
					{ path_.setFileNameExtension(extension); removeFiles(); }


			// --- Processing methods ---
			void appendPath(const std::string& relativePath)
					{ path_.appendPath(relativePath); removeFiles();}
			// Returns the "filename" of a file or subdirectory in "this" directory
			Filename getFileName(const std::string& name) const
					{ return Filename(path_.getFullPath(), name); }
			// Returns true if "this" directory exists
			bool exists() const;
			// Checks to see if any file or subdir exists within this directory
			bool isEmpty() const;
			// Creates "this" directory if it does not exist
			bool create();
			// Returns true if "this" directory has a parent directory
			bool hasParent() const { return !path_.isRootDirectory(); }
			// Returns the parent directory of "this" directory
			Directory parent() const { return Directory(path_.getPath()); }
			// Removes "this" directory if it is empty (requires "force = true" if not empty)
			bool remove(bool force = false);
			// Removes the file contents of "this" directory (also its subdirectorys if
			// "recursive = true")
			bool removeContents(bool recursive = false, const std::string &exclusions = "");
			// Calculates the space occupied by the files in "this" directory (possibly
			// recursively) as well as the number of those files
			std::pair<long64, int> spaceUsed(bool recursive) const;

			// Select methods
			size_type select(const std::string& search, Selection selection = SELECT_FILES_DIRS,
					Sortby sortby = NAME_SORT) const;
			size_type selectAllFiles(Sortby sortby = NAME_SORT) const;
			size_type selectAllSubdirectories(Sortby sortby = NAME_SORT) const;
			const_iterator begin() const { return files_->begin(); }
			const_iterator end() const { return files_->end(); }
			const_reverse_iterator rbegin() const { return files_->rbegin(); }
			const_reverse_iterator rend() const { return files_->rend(); }
			bool eraseSelected();      // Erase "selected" entries from "this" directory

			// Copy files and directories methods
			bool operator <<(const File& source);
			// Copies the 'source' directory into the 'this' directory.
			// By default, this method will create the 'source' directory in the destination
			// directory (if it does not exist already) and will copy its files there. In order
			// to only copy the files from the 'source' directory into the 'this' directory, use
			// the 'contents' manipulator, as in 'dest << contents << source;'.
			// If you also want to copy any subdirectories, use the 'recursive' manipulator.
			// If instead of copying you want to move the directories, use the 'move' manipulator.
			// By default it overwrites any existing file with the same name. If you don't want 
			// to overwrite, use the 'nooverwrite' manipulator.
			// If you don't want to copy all source files, but only selected ones, select them
			// first and then use the 'selected' manipulator, as in 'source.select("*.dat"); 
			// dest << selected << source;'.
			// If a "const" Directory is used and move was requested, an exception will be thrown.
			bool operator <<(const Directory& source);
			bool operator <<(Directory& source);

			// The following function enables the use of manipulators
			Directory& operator <<(Directory& (*f)(Directory&)) { return f(*this); }
			Directory& setSelected() { flags_.selected_ = flags_.contents_ = true; return *this; }
			Directory& setContents() { flags_.contents_ = true; return *this; }
			Directory& setOverwrite(bool over) { flags_.overwrite_ = over; return *this; }
			Directory& setRecursive() { flags_.recursive_ = true; return *this; }
			Directory& setVerbose() { flags_.verbose_ = true; return *this; }
			Directory& setSilent() { flags_.verbose_ = false; return *this; }
			Directory& setMove() { flags_.move_ = true; return *this; }

			// Navigation methods
			bool navigate(EntryNavigator& nav, bool recursive = false,
					Selection sel = SELECT_FILES) const;

			// Printer.
			std::ostream& output(std::ostream& os, int indentation = 0) const;


			typedef std::map<std::string, unsigned int> DirectoryFileList;
			typedef DirectoryFileList::iterator DirectoryFileListIterator;
			static void getAllAvailableFiles(std::string dirStr, DirectoryFileList* resultList, std::string extension = "*");


		private:
			// --- Types and subtypes ---
			struct CopyFlags { bool selected_, contents_, overwrite_, recursive_, move_, verbose_; };


			// --- Data members ---
			Filename path_;
			mutable container_type* files_;
			mutable Sortby sort_;
			mutable CopyFlags flags_;


			// --- Processing methods ---
			void removeFiles() { if ( files_ ) { delete files_; files_ = 0; } }
			bool defaultDirectory(const std::string& entry) const;
			void setFlags(const Directory& d) { flags_ = d.flags_; }
			void setDefaultFlags()
					{ flags_.selected_ = false; flags_.contents_ = false;
					flags_.overwrite_ = true; flags_.recursive_ = false;
					flags_.move_ = false; flags_.verbose_ = false; }
	};


	// The following are manipulators used to control how to copy a directory into another.
	// selected - only the selected entries will be copied as opposed to all entries
	// (default: all). "selected" implies "contents".
	inline Directory& selected(Directory& d) { d.setSelected(); return d; }
	// contents - the contents of the directory will be copied and not the directory itself
	// (default: directory)
	inline Directory& contents(Directory& d) { d.setContents(); return d; }
	// overwrite - existing entries will be overwritten (default: overwrite)
	inline Directory& overwrite(Directory& d) { d.setOverwrite(true); return d; }
	inline Directory& nooverwrite(Directory& d) { d.setOverwrite(false); return d; }
	// recursive - subdirectories will be navigated and their contents copied
	// (default: norecursive)
	inline Directory& recursive(Directory& d) { d.setRecursive(); return d; }
	// move - source entries will be removed (default: nomove)
	inline Directory& move(Directory& d) { d.setMove(); return d; }
	// verbose - progress will be shown through cout
	inline Directory& verbose(Directory& d) { d.setVerbose(); return d; }


	inline std::ostream& operator<<(std::ostream& os, const Directory& dir) { return dir.output(os); }



#ifdef VXWORKS
	// Following is a small C interface to this class' functionality
	// Used for the .cpp file only. this stops the function name from being
		  // mangled in the compilation of directory.cpp.
	extern "C" STATUS copyDirectory(const char* sourceDir, const char* destinationDir,
			BOOL asSubDir, BOOL verbose);
	extern "C" STATUS copyFile(const char* sourceFile, const char* destinationDir, BOOL verbose);
	extern "C" STATUS createDirectory(const char* dir);
	extern "C" STATUS directoryExists(const char* dir);
	extern "C" const char* getNFSRootDirectory();
	extern "C" STATUS copyNFSDirectory(const char* sourceDir, BOOL isSrcNFS,
			const char* destinationDir, BOOL isDestNFS, BOOL asSubDir, BOOL verbose);
	extern "C" STATUS copyNFSFile(const char* sourceFile, BOOL isSrcNFS,
			const char* destinationDir, BOOL isDestNFS, BOOL verbose);
	extern "C" STATUS printDirectoryContents(const char* dir);
	extern "C" STATUS removeDirectoryContents(const char* dir, BOOL recursive);
#endif

#ifndef NO_NAMESPACES
} // namespace util_filesystem
#endif

#else
	// Following is a small C interface to this class' functionality
	// Defined so that .c files can use some functionality of the above
		  // class
	STATUS copyDirectory(const char* sourceDir, const char* destinationDir,
			BOOL asSubDir, BOOL verbose);
	STATUS copyFile(const char* sourceFile, const char* destinationDir, BOOL verbose);
	STATUS createDirectory(const char* dir);
	STATUS directoryExists(const char* dir);
	const char* getNFSRootDirectory();
	STATUS copyNFSDirectory(const char* sourceDir, BOOL isSrcNFS,
			const char* destinationDir, BOOL isDestNFS, BOOL asSubDir, BOOL verbose);
	STATUS copyNFSFile(const char* sourceFile, BOOL isSrcNFS,
			const char* destinationDir, BOOL isDestNFS, BOOL verbose);
	STATUS printDirectoryContents(const char* dir);
	STATUS removeDirectoryContents(const char* dir, BOOL recursive);
#endif // __cplusplus

#endif // DIRECTORY_H

