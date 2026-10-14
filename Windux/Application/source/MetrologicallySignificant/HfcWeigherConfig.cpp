/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcWeigherConfig.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "HobTokenizerCSV.h"
#include "WeigherTags.h"

#include "SessionManager.h"
#include "Session.h"

#include "Hex.h"
#include "Long.h"
#include "Integer.h"
#include "Double.h"
#include "Bool.h"
#include "Debug.h"

#include "MultiLanguageText.h"
#include "MessageLogger.h"
#include "CMReaderWriterEvolTag.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcWeigherConfig::dataType = 0;
std::string HfcWeigherConfig::dataTypeName("HfcWeigherConfig"); 
#endif

std::list<HfcWeigherConfigFieldOffsets> HfcWeigherConfig::fieldList;


HfcWeigherConfig::HfcWeigherConfig()
	: WeigherConfiguration()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

HfcWeigherConfig::HfcWeigherConfig(const char *data)
	: WeigherConfiguration(data)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

HfcWeigherConfig::HfcWeigherConfig(/*const*/ HfcWeigherConfig *cfg)
	: WeigherConfiguration()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	if (!cfg)
		return;

	setConfigType(cfg->getConfigType());
	//setDeviceSignature(cfg->getDeviceSignature());
	setCenterOfMaintenanceZone(cfg->getCenterOfMaintenanceZone());
	setScaleFactor(cfg->getScaleFactor());
	setMaxWeight(cfg->getMaxWeight());
	setGainFactor(cfg->getGainFactor());
	setZeroReference(cfg->getZeroReference());
	setCalibrationReference(cfg->getCalibrationReference());
	setRateOptionTime(cfg->getRateOptionTime());
	setAutoZeroMaintenanceLimit(cfg->getAutoZeroMaintenanceLimit());
	setPrepackMotionCount(cfg->getPrepackMotionCount());
	setInitializeZeroTime(cfg->getInitializeZeroTime());
	setAutoZeroTime(cfg->getAutoZeroTime());
	setRateOptionLimit(cfg->getRateOptionLimit());
	setZeroReferenceLimit(cfg->getZeroReferenceLimit());
	setCalibrationReferenceLimit(cfg->getCalibrationReferenceLimit());
	setSmallMotionLimit(cfg->getSmallMotionLimit());
	setLargeMotionLimit(cfg->getLargeMotionLimit());
	setLargeMotionCount(cfg->getLargeMotionCount());
	setSmallMotionCount(cfg->getSmallMotionCount());
	setNoMotionCount(cfg->getNoMotionCount());
	setZeroTrackingOption(cfg->getZeroTrackingOption());
	setNegativeWeightOption(cfg->getNegativeWeightOption());
	setPowerupMaintenanceZoneLimit(cfg->getPowerupMaintenanceZoneLimit());
	setOperateMaintenanceZoneLimit(cfg->getOperateMaintenanceZoneLimit());
	setFilterSpeed(cfg->getFilterSpeed());
	setReferenceVoltage(cfg->getReferenceVoltage());
	setLastCalibrationDate(cfg->getLastCalibrationDate());
	setNumberOfCalibrations(cfg->getNumberOfCalibrations());
	setLastConfigurationDate(cfg->getLastConfigurationDate());
	setNumberOfConfigurations(cfg->getNumberOfConfigurations());
	setWeigherType(cfg->getWeigherType());

	// Takes care of weighMode and netWeightDualRangeLimitCheck
	setFlags(cfg->getFlags());
	setMinWeightToPrint(cfg->getMinWeightToPrint());
	setWeigherModel(cfg->getWeigherModel());
	setValueMaxEnabled(cfg->getValueMaxEnabled());

}

