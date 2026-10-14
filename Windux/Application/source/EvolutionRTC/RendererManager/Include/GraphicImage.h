#ifndef GRAPHICRECORD_H
#define GRAPHICRECORD_H
#include "Blob.h"
#include "GraphicHeader.h"
#include "ImageRecord.h"
#include "../../PngGraphics/Include/hob2png.h"


#define NIBBLIZED_FILE_EXTENSION	".ho_"
#define BINARY_FILE_EXTENSION		".hob"
#define PNG_FILE_EXTENSION          ".png"
#define SEPARATOR					'.'


#define DOS_MAX_FILE_NAME_LEN		8
#define DOS_MAX_FILE_EXT_LEN		3

#define _BLACK 						0    /* 1 is BLUE*/
#define _WHITE 						15



class GraphicImage
{
	public:

		GraphicImage();
		GraphicImage(std::string filename);
		GraphicImage(Blob *blob);	  //read as a binary file
		~GraphicImage();

		char *getNibblizedRecord();
		void setNibblizedRecord(char *buffer);
		long getNibblizedRecordSize();
		void setNibblizedRecordSize(Blob *blob);
		GraphicHeader *getHeader();
		void setHeader(GraphicHeader *graphicHeader);
		long getHeaderSize();
		void setHeaderSize();

		char *getBinaryImage();
		void setBinaryBuffer(int size);		//for test only
		void setBinaryImage(Blob *blob);
		void setBinaryImage(const char *image, int size);

		long getBinaryImageSize();
		void setBinaryImageSize();
		std::string getFileName();
		void setFileName(std::string name);

		std::string getHoFileName();
		void setHoFileName(std::string name);

		std::string getHobFileName();
		void setHobFileName(std::string name);

		std::string getPngFileName();
		void setPngFileName(std::string name);

		IMAGE_RECORD *getImageRecord();
		void setImageRecord(IMAGE_RECORD *imageRecord);
		void createImageRecord();

		bool isValid();

		RetStatus getPingToHobStatus();
		void setPingToHobStatus(RetStatus status);

	//	void GraphicImage::invertGraphicImage();

	private:
		void createGraphicImageFromPng(std::string PngFilename);	//to be used when converting a png to GraphicImage (hob format)
		void createGraphicImageFromHob(std::string filename);		//to be used when converting a hob to a GraphicImage (hob format)
		void init();

		char		*nibblizedRecord;
		long		nibblizedRecordSize;

		GraphicHeader *header;
		long		headerSize;
		char		*binaryImage;
		long		binaryImageSize;

		std::string		fileName;
		std::string		hoFileName;
		std::string		hobFileName;
		std::string		pngFileName;

		IMAGE_RECORD *graphic;
		RetStatus	pingToHobStatus;	//	In case the UI needs to know the problem.
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};


#endif	//GRAPHICIMAGE_H
