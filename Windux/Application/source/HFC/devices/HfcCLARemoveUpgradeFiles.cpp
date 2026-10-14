/*	Created by Alex Zavala 10/3
*	based off wrapper
*/

#include "HfcCLARemoveUpgradeFiles.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcCLARemoveUpgradeFiles::_dataType_ = 0;
std::string HfcCLARemoveUpgradeFiles::_dataTypeName_("HfcCLARemoveUpgradeFiles"); 
#endif

HfcCLARemoveUpgradeFiles::HfcCLARemoveUpgradeFiles() : CLARemoveUpgradeFiles()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("HfcCLARemoveUpgradeFiles", "Constructor 1");
}

HfcCLARemoveUpgradeFiles::HfcCLARemoveUpgradeFiles(const char *data) : CLARemoveUpgradeFiles(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("HfcCLARemoveUpgradeFiles", "Constructor 2");
}

HfcCLARemoveUpgradeFiles::HfcCLARemoveUpgradeFiles(const char *data, int len, int formatType)	: CLARemoveUpgradeFiles()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	bool traceOn = Debug::isTracing("HfcCLARemoveUpgradeFiles");
	HobString msgDebug = "Constructor 3";
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  formatType = ").append(Integer::toString(formatType));
		Debug::trace("HfcCLARemoveUpgradeFiles", msgDebug2);
		msgDebug.append(":  ");
	}

	switch (formatType)
	{
		case InternalFormat:
			break;

		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short offset = 1;

			if (tok.next(&token) != -1)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("  token = ").append(token);
				Debug::trace("HfcCLARemoveUpgradeFiles", msgDebug2);

				setReceived(offset, (token.length() == 0) ? false : true);
				setState((CLARemoveUpgradeState) Integer::valueOf(token.c_str()));
			}
		}
		break;


		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcCLARemoveUpgradeFiles::~HfcCLARemoveUpgradeFiles()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcCLARemoveUpgradeFiles::serialize(int format)const
{
	switch (format)
	{
		case InternalFormat:
		{
			CLARemoveUpgradeFiles::serialize();
			CLARemoveUpgradeFiles::getSerializedLength();
			HfcObject::serializedData = CLARemoveUpgradeFiles::serializedData;
		}
		break;

		default:
			if (Debug::isTracing("HfcCLARemoveUpgradeFiles"))
			{
				HobString msgDebug = "serialize() - invalid format: ";
				msgDebug.append(Integer::toString(format));
				Debug::trace("HfcCLARemoveUpgradeFiles", msgDebug);
			}
			// todo - throw exception("");
			break;
	}

	return HfcObject::serializedData.data();
}
