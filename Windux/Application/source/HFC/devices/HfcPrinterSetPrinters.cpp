/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcPrinterSetPrinters.h"
#include "PrinterEvent.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrinterSetPrinters::dataType = 0;
std::string HfcPrinterSetPrinters::dataTypeName("HfcPrinterSetPrinters"); 
#endif

HfcPrinterSetPrinters::HfcPrinterSetPrinters(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	//	HTA-50: Initialize the members, in case not all information is set.
	setPrinterType0(MaxPrinterTypes);
	setPrinterType1(MaxPrinterTypes);
	setPrinterType2(MaxPrinterTypes);

	setPrinterNodeId0(0);
	setPrinterNodeId1(0);
	setPrinterNodeId2(0);

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
					case fieldOffsetSPPrinterType0:
						setPrinterType0((PrinterType) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetSPPrinterNodeId0:
					{
//						Debug::trace("HfcPrinterSetPrinters", "HfcPrinterSetPrinters():  NodeId0 = " + token);
						setPrinterNodeId0((USBD_NODE_ID) Hex::longValueOf(token.c_str()));		//	HTA-50:  This string is a Hexidecimal string.
						break;
					}
					case fieldOffsetSPPrinterType1:
						setPrinterType1((PrinterType) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetSPPrinterNodeId1:
					{
//						Debug::trace("HfcPrinterSetPrinters", "HfcPrinterSetPrinters():  NodeId1 = " + token);
						setPrinterNodeId1((USBD_NODE_ID) Hex::longValueOf(token.c_str()));		//	HTA-50:  This string is a Hexidecimal string.
						break;
					}
					case fieldOffsetSPPrinterType2:
						setPrinterType2((PrinterType) Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetSPPrinterNodeId2:
					{
//						Debug::trace("HfcPrinterSetPrinters", "HfcPrinterSetPrinters():  NodeId2 = " + token);
						setPrinterNodeId2((USBD_NODE_ID) Hex::longValueOf(token.c_str()));		//	HTA-50:  This string is a Hexidecimal string.
						break;
					}
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		case SMFormat:
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcPrinterSetPrinters::~HfcPrinterSetPrinters()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *HfcPrinterSetPrinters::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;

	switch (format)
	{
		case BrowserFormat:
		{
//			HfcObject::serializedData = "Serialized Printer SetPrinters : browser format";
			short loopCnt = 1;

			serializedData = Integer::toString(Printer);
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString(PrSetPrinters));

			while(loopCnt < endHfcPrinterSetPrintersFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetSPPrinterType0:
						serializedData.append(Integer::toString(getPrinterType0()));
						break;
					case fieldOffsetSPPrinterType1:
						serializedData.append(Integer::toString(getPrinterType1()));
						break;
					case fieldOffsetSPPrinterType2:
						serializedData.append(Integer::toString(getPrinterType2()));
						break;
					default:
						break;
				}
				loopCnt++;
			}

			msgdata = HfcObject::serializedData.data();
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case SMFormat:
			HfcObject::serializedData = "Serialized Printer SetPrinters : sm format";
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

PrinterType HfcPrinterSetPrinters::getPrinterType0() const
{
	return prType0;
}

void HfcPrinterSetPrinters::setPrinterType0(PrinterType prType)
{
	prType0 = prType;
}

PrinterType HfcPrinterSetPrinters::getPrinterType1() const
{
	return prType1;
}

void HfcPrinterSetPrinters::setPrinterType1(PrinterType prType)
{
	prType1 = prType;
}

PrinterType HfcPrinterSetPrinters::getPrinterType2() const
{
	return prType2;
}

void HfcPrinterSetPrinters::setPrinterType2(PrinterType prType)
{
	prType2 = prType;
}

USBD_NODE_ID HfcPrinterSetPrinters::getPrinterNodeId0() const
{
	return nodeId0;
}

void HfcPrinterSetPrinters::setPrinterNodeId0(USBD_NODE_ID nodeId)
{
	nodeId0 = nodeId;
}

USBD_NODE_ID HfcPrinterSetPrinters::getPrinterNodeId1() const
{
	return nodeId1;
}

void HfcPrinterSetPrinters::setPrinterNodeId1(USBD_NODE_ID nodeId)
{
	nodeId1 = nodeId;
}

USBD_NODE_ID HfcPrinterSetPrinters::getPrinterNodeId2() const
{
	return nodeId2;
}

void HfcPrinterSetPrinters::setPrinterNodeId2(USBD_NODE_ID nodeId)
{
	nodeId2 = nodeId;
}

bool HfcPrinterSetPrinters::validSetup()
{
	bool rv = true;

	Debug::trace("HfcPrinterSetPrinters", "validSetup():  prType0 = " + HobString(Integer::toString(prType0)));
	Debug::trace("HfcPrinterSetPrinters", "validSetup():  prType1 = " + HobString(Integer::toString(prType1)));
	Debug::trace("HfcPrinterSetPrinters", "validSetup():  prType2 = " + HobString(Integer::toString(prType2)));

	if ((prType0 == MaxPrinterTypes) && (prType1 == MaxPrinterTypes) && (prType2 == MaxPrinterTypes))
	{
		// Empty (default screen) no printers ??
		Debug::trace("HfcPrinterSetPrinters", "validSetup():  No printers?");
		rv = false;
	}
	else if ((((prType0 == prType1) || (prType0 == prType2)) && (prType0 != MaxPrinterTypes)) ||
			(((prType1 == prType0) || (prType1 == prType2)) && (prType1 != MaxPrinterTypes)) ||
			(((prType2 == prType0) || (prType2 == prType1)) && (prType2 != MaxPrinterTypes)))
	{
				// same printer type for different printers
		Debug::trace("HfcPrinterSetPrinters", "validSetup():  Same printer type for different printers");
		rv = false;
	}

	Debug::trace("HfcPrinterSetPrinters", "validSetup():  Returning " + HobString(Bool::toString(rv)));
	return rv;
}
