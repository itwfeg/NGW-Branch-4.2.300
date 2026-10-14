/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "WriteSingleField.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WriteSingleField::dataType = 0;
std::string WriteSingleField::dataTypeName("WriteSingleField"); 
#endif


WriteSingleField::WriteSingleField()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}


WriteSingleField::WriteSingleField(HobRecordNum recordNum, int fieldCode, HobString fieldNewValue, bool alsoPrint)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	this->recordNum = recordNum;
	this->fieldCode = fieldCode;
	this->fieldNewValue = fieldNewValue;
	this->shouldAlsoPrint = alsoPrint;
}


WriteSingleField::WriteSingleField(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetWriteSingleFieldRecordNum:
						setRecordNum((HobRecordNum)Long::valueOf(token));
						break;
					case fieldOffsetWriteSingleFieldCode:
						setFieldCode(Integer::valueOf(token));
						break;
					case fieldOffsetWriteSingleFieldNewValue:
					{
						token.unQuoteForCSV();
						setFieldNewValue(token);
						break;
					}
					case fieldOffsetWriteSingleFieldOrigValue:
					{
						token.unQuoteForCSV();
						setFieldOrigValue(token);
						break;
					}
					case fieldOffsetWriteSingleFieldShouldAlsoPrint:
						setShouldAlsoPrint(Bool::valueOf(token));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

WriteSingleField::~WriteSingleField()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


void WriteSingleField::init()
{
	recordNum = 0;
	fieldCode = 0;
	fieldNewValue = "";
	fieldOrigValue = "";
	shouldAlsoPrint = false;
}


HobRecordNum WriteSingleField::getRecordNum()
{
	return recordNum;
}
void WriteSingleField::setRecordNum(HobRecordNum recordNum)
{
	this->recordNum = recordNum;
}

int WriteSingleField::getFieldCode()
{
	return fieldCode;
}
void WriteSingleField::setFieldCode(int fieldCode)
{
	this->fieldCode = fieldCode;
}

HobString WriteSingleField::getFieldNewValue()
{
	return fieldNewValue;
}
void WriteSingleField::setFieldNewValue(HobString fieldNewValue)
{
	this->fieldNewValue = fieldNewValue;
}

HobString WriteSingleField::getFieldOrigValue()
{
	return fieldOrigValue;
}
void WriteSingleField::setFieldOrigValue(HobString fieldOrigValue)
{
	this->fieldOrigValue = fieldOrigValue;
}

bool WriteSingleField::isShouldAlsoPrint()
{
	return shouldAlsoPrint;
}
void WriteSingleField::setShouldAlsoPrint(bool alsoPrint)
{
	shouldAlsoPrint = alsoPrint;
}


const char *WriteSingleField::serialize(int format) const
{
	HobString stringForQuoting;
	switch (format)
	{
		case BrowserFormat:
			serializedData = "";
			for (int loopCnt = 1; loopCnt < endWriteSingleFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetWriteSingleFieldRecordNum:
						serializedData.append(Long::toString(recordNum));
						break;
					case fieldOffsetWriteSingleFieldCode:
						serializedData.append(Integer::toString(fieldCode));
						break;
					case fieldOffsetWriteSingleFieldNewValue:
					{
						if (!fieldNewValue.empty())
						{
							stringForQuoting = fieldNewValue;
							stringForQuoting.quoteForCSV();
							serializedData.append(stringForQuoting);
						}
					}
						break;
					case fieldOffsetWriteSingleFieldOrigValue:		//	Don't send this to the Browser - only comes from the Browser.
					{
							//	This comes from the Browser, but the Browser may not have the original value to correct a problem with the new value.
						if (!fieldOrigValue.empty())
						{
							stringForQuoting = fieldOrigValue;
							stringForQuoting.quoteForCSV();
							serializedData.append(stringForQuoting);
						}
						break;
					}
					case fieldOffsetWriteSingleFieldShouldAlsoPrint:
						serializedData.append(Bool::toString(shouldAlsoPrint));
						break;
					default:
						break;
				}
			}

			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}

