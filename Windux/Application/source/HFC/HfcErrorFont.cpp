/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcErrorFont.h"
#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcErrorFont::dataType = 0;
std::string HfcErrorFont::dataTypeName("HfcErrorFont"); 
#endif

HfcErrorFont::HfcErrorFont()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcErrorFont::HfcErrorFont(long code, short style, Font *font)
	: HfcError(code, style, 0, font->getErrorCode())
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->font = font;
}

HfcErrorFont::~HfcErrorFont()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *HfcErrorFont::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			HfcError::serialize(format);

			for (int loopCnt = fieldOffsetHfcErrFontName; loopCnt < endHfcErrorFontOffsets; loopCnt++)
			{
				serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetHfcErrFontName:
						serializedData.append(font->getFontName());
						break;
					default:
						break;
				}
			}
		}
			break;

		case SMFormat:
			serializedData = "Serialized HfcErrorFont : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}
