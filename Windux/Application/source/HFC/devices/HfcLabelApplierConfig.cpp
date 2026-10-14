/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcLabelApplierConfig.h"
#include "HobTokenizerCSV.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcLabelApplierConfig::_dataType_ = 0;
std::string HfcLabelApplierConfig::_dataTypeName_("HfcLabelApplierConfig"); 
#endif

HfcLabelApplierConfig::HfcLabelApplierConfig() : CompactLabelApplierConfiguration(), labelPosition_(LocNoneSelected)	//Alex Zavala 8/23 changed to Compact
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("HfcLabelApplierConfig", "HfcLabelApplierConfig::HfcLabelApplierConfig()");
}

HfcLabelApplierConfig::HfcLabelApplierConfig(HfcLabelApplierConfig *pHfcLaConfig) : CompactLabelApplierConfiguration()	//Alex Zavala 8/23 changed to Compact...
{
	Debug::trace("HfcLabelApplierConfig", "HfcLabelApplierConfig::HfcLabelApplierConfig(HfcLabelApplierConfig *pHfcLaConfig)");

#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (pHfcLaConfig != NULL)
	{
		setConfigType(pHfcLaConfig->getConfigType());
		setWeigherStationAttached(pHfcLaConfig->isWeigherStationAttached());
		setPrimaryStationAttached(pHfcLaConfig->isPrimaryStationAttached());
		setSecondaryStationAttached(pHfcLaConfig->isSecondaryStationAttached());
		setLeftToRight(pHfcLaConfig->isLeftToRight());
		setFrontDischarge(pHfcLaConfig->isFrontDischarge());
		//setPrimaryJustifyFont(pHfcLaConfig->isPrimaryJustifyFont());
		//setSecondaryJustifyFont(pHfcLaConfig->isSecondaryJustifyFont());
		//setTransferStationType(pHfcLaConfig->getTransferStationType());
		setPrimaryPrinterLabelOffset(pHfcLaConfig->getPrimaryPrinterLabelOffset());
		setSecondaryPrinterLabelOffset(pHfcLaConfig->getSecondaryPrinterLabelOffset());
		setLabelPlacement(pHfcLaConfig->getLabelPlacement());
		setOverdrive(pHfcLaConfig->getOverdrive());//Alex Zavala 11/7
		//setWeigherConveyorSensitivity(pHfcLaConfig->getWeigherConveyorSensitivity());
		///setPrimaryPrinterConveyorSensitivity(pHfcLaConfig->getPrimaryPrinterConveyorSensitivity());
		//setSecondaryPrinterConveyorSensitivity(pHfcLaConfig->getSecondaryPrinterConveyorSensitivity());
		//setPrimaryPrinterMaxJustificationDistance(pHfcLaConfig->getPrimaryPrinterMaxJustificationDistance());
		//setSecondaryPrinterMaxJustificationDistance(pHfcLaConfig->getSecondaryPrinterMaxJustificationDistance());
		//setDiagonalLabelPlacement(pHfcLaConfig->isDiagonalLabelPlacement());
		//setDiagonalOffset(pHfcLaConfig->getDiagonalOffset());
		//setMiniStationLabelPlacement(pHfcLaConfig->getMiniStationLabelPlacement());
		//setLabelPosition(pHfcLaConfig->getLabelPosition());
	}
}

HfcLabelApplierConfig::HfcLabelApplierConfig(const char *data) : CompactLabelApplierConfiguration(data), labelPosition_(LocNoneSelected)//Changed to compact
{
	Debug::trace("HfcLabelApplierConfig", "HfcLabelApplierConfig::HfcLabelApplierConfig(const char *data)");

#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	Debug::trace("HfcLabelApplierConfig", "HfcLabelApplierConfig(const char *data)");
}


