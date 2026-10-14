/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef FONTFILENAMES_H
#define FONTFILENAMES_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobList.h"
#include "Font.h"
#include <list>

class FontFileNames : public HobList
{
	public:

		FontFileNames(short fontTypes = Font::FONTTYPE_CUSTOM);
		FontFileNames(const char *data, int len, int formatType);

		~FontFileNames();

		void findFontNames(short fontTypes = Font::FONTTYPE_CUSTOM);

		virtual const char *serialize(int format) const;
};

#endif //FONTFILENAMES_H
