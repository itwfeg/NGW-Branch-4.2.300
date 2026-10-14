/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcEvolWrapperPackageDesc.h"
#include "HobTokenizerCSV.h"
#include "Configuration.h"
#include "HobString.h"

#include "Bool.h"
#include "Integer.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

	int HfcEvolWrapperPackageDesc::_dataType_ = 0;
	std::string HfcEvolWrapperPackageDesc::_dataTypeName_("HfcEvolWrapperPackageDesc"); 
#endif

HfcEvolWrapperPackageDesc::HfcEvolWrapperPackageDesc() : EvolWrapperPackageDesc()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

HfcEvolWrapperPackageDesc::HfcEvolWrapperPackageDesc(const char *data) : EvolWrapperPackageDesc(data)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperPackageDesc", "Constructor()");
}

HfcEvolWrapperPackageDesc::HfcEvolWrapperPackageDesc(EvolWrapperPackageDesc *ewpd)
	: EvolWrapperPackageDesc(ewpd)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	Debug::trace("HfcEvolWrapperPackageDesc", "Constructor(2)");
}

HfcEvolWrapperPackageDesc::~HfcEvolWrapperPackageDesc()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

void HfcEvolWrapperPackageDesc::show()
{
	if (Debug::isTracing("HfcEvolWrapperPackageDesc"))
	{
		Debug::trace("HfcEvolWrapperPackageDesc", "HfcEvolWrapperPackageDesc: ");
		Debug::trace("HfcEvolWrapperPackageDesc", HobString("                 id: ") + HobString(Integer::toString(getId())));
		Debug::trace("HfcEvolWrapperPackageDesc", HobString("             length: ") + HobString(Integer::toString(getLength())));
		Debug::trace("HfcEvolWrapperPackageDesc", HobString("              width: ") + HobString(Integer::toString(getWidth())));
		Debug::trace("HfcEvolWrapperPackageDesc", HobString("             height: ") + HobString(Integer::toString(getHeight())));
		Debug::trace("HfcEvolWrapperPackageDesc", HobString("                lip: ") + HobString(Integer::toString(getLip())));
#ifdef NEW_FILM_WIDTH_VAR
		Debug::trace("HfcEvolWrapperPackageDesc", HobString(" filmWidthSelection: ") + HobString(Integer::toString(getFilmWidthSelection())));
#else
		Debug::trace("HfcEvolWrapperPackageDesc", HobString("filmWidthUsedToWrap: ") + HobString(Integer::toString(getFilmWidthUsedToWrap())));
#endif
		Debug::trace("HfcEvolWrapperPackageDesc", HobString("       wrapBoxIndex: ") + HobString(Integer::toString(getWrapBoxIndex())));
		Debug::trace("HfcEvolWrapperPackageDesc", HobString("     heightBoxIndex: ") + HobString(Integer::toString(getHeightBoxIndex())));
		Debug::trace("HfcEvolWrapperPackageDesc", HobString(" usedWrapBoxBestFit: ") + HobString(Bool::toString(hasUsedWrapBoxBestFit())));
		Debug::trace("HfcEvolWrapperPackageDesc", HobString(" modifiedNetStretch: ") + HobString(Bool::toString(hasModifiedNetStretch())));
	}
}

const char *HfcEvolWrapperPackageDesc::serialize(int format)const
{
	HfcObject::serializedData = "";
	HobString result;

	Debug::trace("HfcEvolWrapperPackageDesc", "serialize()");
	switch (format)
	{
		case BrowserFormat:
		{
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while (loopCnt < endHfcEvolWrPackageDescFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetEvolWrPDId:
					{
						HfcObject::serializedData.append(Integer::toString(getId()));
						break;
					}
					case fieldOffsetEvolWrPDLength:
					{
						Configuration::getInstance()->formatDecimalValue(getLength(), result, 1);
						HfcObject::serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrPDWidth:
					{
						Configuration::getInstance()->formatDecimalValue(getWidth(), result, 1);
						HfcObject::serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrPDHeight:
					{
						Configuration::getInstance()->formatDecimalValue(getHeight(), result, 1);
						HfcObject::serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrPDLip:
					{
						Configuration::getInstance()->formatDecimalValue(getLip(), result, 1);
						HfcObject::serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrPDFilmWidthUsedToWrap:
					{
						HobString result;
						switch (getFilmWidthSelection())
						{
							case UpperRoll:
								result = "Upper";
								break;
							case LowerRoll:
								result = "Lower";
								break;
							case Automatic:
								result = "Auto";
								break;
							case NoFilmWidthSelected:
								result = "None";
								break;
							default:
								result = "";
								break;
						}
//						HfcObject::serializedData.append(Integer::toString(getFilmWidthSelection()));
						HfcObject::serializedData.append(result);
						break;
					}
					case fieldOffsetEvolWrPDWrapBoxIndex:
					{
						HfcObject::serializedData.append(Integer::toString(getWrapBoxIndex()));
						break;
					}
					case fieldOffsetEvolWrPDHeightBoxIndex:
					{
						HfcObject::serializedData.append(Integer::toString(getHeightBoxIndex()));
						break;
					}
					case fieldOffsetEvolWrPDTrayName:
					{
						HobString tray = getTrayName();
						tray.quote();
						HfcObject::serializedData.append(tray);
						break;
					}
					case fieldOffsetEvolWrPDUsedWrapBoxBestFit:
					{
						HfcObject::serializedData.append(Bool::toString(hasUsedWrapBoxBestFit()));
						break;
					}
					case fieldOffsetEvolWrPDModifiedNetStretch:
					{
						HfcObject::serializedData.append(Bool::toString(hasModifiedNetStretch()));
						break;
					}
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
	Debug::trace("HfcEvolWrapperPackageDesc", "serialize():  " + 	HfcObject::serializedData);
	return 	HfcObject::serializedData.c_str();
}
