#include "GraphicImage.h"

#include <iostream>
#include <string.h>
using namespace std;

#include "png2hob.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GraphicImage::dataType = 0;
std::string GraphicImage::dataTypeName("GraphicImage"); 
#endif

GraphicImage::GraphicImage(string filename)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
																					
	string::size_type idx;

	init();
	
	if((idx = filename.rfind(".png")) != string::npos || (idx = filename.rfind(".PNG")) != string::npos)
	{
	 	createGraphicImageFromPng(filename);
	}
	else if((idx = filename.rfind(".hob")) != string::npos)
	{
		 createGraphicImageFromHob(filename);
	}
	else
	{
		cout<<"GraphicImage -- invalid file extension"<<endl;
	}
}

GraphicImage::GraphicImage()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	
	setFileName("/ram0/TestImage");
	setHobFileName(getFileName());
	setPngFileName(getFileName());	
}
GraphicImage::GraphicImage(Blob *blob)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	
	header = new GraphicHeader(blob);
	setHeaderSize();
	setNibblizedRecordSize(blob);
	setBinaryImageSize();
	setBinaryImage(blob);
	setFileName("/ram0/TestImage");
	setHobFileName(getFileName());
	setPngFileName(getFileName());
	createImageRecord();
}

GraphicImage::~GraphicImage()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if ( header )
		delete header;

	if ( graphic )
		delete graphic;

	if ( binaryImage )
		free( binaryImage );
}

void GraphicImage::init()
{
	nibblizedRecord = NULL;
	nibblizedRecordSize = 0;
	header = NULL;
	headerSize = 0;
	binaryImage = NULL;
	binaryImageSize = 0;
	graphic = NULL;
	setFileName("");
	setHoFileName("");
	setHobFileName("");
	setPngFileName("");
}


void GraphicImage::createGraphicImageFromPng(string PngFilename)
{
//convert the png file to a dominator GraphicImage object
	graphic = new IMAGE_RECORD;
	header = new GraphicHeader;

	graphic->file_name = NULL;
	graphic->ho__file_name = NULL;
	graphic->hob_file_name = NULL;
	graphic->png_file_name = NULL;

	graphic->binary_image = NULL;
	graphic->binary_image_size = 0;

	pngFileName = PngFilename;
	graphic->png_file_name = const_cast<char *> (PngFilename.c_str());

	pingToHobStatus = PngToHob(graphic);
	if(pingToHobStatus == STATUS_OK)
	{
		nibblizedRecord = graphic->nibblized_record;
		nibblizedRecordSize = graphic->nibblized_record_size;
		header->setResourceType(graphic->res_type);
	 	header->setResourceId(graphic->res_id);
		header->setResourceLoaded(graphic->res_loaded);
		header->setResourceLength(graphic->res_length);
		header->setResourceName(graphic->Name);
		header->setBase(graphic->base);
		header->setPitch(graphic->pitch);
		header->setLeft(graphic->left);
		header->setTop(graphic->top);
		header->setRight(graphic->right);
		header->setBottom(graphic->bottom);
		headerSize = graphic->header_size;
		binaryImage = graphic->binary_image;
		binaryImageSize = graphic->binary_image_size;
		if (graphic->file_name)
			fileName.assign(graphic->file_name);
		if (graphic->ho__file_name)
			hoFileName.assign(graphic->ho__file_name);
		if (graphic->hob_file_name)
			hobFileName.assign(graphic->hob_file_name);
		if (graphic->png_file_name)
 			pngFileName.assign(graphic->png_file_name);

		long i;
		//UINT32 size = graphic->bottom * graphic->pitch;  
		//invert the binary image
		for(i = 0 ; i < graphic->binary_image_size; i++ )
		{
			*(binaryImage + i ) = (~ ( *(binaryImage + i ) ));
		}
	}
	else
	{
		//	PTR 1667: These pointers are not initialized properly, so get rid of them.
		if (header)
		{
			delete header;
			header = NULL;
		}

		if (graphic)
		{
			if ( graphic->binary_image )
				free( graphic->binary_image );

			delete graphic;
			graphic = NULL;
		}
		cout << "GraphicImage -- conversion from png failed: " << pingToHobStatus << endl;
  	}
}

void GraphicImage::createGraphicImageFromHob(string /* filename */)
{

}
char *GraphicImage::getNibblizedRecord()
{
	return nibblizedRecord;
}
void GraphicImage::setNibblizedRecord(char *buffer)
{
	nibblizedRecord	= buffer;
}

long GraphicImage::getNibblizedRecordSize()
{
	return nibblizedRecordSize;
}
void GraphicImage::setNibblizedRecordSize(Blob *blob)
{
	//because the graphic is read in a binary stream
	nibblizedRecordSize	= blob->getSize();
}

