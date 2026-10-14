#include <iostream>


#include <iomanip>


#include "directory.h"

#include "file.h"

#if defined(VXWORKS) || defined( __linux__ )
	#include "../platform/platform.h"
	#include "../e_string/utilstring.h"
#else
	#include "..\platform\platform.h"
	#include "..\e_string\utilstring.h"
#endif

#include "Debug.h"

#ifndef NO_NAMESPACES
namespace util_filesystem
{
#endif


#ifndef NO_NAMESPACES
using util_platform::Platform;
#endif
using namespace std;


bool Directory::exists() const
{
	return Platform::getInstance().fileExists(getFullPath());
}


bool Directory::isEmpty() const
{
	// Make a copy of this directory so we do not affect its "current" selection
	Directory duplicate(*this);

	// If there are no files or folders (zero) return true (false if not zero).
	return (duplicate.select(Platform::getInstance().allFiles(), SELECT_FILES_DIRS) == 0);
}


bool Directory::create()
{
	bool result = true;

	// If the parent directory does not exist, create it first
	if ( path_.hasParent() )
	{
		Directory parent(path_.getPath());
		if ( !parent.exists() )
			result = parent.create();
	}

	// Now that the parent directory exists, create "this" directory
	if ( result )
		return Platform::getInstance().createDirectory(getFullPath());
	else
		return false;
}


bool Directory::remove(bool force)
{
	bool result = false;

	if ( select(Platform::getInstance().allFiles()) )
	{
		if ( force && removeContents(true) )
			result = true;
	}
	else
		result = true;

	return result ? Platform::getInstance().deleteDirectory(getFullPath()) : false;
}


bool Directory::removeContents(bool recursive, const std::string &exclusions)
{
	bool result = true;

	select(Platform::getInstance().allFiles(), SELECT_FILES_DIRS);

	const_iterator iter = begin();
	while ( iter != end() )
	{
		if ( iter->getType() == DirectoryEntry::DIR_ENTRY && recursive )
		{
			// If the entry is a subdirectory and we are removing contents recursively, delete
			// the subdirectory's contents recursively and then remove the subdirectory itself
			Directory subdir(getFileName(iter->getName()));
			if ( !subdir.removeContents(recursive) )
				result = false;
			else
			{
				if ( !subdir.remove() )
					result = false;
			}
		}
		else if ( iter->getType() == DirectoryEntry::FILE_ENTRY )
		{
			// If the entry corresponds to a file, then remove the file, if it's not in the exclusion "list"
			string delFilename = iter->getName();

			// We don't currently handle if the exclusion list contains wildcards, like 'don't delete *.txt files'
			if (exclusions.empty() || exclusions.find(delFilename) == std::string::npos)
			{
				// This file is not in the list of exclusions, so we can go ahead and delete it.
				File file(getFileName(iter->getName()));
				if ( !file.remove() )
					result = false;
			}
		}

		++iter;
	}

	return result;
}


std::pair<long64, int> Directory::spaceUsed(bool recursive) const
{
	struct SpaceUsed : public EntryNavigator
	{
		SpaceUsed() : files_(0), total_(0) {}
		bool operator()(const Directory&, const DirectoryEntry& entry)
				{ ++files_; total_ += entry.getSize(); return true; }
		int files_;
		long64 total_;
	} su;

	navigate(su, recursive, SELECT_FILES);

	return make_pair(su.total_, su.files_);
}


Directory::size_type Directory::select(const std::string& search, Selection type,
		Sortby sortby) const
{
	// Initialize the list of files
	if ( sort_ == sortby && files_ )
		files_->clear();
	else
	{
		if ( files_ )
			delete files_;

		switch ( sortby )
		{
			case NAME_SORT:
				files_ = new container_type(Sorter(NAME_SORT));
				break;
			case MODIFIED_SORT:
				files_ = new container_type(Sorter(MODIFIED_SORT));
				break;
			case SIZE_SORT:
				files_ = new container_type(Sorter(SIZE_SORT));
				break;
			case NO_SORT:
			default:
				files_ = new container_type(Sorter(NO_SORT));
				break;
		}

		sort_ = sortby;
	}


	int count = 0;
    string fileName = "";
	const Platform& os = Platform::getInstance();
	Filename selection(path_.getFullPath(), search);
	Platform::FileSearch* fs = os.findFileFirst(selection.getFullPath());
	while ( fs )
	{
		if ( !defaultDirectory(fs->getFileName()) )
		{
			switch ( type )
			{
				case SELECT_DIRS:
				{
					if ( fs->isDirectory() )
						files_->insert(DirectoryEntry(fs, ++count));

					break;
				}
				case SELECT_FILES_DIRS:
				{
					fileName = fs->getFileName();
					if( (fileName.find("db-shm") == std::string::npos) && (fileName.find("db-wal") == std::string::npos) )
					{
						files_->insert(DirectoryEntry(fs, ++count));
					}
					fileName = "";
					break;
				}
				case SELECT_FILES:
				default:
				{
					if ( fs->isFile() )
						files_->insert(DirectoryEntry(fs, ++count));

					break;
				}
			}
		}

		if ( !os.findFileNext(fs) )
			break;
	}

	if ( fs )
		os.findFileDelete(fs);

	return files_->size();
}


Directory::size_type Directory::selectAllFiles(Sortby sortby) const
{
	return select(Platform::getInstance().allFiles(), SELECT_FILES, sortby);
}


Directory::size_type Directory::selectAllSubdirectories(Sortby sortby) const
{
	return select(Platform::getInstance().allFiles(), SELECT_DIRS, sortby);
}


bool Directory::eraseSelected()
{
	bool result = true;

	if ( files_ )
	{
		const_iterator iter = files_->begin();
		while ( iter != files_->end() )
		{
			switch ( iter->getType() )
			{
				case DirectoryEntry::FILE_ENTRY:
				{
					File file(getFileName(iter->getName()));
					if ( !file.remove() )
						result = false;

					break;
				}
				case DirectoryEntry::DIR_ENTRY:
				{
					Filename name(getFileName(iter->getName()));
					Directory subdir(name);
					subdir.remove(true);

					break;
				}
            #if defined(LINUX_FIX)
                default:
                    // Nothing: handle for other cases.
                    break;
            #endif
			}

			++iter;
		}
	}

	return result;
}


bool Directory::operator <<(const File& source)
{
	bool result = false;
	const Platform& os = Platform::getInstance();

	if ( flags_.verbose_ )
			cout << "\t" << source.getFileName() << "... ";

	if ( source.exists() )
	{
		Filename destinationPath = getFileName(source.getFileName());

		if ( flags_.overwrite_ || !os.fileExists(destinationPath.getFullPath()) )
			result = os.copyFile(source.getFullPath(), destinationPath.getFullPath());

		if ( result && flags_.move_ )
			result = os.deleteFile(source.getFullPath());

		if( flags_.verbose_ )
		{
			if ( flags_.move_ )
			{
				if ( result )
					cout << "Moved" << endl;
				else
					cout << "Failed to move" << endl;
			}
			else
			{
				if ( result )
					cout << "Copied" << endl;
				else
					cout << "Failed to copy" << endl;
			}

			cout << endl;
		}
	}
	else
	{
		if ( flags_.verbose_ )
			cout << "Does not exist" << endl;
	}

	return result;
}


// Copying directory "source" into the current directory.
bool Directory::operator <<(const Directory& source)
{
	// If the source is "const" we will not allow to move it, just to copy it.
	if ( flags_.move_ )
		throw CannotMoveDirectory("Cannot move '" + source.getFullPath() + "' directory since it is \"const\".");

	return *this << const_cast<Directory&>(source);
}


// Copying directory "source" into the current directory.
bool Directory::operator <<(Directory& source)
{
	bool result = true;
	const Platform& os = Platform::getInstance();

	// Step 1: determine the destination of the copy: if "contents" is requested then the
	// destination is the current directory, otherwise the destination is a subdirectory of
	// the current directory with the same name as the source directory. If the latter, call
	// the same operator for the new destination.
	if ( !flags_.contents_ )
	{
		Directory subdir(getFileName(source.getDirectoryName()));
		subdir.setFlags(*this);
		return subdir << contents << source;
	}

	// Step 2: create the destination directory if it does not exist
	if ( !exists() )
	{
		if ( flags_.verbose_ )
			cout << getFullPath() << "... ";

		if ( !create() )
			result = false;

		if ( flags_.verbose_ )
		{
			if ( result )
				cout << "Created" << endl;
			else
				cout << "Failed to create" << endl;
		}
	}

	if ( result )
	{
		// Step 3: navigate through the entries of the source directory (the currently selected
		// ones or all of the existing entries, based on whether "selected" was requested)
		Directory sourceDuplicate(source);
		if ( !flags_.selected_ )
			sourceDuplicate.select(Platform::getInstance().allFiles(), SELECT_FILES_DIRS);
		const_iterator iter = sourceDuplicate.begin();
		while ( iter != sourceDuplicate.end() )
		{
			switch ( iter->getType() )
			{
				case DirectoryEntry::FILE_ENTRY:
				{
					// Step 4: if the navigated entry is a file, it should be copied onto the destination
					// directory (overwriting any preexisting file only if "overwrite" was requested)
					File newFile = getFileName(iter->getName());
					File oldFile = sourceDuplicate.getFileName(iter->getName());
					if ( !newFile.exists() || flags_.overwrite_ )
					{
						if ( flags_.verbose_ )
							cout << "\t" << oldFile.getFileName() << "... ";

						result = os.copyFile(oldFile.getFullPath(), newFile.getFullPath());

						// Step 5: the copied file should be deleted from the source directory if "move"
						// was requested
						if ( result && flags_.move_ )
							result = oldFile.remove();
	
						if ( flags_.verbose_ )
						{
							if ( result )
							{
								if ( flags_.move_ )
									cout << "Moved" << endl;
								else
									cout << "Copied" << endl;
							}
							else
							{
								if ( flags_.move_ )
									cout << "Failed to move" << endl;
								else
									cout << "Failed to copy" << endl;
							}
						}
					}
					else
					{
						if ( flags_.verbose_ )
						{
							if ( flags_.move_ )
								cout << "Failed to move, destination already existed" << endl;
							else
								cout << "Failed to copy, destination already existed" << endl;
						}
					}

					break;
				}
				case DirectoryEntry::DIR_ENTRY:
				{
					// Step 6: if the navigated entry is a subdirectory and "recursive" was
					// requested, the entry will recursively become the new source directory and
					// the operation is repeated. Otherwise ignore the entry
					if ( flags_.recursive_ )
					{
						Directory newSource(sourceDuplicate.getFileName(iter->getName()));
						Directory newDestination(getFileName(iter->getName()));
						newDestination.setFlags(*this);
						result = newDestination << contents << newSource;

						// Step 7: the copied subdirectory should be deleted from the source directory
						// if "move" was requested
						if ( result && flags_.move_ )
						{
							if ( flags_.verbose_ )
								cout << newSource.getFullPath() << "... ";

							result = newSource.remove();
						}

						if ( flags_.move_ && flags_.verbose_ )
						{
							if ( result )
								cout << "Removed" << endl;
							else
								cout << "Failed to delete" << endl;
						}
					}

					break;
				}
            #if defined(LINUX_FIX)
                default:
                    // Nothing: handle for other cases.
                    break;
            #endif
			}

			if ( !result )
				break;

			++iter;
		}
	}

	// Step 8: delete the original source directory if "move" but not "contents" were
	// requested
	if ( result && flags_.move_ && !flags_.contents_ )
		source.remove();

	// Step 9: set all copy flags to their defaults so current values do not affect next copy
	setDefaultFlags();


	return result;
}


bool Directory::navigate(EntryNavigator& nav, bool recursive, Selection sel) const
{
	bool result = true;
	const Platform& os = Platform::getInstance();
	Filename selection = getFileName(os.allFiles());

	Platform::FileSearch* fs = os.findFileFirst(selection.getFullPath());
	while ( fs )
	{
		if ( fs->isFile() )
		{
			if ( sel == SELECT_FILES || sel == SELECT_FILES_DIRS )
			{
				if ( !nav(*this, DirectoryEntry(fs)) )
					result = false;
			}
		}

		if ( fs->isDirectory() && !defaultDirectory(fs->getFileName()) )
		{
			if ( sel == SELECT_DIRS || sel == SELECT_FILES_DIRS )
			{
				if ( !nav(*this, DirectoryEntry(fs)) )
					result = false;
			}

			if ( recursive )
			{
				Directory subdir(getFileName(fs->getFileName()));
				subdir.navigate(nav, recursive, sel);
			}
		}

		if ( !os.findFileNext(fs) )
			break;
	}

	if ( fs )
		os.findFileDelete(fs);

	return result;
}


bool Directory::defaultDirectory(const std::string& entry) const
{
	if ( entry.size() == 1 && entry[0] == '.' )
		return true;
	if ( entry.size() == 2 && entry[0] == '.' && entry[1] == '.' )
		return true;

	return false;
}



bool Directory::Sorter::operator()(const DirectoryEntry& e1,
		const DirectoryEntry& e2) const
{
	switch ( sort_ )
	{
		case Directory::NAME_SORT:
		{
			if ( e1.getName() != e2.getName() )
				return e1.getName() < e2.getName();

			break;
		}
		case Directory::MODIFIED_SORT:
		{
			if ( e1.getModifiedDate() != e2.getModifiedDate() )
				return e1.getModifiedDate() < e2.getModifiedDate();

			break;
		}
		case Directory::SIZE_SORT:
		{
			if ( e1.getSize() != e2.getSize() )
				return e1.getSize() < e2.getSize();

			break;
		}
    #if defined(LINUX_FIX)
        default:
            // Nothing: handle the other cases.
            break;
    #endif
	}

	return e1.getOrder() < e2.getOrder();
}


ostream& Directory::output(ostream& os, int indentation) const
{
	static int totalFiles = 0;
	static int totalDirectories = 0;

	// Lets print the directory's name first
	os << setw(indentation * 2) << " " << getDirectoryName() << "/" << endl;
	++indentation;
	if ( indentation == 1 )
	{
		totalFiles = 0;
		totalDirectories = 0;
	}

	// First select all files
	selectAllFiles();

	// Lets print their names
	int localFiles = 0;
	Directory::const_iterator iter = begin();
	while ( iter != end() )
	{
		os << setw(indentation * 2) << " " << (*iter++).getName() << endl;
		++localFiles;
		++totalFiles;
	}
	// Print number of files
	if ( localFiles )
		os << setw(indentation * 2) << " " << localFiles << " files in this directory" << endl;

	// Now select all subdirectories
	selectAllSubdirectories();

	// Lets navigate them all recursively
	iter = begin();
	while ( iter != end() )
	{
		Directory d(getFileName((*iter++).getName()));
		d.output(os, indentation);
		++totalDirectories;
	}

	if ( indentation == 1 )  // Back to top level
	{
		os << endl;
		os << setw(indentation * 2) << " " << totalFiles << " total files" << endl;
		os << setw(indentation * 2) << " " << totalDirectories << " total directories" << endl;
	}


	return os;
}


void Directory::getAllAvailableFiles(std::string dirStr, DirectoryFileList* resultList, std::string extension)
{
//	Remember to create or clear resultList BEFORE calling this method, since it is recursive.
//	That way you will get the correct list.
	if (resultList && dirStr.size())
	{
		util_filesystem::Directory processingDir("");
		processingDir.appendPath(dirStr);
		if (processingDir.exists())
		{
			Filename fileType(extension);
			util_filesystem::Directory::const_iterator dirIter;
			processingDir.select("*");			//	Get the list of files/folders in this directory
			dirIter = processingDir.begin();
			while (dirIter != processingDir.end())
			{
				bool isDirectory = dirIter->isDirectory();
				bool isFile = dirIter->isFile();

				if (isFile)
				{
//					Debug::trace("ProcessDownloadedFiles", "getAllAvailableFiles(" + dirStr + "):  File = " + dirIter->getName());
					File theFile (processingDir.getFullPath(), dirIter->getName());
					if ((theFile.getFileNameExtension() == fileType.getFileNameExtension()) || (fileType.getFileNameExtension() == "*"))
					{
						resultList->insert(make_pair(theFile.getFullPath(), (unsigned int)dirIter->getSize()));
						Debug::trace("ProcessDownloadedFiles", "getAllAvailableFiles(" + dirStr + "):  Added to resultList [" + theFile.getFullPath() + "] with a size of " + util_string::longlongtostring(dirIter->getSize()));
					}
				}
				else if (isDirectory)
				{
//					Debug::trace("ProcessDownloadedFiles", "getAllAvailableFiles(" + dirStr + "):  Directory = " + dirIter->getName());
					util_filesystem::Directory subDir(processingDir);
					subDir.appendPath(dirIter->getName());
					getAllAvailableFiles(subDir.getFullPath(), resultList, extension);
				}
				++dirIter;
			}
		}
	}
}


#ifdef VXWORKS
STATUS copyDirectory(const char* sourceDir, const char* destinationDir, BOOL asSubDir, BOOL verboseMode)
{
	Directory s(sourceDir), d(destinationDir);

	if ( asSubDir )
	{
		if ( verboseMode )
			cout << "Copying directory " << sourceDir << " into directory " <<
					destinationDir << endl;

		if ( verboseMode )
			return d << verbose << recursive << s;
		else
			return d << recursive << s;
	}
	else
	{
		if ( verboseMode )
			cout << "Copying contents of directory " << sourceDir << " into directory " <<
					destinationDir << endl;

		if ( verboseMode )
			return d << verbose << contents << recursive << s;
		else
			return d << contents << recursive << s;
	}
}


STATUS copyFile(const char* sourceFile, const char* destinationDir, BOOL verboseMode)
{
	File f(sourceFile);
	Directory d(destinationDir);

	if ( verboseMode )
		return d << verbose << f;
	else
		return d << f;
}


STATUS createDirectory(const char* dir)
{
	Directory d(dir);

	return d.create();
}


STATUS directoryExists(const char* dir)
{
	Directory d(dir);

	return d.exists();
}


const char* getNFSRootDirectory()
{
	return Platform::getInstance().getNFSRootDirectory();
}


STATUS copyNFSDirectory(const char* sourceDir, BOOL isSrcNFS,
		const char* destinationDir, BOOL isDestNFS, BOOL asSubDir, BOOL verbose)
{
	char source[strlen(sourceDir) + Directory::NFS_ROOTDIRECTORY_SIZE];
	char destination[strlen(destinationDir) + Directory::NFS_ROOTDIRECTORY_SIZE];
	const char* nfsRoot = getNFSRootDirectory();

	if (isSrcNFS)
	{
		strcpy(source, nfsRoot);
		strcat(source, sourceDir);
	}
	else
		strcpy(source, sourceDir);

	if (isDestNFS)
	{
		strcpy(destination, nfsRoot);
		strcat(destination, destinationDir);
	}
	else
		strcpy(destination, destinationDir);

	return copyDirectory(source, destination, asSubDir, verbose);
}


STATUS copyNFSFile(const char* sourceFile, BOOL isSrcNFS,
		const char* destinationDir, BOOL isDestNFS, BOOL verbose)
{
	char source[strlen(sourceFile) + Directory::NFS_ROOTDIRECTORY_SIZE];
	char destination[strlen(destinationDir) + Directory::NFS_ROOTDIRECTORY_SIZE];
	const char* nfsRoot = getNFSRootDirectory();

	if (isSrcNFS)
	{
		strcpy(source, nfsRoot);
		strcat(source, sourceFile);
	}
	else
		strcpy(source, sourceFile);

	if (isDestNFS)
	{
		strcpy(destination, nfsRoot);
		strcat(destination, destinationDir);
	}
	else
		strcpy(destination, destinationDir);

	return copyFile(source, destination, verbose);
}


STATUS printDirectoryContents(const char* dir)
{
	Directory d(dir);
	cout << d;

	return OK;
}


STATUS removeDirectoryContents(const char* dir, BOOL recursive)
{
	Directory d(dir);

	return d.removeContents(recursive);
}

#endif



#ifndef NO_NAMESPACES
} // namespace util_filesystem
#endif

