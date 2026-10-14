/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperRemoveUpgradeFiles.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcEvolWrapperRemoveUpgradeFiles::_dataType_ = 0;
std::string HfcEvolWrapperRemoveUpgradeFiles::_dataTypeName_("HfcEvolWrapperRemoveUpgradeFiles"); 
#endif

HfcEvolWrapperRemoveUpgradeFiles::HfcEvolWrapperRemoveUpgradeFiles() : EvolWrapperRemoveUpgradeFiles()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("HfcEvolWrapperRemoveUpgradeFiles", "Constructor 1");
}

HfcEvolWrapperRemoveUpgradeFiles::HfcEvolWrapperRemoveUpgradeFiles(const char *data) : EvolWrapperRemoveUpgradeFiles(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("HfcEvolWrapperRemoveUpgradeFiles", "Constructor 2");
}

HfcEvolWrapperRemoveUpgradeFiles::HfcEvolWrapperRemoveUpgradeFiles(const char *data, int len, int formatType)	: EvolWrapperRemoveUpgradeFiles()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	bool traceOn = Debug::isTracing("HfcEvolWrapperRemoveUpgradeFiles");
	HobString msgDebug = "Constructor 3";
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("  formatType = ").append(Integer::toString(formatType));
		Debug::trace("HfcEvolWrapperRemoveUpgradeFiles", msgDebug2);
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
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("  token = ").append(token);
					Debug::trace("HfcEvolWrapperRemoveUpgradeFiles", msgDebug2);
				}
				setReceived(offset, (token.length() == 0) ? false : true);

				setState((WrEvolRemoveUpgradeState) Integer::valueOf(token.c_str()));
			}
		}
		break;


		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcEvolWrapperRemoveUpgradeFiles::~HfcEvolWrapperRemoveUpgradeFiles()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperRemoveUpgradeFiles::serialize(int format)const
{
	switch (format)
	{
		case InternalFormat:
		{
			EvolWrapperRemoveUpgradeFiles::serialize();
			EvolWrapperRemoveUpgradeFiles::getSerializedLength();
			HfcObject::serializedData = EvolWrapperRemoveUpgradeFiles::serializedData;
		}
		break;

		default:
			if (Debug::isTracing("HfcEvolWrapperRemoveUpgradeFiles"))
			{
				HobString msgDebug = "serialize() - invalid format: ";
				msgDebug.append(Integer::toString(format));
				Debug::trace("HfcEvolWrapperRemoveUpgradeFiles", msgDebug);
			}
			// todo - throw exception("");
			break;
	}

	return HfcObject::serializedData.data();
}