HfcWeigherConfig::HfcWeigherConfig(const char *data, int len, int formatType)
	: WeigherConfiguration()
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
					case fieldOffsetFilterSpeed:
						setFilterSpeed((WgFilterSpeed)Integer::valueOf(token));
						break;
					case fieldOffsetScaleWeighMode:
						setScaleWeighMode(Bool::valueOf(token) ? WeighMode_Metric : WeighMode_Avoir);
						Debug::trace("HfcWeigherConfig", "scaleWeighMode = " + HobString(Integer::toString(getScaleWeighMode())));
						break;
					case fieldOffsetScaleFactor:
						setScaleFactor((UINT32)Long::valueOf(token));
						Debug::trace("HfcWeigherConfig", "BrowserFormat scaleFactor = " + HobString(Long::toString(getScaleFactor())));
						break;
					case fieldOffsetMaxScaleWeight:
						token.unQuoteForCSV();
						setMaxScaleWeight((HobWeight)Long::valueOf(token));
						Debug::trace("HfcWeigherConfig", "maxScaleWeight = " + HobString(Long::toString(getMaxScaleWeight())));
						break;
					case fieldOffsetMinWeightToPrint:
					{
						// In the WeigherConfiguration class that contains the data from the weigher eep, the minimum weight to print is stored in the format
						// corresponding to the weigher model. For single-range weighers and dual-range weighers in single-range mode that is 2-implied decimal places,
						// for dual-range weighers that is 3-implied decimal places.
						// The UI always edits and formats the min weight to print with 3-decimal places.
						token.unQuoteForCSV();
						double newWeight = Double::valueOf(token);
						if (token.find(".") != std::string::npos)
							newWeight *= 1000.0;

						double newRoundWeight = Rounding::hobround(newWeight);

						// If the scale factor is 1, then whatever weigher model this is, it is being set in single-range mode
						// (and is avoir mode since scaleFactor 1 is only for avoir mode)
						// Since the min weight to print is assumed to be 3 impled decimal places, in the application,
						// it still needs to be stored in the eep with 2 implied decimal places when the scaleFactor is 1
						if (getScaleFactor() == 1)
						{
							newRoundWeight /= 10;
							Debug::trace("HfcWeigherConfig", "Construct(data,len,format) EvolutionTaggedFormat scaleFactor is 1 so /= 10 ");
						}
						else
							Debug::trace("HfcWeigherConfig", "Construct(data,len,format) EvolutionTaggedFormat scaleFactor is 5 so DO NOTHING ");


						
						setMinWeightToPrint((HobWeight)newRoundWeight);
						Debug::trace("HfcWeigherConfig", "BrowserFormat fieldOffsetMinWeightToPrint minWeightToPrint = " + util_string::longtostring(getMinWeightToPrint()));
						break;
					}
					case fieldOffsetWeigherModel:
						setWeigherModel((WeigherModel) Integer::valueOf(token));
						Debug::trace("HfcWeigherConfig", "weigherModel = " + HobString(Integer::toString(getWeigherModel())));
						break;
					case fieldOffsetUsingNetWeightDualRangeLimitCheck:
						setNetWeightDualRangeLimitCheck(Bool::valueOf(token));
						Debug::trace("HfcWeigherConfig", "netWeightDualRangeLimitCheck = " + HobString(Bool::toString(isNetWeightDualRangeLimitCheck())));
						break;
					case fieldOffsetScaleValuemax:
						setValueMaxEnabled(Integer::valueOf(token));
						Debug::trace("HfcWeigherConfig", "DLF Const BrowserFormat fieldOffsetScaleValuemax");
						Debug::trace("HfcWeigherConfig", "getValueMaxEnabled() = " + HobString(Integer::toString(getValueMaxEnabled())));
						break;
					//	Configuration and Calibration Date/Times will only be set when
					//	the weigher is calibrated or when the configuration is changed.
					case fieldOffsetCalibrationDate:
					case fieldOffsetCalibrationTimes:
					case fieldOffsetConfigurationDate:
					case fieldOffsetConfigurationTimes:
					case fieldOffsetDualRangeLimit:
						break;

					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		{
			Debug::trace("HfcWeigherConfig", "HfcWeigherConfig(EvolutionTaggedFormat)");
			WeigherTags weigherTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0, tagOffset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				HobString tokenData = "";

				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? weigherTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						switch (offset)
						{
							case fieldOffsetWeigherModel:
								setWeigherModel( (WeigherModel) Integer::valueOf(temp) );
								break;
							case fieldOffsetFilterSpeed:
								setFilterSpeed( (WgFilterSpeed) Integer::valueOf(temp) );
								break;
							case fieldOffsetScaleFactor:
								setScaleFactor( Long::valueOf(temp) );
								Debug::trace("HfcWeigherConfig", "EvolutionTaggedFormat scaleFactor = " + HobString(Long::toString(getScaleFactor())));
								break;
							case fieldOffsetMaxScaleWeight:
								setMaxScaleWeight((HobWeight)Long::valueOf(temp));
								Debug::trace("HfcWeigherConfig", "maxScaleWeight = " + HobString(Long::toString(getMaxScaleWeight())));
								break;
							case fieldOffsetMinWeightToPrint:
							{
								// In the WeigherConfiguration class that contains the data from the weigher eep, the minimum weight to print is stored in the format
								// corresponding to the weigher model. For single-range weighers and dual-range weighers in single-range mode that is 2-implied decimal places,
								// for dual-range weighers that is 3-implied decimal places.
								// The UI always edits and formats the min weight to print with 3-decimal places.
								temp.unQuoteForCSV();
								double newWeight = Double::valueOf(temp);
								if (temp.find(".") != std::string::npos)
									newWeight *= 1000.0;
								double newRoundWeight = Rounding::hobround(newWeight);

								// If the scale factor is 1, then whatever weigher model this is, it is being set in single-range mode
								// (and is avoir mode since scaleFactor 1 is only for avoir mode)
								// Since the min weight to print is assumed to be 3 impled decimal places, in the application,
								// it still needs to be stored in the eep with 2 implied decimal places when the scaleFactor is 1
								if (getScaleFactor() == 1)
								{
									newRoundWeight /= 10;
									Debug::trace("HfcWeigherConfig", "Construct(data,len,format) EvolutionTaggedFormat scaleFactor is 1 so /= 10 ");
								}
								else
									Debug::trace("HfcWeigherConfig", "Construct(data,len,format) EvolutionTaggedFormat scaleFactor is 5 so DO NOTHING " );

								setMinWeightToPrint((HobWeight)newRoundWeight);
								break;
							}
							case fieldOffsetScaleWeighMode:
								setScaleWeighMode( Integer::valueOf(temp) ? WeighMode_Avoir : WeighMode_Metric );
								Debug::trace("HfcWeigherConfig", "scaleWeighMode = " + HobString(Integer::toString(getScaleWeighMode())));
								break;
							case fieldOffsetScaleValuemax:
								setValueMaxEnabled(Integer::valueOf(temp));
								Debug::trace("HfcWeigherConfig", "DLF EvolTaggedFormat fieldOffsetScaleValuemax");
								Debug::trace("HfcWeigherConfig", "getValueMaxEnabled() = " + HobString(Integer::toString(getValueMaxEnabled())));
								break;
							default:
								break;
						}
					}
				}
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcWeigherConfig::~HfcWeigherConfig()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void HfcWeigherConfig::init()
{
	Debug::trace("HfcWeigherConfig", "init()");
	if (isScaleMetric())
		setMaxScaleWeight(getMaxWeight() / 20000);
	else
		setMaxScaleWeight(getMaxWeight() / 10000);

	Debug::trace("HfcWeigherConfig", "init(): maxScaleWeight = " + HobString(Long::toString(getMaxScaleWeight())) + HobString(" ") + HobString((isScaleMetric() ? "kg" : "lb")));

	initFieldList();
}