HfcLabelApplierConfig::HfcLabelApplierConfig(const char *data, int len, int formatType)	: CompactLabelApplierConfiguration(), labelPosition_(LocNoneSelected)//Changed to compact
{
	Debug::trace("HfcLabelApplierConfig", "HfcLabelApplierConfig::HfcLabelApplierConfig(const char *data, int len, int formatType)");

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
//					case fieldOffsetTransferStationType:
//						setTransferStationType((TransferStationType)Integer::valueOf(token.c_str()));
//						break;
					case fieldOffsetWeigherStationAttached:
					{
						bool weigherAttached = Bool::valueOf(token.c_str());
						Debug::trace("LabelApplierConfiguration", HobString("setting *** weigherAttached: ") + HobString(Bool::toString(weigherAttached)));
//						setWeigherStationAttached(Bool::valueOf(token.c_str()));
						setWeigherStationAttached(weigherAttached);
						break;
					}
					case fieldOffsetPrimaryStationAttached:
						setPrimaryStationAttached(Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetSecondaryStationAttached:
						setSecondaryStationAttached(Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetLeftToRight:
						setLeftToRight(Bool::valueOf(token.c_str()));
						break;
					case fieldOffsetFrontDischarge:
						setFrontDischarge(Bool::valueOf(token.c_str()));
						break;
//					case fieldOffsetWeigherConveyorSensitivity:
//						setWeigherConveyorSensitivity((ConveyorSensitivity) Integer::valueOf(token.c_str()));
//						break;
//					case fieldOffsetPrimaryPrinterConveyorSensitivity:
//						setPrimaryPrinterConveyorSensitivity((ConveyorSensitivity) Integer::valueOf(token.c_str()));
//						break;
//					case fieldOffsetSecondaryPrinterConveyorSensitivity:
//						setSecondaryPrinterConveyorSensitivity((ConveyorSensitivity) Integer::valueOf(token.c_str()));
//						break;
					case fieldOffsetPrimaryPrinterLabelOffset:
						setPrimaryPrinterLabelOffset((UINT8)Integer::valueOf(token.c_str()));
						break;
//					case fieldOffsetPrimaryPrinterMaxJustificationDistance:
//						setPrimaryPrinterMaxJustificationDistance((UINT8)Integer::valueOf(token.c_str()));
//						break;
					case fieldOffsetSecondaryPrinterLabelOffset:
						setSecondaryPrinterLabelOffset((UINT8)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetLabelPlacement:
						setLabelPlacement((UINT8)Integer::valueOf(token.c_str()));
						break;
					case fieldOffsetOverdrive://Alex Zavala 11/7
						setOverdrive((UINT8)Integer::valueOf(token.c_str()));
						break;
//					case fieldOffsetMiniStationLabelPlacement:
//			 		{
//			 			setMiniStationLabelPlacement((LaPlacement)Integer::valueOf(token.c_str()));
//						Debug::trace("HfcLabelApplierConfig", "Constructor 4:   miniStationLabelPlacement = " + inttostring(getMiniStationLabelPlacement()) + "   token = " + token);
//						break;
//					}
//					case fieldOffsetDiagonalLabelPlacement:
//			 			setDiagonalLabelPlacement(Bool::valueOf(token.c_str()));
//						break;
//					
//					case fieldOffsetLabelPosition:	//	Don't care here because they can't change it.
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

HfcLabelApplierConfig::~HfcLabelApplierConfig()
{
	Debug::trace("HfcLabelApplierConfig", "HfcLabelApplierConfig::~HfcLabelApplierConfig()");

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *HfcLabelApplierConfig::serialize(int format)const
{
	switch (format)
	{
		case InternalFormat:
		{
			Debug::trace("HfcLabelApplierConfig", "serialize(InternalFormat)");
			//LabelApplierConfiguration::serialize();
			CompactLabelApplierConfiguration::serialize();
			//HfcObject::serializedData = LabelApplierConfiguration::serializedData;
			HfcObject::serializedData = CompactLabelApplierConfiguration::serializedData;
		}
		break;

		case BrowserFormat:
		{
			Debug::trace("HfcLabelApplierConfig", "serialize(BrowserFormat)");
			short loopCnt = 1;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcLabelApplierConfigFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
//					case fieldOffsetTransferStationType:
//			 			HfcObject::serializedData.append(Integer::toString(getTransferStationType()));
//						break;
					case fieldOffsetWeigherStationAttached:
			 			HfcObject::serializedData.append(Bool::toString(isWeigherStationAttached()));
						Debug::trace("HfcLabelApplierConfig", "WeigherStationAttached: " + HobString(Bool::toString(isWeigherStationAttached())));
						break;
					case fieldOffsetPrimaryStationAttached:
			 			HfcObject::serializedData.append(Bool::toString(isPrimaryStationAttached()));
						Debug::trace("HfcLabelApplierConfig", "PrimaryStationAttached: " + HobString(Bool::toString(isPrimaryStationAttached())));
						break;
					case fieldOffsetSecondaryStationAttached:
			 			HfcObject::serializedData.append(Bool::toString(isSecondaryStationAttached()));
						Debug::trace("HfcLabelApplierConfig", "SecondaryStationAttached: " + HobString(Bool::toString(isSecondaryStationAttached())));
						break;
					case fieldOffsetLeftToRight:
			 			HfcObject::serializedData.append(Bool::toString(isLeftToRight()));
						Debug::trace("HfcLabelApplierConfig", "LeftToRight: " + HobString(Bool::toString(isLeftToRight())));
						break;
					case fieldOffsetFrontDischarge:
			 			HfcObject::serializedData.append(Bool::toString(isFrontDischarge()));
						Debug::trace("HfcLabelApplierConfig", "FrontDischarge: " + HobString(Bool::toString(isFrontDischarge())));
						break;
//					case fieldOffsetWeigherConveyorSensitivity:
//			 			HfcObject::serializedData.append(Integer::toString(getWeigherConveyorSensitivity()));
//						break;
//					case fieldOffsetPrimaryPrinterConveyorSensitivity:
//			 			HfcObject::serializedData.append(Integer::toString(getPrimaryPrinterConveyorSensitivity()));
//						break;
//					case fieldOffsetSecondaryPrinterConveyorSensitivity:
//			 			HfcObject::serializedData.append(Integer::toString(getSecondaryPrinterConveyorSensitivity()));
//						break;
					case fieldOffsetPrimaryPrinterLabelOffset:
			 			HfcObject::serializedData.append(Integer::toString(getPrimaryPrinterLabelOffset()));
						Debug::trace("HfcLabelApplierConfig", "PrimaryPrinterLabelOffset: " + HobString(Integer::toString(getPrimaryPrinterLabelOffset())));
						break;
//					case fieldOffsetPrimaryPrinterMaxJustificationDistance:
//			 			HfcObject::serializedData.append(Integer::toString(getPrimaryPrinterMaxJustificationDistance()));
//						break;
					case fieldOffsetSecondaryPrinterLabelOffset:
			 			HfcObject::serializedData.append(Integer::toString(getSecondaryPrinterLabelOffset()));
						Debug::trace("HfcLabelApplierConfig", "SecondaryPrinterLabelOffset: " + HobString(Integer::toString(getSecondaryPrinterLabelOffset())));
						break;
					case fieldOffsetLabelPlacement:
						HfcObject::serializedData.append(Integer::toString(getLabelPlacement()));
						Debug::trace("HfcLabelApplierConfig", "LabelPlacement: " + HobString(Integer::toString(getLabelPlacement())));
						break;
					case fieldOffsetOverdrive://Alex Zavala 11/7
						HfcObject::serializedData.append(Integer::toString(getOverdrive()));
						Debug::trace("HfcLabelApplierConfig", "Overdrive: " + HobString(Integer::toString(getOverdrive())));
						break;
//					case fieldOffsetMiniStationLabelPlacement:
//			 			HfcObject::serializedData.append(Integer::toString((UINT8) getMiniStationLabelPlacement()));
//						break;
//					case fieldOffsetDiagonalLabelPlacement:
//			 			HfcObject::serializedData.append(Bool::toString(isDiagonalLabelPlacement()));
//						break;
//					case fieldOffsetLabelPosition:
//			 			if (isLeftToRight())		//	No conversion needed.
//							HfcObject::serializedData.append(Integer::toString((UINT8) getMiniStationLabelPlacement()));
//						else if (getMiniStationLabelPlacement() == LaLeadingEdge)
//							HfcObject::serializedData.append(Integer::toString(LocLeft));
//						else
//							HfcObject::serializedData.append(Integer::toString(LocRight));
//						break;
				}
				loopCnt++;
			}
		}
		break;

		default:
			// todo - throw exception("invalid format");
			Debug::trace("HfcLabelApplierConfig", "serialize(default!!!)");
			break;
	}
	return HfcObject::serializedData.data();
}

bool HfcLabelApplierConfig::update(HfcLabelApplierConfig &hfcLaConfig)
{
	//return LabelApplierConfiguration::update(dynamic_cast<LabelApplierConfiguration &> (hfcLaConfig));
	return CompactLabelApplierConfiguration::update(dynamic_cast<CompactLabelApplierConfiguration &> (hfcLaConfig));
}

LaPosLoc HfcLabelApplierConfig::getLabelPosition()
{
	Debug::trace("HfcLabelApplierConfig", "getLabelPosition() = " + inttostring((int)labelPosition_));
	return labelPosition_;
}

void HfcLabelApplierConfig::setLabelPosition(LaPosLoc value)
{
	labelPosition_ = value;
	Debug::trace("HfcLabelApplierConfig", "setLabelPosition() = " + inttostring((int)labelPosition_));
}
