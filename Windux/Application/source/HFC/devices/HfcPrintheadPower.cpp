/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerCSV.h"
#include "HfcPrintheadPower.h"
#include "Configuration.h"

#include "Double.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcPrintheadPower::_dataType_ = 0;
std::string HfcPrintheadPower::_dataTypeName_("HfcPrintheadPower"); 
#endif

HfcPrintheadPower::HfcPrintheadPower() : PrintheadPower()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcPrintheadPower::HfcPrintheadPower(const char *data) : PrintheadPower(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcPrintheadPower::HfcPrintheadPower(const char *data, int len, int formatType)	: PrintheadPower()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

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
					case fieldOffsetPHPPrinterType:
						setPrinterType((PrinterType)Integer::valueOf(token));
						break;
					case fieldOffsetPHPPowerState:
						if ((PrinterStateType) Integer::valueOf(token) == PrinterOn)
						{
							setPowerState();
						}
						else
						{
							clearPowerState();
						}
						break;
					case fieldOffsetPHPShortCircuitProtection:
						if ((PrinterStateType) Integer::valueOf(token) == PrinterOn)
						{
							setShortCircuitProtection();
						}
						else
						{
							clearShortCircuitProtection();
						}
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

HfcPrintheadPower::~HfcPrintheadPower()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcPrintheadPower::serialize(int format) const
{
	const char *msgdata = NULL;
	int msglen = 0;
	HobString tempStr;

	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcPrintheadPowerFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetPHPPrinterType:
						HfcObject::serializedData.append(Integer::toString(getPrinterType()));
						break;
					case fieldOffsetPHPPowerState:
						HfcObject::serializedData.append(Integer::toString(getPowerState()));
						break;
					case fieldOffsetPHPShortCircuitProtection:
						HfcObject::serializedData.append(Integer::toString(getShortCircuitProtection()));
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

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.c_str();
}
