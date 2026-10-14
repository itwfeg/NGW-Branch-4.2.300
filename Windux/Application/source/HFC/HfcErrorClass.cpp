/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcErrorClass.h"
#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcErrorClass::dataType = 0;
std::string HfcErrorClass::dataTypeName("HfcErrorClass"); 
#endif

HfcErrorClass::HfcErrorClass()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HfcErrorClass::HfcErrorClass(long code, short style, HobRecordNum recordNum, long errorType, Class *errClass)
	: HfcError(code, style, recordNum, errorType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->errClass  = errClass;
}

HfcErrorClass::~HfcErrorClass()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *HfcErrorClass::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			HfcError::serialize(format);

			for (int loopCnt = fieldOffsetHfcErrClassLinkStatus; loopCnt < endHfcErrorClassOffsets; loopCnt++)
			{
				serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetHfcErrClassLinkStatus:
						serializedData.append(Integer::toString(errClass->getClassLinkStatus()));
						break;
					case fieldOffsetHfcErrBadClass:
						serializedData.append(Long::toString((long)errClass->getBadClass()));
						break;
					case fieldOffsetHfcErrBadReportingClass:
						serializedData.append(Long::toString((long)errClass->getBadReportingClass()));
						break;
					default:
						break;
				}
			}
		}
			break;

		case SMFormat:
			serializedData = "Serialized HfcErrorClass : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}
