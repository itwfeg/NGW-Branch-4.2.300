/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcErrorIndexNum.h"
#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcErrorIndexNum::dataType = 0;
std::string HfcErrorIndexNum::dataTypeName("HfcErrorIndexNum"); 
#endif

HfcErrorIndexNum::HfcErrorIndexNum()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcErrorIndexNum::HfcErrorIndexNum(long code, short style, HobRecordNum recordNum, long errorType, int indexNum)
	: HfcError(code, style, recordNum, errorType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->indexNum  = indexNum;
}

HfcErrorIndexNum::~HfcErrorIndexNum()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *HfcErrorIndexNum::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			HfcError::serialize(format);

			serializedData.append(BrowserFormatSeparator);
			serializedData.append(Integer::toString(indexNum));
		}
			break;

		case SMFormat:
			serializedData = "Serialized HfcErrorIndexNum : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}
