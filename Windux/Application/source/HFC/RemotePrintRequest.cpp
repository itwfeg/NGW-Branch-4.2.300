/* HT Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * ITWFEG Hobart. Troy, Ohio  USA
 */

#include "RemotePrintRequest.h"
#include "HobTokenizerBar.h"
#include "Configuration.h"

#include "Integer.h"
#include "Hex.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int RemotePrintRequest::dataType = 0;
std::string RemotePrintRequest::dataTypeName("RemotePrintRequest"); 
#endif

std::list<RemotePrintRequestFieldOffsets> RemotePrintRequest::fieldList;


RemotePrintRequest::RemotePrintRequest()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

RemotePrintRequest::RemotePrintRequest(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BarFormat:
		{
			HobString temp(data, len);
			std::list<RemotePrintRequestFieldOffsets>::iterator fieldIter = fieldList.begin();
			int offset;

			HobTokenizerBar tok(&temp);
			HobString token;

			if (tok.next(&token) != -1 && token.size() != 0)
			{
				// We're receiving the original command from the external requester as the first token.  smBackend used to strip this off
				// when it converted it to Device / Event pair, now it just gets passed through. Just ignore it.
				int nextTokenReturnVal = 0;
				if (token.at(0) == 'p')
					nextTokenReturnVal = tok.next(&token);

				// Now start with the second token
				while ((nextTokenReturnVal != -1) && (fieldIter != fieldList.end()))
				{
					offset = *fieldIter;
					setReceived(offset, (token.length() == 0) ? false : true);
					if (token.length())
					{
						token.unQuoteForCSV();
						updateField(offset, token);
					}
					fieldIter++;
					nextTokenReturnVal = tok.next(&token);
				}
			}

			show();

			break;
		}

		default:
			break;
	}
}

RemotePrintRequest::~RemotePrintRequest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("RemotePrintRequest.Delete", string("    RemotePrintRequest: ") + Hex::toString((long long) this) + "  productNum: " + Long::toString(productNum));
}


//	private
void RemotePrintRequest::init()
{
	productNum = 0;
	unitPrice = 0;
	byCount = 0;
	expandedTextNum = 0;
	labelTypeNum = 0;
	printedLocally = false;
	numOfLabels = 0;
	numOfDays = -1;
	
	initFieldList();
}


// static
void RemotePrintRequest::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetRemotePrintProductNum);
		fieldList.push_back(fieldOffsetRemotePrintUnitPrice);
		fieldList.push_back(fieldOffsetRemotePrintByCount);
		fieldList.push_back(fieldOffsetRemotePrintExpandedTextNum);
		fieldList.push_back(fieldOffsetRemotePrintLabelTypeNum);
		fieldList.push_back(fieldOffsetRemotePrintPrintedLocally);
		fieldList.push_back(fieldOffsetRemotePrintNumOfLabels);
		fieldList.push_back(fieldOffsetRemotePrintSellByDate);
	}
}


// Returns true if the field was updated, false if not
bool RemotePrintRequest::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("RemotePrintRequest"))
		Debug::trace("RemotePrintRequest", string("updateField() - fieldOffset ") + Integer::toString(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetRemotePrintProductNum:
			setProductNum(Long::valueOf(newValue));
			break;
		case fieldOffsetRemotePrintUnitPrice:
		{
			setUnitPrice( MetSig::determineImplicitDecimalPrice(newValue) );
			break;
		}
		case fieldOffsetRemotePrintByCount:
			setByCount(Integer::valueOf(newValue));
			break;
		case fieldOffsetRemotePrintExpandedTextNum:
			setExpandedTextNum(Long::valueOf(newValue));
			break;
		case fieldOffsetRemotePrintLabelTypeNum:
			setLabelTypeNum(Long::valueOf(newValue));
			break;
		case fieldOffsetRemotePrintPrintedLocally:
			setPrintedLocally(Bool::valueOf(newValue));
			break;
		case fieldOffsetRemotePrintNumOfLabels:
			setNumOfLabels(Integer::valueOf(newValue));
			break;
		case fieldOffsetRemotePrintSellByDate:
			setSellByDays(Integer::valueOf(newValue));
			break;
		default:
			return false;
			break;
	}

	return true;
}

void RemotePrintRequest::getFieldAsString(int fieldCode, HobString &result, int format) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetRemotePrintProductNum:
			result = Long::toString(productNum);
			break;
		case fieldOffsetRemotePrintUnitPrice:
			result = Long::toString(unitPrice);
			break;
		case fieldOffsetRemotePrintByCount:
			result = Integer::toString(byCount);
			break;
		case fieldOffsetRemotePrintExpandedTextNum:
			result = Long::toString(expandedTextNum);
			break;
		case fieldOffsetRemotePrintLabelTypeNum:
			result = Long::toString(labelTypeNum);
			break;
		case fieldOffsetRemotePrintPrintedLocally:
			result = Bool::toString(printedLocally);
			break;
		case fieldOffsetRemotePrintNumOfLabels:
			result = Integer::toString(numOfLabels);
			break;
		case fieldOffsetRemotePrintSellByDate:
			result = Integer::toString(numOfDays);
			break;
		default:
			break;
	}
}

const char *RemotePrintRequest::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<RemotePrintRequestFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			HobString fieldData = "";

			fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				if (*fieldIter > 1)
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, BrowserFormat);

				//if (isFieldQuoted(*fieldIter))
				//	fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = string("RemotePrintRequest::serialize - unknown format:") + Integer::toString(format);
			break;
	}
	return serializedData.data();
}

void RemotePrintRequest::show()
{
	bool isTracing = Debug::isTracing("ProductManager.remotePrintShow");
	if (isTracing)
	{
		Debug::trace("ProductManager.remotePrint", "RemotePrintRequest: ");
		Debug::trace("ProductManager.remotePrint", string("productNum: ") + Long::toString(productNum));
		Debug::trace("ProductManager.remotePrint", string("unitPrice: ") + Long::toString(unitPrice));
		Debug::trace("ProductManager.remotePrint", string("byCount: ") + Integer::toString(byCount));
		Debug::trace("ProductManager.remotePrint", string("expandedTextNum: ") + Long::toString(expandedTextNum));
		Debug::trace("ProductManager.remotePrint", string("labelTypeNum: ") + Long::toString(labelTypeNum));
		Debug::trace("ProductManager.remotePrint", string("printedLocally: ") + Bool::toString(printedLocally));
		Debug::trace("ProductManager.remotePrint", string("numOfLabels: ") + Integer::toString(numOfLabels));
		Debug::trace("ProductManager.remotePrint", string("numOfLabels: ") + Integer::toString(numOfDays));
	}
}
