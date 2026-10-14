/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcErrorGraphic.h"
#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcErrorGraphic::dataType = 0;
std::string HfcErrorGraphic::dataTypeName("HfcErrorGraphic"); 
#endif

HfcErrorGraphic::HfcErrorGraphic()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcErrorGraphic::HfcErrorGraphic(long code, short style, Graphic *graphic)
	: HfcError(code, style, 0, graphic->getErrorCode())
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->graphic = graphic;
}

HfcErrorGraphic::~HfcErrorGraphic()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *HfcErrorGraphic::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			HfcError::serialize(format);

			for (int loopCnt = fieldOffsetHfcErrGraphicIndex; loopCnt < endHfcErrorGraphicOffsets; loopCnt++)
			{
				serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetHfcErrGraphicIndex:
						serializedData.append(Integer::toString(graphic->getIndex()));
						break;
					case fieldOffsetHfcErrGraphicName:
						serializedData.append(graphic->getGraphicName());
						break;
					default:
						break;
				}
			}
		}
			break;

		case SMFormat:
			serializedData = "Serialized HfcErrorGraphic : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}
