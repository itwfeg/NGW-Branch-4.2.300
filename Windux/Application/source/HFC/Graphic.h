/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "evolution.h"
#include "HfcObject.h"
#include "tags/ResourceTags.h"
#include "GraphicImage.h"
#include "filesys/file.h"

#include <list>

using namespace util_filesystem;	

class Graphic : public HfcObject
{
	public:

		static const short GRAPHIC1	= 0;
		static const short GRAPHIC2	= 1;
		static const short GRAPHIC3	= 2;
		static const short GRAPHIC4	= 3;

		static const short GRAPHICTYPE_ROM		= 0;
		static const short GRAPHICTYPE_CUSTOM	= 1;
		static const short GRAPHICTYPE_ALL		= 2;

		static const short TYPE_UNKNOWN				= 0;
		static const short TYPE_LABELGRAPHIC		= 1;
		static const short TYPE_USERLABELGRAPHIC	= 2;
		static const short TYPE_ROMLABELGRAPHIC		= 3;
		static const short TYPE_PRODUCTGRAPHIC		= 4;
		static const short TYPE_CATEGORYGRAPHIC		= 5;
		static const short TYPE_SPECIALSGRAPHIC		= 6;
		static const short TYPE_STREAMINGGRAPHIC	= 7;
		static const short TYPE_SELFSERVEGRAPHIC	= 8;
		static const short TYPE_SOFTKEYGRAPHIC		= 9;
		static const short TYPE_PLANOGRAMGRAPHIC	= 10;
		static const short TYPE_VIDEOS				= 11;

		static const short SUBTYPE_USER				= 1;
		static const short SUBTYPE_SYSTEM			= 2;
		static const short SUBTYPE_USERANDSYSTEM	= 3;


		Graphic();
		Graphic(const Graphic &g);
		Graphic(const char *data, int len, int formatType);
		Graphic(const std::string& graphicName, short type = TYPE_LABELGRAPHIC, Filename *filePathInfo = NULL);

		~Graphic();

		void load();
		void unload();		//	HTA-15

		short getIndex() const;
		void setIndex(short index);

		short getType() const;
		void setType(short type, Filename *filePathInfo = NULL);

		const File &getGraphicFile() const;
		const std::string &getGraphicNameBase() const;

		const std::string &getGraphicName() const;
		void setGraphicName(const std::string &graphicName);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		GraphicImage *getGraphicImage();
		void setGraphicImage(GraphicImage *graphicName);
		void setPngBinaryImage(const std::string &image);
		void setPngBinaryImage(const char* data, int size);

		bool exists() const;
		bool isLoaded() const;

		bool isCustom() const;	// is this a custom graphic?
		bool isRom() const;	// is this a ROM graphic?

		bool deleteRecord();
		bool saveRecord();

		int readImageFromFile(HobString &data) const;

		virtual const char *serialize(int format) const;

		static Graphic *findGraphic(const std::string &graphicName, short type = TYPE_LABELGRAPHIC);

		static void initFieldList();

		void operator=(const Graphic &g);


	protected:

		short index;		// Graphic1, Graphic2, Graphic3 or Graphic4.
		short type;			// see TYPE_XXX static const shorts defined above.

		// This is the name of the graphic shown to the user via the UI.
		// i.e. It is the base part of the filename (excluding directory and extension).
		//HobString		graphicName;

		File			graphicFile;
		HobErrorCode	errorCode;
		GraphicImage	*graphicImage;
		bool			loaded;

		// SOF - 4019 Big Y Self - Serve: Store logo rendering seems to fail frequently on label - tied back to SOF-3335 Printing noise for store graphic
		std::list<GraphicImage*> graphicImageToDelete;

		static std::list<ResourceTagsFieldOffsets> fieldList;

		void init();

		bool locateLabelGraphic();


	private:

		const std::string determinePath();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //GRAPHIC_H
