#include "GraphicHeader.h"
#include "string.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GraphicHeader::dataType = 0;
std::string GraphicHeader::dataTypeName("GraphicHeader"); 
#endif

GraphicHeader::GraphicHeader()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

#if defined(LINUX_FIX)
    // INT32 byteCounter ::: Unused variable
#else
	INT32 byteCounter = 0;
#endif
	base = 0;
}
GraphicHeader::GraphicHeader(Blob *blob)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	INT32 byteCounter = 0;
	
	if(!blob->isNibblized())
	{
	 byteCounter =	setResourceType(blob, byteCounter);
	 byteCounter =	setResourceId(blob, byteCounter);
	 byteCounter =	setResourceLoaded(blob, byteCounter);
	 byteCounter =	setResourceLength(blob, byteCounter);
	 byteCounter =	setResourceName(blob, byteCounter);
	 byteCounter =	setBase(blob, byteCounter);
	 byteCounter =	setPitch(blob, byteCounter);
	 byteCounter =	setLeft(blob, byteCounter);
	 byteCounter =	setTop(blob, byteCounter);
	 byteCounter =	setRight(blob, byteCounter);
	 byteCounter =	setBottom(blob, byteCounter);
	 byteCounter += sizeOfResType; 	//for the dummy characters
	 sizeOfHeader = byteCounter;
	}
	return;
}
GraphicHeader::~GraphicHeader()
{

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

INT8 *GraphicHeader::getResourceType()
{
	return resourceType;
}
void GraphicHeader::setResourceType(char *type)				//for test only, removed when finished
{
	resourceType[0] = type[0];
	resourceType[1] = type[1];
	resourceType[2] = type[2];
	resourceType[3] = type[3];	
}
INT32 GraphicHeader::setResourceType(Blob *blob, INT32 counter)
{
	blob->getBytes((long int)counter, (UINT8 *)resourceType, (UINT32)sizeOfResType);
	return counter += sizeOfResType;
}

UINT16 GraphicHeader::getResourceId()
{
	return resourceId;
}
void GraphicHeader::setResourceId(UINT16 id)					//for test only, removed when finished
{
	resourceId = id;
}
INT32 GraphicHeader::setResourceId(Blob *blob, INT32 counter)
{
	resourceId = blob->getUint16(counter);
	resourceId = blob->ShortSwapBytes(resourceId);
	return counter += sizeof(resourceId);
	
	  
}

INT16 GraphicHeader::getResourceLoaded()
{
	return resourceLoaded;
}
void GraphicHeader::setResourceLoaded(INT16 load)				//for test only, removed when finished
{
	resourceLoaded = load;
}
INT32 GraphicHeader::setResourceLoaded(Blob *blob, INT32 counter)
{
	resourceLoaded = blob->getInt16(counter);
	resourceLoaded = blob->ShortSwapBytes(resourceLoaded);
	return counter += sizeof(resourceLoaded);
	
}

INT16 GraphicHeader::getResourceLength()
{
	return resourceLength;
}
void GraphicHeader::setResourceLength(INT16 length)			//for test only, removed when finished
{
	resourceLength = length;
}
INT32 GraphicHeader::setResourceLength(Blob *blob, INT32 counter)
{
	resourceLength = blob->getInt16(counter);
	resourceLength = blob->ShortSwapBytes(resourceLength);
	return counter += sizeof(resourceLength);
}

INT8 *GraphicHeader::getResourceName()
{
	return name;
}
void GraphicHeader::setResourceName(char *resName)				
{
	strncpy((char * )name, resName, sizeOfResName);
}
INT32 GraphicHeader::setResourceName(Blob *blob, INT32 counter)
{
	blob->getBytes((long int)counter, (UINT8 *)name, (UINT32)sizeOfResName);
	return counter += sizeOfResName; 
}

UINT32 GraphicHeader::getBase()
{
	return base;
}
void GraphicHeader::setBase(UINT32 address) 		
{
	base = address;
}
INT32 GraphicHeader::setBase(Blob *blob, INT32 counter)
{
	base = blob->getUint32(counter);
	base = blob->ULongSwapBytes(base);
	return counter += sizeof(base);
}

INT16 GraphicHeader::getPitch()
{
	return pitch;
}
void GraphicHeader::setPitch(INT16 bitPitch)	 	//for test only, removed when finished
{
	pitch = bitPitch;
}
INT32 GraphicHeader::setPitch(Blob *blob, INT32 counter)
{
	pitch = blob->getInt16(counter);
	pitch = blob->ShortSwapBytes(pitch);
	return counter += sizeof(pitch);
}

INT16 GraphicHeader::getLeft()
{
	return left;
}
void GraphicHeader::setLeft(INT16 bitLeft)		//for test only, removed when finished
{
	left = bitLeft;
}
INT32 GraphicHeader::setLeft(Blob *blob, INT32 counter)
{
	left = blob->getInt16(counter);
	left = blob->ShortSwapBytes(left);
	return counter += sizeof(left);

}

INT16 GraphicHeader::getTop()
{
	return top;
}
void GraphicHeader::setTop(INT16 bitTop)			//for test only, removed when finished
{
	top = bitTop;
}
INT32 GraphicHeader::setTop(Blob *blob, INT32 counter)
{
	top = blob->getInt16(counter);
	top = blob->ShortSwapBytes(top);
	return counter += sizeof(top);
}

INT16 GraphicHeader::getRight()
{
	return right;
}
void GraphicHeader::setRight(INT16 bitRight)			//for test only, removed when finished
{
	right = bitRight;
}
INT32 GraphicHeader::setRight(Blob *blob, INT32 counter)
{
	right = blob->getInt16(counter);
	right = blob->ShortSwapBytes(right);
	return counter += sizeof(right);
}

INT16 GraphicHeader::getBottom()
{
	return bottom;
}
void GraphicHeader::setBottom(INT16 bitBottom)		//for test only, removed when finished
{
	bottom = bitBottom;
}
INT32 GraphicHeader::setBottom(Blob *blob, INT32 counter)
{
	bottom = blob->getInt16(counter);
	bottom = blob->ShortSwapBytes(bottom);
	return counter += sizeof(bottom);
}

INT32 GraphicHeader::getHeaderSize()
{
	return sizeOfHeader;
}


void GraphicHeader::show()
{
	// cout << "GraphicHeader: pitch:" << pitch << "  left:" << left << "  right:" << right << "  top:" << top << "  bottom:" << bottom << endl;
}

