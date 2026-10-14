#include "ResourceHeader.h"

#include "Debug.h"
#include "HobString.h"

#include "utilstring.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ResourceHeader::dataType = 0;
std::string ResourceHeader::dataTypeName("ResourceHeader"); 
#endif

ResourceHeader::ResourceHeader()
	: majorVersion_(_currentMajorVersion_), minorVersion_(_currentMinorVersion_),
				  resourceId(65535), resourceLoaded(255), resourceLength(0) 
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	strcpy((char *)name, "Name Not Set!!");
}
ResourceHeader::~ResourceHeader() 
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

INT32 ResourceHeader::setMajorVersion(Blob *blob, INT32 counter)
{
	majorVersion_ = blob->getInt8(counter);
	return ++counter;
}


INT32 ResourceHeader::setMinorVersion(Blob *blob, INT32 counter)
{
	minorVersion_ = blob->getInt8(counter);
	return ++counter;
}


void ResourceHeader::setResourceType(const char* type)
{
	memcpy(resourceType, type, _sizeOfType_);
}


INT32 ResourceHeader::setResourceType(Blob *blob, INT32 counter)
{
	blob->getBytes(counter, resourceType, _sizeOfType_);
	return counter += _sizeOfType_;
}


INT32 ResourceHeader::setResourceId(Blob *blob, INT32 counter)
{
	resourceId = blob->getUint16(counter);
	resourceId = blob->ShortSwapBytes(resourceId);
	return counter += sizeof(resourceId);
}


INT32 ResourceHeader::setResourceLoaded(Blob *blob, INT32 counter)
{
	resourceLoaded = blob->getInt8(counter);
	return counter += (sizeof(resourceLoaded) + 1);
}


INT32 ResourceHeader::setResourceLength(Blob *blob, INT32 counter)
{
	resourceLength = blob->getUint16(counter);
	resourceLength = blob->UShortSwapBytes( (unsigned short)resourceLength );
	return counter += sizeof(resourceLength);
}


void ResourceHeader::setResourceName(const char* n)
{
	size_t size = strlen(n);

#if defined(LINUX_FIX)
	size = size < (size_t)_sizeOfName_ ? size + 1 : (size_t)_sizeOfName_;
#else
	size = size < _sizeOfName_ ? size + 1 : _sizeOfName_;
#endif
	memcpy(name, n, _sizeOfName_);
}


INT32 ResourceHeader::setResourceName(Blob *blob, INT32 counter)
{
	blob->getBytes(counter, name, _sizeOfName_);
	return counter += _sizeOfName_;
}


void ResourceHeader::writeBinaryData(Blob& binaryData)
{
	binaryData.append((UINT8)majorVersion_);
	binaryData.append((UINT8)minorVersion_);

	binaryData.append(resourceType, _sizeOfType_);

	UINT16 temp = binaryData.ShortSwapBytes(resourceId);
	binaryData.append(temp);

	binaryData.append(resourceLoaded);
	binaryData.append((UINT8)0);

	temp = binaryData.ShortSwapBytes((short)resourceLength);
	binaryData.append(temp);

	binaryData.append(name, _sizeOfName_);
}


bool ResourceHeader::verifyTypeLabel()
{
	return  resourceType[0] == 'L' &&
			resourceType[1] == 'A' &&
			resourceType[2] == 'B' &&
			resourceType[3] == 'L';
}


void ResourceHeader::show()
{
//	cout << "ResourceHeader - majorVer:" << majorVersion_ << " minorVer:" << minorVersion_ <<
//				" resourceId:" << resourceId << " resourceLoaded:" << resourceLoaded <<
//				" resourceLength:" << resourceLength << " name:'" << name << "'" << endl;

	if ( Debug::isTracing("LabelType.binary") )
	{
		Debug::trace("LabelType.binary", 24, "Resource header data:");
		Debug::trace("LabelType.binary", 24, "   Major Version:   " + util_string::inttostring(majorVersion_));
		Debug::trace("LabelType.binary", 24, "   Minor Version:   " + util_string::inttostring(minorVersion_));
		Debug::trace("LabelType.binary", 24, "   Resource ID:     " + util_string::inttostring(resourceId));
		Debug::trace("LabelType.binary", 24, "   Resource Loaded: " + util_string::inttostring(resourceLoaded));
		Debug::trace("LabelType.binary", 24, "   Resource Length: " + util_string::inttostring(resourceLength));
		Debug::trace("LabelType.binary", 24, "   Resource Name:   " + std::string((const char*)name));
	}
}
