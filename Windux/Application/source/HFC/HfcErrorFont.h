/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCERRORFONT_H
#define HFCERRORFONT_H

#include "evolution.h"
#include "HfcObject.h"
#include "Font.h"
#include "HfcError.h"

typedef enum HfcErrorFontOffsets
{
	fieldOffsetHfcErrFontName = endHfcErrorFieldOffsets,
	endHfcErrorFontOffsets
} HfcErrorFontOffsets_t;

class HfcErrorFont : public HfcError
{
	public:

		HfcErrorFont();
		HfcErrorFont(long code, short style, Font *font);

		~HfcErrorFont();

		virtual const char *serialize(int format) const;


	private:
		Font *font;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCERRORFONT_H