GraphicHeader *GraphicImage::getHeader()
{
	return	header;
}
void GraphicImage::setHeader(GraphicHeader *graphicHeader)
{
	header = graphicHeader;
}

long GraphicImage::getHeaderSize()
{
	return headerSize; 
}
void GraphicImage::setHeaderSize()
{
	headerSize = header->getHeaderSize();
}

char *GraphicImage::getBinaryImage()
{
	return binaryImage;
}
void GraphicImage::setBinaryBuffer(int size)
{
	if (binaryImage)
	{
		free (binaryImage);
		binaryImage = NULL;
	}
	binaryImage	= (char *) malloc(size);
}
void GraphicImage::setBinaryImage(Blob *blob)
{
	if (binaryImage)
	{
		free (binaryImage);
		binaryImage = NULL;
	}
	if (binaryImageSize > 0)
	{
		binaryImage	= (char *) malloc(binaryImageSize);

		if (binaryImage)
			blob->getBytes(headerSize, (UINT8 *)binaryImage, (UINT32)binaryImageSize); 
	}
}
void GraphicImage::setBinaryImage(const char *image, int size)
{
	if (binaryImage)
	{
		free (binaryImage);
		binaryImage = NULL;
	}
	if (size > 0)
	{
		binaryImage	= (char *) malloc(size);
		binaryImageSize = size;
		if (binaryImage)
		{
			memcpy(binaryImage, image, size);

			// Assume the image being set is valid.
			pingToHobStatus = STATUS_OK;

			if (header)
			{
				delete header;
				header = (GraphicHeader *)NULL;
				headerSize = 0L;
			}
		}
	}
}

long GraphicImage::getBinaryImageSize()
{
	return binaryImageSize;
}
void GraphicImage::setBinaryImageSize()
{
	binaryImageSize = nibblizedRecordSize - headerSize;
}

string GraphicImage::getFileName()
{
	return fileName;
}
void GraphicImage::setFileName(string name)
{
	fileName.assign(name); 
}

string GraphicImage::getHoFileName()
{
	return hoFileName;
}
void GraphicImage::setHoFileName(string name)
{
	hoFileName.assign(name); 
	hoFileName.append(NIBBLIZED_FILE_EXTENSION); 
}

string GraphicImage::getHobFileName()
{
	return hobFileName;
}
void GraphicImage::setHobFileName(string name)
{
	hobFileName.assign(name); 
	hobFileName.append(BINARY_FILE_EXTENSION); 
}

string GraphicImage::getPngFileName()
{
	return pngFileName;
} 
void GraphicImage::setPngFileName(string name)
{
	pngFileName.assign(name); 
	pngFileName.append(PNG_FILE_EXTENSION); 
}

IMAGE_RECORD *GraphicImage::getImageRecord()
{
	return graphic;
}
void GraphicImage::setImageRecord(IMAGE_RECORD *imageRecord)
{
	graphic = imageRecord;
}
void GraphicImage::createImageRecord()
{
	graphic = new IMAGE_RECORD;
	 
	graphic->nibblized_record = 	nibblizedRecord;
	graphic->nibblized_record_size = nibblizedRecordSize;
	strcpy(graphic->res_type, ( char * ) header->getResourceType());
	graphic->res_id = header->getResourceId();
	graphic->res_loaded = header->getResourceLoaded();
	graphic->res_length = header->getResourceLength();
	strcpy(graphic->Name, ( char * ) header->getResourceName());
	graphic->base = header->getBase();
	graphic->pitch = header->getPitch();
	graphic->left = header->getLeft();
	graphic->top = header->getTop();
	graphic->right = header->getRight();
	graphic->bottom = header->getBottom();
	graphic->header_size = headerSize;
	graphic->binary_image = binaryImage;
	graphic->binary_image_size = binaryImageSize;
	graphic->file_name = const_cast<char *> (fileName.c_str()); 
	graphic->ho__file_name = const_cast<char *> (hoFileName.c_str()); 
	graphic->hob_file_name = const_cast<char *> (hobFileName.c_str()); 
	graphic->png_file_name = const_cast<char *> (pngFileName.c_str());
}

bool GraphicImage::isValid()
{
	if(pingToHobStatus == STATUS_OK)
		return true;
	else
		return false;
}

RetStatus GraphicImage::getPingToHobStatus()
{
	return pingToHobStatus;
}

void GraphicImage::setPingToHobStatus(RetStatus status)
{
	pingToHobStatus = status;
}

/*
void GraphicImage::invertGraphicImage()
{
	UINT32 i;
	UINT32 size = graphic->bottom * graphic->pitch;  
	//invert the binary image
	for(i = 0 ; i < size; i++ )
	{
		*(graphic->binary_image + i ) = (~ ( *(graphic->binary_image + i ) ));
	}
}
*/