void HfcWeigherConfig::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetFilterSpeed);
		fieldList.push_back(fieldOffsetScaleWeighMode);
		fieldList.push_back(fieldOffsetWeigherModel);
		fieldList.push_back(fieldOffsetScaleFactor);
		fieldList.push_back(fieldOffsetMaxScaleWeight);
		fieldList.push_back(fieldOffsetMinWeightToPrint);

		WeigherTags weigherTags((FieldOffsetsList *)&fieldList);
	}
}

const char *HfcWeigherConfig::serialize(int format) const
{
	switch (format)
	{
		case InternalFormat:
		{
			WeigherConfiguration::serialize();
			int msglen = WeigherConfiguration::getSerializedLength();

			HfcObject::serializedData = WeigherConfiguration::serializedData;
			msglen = HfcObject::getSerializedLength();
		}
		break;

		case BrowserFormat:
		{
			short loopCnt = 1;
			HobString stringForQuoting;

			HfcObject::serializedData = Integer::toString((int)getDeviceType());
			HfcObject::serializedData.append(BrowserFormatSeparator);
			HfcObject::serializedData.append(Integer::toString((int)getEventType()));

			while(loopCnt < endHfcWeigherConfigFieldOffsets)
			{
				HfcObject::serializedData.append(BrowserFormatSeparator);

				switch (loopCnt)
				{
					case fieldOffsetFilterSpeed:
						HfcObject::serializedData.append(Integer::toString(getFilterSpeed()));
						break;
					case fieldOffsetScaleWeighMode:
						HfcObject::serializedData.append(Bool::toString(isScaleMetric()));
						break;
					case fieldOffsetScaleFactor:
						HfcObject::serializedData.append(Long::toString(getScaleFactor()));
						break;
					case fieldOffsetMaxScaleWeight:
						stringForQuoting = Long::toString(getMaxScaleWeight());
						stringForQuoting.quoteForCSV();
						HfcObject::serializedData.append(stringForQuoting);
						break;
					case fieldOffsetMinWeightToPrint:
					{
						long minWeight = getMinWeightToPrint();
						Debug::trace("HfcWeigherConfig", "BrowserFormat serialize(): initial minweight = " + HobString(Long::toString(minWeight)) );

						// If the scale factor is 1, then whatever weigher model this is, it is being set in single-range mode
						// (and is avoir mode since scaleFactor 1 is only for avoir mode)
						// Since the min weight to print is stored in the eep with 2-implied decimal places when the scaleFactor is 1
						// and the application wants it to be 3-implied decimal places, we need to multiply by 10 to get the third decimal place
						if ( getScaleFactor() == 1 )
						{
							minWeight *= 10;
							Debug::trace("HfcWeigherConfig", "BrowserFormat serialize() factor=1 so *= 10: minweight = " + HobString(Long::toString(minWeight)));
						}
						else if( getScaleFactor() == 5 )
						{
							// test 1, as 3 digit when factor is 5, showed this to be 6 on remote drop, and it should of been 60. So add multiplier 
							Debug::trace("HfcWeigherConfig", "BrowserFormat serialize() factor=5 so do nothing. Should I?: minweight = " + HobString(Long::toString(minWeight)));
						}

						HobString stringForQuoting;
						Configuration::getInstance()->formatWeight(minWeight, stringForQuoting);
						stringForQuoting.quoteForCSV();
						Debug::trace("HfcWeigherConfig", "BrowserFormat serialize(): minweight = " + HobString(Long::toString(minWeight)) + HobString("    minWeightToPrintStr = ") + stringForQuoting);
						HfcObject::serializedData.append(stringForQuoting);
						break;
					}

					case fieldOffsetCalibrationDate:
					{
						HobDateTime tempTime(getLastCalibrationDate());
						stringForQuoting = ((getNumberOfCalibrations() > 0) ? tempTime.formatDate(Configuration::getInstance()->getDateDisplayFormat()) : " ");
						stringForQuoting.quoteForCSV();
						HfcObject::serializedData.append(stringForQuoting);
						break;
					}
					case fieldOffsetCalibrationTimes:
						HfcObject::serializedData.append(Integer::toString(getNumberOfCalibrations()));
						break;
					case fieldOffsetConfigurationDate:
					{
						HobDateTime tempTime(getLastConfigurationDate());
						stringForQuoting = ((getNumberOfConfigurations() > 0) ? tempTime.formatDate(Configuration::getInstance()->getDateDisplayFormat()) : " ");
						stringForQuoting.quoteForCSV();
						HfcObject::serializedData.append(stringForQuoting);
						break;
					}
					case fieldOffsetConfigurationTimes:
						HfcObject::serializedData.append(Integer::toString(getNumberOfConfigurations()));
						break;
					case fieldOffsetWeigherModel:
						HfcObject::serializedData.append(Integer::toString(getWeigherModel()));
						break;
					case fieldOffsetUsingNetWeightDualRangeLimitCheck:
						HfcObject::serializedData.append(Bool::toString(isNetWeightDualRangeLimitCheck()));
						break;
					case fieldOffsetDualRangeLimit:
					{
						int limit = Configuration::getInstance()->toIntCfgI((isScaleMetric()) ? CfgInternal::DUALRANGELIMITMETRIC : CfgInternal::DUALRANGELIMITAVOIR);
						int weightDivisor = (int)Configuration::getInstance()->getWeightDivisor();
						HfcObject::serializedData.append(Integer::toString(limit / weightDivisor));
						break;
					}
					case fieldOffsetScaleValuemax:
						HfcObject::serializedData.append(Integer::toString(getValueMaxEnabled()));
						Debug::trace("HfcWeigherConfig", "serialize BrowserFormat fieldOffsetScaleValuemax");
						Debug::trace("HfcWeigherConfig", "getValueMaxEnabled() = " + HobString(Integer::toString(getValueMaxEnabled())));
						break;
					default:
						break;
				}
				loopCnt++;
			}
		}
		break;

		case EvolutionTaggedFormat:
		{
			//	Initialize serializedData with the 'RT' & function code, because AbstractSession::send() won't do it.
			HfcObject::serializedData = CMReaderWriterEvolTag::EVOL_TAGGED_IDENTIFIER;

			char rtData[10];
			CMReaderWriterEvolTag::determineRecordType(rtData, getDeviceType(), getEventType());
			HfcObject::serializedData.append(rtData);
			HfcObject::serializedData.append(HobString(1, CMReaderWriterEvolTag::UNIT_SEPARATOR));		// ALS-5

			WeigherTags weigherTags;
			HobString tag = "";
			HobString fieldData = "";
			
			list<HfcWeigherConfigFieldOffsets>::const_iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				tag = weigherTags.getTag(*fieldIter);
				switch (*fieldIter)
				{
					case fieldOffsetFilterSpeed:
						fieldData =	Integer::toString( (int)getFilterSpeed() );
						break;
					case fieldOffsetScaleWeighMode:
						fieldData =	Bool::toString( isScaleAvoir() );
						break;
					case fieldOffsetWeigherModel:
						fieldData =	Integer::toString( (int)getWeigherModel() );
						break;
					case fieldOffsetScaleFactor:
						fieldData =	Integer::toString( (int)getScaleFactor() );
						break;
					case fieldOffsetMaxScaleWeight:
						fieldData =	Long::toString( getMaxScaleWeight() );
						break;
					case fieldOffsetMinWeightToPrint:
					{
						long minWeight = getMinWeightToPrint();
						Debug::trace("HfcWeigherConfig", "EvolutionTaggedFormat serialize(): initial minweight = " + HobString(Long::toString(minWeight)));

						// If the scale factor is 1, then whatever weigher model this is, it is being set in single-range mode
						// (and is avoir mode since scaleFactor 1 is only for avoir mode)
						// Since the min weight to print is stored in the eep with 2-implied decimal places when the scaleFactor is 1
						// and the application wants it to be 3-implied decimal places, we need to multiply by 10 to get the third decimal place
						if ( getScaleFactor() == 1 )
						{
							minWeight *= 10;
							Debug::trace("HfcWeigherConfig", "EvolutionTaggedFormat serialize() factor=1 so *= 10: minweight = " + HobString(Long::toString(minWeight)));

						}
						else if ( getScaleFactor() == 5 )
						{
							Debug::trace("HfcWeigherConfig", "EvolutionTaggedFormat serialize() factor=5 so do nothing. Should I?: minweight = " + HobString(Long::toString(minWeight)));

						}
						HobString stringForQuoting;
						Configuration::getInstance()->formatWeight(minWeight, stringForQuoting);
						Debug::trace("HfcWeigherConfig", "EvolutionTaggedFormat serialize(): minweight = " + HobString(Long::toString(minWeight)) + HobString("    minWeightToPrintStr = ") + stringForQuoting);

						fieldData =	stringForQuoting;
						break;
					}

					case fieldOffsetScaleValuemax:
						fieldData = Integer::toString((int)getValueMaxEnabled());
						Debug::trace("HfcWeigherConfig", "EvolTaggedFormat serialize EvolTaggedFormat fieldOffsetScaleValuemax");
						Debug::trace("HfcWeigherConfig", "getValueMaxEnabled() = " + HobString(Integer::toString(getValueMaxEnabled())));
						break;
					default:
						tag = "";
						fieldData = "";
						break;
				}

				if (tag.size())
				{
					HfcObject::serializedData.append(tag);
					HfcObject::serializedData.append(fieldData);
					HfcObject::serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return HfcObject::serializedData.data();
}

bool HfcWeigherConfig::update(HfcWeigherConfig *weigherConfig)
{
	bool hasChanged = false;		// if ANYTHING has changed
	short loopCnt = 1;

	bool maxWtHasChanged = false;		    // covers the next 2 fields, if ANYTHING changed that MIGHT impact the maxWeightAllowed
	bool scaleWeighModeHasChanged = false;	// if just the weigh mode has changed
	bool maxScaleWeightHasChanged = false;	// if just the max weight has changed
	

	while (loopCnt < endHfcWeigherConfigFieldOffsets)
	{
		if(weigherConfig->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetFilterSpeed:
					if (getFilterSpeed() != weigherConfig->getFilterSpeed())
					{
						hasChanged = true;
						setFilterSpeed(weigherConfig->getFilterSpeed());
					}
					break;
				case fieldOffsetScaleWeighMode:
					if (getScaleWeighMode() != weigherConfig->getScaleWeighMode())
					{
						Debug::trace("HfcWeigherConfig", "scaleWeighMode has changed: Old value = " + HobString(Integer::toString(getScaleWeighMode())) + HobString("    New value = ") + HobString(Integer::toString(weigherConfig->getScaleWeighMode())));
						hasChanged = true;
						scaleWeighModeHasChanged = true;	// to test when and what we want to update and do we REALLY want to update the maxWt
						setScaleWeighMode(weigherConfig->getScaleWeighMode());
						Configuration::getInstance()->setScaleWeighMode(getScaleWeighMode());
						Debug::trace("HfcWeigherConfig", "scaleWeighMode has changed: " + HobString(Integer::toString(Configuration::getInstance()->getScaleWeighMode())));

						Debug::trace("HfcWeigherConfig", "update(): processing fieldOffsetScaleWeighMode - old gain factor was: " + HobString(Long::toString(getGainFactor())) );
						Debug::trace("HfcWeigherConfig", "update(): processing fieldOffsetScaleWeighMode - old CenterOfMaintenanceZone was: " + HobString(Long::toString(getCenterOfMaintenanceZone())) );
						if (isScaleMetric())
						{
							setGainFactor(WeigherConfiguration::convertGainFactorToMetric(getGainFactor()));

							// not applicable to KPC
							if ( WgCanKpc != getWeigherModel() )
								setCenterOfMaintenanceZone(WeigherConfiguration::convertGainFactorToMetric(getCenterOfMaintenanceZone()));
						}
						else
						{
							setGainFactor(WeigherConfiguration::convertGainFactorToAvoir(getGainFactor()));

							// not applicable to KPC
							if ( WgCanKpc != getWeigherModel() )
								setCenterOfMaintenanceZone(WeigherConfiguration::convertGainFactorToAvoir(getCenterOfMaintenanceZone()));
						}
						Debug::trace("HfcWeigherConfig", "update(): processing fieldOffsetScaleWeighMode - new gain factor is: " + HobString(Long::toString(getGainFactor())) );
						Debug::trace("HfcWeigherConfig", "update(): processing fieldOffsetScaleWeighMode - new CenterOfMaintenanceZone is: " + HobString(Long::toString(getCenterOfMaintenanceZone())) );
					}
					break;
				case fieldOffsetScaleFactor:
					if (getScaleFactor() != weigherConfig->getScaleFactor())
					{
						Debug::trace("HfcWeigherConfig", "scaleFactor has changed: Old value = " + HobString(Integer::toString(getScaleFactor())) + HobString("    New value = ") + HobString(Integer::toString(weigherConfig->getScaleFactor())));
						hasChanged = true;
						setScaleFactor(weigherConfig->getScaleFactor());
					}
					break;
				case fieldOffsetMaxScaleWeight:
					if (getMaxScaleWeight() != weigherConfig->getMaxScaleWeight())
					{
						Debug::trace("HfcWeigherConfig", "maxScaleWeight has changed: Old value = " + HobString(Long::toString(getMaxScaleWeight())) + HobString("    New value = ") + HobString(Long::toString(weigherConfig->getMaxScaleWeight())));
						hasChanged = true;
						maxScaleWeightHasChanged = true;	// to test when and what we want to update and do we REALLY want to update the maxWt
						setMaxScaleWeight(weigherConfig->getMaxScaleWeight());
					}
					break;
				case fieldOffsetMinWeightToPrint:
					if (getMinWeightToPrint() != weigherConfig->getMinWeightToPrint())
					{
						hasChanged = true;

						// No conversion has to be done here.
						setMinWeightToPrint(weigherConfig->getMinWeightToPrint());

						// Configuration stores the min weight to print with 3-decimal places, so multiply the WeigherConfiguration min weight to print by 10 if it's in single range mode.
						long minWeightForConfiguration = getMinWeightToPrint();
						if ( getScaleFactor() == 1 )
						{
							Debug::trace("HfcWeigherConfig.checkScaleFactorForMinWt", "HfcWghConfig scaleFactor=1 so " + longtostring(minWeightForConfiguration) + "*=10");
							minWeightForConfiguration *= 10;
							Debug::trace("HfcWeigherConfig.checkScaleFactorForMinWt", "minWeightForConfiguration after " + longtostring(minWeightForConfiguration));
						}
						else if ( getScaleFactor() == 5 )
						{
							// TEST TEST TEST since we are shifted one. First pass, this made this work.
							Debug::trace("HfcWeigherConfig.checkScaleFactorForMinWt", "HfcWghConfig scaleFactor=5 so " + longtostring(minWeightForConfiguration) + "*=10");
						}

						Configuration::getInstance()->setMinWeightToPrint(minWeightForConfiguration);

						Debug::trace("HfcWeigherConfig", "minWeightToPrint has changed: " + HobString(Long::toString(Configuration::getInstance()->getMinWeightToPrint())));
					}
					break;
				case fieldOffsetWeigherModel:
					if (getWeigherModel() != weigherConfig->getWeigherModel())
					{
						Debug::trace("HfcWeigherConfig", "weigherModel has changed: Old value = " + HobString(Integer::toString(getWeigherModel())) + HobString("    New value = ") + HobString(Integer::toString(weigherConfig->getWeigherModel())));
						hasChanged = true;
						setWeigherModel(weigherConfig->getWeigherModel());
						Configuration::getInstance()->setWeigherModel(getWeigherModel());
						Debug::trace("HfcWeigherConfig", "Config weigherModel has changed: " + HobString(Integer::toString(Configuration::getInstance()->getWeigherModel())));
					}
					break;
				case fieldOffsetUsingNetWeightDualRangeLimitCheck:
					if (isNetWeightDualRangeLimitCheck() != weigherConfig->isNetWeightDualRangeLimitCheck())
					{
						hasChanged = true;

						setNetWeightDualRangeLimitCheck(weigherConfig->isNetWeightDualRangeLimitCheck());
						Configuration::getInstance()->setNetWeightDualRangeLimitCheck(isNetWeightDualRangeLimitCheck());
						Debug::trace("HfcWeigherConfig", "netWeightDualRangeLimitCheck has changed: " + HobString(Bool::toString(Configuration::getInstance()->isNetWeightDualRangeLimitCheck())));
					}
					break;
				case fieldOffsetScaleValuemax:
					if (getValueMaxEnabled() != weigherConfig->getValueMaxEnabled())
					{
						Debug::trace("HfcWeigherConfig", "update( ) scaleValueMax has changed: Old value = " + HobString(Integer::toString(getValueMaxEnabled())) + HobString("    New value = ") + HobString(Integer::toString(weigherConfig->getValueMaxEnabled())));
						hasChanged = true;
						setValueMaxEnabled(weigherConfig->getValueMaxEnabled());
					}
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}

	if( maxScaleWeightHasChanged || scaleWeighModeHasChanged )
		maxWtHasChanged = true;

	if ( maxWtHasChanged )
	{
		//	We want to make sure that all of the values are set before making the MaxWeight calculations.
		HobWeight tempWeight;
		int effectiveScaleFactorAtMaxWeight;
		if (isScaleMetric())
		{
			// potential bug?  Why always 5? When ever I go from Avoir to Metric, no matter if
			// I am coming from Avoir 0.01 or 0.005, the the metric div size is 0.002 (2) not 0.005 (5)
			effectiveScaleFactorAtMaxWeight = 5;
			tempWeight = ((getMaxScaleWeight() * 1000) + (effectiveScaleFactorAtMaxWeight * 9)) * 20;
		}
		else
		{
			// when going from metric to avoir to, if I am metric 0.002(2) then avoir, it goes to 0.01(1) div avoir.
			// if I am metric 0.005(5) then to avoir, it goes to 0.001(1) div avoir. So I think the effectiveFactor MIGHT be ok
			effectiveScaleFactorAtMaxWeight = 1;
			tempWeight = ((getMaxScaleWeight() * 100) + (effectiveScaleFactorAtMaxWeight * 9)) * 100;
		}

		setMaxWeight(tempWeight);
		
		Debug::trace("HfcWeigherConfig", HobString("maxWeight to the Weigher has changed. tempWeight = ") + HobString(Long::toString(tempWeight)) + HobString("    maxWeight = ") + HobString(Long::toString(getMaxWeight())) + HobString("    maxScaleWeight = ") + HobString(Long::toString(getMaxScaleWeight())) + HobString((isScaleMetric() ? " kg" : " lb")));
	}
	return hasChanged;
}

void HfcWeigherConfig::updateConfigurationDateAndTimes()
{
	HobDateTime dateNow;
	dateNow.setToCurrent();
	setLastConfigurationDate((UINT32)dateNow.getCalendarTime());
	setNumberOfConfigurations(getNumberOfConfigurations() + 1);

	// FT-153 diagnostic logging
	// log out configuration stats
	hfc::MultiLanguageText internationalize;
	MessageLogger::logMessage(MessageLogger::InfoLog,
		internationalize("weigherConfiguredDate") + " " + dateNow.convertToDBFormat() + "\n" +
		internationalize("weigherConfiguredCount") + " " + util_string::inttostring(getNumberOfConfigurations()) );
}

void HfcWeigherConfig::updateCalibrationDateAndTimes()
{
	HobDateTime dateNow;
	dateNow.setToCurrent();
	setLastCalibrationDate((UINT32)dateNow.getCalendarTime());
	setNumberOfCalibrations(getNumberOfCalibrations() + 1);

	// FT-153 diagnostic logging
	// log out calibration stats
	hfc::MultiLanguageText internationalize;
	MessageLogger::logMessage(MessageLogger::InfoLog,
		internationalize("weigherCalibrationDate") + " " + dateNow.convertToDBFormat() + "\n" +
		internationalize("weigherCalibrationCount") + " " + util_string::inttostring(getNumberOfCalibrations()) );
}

HobWeight HfcWeigherConfig::getMaxScaleWeight() const
{
	return maxScaleWeight;
}
void HfcWeigherConfig::setMaxScaleWeight(HobWeight weight)
{
	maxScaleWeight = weight;
}


bool HfcWeigherConfig::configChanged(HfcWeigherConfig *cfg)
{
	if (!cfg)
		return true;

	if (Debug::isTracing("ConfigurationManager.weigherConfig"))
	{
		Debug::trace("ConfigurationManager.weigherConfig", HobString("HfcWeigherConfig::configChanged()  filterSpeed: ") + HobString(Integer::toString((int)getFilterSpeed())) +
		              HobString("  passed in filterSpeed: ") + HobString(Integer::toString((int)cfg->getFilterSpeed())) );
		Debug::trace("ConfigurationManager.weigherConfig", HobString("HfcWeigherConfig::configChanged()  scaleFactor: ") + HobString(Integer::toString((int)getScaleFactor())) +
		              HobString("  passed in scaleFactor: ") + HobString(Integer::toString((int)cfg->getScaleFactor())) );
		Debug::trace("ConfigurationManager.weigherConfig", HobString("HfcWeigherConfig::configChanged()  maxWeight: ") + HobString(Integer::toString((int)getMaxWeight())) +
	   	           HobString("  passed in maxWeight: ") + HobString(Integer::toString((int)cfg->getMaxWeight())) );
		Debug::trace("ConfigurationManager.weigherConfig", HobString("HfcWeigherConfig::configChanged()  minWeightToPrint: ") + HobString(Integer::toString((int)getMinWeightToPrint())) +
	      	        HobString("  passed in minWeightToPrint: ") + HobString(Integer::toString((int)cfg->getMinWeightToPrint())) );
		Debug::trace("ConfigurationManager.weigherConfig", HobString("HfcWeigherConfig::configChanged()  netWeightDualRangeLimitCheck: ") + HobString(Bool::toString(isNetWeightDualRangeLimitCheck())) +
	      	        HobString("  passed in netWeightDualRangeLimitCheck: ") + HobString(Bool::toString(cfg->isNetWeightDualRangeLimitCheck())) );
	}

	if (getFilterSpeed() == cfg->getFilterSpeed() &&
			getScaleFactor() == cfg->getScaleFactor() &&
			getMaxWeight() == cfg->getMaxWeight() &&
			getMinWeightToPrint() == cfg->getMinWeightToPrint() &&
		 	getWeigherModel() == cfg->getWeigherModel() &&
			isNetWeightDualRangeLimitCheck() == cfg->isNetWeightDualRangeLimitCheck() &&
			getFlags() == cfg->getFlags() &&
			getValueMaxEnabled() == cfg->getValueMaxEnabled() )
			return false;
		else
			return true;
}


bool HfcWeigherConfig::isValid()
{
	// So far, this is all I need to validate the configuration.  More validations may be needed in the future.

	bool valid = true;

	UINT32 factor = getScaleFactor();

	if ( getScaleWeighMode() == WeighMode_Avoir )
	{
		if ( factor != 1 && factor != 5)
			valid = false;
	}
	else if ( getScaleWeighMode() == WeighMode_Metric )
	{
		if ( factor != 2 && factor != 5)
			valid = false;
	}
	else
		valid = false;

	return valid;
}
