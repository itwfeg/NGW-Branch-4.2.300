/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef FONT_H
#define FONT_H

#include "evolution.h"
#include "HfcObject.h"
#include "ResourceTags.h"

#include "util/filesys/filename.h"

#include <list>


class Font : public HfcObject
{
	public:

		static const short FONTTYPE_ROM		= 0;
		static const short FONTTYPE_CUSTOM	= 1;
		static const short FONTTYPE_ALL		= 2;


		Font();
		Font(const Font &f);
		Font(const char *data, int len, int formatType);
		Font(const HobString &fontName);

		~Font();

		void load();

		const std::string &getFontNameBase() const;
		const HobString &getFontName() const;
		void setFontName(const HobString &fontName);

		HobErrorCode getErrorCode() const;
		void setErrorCode(HobErrorCode errorCode);

		bool isCustom() const;	// is this a custom font?
		bool isRom() const;	// is this a ROM font?
		void setCustom(bool flag);

		int readImageFromFile(HobString &data) const;

		virtual const char *serialize(int format) const;

		static void initFieldList();

		bool deleteRecord();
		bool saveRecord();

		static Font *findFont(HobString fontName);

		void operator=(const Font &f);

		void show() const;

	private:
		HobString		fontName;
		util_filesystem::Filename		fontFileName;
		HobErrorCode	errorCode;

		mutable HobString binaryImage;

		bool custom;	// is this a custom graphic or a ROM graphic?
		
		static std::list<ResourceTagsFieldOffsets> fieldList;

		void init();

		#define TTF_FILE_EXTENSION ".ttf"

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif // FONT_H
