#ifndef RESOURCEHEADER_H
#define RESOURCEHEADER_H

#include "Blob.h"


class ResourceHeader
{
	public:
		ResourceHeader();
		~ResourceHeader();

		int getMajorVersion() { return majorVersion_; }
		void setMajorVersion(int majorVersion) { majorVersion_ = majorVersion; }
		INT32 setMajorVersion(Blob *blob, INT32 counter);

		int getMinorVersion() { return minorVersion_; }
		void setMinorVersion(int minorVersion) { minorVersion_ = minorVersion; }
		INT32 setMinorVersion(Blob *blob, INT32 counter);

		UINT8 *getResourceType() { return resourceType; }
		void setResourceType(const char* type);
		INT32 setResourceType(Blob *blob, INT32 counter);

		UINT16 getResourceId() { return resourceId; }
		void setResourceId(UINT16 id) { resourceId = id; }
		INT32  setResourceId(Blob *blob, INT32 counter);

		bool isResourceLoaded() { return (resourceLoaded != 0); }
		void setResourceLoaded(bool loaded) { resourceLoaded = loaded ? 1 : 0; }
		INT32 setResourceLoaded(Blob *blob, INT32 counter);
    
		INT16 getResourceLength() { return resourceLength; }
    
		void setResourceLength(int length) { resourceLength = (UINT16)length; }
		INT32 setResourceLength(Blob *blob, INT32 counter);

		UINT8 *getResourceName() { return name; }
		void setResourceName(const char* name);
		INT32 setResourceName(Blob *blob, INT32 counter);


		void writeBinaryData(Blob& binaryData);

		bool verifyTypeLabel();
		bool verifyVersion()
				{ return (majorVersion_ <= _currentMajorVersion_ && minorVersion_ <= _currentMinorVersion_); }

		static int getSize() { return 1 + 1 + _sizeOfType_ + 2 + 1 + 1 + 2 + _sizeOfName_; }

		static const int _currentMajorVersion_ = 1;
		static const int _currentMinorVersion_ = 0;
		static const int _sizeOfType_ = 4;
		static const int _sizeOfName_ = 32;

		void show();


	private:

		int majorVersion_, minorVersion_;  /* Resource format's major and minor versions */
		UINT8 resourceType[_sizeOfType_];  /* "LABL"=label, "FONT"=font, "PICT"=graphic */
		UINT16 resourceId;                 /* Resource Id */
		UINT8	resourceLoaded;              /* Indicates if data follows the header structure */
    	UINT16 resourceLength;             /* Length of resource's binary form (in bytes) */
    	UINT8 name[_sizeOfName_];          /* Resource name */

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//RESOURCEHEADER_H
