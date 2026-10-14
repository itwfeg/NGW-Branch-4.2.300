#ifndef GRAPHICHEADER
#define GRAPHICHEADER

#include "Blob.h"

class GraphicHeader
{
	public:
		GraphicHeader();
		GraphicHeader(Blob *blob);
		~GraphicHeader();

		INT8 *getResourceType();
		void setResourceType(char *type);				
		INT32 setResourceType(Blob *blob, INT32 counter);
		UINT16 getResourceId();
		void setResourceId(UINT16 id);					
		INT32  setResourceId(Blob *blob, INT32 counter);

		INT16 getResourceLoaded();
		void setResourceLoaded(INT16 load);				
		INT32 setResourceLoaded(Blob *blob, INT32 counter);
		
		INT16 getResourceLength();
		void setResourceLength(INT16 length);			
		INT32 setResourceLength(Blob *blob, INT32 counter);

		INT8 *getResourceName();
		void setResourceName(char *resName);				
		INT32 setResourceName(Blob *blob, INT32 counter);

		UINT32 getBase();
		void setBase(UINT32 address); 		
		INT32 setBase(Blob *blob, INT32 counter);
		
		INT16 getPitch();
		void setPitch(INT16 bitPitch); 	
		INT32 setPitch(Blob *blob, INT32 counter);
		
		INT16 getLeft();
		void setLeft(INT16 bitLeft);		
		INT32 setLeft(Blob *blob, INT32 counter);

		INT16 getTop();
		void setTop(INT16 bitTop);			
		INT32 setTop(Blob *blob, INT32 counter);

		INT16 getRight();
		void setRight(INT16 bitRight);		
		INT32 setRight(Blob *blob, INT32 counter);

		INT16 getBottom();
		void setBottom(INT16 bitBottom);		
		INT32 setBottom(Blob *blob, INT32 counter);

		INT32 getHeaderSize();

		void show();


	private:
		static const int sizeOfResType = 4;
		static const int sizeOfResName = 32;

		INT8 resourceType[sizeOfResType];
		UINT16 resourceId;
		INT16 resourceLoaded;
		INT16 resourceLength;
		INT8 name[sizeOfResName];

		UINT32 base;
		INT16 pitch;
		INT16 left;
		INT16 top;
		INT16 right;
		INT16 bottom;
		INT8 dummy[sizeOfResType]; 

		INT32 sizeOfHeader;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif


};
#endif	//GRAPHICHEADER_H


