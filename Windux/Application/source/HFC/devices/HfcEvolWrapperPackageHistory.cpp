/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Configuration.h"
#include "HfcObject.h"
#include "HfcEvolWrapperPackageHistory.h"
#include "HfcWrapperConfig.h"
#include "Integer.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

	int HfcEvolWrapperPackageHistory::_dataType_ = 0;
	std::string HfcEvolWrapperPackageHistory::_dataTypeName_("HfcEvolWrapperPackageHistory"); 
#endif

HfcEvolWrapperPackageHistory::HfcEvolWrapperPackageHistory()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcEvolWrapperPackageHistory::HfcEvolWrapperPackageHistory(const char *data)
	: EvolWrapperPackageHistory(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	bool traceOn = Debug::isTracing("HfcEvolWrapperPackageHistory");
	HobString msgDebug = "";
	Configuration *config = Configuration::getInstance();

	if (traceOn)
	{
		msgDebug = "HfcEvolWrapperPackageHistory():  ";
		Debug::trace("HfcEvolWrapperPackageHistory", msgDebug + "constructor, passing in const char *data");
	}

	HobString idStr = "", widthStr = "", lengthStr = "", heightStr = "", lipStr = "", filmStr = "", bestFitStr = "", netStretchStr = "";

	EvolWrapperPackageHistory::const_iterator iter = begin();
	while (iter != end())
	{
		idStr = Integer::toString(iter->packId_);
		config->formatDecimalValue(iter->width_, widthStr, 1);
		config->formatDecimalValue(iter->length_, lengthStr, 1);
		config->formatDecimalValue(iter->height_, heightStr, 1);
		config->formatDecimalValue(iter->lip_, lipStr, 1);
		switch (iter->filmSelection_)
		{
			case UpperRoll:
				filmStr = "Upper";
				break;
			case LowerRoll:
				filmStr = "Lower";
				break;
			case Automatic:
				filmStr = "Auto";
				break;
			case NoFilmWidthSelected:
				filmStr = "None";
				break;
			default:
				filmStr = "";
				break;
		}
		bestFitStr = Bool::toString(iter->usedWrapBoxBestFit_);
		netStretchStr = Bool::toString(iter->modifiedNetStretch_);

		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("  id = [").append(idStr).append("]");
			msgDebug2.append("  width = [").append(widthStr).append("]");
			msgDebug2.append("  length = [").append(lengthStr).append("]");
			msgDebug2.append("  height = [").append(heightStr).append("]");
			msgDebug2.append("  lip = [").append(lipStr).append("]");
			msgDebug2.append("  filmSelected = [").append(filmStr).append("]");
			msgDebug2.append("  usedWrapBoxBestFit = [").append(bestFitStr).append("]");
			msgDebug2.append("  modifiedNetStretch = [").append(netStretchStr).append("]");
			msgDebug2.append("  trayName_ = [").append(iter->getTrayName()).append("]");
			Debug::trace("HfcEvolWrapperPackageHistory", msgDebug2);
		}
		idList.push_back(idStr);
		widthList.push_back(widthStr);
		lengthList.push_back(lengthStr);
		heightList.push_back(heightStr);
		lipList.push_back(lipStr);
		filmSelectedList.push_back(filmStr);
		usedWrapBoxBestFitList.push_back(bestFitStr);
		modifiedNetStretchList.push_back(netStretchStr);

		HobString trayName = iter->getTrayName();
		trayName.quote();
		trayNameList.push_back(trayName);

		++iter;
	}
}

HfcEvolWrapperPackageHistory::~HfcEvolWrapperPackageHistory()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcEvolWrapperPackageHistory::serialize(int format)const
{
	HfcObject::serializedData = "";

	switch (format)
	{
		case BrowserFormat:
		{
			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));
			HfcObject::serializedData.append(BrowserFormatSeparator);

			HfcObject::serializedData.append(idList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(widthList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(lengthList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(heightList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(lipList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(filmSelectedList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(trayNameList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(usedWrapBoxBestFitList.serialize(format));
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(modifiedNetStretchList.serialize(format));
		}
		break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}
