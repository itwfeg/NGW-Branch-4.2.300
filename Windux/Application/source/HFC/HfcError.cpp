/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcError.h"
#include "HfcErrorTags.h"
#include "ProductDevEvent.h"
#include "DataTypeEvent.h"
#include "CMReaderWriterEvolTag.h"
#include "CMReaderWriterSyncTag.h"

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcError::dataType = 0;
std::string HfcError::dataTypeName("HfcError"); 
#endif

std::list<HfcErrorFieldOffsets> HfcError::browserFieldList, HfcError::evolFieldList;


HfcError::HfcError()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

HfcError::HfcError(long code, short style, HobRecordNum recordNum, long errorType)
	: HfcResponse(code, style)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	this->recordNum = recordNum;
	this->errorType = errorType;
}

HfcError::HfcError(int code, EvolDeviceType dev, const HobString &tag, const HobString &value)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	errorCode = code;
	device = dev;
	failedTag = tag;
	keyValue = value;
}


HfcError::HfcError(int code)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	errorCode = code;
}


HfcError::HfcError(int code, EvolDeviceType dev, UINT16 evt, const HobString &tag, const HobString &value)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	errorCode = code;
	device = dev;
	event = evt;
	failedTag = tag;
	keyValue = value;
}


HfcError::HfcError(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case SynchronizationTaggedFormat:
		{
			HfcErrorTags errorTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if ( token )
			{
				while ( tok.next(token) != -1 )
				{
					//get the tag and find out what offset goes with the tag
		  	 		temp = token->getTag();
					if ( temp.size() )
					{
						//if we can't find it in the expanded text tags then check in special message tags
						offset = errorTags.getOffset(temp);
					}
					else
						offset = 0;

					if ( offset )
					{
						setReceived(offset, true);
						temp = token->getData();
						switch ( offset )
						{
							case fieldOffsetHfcErrErrorCode:
								errorCode = Integer::valueOf(temp);
								break;
							case fieldOffsetHfcErrRecordType:
							{
								if ( temp.size() )
								{
									UINT8 evt = 0;
									CMReaderWriterSyncTag::determineDeviceAndEvent(temp.c_str(), &device, &evt);
									event = evt;
								}

								break;
							}
							case fieldOffsetHfcErrFailedTag:
								failedTag = temp;
								break;
							case fieldOffsetHfcErrKeyValue:
								keyValue = temp;
								break;
							default:
								break;
						}
					}
				}	

				delete token;
			}

			break;
		}
		default:
		{
			// todo - throw exception("invalid format");
			break;
		}
	}
}


HfcError::~HfcError()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

//	private
void HfcError::init()
{
	recordNum = (HobRecordNum) RECORD_NOT_FOUND;
	errorType = 0;
	data = "";

	errorCode = 0;
	device = UnknownDevice;
	event = UNKNOWN_EVENT;
	failedTag = "";
	keyValue = "";

	initFieldList();
}


// static
void HfcError::initFieldList()
{
	if (browserFieldList.empty())
	{
		/*lcg rev 0.1browserFieldList.push_back();
		browserFieldList.push_back();
		browserFieldList.push_back();
		browserFieldList.push_back();*/
		browserFieldList.push_back(fieldOffsetHfcErrRecordNum);
		browserFieldList.push_back(fieldOffsetHfcErrErrorType);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetHfcErrErrorCode);
		evolFieldList.push_back(fieldOffsetHfcErrRecordType);
		evolFieldList.push_back(fieldOffsetHfcErrFailedTag);
		evolFieldList.push_back(fieldOffsetHfcErrKeyValue);

		HfcErrorTags hfcErrorTags((FieldOffsetsList *)&evolFieldList);
	}
}


const HobString &HfcError::getData() const
{
	return data;
}
void HfcError::setData(const HobString &data)
{
	this->data = data;
}


int HfcError::getErrorCode() const
{
	return errorCode;
}
void HfcError::setErrorCode(int code)
{
	errorCode = code;
}


EvolDeviceType HfcError::getDevice() const
{
	return device;
}
void HfcError::setDevice(EvolDeviceType dev)
{
	device = dev;
}


int HfcError::getEvent() const
{
	return event;
}
void HfcError::setEvent(int evt)
{
	event = evt;
}


const HobString &HfcError::getFailedTag() const
{
	return failedTag;
}
void HfcError::setFailedTag(const HobString &tag)
{
	failedTag = tag;
}


const HobString &HfcError::getKeyValue() const
{
	return keyValue;
}
void HfcError::setKeyValue(const HobString &value)
{
	keyValue = value;
}


const char *HfcError::serialize(int format) const
{
	serializedData = "";

	if ( getFormatType() != UnknownFormat )
		format = getFormatType();

	switch ( format )
	{
		case BrowserFormat:
		{
			HfcResponse::serialize(format);

			for ( int loopCnt = fieldOffsetHfcErrRecordNum; loopCnt < endHfcErrorFieldOffsets; loopCnt++ )
			{
				serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetHfcErrRecordNum:
						serializedData.append(Long::toString((long)recordNum));
						break;
					case fieldOffsetHfcErrErrorType:
						serializedData.append(Long::toString(errorType));
						break;
					default:
						break;
				}
			}

			break;
		}
		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		{
			HfcErrorTags hfcErrorTags;
			HobString tag = "";
			HobString fieldData = "";
			
			std::list<HfcErrorFieldOffsets>::iterator fieldIter = evolFieldList.begin();
			while ( fieldIter != evolFieldList.end() )
			{
				tag = hfcErrorTags.getTag(*fieldIter);
				if ( tag.size() )
				{
					getFieldAsString(*fieldIter, fieldData, format);

					if ( fieldData.size() )
					{
						serializedData.append(tag);
						serializedData.append(fieldData);
						serializedData.append(UnitSeparator);
					}
				}
				fieldIter++;
			}

			break;
		}
		default:
			serializedData = HobString("HfcError::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}

	return serializedData.data();
}


void HfcError::getFieldAsString(int fieldCode, HobString &result, int format) const
{
	switch (fieldCode)
	{
		case fieldOffsetHfcErrErrorCode:
			result =	Integer::toString(errorCode);
			break;
		case fieldOffsetHfcErrRecordType:
		{
			if (format == EvolutionTaggedFormat)
			{
				char recordType[3] = {'\0'};

				if ( device != UnknownDevice )
				{
					int evt = event;
					if ( event == UNKNOWN_EVENT )
						evt = (device == ProductDev) ? (int)ProductWrite : (int)DtWrite;
					CMReaderWriterEvolTag::determineReaderRecordType(recordType, device, evt);
				}

				result =	recordType;
			}
			else if (format == SynchronizationTaggedFormat)
			{
				char recordType[3] = {'\0'};

				CMReaderWriterSyncTag::determineReaderRecordType(recordType, device, event);

				result =	recordType;
			}
			else
				result =	Integer::toString((int)device);
			break;
		}
		case fieldOffsetHfcErrFailedTag:
			result =	failedTag;
			break;
		case fieldOffsetHfcErrKeyValue:
			result =	keyValue;
			break;
		default:
			result = "";
			break;
	}
}
