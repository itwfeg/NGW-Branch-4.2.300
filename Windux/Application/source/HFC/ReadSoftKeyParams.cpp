/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "ReadSoftKeyParams.h"

#include "HobTokenizerCSV.h"

#include "Long.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ReadSoftKeyParams::dataType_ = 0;
std::string ReadSoftKeyParams::dataTypeName_("ReadSoftKeyParams"); 
#endif

ReadSoftKeyParams::ReadSoftKeyParams()
{
#ifdef USE_ACCOUNTANT
	if (dataType_ == 0)
		dataType_ = util_acc::registerDataType(dataTypeName_);

	util_acc::addInstance(dataType_);
#endif

	init();
}

ReadSoftKeyParams::ReadSoftKeyParams(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType_ == 0)
		dataType_ = util_acc::registerDataType(dataTypeName_);

	util_acc::addInstance(dataType_);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			Debug::trace("ReadSoftKeyParams", HobString("constructor-browserFormat()"));
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetRSKPDataType:
						dataType = (short)Integer::valueOf(token);
						Debug::trace("ReadSoftKeyParams", HobString("constructor - datatype:") + token);
						break;
					case fieldOffsetRSKPRecordNum:
						recordNum = (HobRecordNum)Long::valueOf(token);
						Debug::trace("ReadSoftKeyParams", HobString("constructor - recordNum:") + token);
						break;
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

ReadSoftKeyParams::~ReadSoftKeyParams()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType_);
#endif

}

void ReadSoftKeyParams::init()
{
	recordNum = (HobRecordNum) RECORD_NOT_FOUND;
	dataType = 0;
}


short ReadSoftKeyParams::getDataType() const
{
	return dataType;
}
void ReadSoftKeyParams::setDataType(short type)
{
	dataType = type;
}

HobRecordNum ReadSoftKeyParams::getRecordNum() const
{
	return recordNum;
}
void ReadSoftKeyParams::setRecordNum(HobRecordNum num)
{
	recordNum = num;
}


const char *ReadSoftKeyParams::serialize(int formatType) const
{
	switch (formatType)
	{
		case BrowserFormat:
		{
			serializedData = "";
			for (int loopCnt = 1; loopCnt < endReadSoftKeyParamsFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetRSKPDataType:
						serializedData.append(Integer::toString(dataType));
						break;
					case fieldOffsetRSKPRecordNum:
						serializedData.append(Long::toString(recordNum));
						break;
					default:
						break;
				}
			}
			break;
		}
			 
		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}
