#include "PAL_platformheader.h"

#include "filename.h"



#if !defined( _WIN32 )
#  include "../platform/platform.h"
#else
#  include "..\platform\platform.h"
#endif



#ifndef NO_NAMESPACES
namespace util_filesystem
{
#endif


#ifndef NO_NAMESPACES
using util_platform::Platform;
#endif
using namespace std;


Filename::Filename(const string& fullname)
		: fullpath_(), path_(), filename_(), drive_(), namebase_(), extension_()
{
	parsePath(fullname, true, false);
	setFullPathInternal();
}


Filename::Filename(const std::string& path, const std::string& filename)
		: fullpath_(), path_(), filename_(), drive_(), namebase_(), extension_()
{
	parsePath(path, false, false);
	parsePath(filename, true, true);
	setFullPathInternal();
}


Filename::Filename(const Filename& fn)
		: fullpath_(fn.fullpath_), path_(fn.path_), filename_(fn.filename_),
		drive_(fn.drive_), namebase_(fn.namebase_), extension_(fn.extension_)
{
}


Filename::~Filename()
{
}


void Filename::setDriveName(const std::string& drive)
{
	const Platform& os = Platform::getInstance();
	string::size_type oldIndex = os.parseDriveName(path_);
	string::size_type newIndex = os.parseDriveName(drive);
	newIndex = (newIndex == string::npos) ? 0 : newIndex;

	// Replace the old volume information, if any, with the new volume information, if any
	path_.replace(0, (oldIndex == string::npos) ? 0 : oldIndex, drive, 0, newIndex);

	drive_.assign(drive, 0, newIndex);

	// Since the client code has modified part of the path, we need to rebuild the fullpath.
	setFullPathInternal();
}


void Filename::setFileNameBase(const std::string& name)
{
	string oldExtension(extension_);

	parseFilename(name);

	string::size_type dot = filename_.find_last_of('.');
	if ( dot != string::npos )
		filename_.replace(dot + 1, filename_.size(), oldExtension);

	extension_ = oldExtension;

	// Since the client code has modified part of the name, we need to rebuild the fullpath.
	setFullPathInternal();
}


void Filename::setFileNameExtension(const std::string& ext)
{
	string::size_type oldDot = filename_.find_last_of('.');
	string::size_type newDot = ext.find_last_of('.');
	string::size_type extBegin = (newDot == string::npos) ? 0 : newDot + 1;
	string::size_type extSize = ext.size();

	if ( oldDot == string::npos )
	{
		// If the original filename had no extension, add the dot and append the extension
		if ( extBegin < extSize )
		{
			filename_ += '.';
			filename_.append(ext, extBegin, extSize - extBegin);
			extension_.assign(ext, extBegin, extSize - extBegin);
		}
		else
			extension_.erase();
	}
	else
	{
		// If the original filename had a extension, replace it with the new one
		if ( extBegin < extSize )
		{
			++oldDot;
			filename_.replace(oldDot, filename_.size() - oldDot, ext, extBegin, extSize - extBegin);
			extension_.assign(ext, extBegin, extSize - extBegin);
		}
		else
		{
			filename_.erase(oldDot);
			extension_.erase();
		}
	}

	// Since the client code has modified part of the name, we need to rebuild the fullpath.
	setFullPathInternal();
}


bool Filename::hasParent() const
{
	const Platform& os = Platform::getInstance();
	string::size_type drive = os.parseDriveName(path_);

	return !( os.isRootDirectory(path_) || path_.size() == drive );
}


bool Filename::isAbsolutePath() const
{
	return Platform::getInstance().isAbsolutePath(filename_);
}


bool Filename::isRootDirectory() const
{
	return Platform::getInstance().isRootDirectory(filename_);
}



// This function receives the path associated with this filename and parses it, assigning
// values to "path_" and "filename_" data members appropriately. If "includesFilename"
// is true, then the last element in the path is a filename, otherwise it is a subdirectory.
// If "appendPath" is true, then the new path is appended to the current "path_",
// instead of replacing it (approach used when it is false).
void Filename::parsePath(const std::string& path, bool includesFilename, bool appendPath)
{
	const Platform& os = Platform::getInstance();
	string tempPath;

	// 1. First substitute any "altSlash" with "slash" into "tempPath". This is
	// done to enable the receiving of paths from different types of file systems
	// (i.e., Windows style versus UNIX style)
	string::size_type index1 = 0;
	string::size_type index2 = path.find_first_of(os.altSlash(), index1);
	while ( index2 != string::npos )
	{
		if ( index2 - index1 )
			tempPath.append(path, index1, index2 - index1);

		tempPath.append(1, os.slash());

		index1 = index2 + 1;
		index2 = path.find_first_of(os.altSlash(), index1);
	}
	// Add the rest (possibly the whole) of the path to "tempPath"
	if ( index1 < path.size() )
		tempPath.append(path, index1, path.size() - index1);


	// 2. Next, separate "name.extension" from the parent "directory" and process it
	if ( includesFilename )
	{
		// If there is an ending slash, then there is no filename, only directory
		if ( tempPath.size() && tempPath.at(tempPath.size() - 1) != os.slash() )
		{
			// Separate "name.extension" from parent "directory"
			string::size_type index = tempPath.find_last_of(os.slash());
			if ( index != string::npos )
			{
				parseFilename(tempPath.substr(index + 1));
				// Leave the slash in case it refers to the root directory
				tempPath.erase(index + 1);
			}
			else
			{
				parseFilename(tempPath);
				return;
			}
		}
	}


	// 3. Parse now the "directory" and append it to the current "directory" if it applies
	string::size_type size = tempPath.size();
	if ( size )
	{
		// If the "directory" is an absolute path or "appendPath" is false, then we will
		// replace the current "directory", if any. Otherwise, we will append to it.
		if ( !appendPath || os.isAbsolutePath(tempPath) )
		{
			// If the path ends in slash and is not the root directory, remove it
			if ( size > 1 && tempPath[size - 1] == os.slash() && !os.isRootDirectory(tempPath) )
				path_.assign(tempPath, 0, size - 1);
			else
				path_.assign(tempPath);

			// Parse the "volume" portion of the path
			parseVolume();
		}
		else
		{  // Append path
			// Remove ending slash if any
			if ( size && tempPath[size - 1] == os.slash() )
				tempPath.erase(size - 1);

			// Add a slash if current directory is not root directory
			string::size_type oldSize = path_.size();
			if ( oldSize )
			{
				if ( oldSize > 1 || path_[0] != os.slash() )
					path_.append(1, os.slash());
			}

			// Assign/append value to "path_"
			path_.append(tempPath);
		}
	}
}


void Filename::parseVolume()
{
	string::size_type vol = Platform::getInstance().parseDriveName(path_);
	if ( vol )
		drive_.assign(path_, 0, vol);
	else
		drive_.erase();
}


void Filename::parseFilename(const std::string& filename)
{
	const Platform& os = Platform::getInstance();
	string tempFilename;

	// 1. First substitute any "altSlash" with "slash" into "tempFilename". 
	// This is done to enable the receiving of paths
	string::size_type index1 = 0;
	string::size_type index2 = filename.find_first_of(os.altSlash(), index1);
	while ( index2 != string::npos )
	{
		if ( index2 - index1 )
			tempFilename.append(filename, index1, index2 - index1);

		tempFilename.append(1, os.slash());

		index1 = index2 + 1;
		index2 = filename.find_first_of(os.altSlash(), index1);
	}
	// Add the rest (possibly the whole) of the path to "tempPath"
	if ( index1 < filename.size() )
		tempFilename.append(filename, index1, filename.size() - index1);


	// 2. Separate any path if present from the filename
	string::size_type nameBegin = tempFilename.find_last_of(os.slash());
	if ( nameBegin != string::npos )
	{
		parsePath(tempFilename.substr(0, nameBegin), false, true);
		++nameBegin;
	}
	else
		nameBegin = 0;

	filename_.assign(tempFilename, nameBegin, tempFilename.size() - nameBegin);

	parseBaseExtension();
}


void Filename::parseBaseExtension()
{
	// Separate "name" from "extension"
	string::size_type dot = filename_.find_last_of('.');
	if ( dot != string::npos )
	{
		namebase_.assign(filename_, 0, dot++);
		extension_.assign(filename_, dot, filename_.size() - dot);
	}
	else
	{
		namebase_.assign(filename_);
		extension_.erase();
	}
}


void Filename::setFullPathInternal()
{
	if ( path_.size() )
	{
		fullpath_ = path_;

		if ( filename_.size() )
		{
			const Platform& os = Platform::getInstance();

			// If the "path_" is the root directory then do not add a slash
			if ( os.isRootDirectory(path_) || path_.at(path_.size() - 1) == os.slash() )
				fullpath_ += filename_;
			else
				fullpath_ += os.slash() + filename_;
		}
	}
	else
		fullpath_ = filename_;
}



const std::string Filename::_empty_ = "";



#ifndef NO_NAMESPACES
} // namespace util_filesystem
#endif

