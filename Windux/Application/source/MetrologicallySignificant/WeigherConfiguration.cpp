//#include "vxWorks.h"
#include "WeigherEvent.h"
#include "WeigherConfiguration.h"
#include "WeigherInquire.h"
#include "Rounding.h"

#include "math.h"

#include "util/e_string/utilstring.h"
#include "HobString.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "Long.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherConfiguration::dataType = 0;
std::string WeigherConfiguration::dataTypeName("WeigherConfiguration"); 
#endif

WeigherConfiguration::WeigherConfiguration(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Weigher, WgConfig, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	configType = WgDefault;

    deviceSignature[0]			= 'W';
    deviceSignature[1]        	= 'G';
    deviceSignature[2]        	= '\0';
    deviceSignature[3]        	= '\0';
    deviceSignature[4]        	= '\0';
    deviceSignature[5]        	= '\0';

    centerOfMaintenanceZone 	= 0;
    scaleFactor               	= 1;
    maxWeight                 	= 900000;
    gainFactor                	= 10000;
    zeroReference             	= 0;
    calibrationReference      	= 0;
	rateOptionTime           	= 15;
	autoZeroMaintenanceLimit	= 50;
	prepackMotionCount       	= 2;
    initializeZeroTime       	= 1;
    autoZeroTime             	= 3;
	rateOptionLimit				= 60;
	zeroReferenceLimit       	= 60;
	calibrationReferenceLimit	= 60;
	smallMotionLimit         	= 15;
    largeMotionLimit         	= 100;
	largeMotionCount         	= 36;
	smallMotionCount         	= 8;
	noMotionCount				= 24;
    zeroTrackingOption       	= 1; /* TRUE */
    negativeWeightOption     	= 0; /* FALSE */
	powerupMaintenanceZoneLimit = 30000;
	operateMaintenanceZoneLimit = 6000;
    filterSpeed               	= SlowFilterSpeed;
	referenceVoltage			= 0;
	numberOfCalibrations		= 0;
	lastCalibrationDate			= 0;
	numberOfCalibrations		= 0;
	lastConfigurationDate		= 0;

	weigherType = FirstWeigher;

	flags = 0xff;
	minWeightToPrint = 0;
	model = WmNone;
	valueMax = 0;				// jhc valuemax - added default
	azsmMotionLimit				= 9;
}

WeigherConfiguration::WeigherConfiguration(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &configType;
	for (unsigned int i = 0; i < sizeof(configType); i++)
	{
		*pChar++ = buffer[j++];
	}

	for (unsigned int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = buffer[j++];
	}

	pChar = (char *) &centerOfMaintenanceZone;
	for (unsigned int i = 0; i < sizeof(centerOfMaintenanceZone); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &scaleFactor;
	for (unsigned int i = 0; i < sizeof(scaleFactor); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &maxWeight;
	for (unsigned int i = 0; i < sizeof(maxWeight); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &gainFactor;
	for (unsigned int i = 0; i < sizeof(gainFactor); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &zeroReference;
	for (unsigned int i = 0; i < sizeof(zeroReference); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &calibrationReference;
	for (unsigned int i = 0; i < sizeof(calibrationReference); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rateOptionTime;
	for (unsigned int i = 0; i < sizeof(rateOptionTime); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &autoZeroMaintenanceLimit;
	for (unsigned int i = 0; i < sizeof(autoZeroMaintenanceLimit); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &prepackMotionCount;
	for (unsigned int i = 0; i < sizeof(prepackMotionCount); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &initializeZeroTime;
	for (unsigned int i = 0; i < sizeof(initializeZeroTime); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &autoZeroTime;
	for (unsigned int i = 0; i < sizeof(autoZeroTime); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rateOptionLimit;
	for (unsigned int i = 0; i < sizeof(rateOptionLimit); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &zeroReferenceLimit;
	for (unsigned int i = 0; i < sizeof(zeroReferenceLimit); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &calibrationReferenceLimit;
	for (unsigned int i = 0; i < sizeof(calibrationReferenceLimit); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &smallMotionLimit;
	for (unsigned int i = 0; i < sizeof(smallMotionLimit); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &largeMotionLimit;
	for (unsigned int i = 0; i < sizeof(largeMotionLimit); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &largeMotionCount;
	for (unsigned int i = 0; i < sizeof(largeMotionCount); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &smallMotionCount;
	for (unsigned int i = 0; i < sizeof(smallMotionCount); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &noMotionCount;
	for (unsigned int i = 0; i < sizeof(noMotionCount); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &zeroTrackingOption;
	for (unsigned int i = 0; i < sizeof(zeroTrackingOption); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &negativeWeightOption;
	for (unsigned int i = 0; i < sizeof(negativeWeightOption); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &powerupMaintenanceZoneLimit;
	for (unsigned int i = 0; i < sizeof(powerupMaintenanceZoneLimit); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &operateMaintenanceZoneLimit;
	for (unsigned int i = 0; i < sizeof(operateMaintenanceZoneLimit); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &filterSpeed;
	for (unsigned int i = 0; i < sizeof(filterSpeed); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &lastCalibrationDate;
	for (unsigned int i = 0; i < sizeof(lastCalibrationDate); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &numberOfCalibrations;
	for (unsigned int i = 0; i < sizeof(numberOfCalibrations); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &lastConfigurationDate;
	for (unsigned int i = 0; i < sizeof(lastConfigurationDate); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &numberOfConfigurations;
	for (unsigned int i = 0; i < sizeof(numberOfConfigurations); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &weigherType;
	for (unsigned int i = 0; i < sizeof(weigherType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &flags;
	for (unsigned int i = 0; i < sizeof(flags); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &minWeightToPrint;
	for (unsigned int i = 0; i < sizeof(minWeightToPrint); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &model;
	for (unsigned int i = 0; i < sizeof(model); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&valueMax;
	for (unsigned int i = 0; i < sizeof(valueMax); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *)&azsmMotionLimit;
	for (unsigned int i = 0; i < sizeof(azsmMotionLimit); i++)
	{
		*pChar++ = buffer[j++];
	}
	//SOF-3958
	if( model == WmHLX1 ) {
		valueMax = 0;
	}
}

WeigherConfiguration::WeigherConfiguration(WgConfigType configType,
										   WeigherInquire &wgInquire,
										   Hobart::CEvolMsgReceiver *pHobThread) :	EvolMessage(Weigher, WgConfig, pHobThread),
																	configType(configType),
																	centerOfMaintenanceZone(wgInquire.getCenterOfMaintenanceZone()),
																	scaleFactor(wgInquire.getScaleFactor()),
																	maxWeight(wgInquire.getMaxWeight()),
																	gainFactor(wgInquire.getGainFactor()),
																	zeroReference(wgInquire.getZeroReference()),
																	calibrationReference(wgInquire.getCalibrationReference()),
																	rateOptionTime(wgInquire.getRateOptionLimit()),
																	autoZeroMaintenanceLimit(wgInquire.getAutoZeroMaintenanceLimit()),
																	prepackMotionCount(wgInquire.getPrepackMotionCount()),
																	initializeZeroTime(wgInquire.getInitializeZeroTime()),
																	autoZeroTime(wgInquire.getAutoZeroTime()),
																	rateOptionLimit(wgInquire.getRateOptionLimit()),
																	zeroReferenceLimit(wgInquire.getZeroReferenceLimit()),
																	calibrationReferenceLimit(wgInquire.getCalibrationReferenceLimit()),
																	smallMotionLimit(wgInquire.getSmallMotionLimit()),
																	largeMotionLimit(wgInquire.getLargeMotionLimit()),
																	largeMotionCount(wgInquire.getLargeMotionCount()),
																	smallMotionCount(wgInquire.getSmallMotionCount()),
																	noMotionCount(wgInquire.getNoMotionCount()),
																	zeroTrackingOption(wgInquire.getZeroTrackingOption()),
																	negativeWeightOption(wgInquire.getNegativeWeightOption()),
																	powerupMaintenanceZoneLimit(wgInquire.getPowerupMaintenanceZoneLimit()),
																	operateMaintenanceZoneLimit(wgInquire.getOperateMaintenanceZoneLimit()),
																	filterSpeed(wgInquire.getFilterSpeed()),
																	referenceVoltage(wgInquire.getReferenceVoltage()),
																	lastCalibrationDate(wgInquire.getLastCalibrationDate()),
																	numberOfCalibrations(wgInquire.getNumberOfCalibrations()),
																	lastConfigurationDate(wgInquire.getLastConfigurationDate()),
																	numberOfConfigurations(wgInquire.getNumberOfConfigurations()),
																	weigherType(wgInquire.getWeigherType()),
																	flags(wgInquire.getFlags()),
																	minWeightToPrint(wgInquire.getMinWeightToPrint()),
																	model(wgInquire.getWeigherModel()),
																	valueMax(wgInquire.getValueMaxEnabled()),
																	azsmMotionLimit(wgInquire.getAzsmMotionLimit())
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	wgInquire.getDeviceSignature(deviceSignature);
}

WeigherConfiguration::~WeigherConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

const char *WeigherConfiguration::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &configType;
	for (unsigned int i = 0; i < sizeof(configType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &deviceSignature;
	for (unsigned int i = 0; i < sizeof(deviceSignature); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &centerOfMaintenanceZone;
	for (unsigned int i = 0; i < sizeof(centerOfMaintenanceZone); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &scaleFactor;
	for (unsigned int i = 0; i < sizeof(scaleFactor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &maxWeight;
	for (unsigned int i = 0; i < sizeof(maxWeight); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &gainFactor;
	for (unsigned int i = 0; i < sizeof(gainFactor); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &zeroReference;
	for (unsigned int i = 0; i < sizeof(zeroReference); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &calibrationReference;
	for (unsigned int i = 0; i < sizeof(calibrationReference); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rateOptionTime;
	for (unsigned int i = 0; i < sizeof(rateOptionTime); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &autoZeroMaintenanceLimit;
	for (unsigned int i = 0; i < sizeof(autoZeroMaintenanceLimit); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &prepackMotionCount;
	for (unsigned int i = 0; i < sizeof(prepackMotionCount); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &initializeZeroTime;
	for (unsigned int i = 0; i < sizeof(initializeZeroTime); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &autoZeroTime;
	for (unsigned int i = 0; i < sizeof(autoZeroTime); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rateOptionLimit;
	for (unsigned int i = 0; i < sizeof(rateOptionLimit); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &zeroReferenceLimit;
	for (unsigned int i = 0; i < sizeof(zeroReferenceLimit); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &calibrationReferenceLimit;
	for (unsigned int i = 0; i < sizeof(calibrationReferenceLimit); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &smallMotionLimit;
	for (unsigned int i = 0; i < sizeof(smallMotionLimit); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &largeMotionLimit;
	for (unsigned int i = 0; i < sizeof(largeMotionLimit); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &largeMotionCount;
	for (unsigned int i = 0; i < sizeof(largeMotionCount); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &smallMotionCount;
	for (unsigned int i = 0; i < sizeof(smallMotionCount); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &noMotionCount;
	for (unsigned int i = 0; i < sizeof(noMotionCount); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &zeroTrackingOption;
	for (unsigned int i = 0; i < sizeof(zeroTrackingOption); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &negativeWeightOption;
	for (unsigned int i = 0; i < sizeof(negativeWeightOption); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &powerupMaintenanceZoneLimit;
	for (unsigned int i = 0; i < sizeof(powerupMaintenanceZoneLimit); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &operateMaintenanceZoneLimit;
	for (unsigned int i = 0; i < sizeof(operateMaintenanceZoneLimit); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &filterSpeed;
	for (unsigned int i = 0; i < sizeof(filterSpeed); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &lastCalibrationDate;
	for (unsigned int i = 0; i < sizeof(lastCalibrationDate); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &numberOfCalibrations;
	for (unsigned int i = 0; i < sizeof(numberOfCalibrations); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &lastConfigurationDate;
	for (unsigned int i = 0; i < sizeof(lastConfigurationDate); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &numberOfConfigurations;
	for (unsigned int i = 0; i < sizeof(numberOfConfigurations); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &weigherType;
	for (unsigned int i = 0; i < sizeof(weigherType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &flags;
	for (unsigned int i = 0; i < sizeof(flags); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &minWeightToPrint;
	for (unsigned int i = 0; i < sizeof(minWeightToPrint); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &model;
	for (unsigned int i = 0; i < sizeof(model); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	// should valuemax be here or is it missing on purpose?  Yes, it should be here
	pChar = (char *)&valueMax;
	for (unsigned int i = 0; i < sizeof(valueMax); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *)&azsmMotionLimit;
	for (unsigned int i = 0; i < sizeof(azsmMotionLimit); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WeigherConfiguration::show()
{
	Debug::trace("WeigherConfiguration", "WeigherConfiguration: ");

	EvolMessage::show();

	Debug::trace("WeigherConfiguration", "deviceSignature: ");
	Debug::trace("WeigherConfiguration", Hex::toString((const char *) &deviceSignature[0], SystemDevSignatureSize));

	if (configType & WgCalibInProgress)
	{
		Debug::trace("WeigherConfiguration", "configType: WgCalibInProgress");
	}
	
	if (configType == WgPermanent)
	{
		Debug::trace("WeigherConfiguration", "configType: WgPermanent");
	}
	else if (configType == WgTemporary)
	{
		Debug::trace("WeigherConfiguration", "configType: WgTemporary");
	}
	else if (configType == WgDefault)
	{
		Debug::trace("WeigherConfiguration", "configType: WgDefault");
	}
	else
	{
		Debug::trace("WeigherConfiguration", HobString("configType: Unknown ") +
											 HobString(Integer::toString((UINT16) configType)));
	}

	Debug::trace("WeigherConfiguration", HobString("centerOfMaintenanceZone: ") +
										 HobString(Integer::toString(centerOfMaintenanceZone)));
	Debug::trace("WeigherConfiguration", HobString("scaleFactor: ") +
										 HobString(Integer::toString(scaleFactor)));
	Debug::trace("WeigherConfiguration", HobString("maxWeight: ") +
										 HobString(Integer::toString(maxWeight)));
	Debug::trace("WeigherConfiguration", HobString("gainFactor: ") +
										 HobString(Integer::toString(gainFactor)));
	Debug::trace("WeigherConfiguration", HobString("zeroReference: ") +
										 HobString(Integer::toString(zeroReference)));
	Debug::trace("WeigherConfiguration", HobString("calibrationReference: ") +
										 HobString(Integer::toString(calibrationReference)));
	Debug::trace("WeigherConfiguration", HobString("rateOptionTime: ") +
										 HobString(Integer::toString(rateOptionTime)));
	Debug::trace("WeigherConfiguration", HobString("autoZeroMaintenanceLimit: ") +
										 HobString(Integer::toString(autoZeroMaintenanceLimit)));
	Debug::trace("WeigherConfiguration", HobString("prepackMotionCount: ") +
										 HobString(Integer::toString(prepackMotionCount)));
	Debug::trace("WeigherConfiguration", HobString("initializeZeroTime: ") +
										 HobString(Integer::toString(initializeZeroTime)));
	Debug::trace("WeigherConfiguration", HobString("autoZeroTime: ") +
										 HobString(Integer::toString(autoZeroTime)));
	Debug::trace("WeigherConfiguration", HobString("rateOptionLimit: ") +
										 HobString(Integer::toString(rateOptionLimit)));
	Debug::trace("WeigherConfiguration", HobString("zeroReferenceLimit: ") +
										 HobString(Integer::toString(zeroReferenceLimit)));
	Debug::trace("WeigherConfiguration", HobString("calibrationReferenceLimit: ") +
										 HobString(Integer::toString(calibrationReferenceLimit)));
	Debug::trace("WeigherConfiguration", HobString("smallMotionLimit: ") +
										 HobString(Integer::toString(smallMotionLimit)));
	Debug::trace("WeigherConfiguration", HobString("largeMotionLimit: ") +
										 HobString(Integer::toString(largeMotionLimit)));
	Debug::trace("WeigherConfiguration", HobString("largeMotionCount: ") +
										 HobString(Integer::toString(largeMotionCount)));
	Debug::trace("WeigherConfiguration", HobString("smallMotionCount: ") +
										 HobString(Integer::toString(smallMotionCount)));
	Debug::trace("WeigherConfiguration", HobString("noMotionCount: ") +
										 HobString(Integer::toString(noMotionCount)));
	Debug::trace("WeigherConfiguration", HobString("zeroTrackingOption: ") +
										 HobString(Integer::toString(zeroTrackingOption)));
	Debug::trace("WeigherConfiguration", HobString("negativeWeightOption: ") +
										 HobString(Integer::toString(negativeWeightOption)));
	Debug::trace("WeigherConfiguration", HobString("powerupMaintenanceZoneLimit: ") +
										 HobString(Integer::toString(powerupMaintenanceZoneLimit)));
	Debug::trace("WeigherConfiguration", HobString("operateMaintenanceZoneLimit: ") +
										 HobString(Integer::toString(operateMaintenanceZoneLimit)));

	if (filterSpeed == NormalFilterSpeed)
	{
		Debug::trace("WeigherConfiguration", "filterSpeed: NormalFilterSpeed");
	}
	else if (filterSpeed == SlowFilterSpeed)
	{
		Debug::trace("WeigherConfiguration", "filterSpeed: SlowFilterSpeed");
	}
	else if (filterSpeed == FastFilterSpeed)
	{
		Debug::trace("WeigherConfiguration", "filterSpeed: FastFilterSpeed");
	}
	else
	{
		Debug::trace("WeigherConfiguration", HobString("filterSpeed: Unknown ") +
											 HobString(Integer::toString((UINT16) filterSpeed)));
	}

	Debug::trace("WeigherConfiguration", HobString("lastCalibrationDate: ") + HobString(Integer::toString(lastCalibrationDate)));
//										 lastCalibrationDate.formatDate(FormatDate_mmmDDYY));
	Debug::trace("WeigherConfiguration", HobString("numberOfCalibrations: ") +
										 HobString(Integer::toString(numberOfCalibrations)));

	Debug::trace("WeigherConfiguration", HobString("lastConfigurationDate: ") + HobString(Integer::toString(lastConfigurationDate)));
//										 lastConfigurationDate.formatDate(FormatDate_mmmDDYY));
	Debug::trace("WeigherConfiguration", HobString("numberOfConfigurations: ") +
										 HobString(Integer::toString(numberOfConfigurations)));									 
	Debug::trace("WeigherConfiguration", HobString("AzsmMotionLimit: ") +
										 HobString(Integer::toString(azsmMotionLimit)));										 

	if (weigherType == FirstWeigher)
	{
		Debug::trace("WeigherConfiguration", "weigherType: FirstWeigher");
	}
	else if (weigherType == SecondWeigher)
	{
		Debug::trace("WeigherConfiguration", "weigherType: SecondWeigher");
	}
	else
	{
		Debug::trace("WeigherConfiguration", HobString("weigherType: Unknown ") +
											 HobString(Integer::toString((UINT16) weigherType)));
	}

	Debug::trace("WeigherConfiguration", HobString("flags: ") +
										 HobString(Hex::toString(flags)));
	Debug::trace("WeigherConfiguration", HobString("minWeightToPrint: ") +
										 HobString(Integer::toString(minWeightToPrint)));
	Debug::trace("WeigherConfiguration", HobString("model: ") +
										 HobString(Integer::toString(model)));

	Debug::trace("WeigherConfiguration", HobString("valueMax: ") +
		HobString(Integer::toString(valueMax)));
}

void WeigherConfiguration::clear()
{
	configType = WgPermanent;

	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = '\0';
	}

    centerOfMaintenanceZone 	= 0;
    scaleFactor               	= 0;
    maxWeight                 	= 0;
    gainFactor                	= 0;
    zeroReference             	= 0;
    calibrationReference      	= 0;
	rateOptionTime           	= 0;
	autoZeroMaintenanceLimit	= 0;
	prepackMotionCount       	= 0;
    initializeZeroTime       	= 0;
    autoZeroTime             	= 0;
	autoZeroTime				= 0;
	zeroReferenceLimit       	= 0;
	calibrationReferenceLimit	= 0;
	smallMotionLimit         	= 0;
    largeMotionLimit         	= 0;
	largeMotionCount         	= 0;
	smallMotionCount         	= 0;
	noMotionCount				= 0;
    zeroTrackingOption       	= 0; /* FALSE */
    negativeWeightOption     	= 0; /* FALSE */
	powerupMaintenanceZoneLimit = 0;
	operateMaintenanceZoneLimit = 0;
    filterSpeed               	= NormalFilterSpeed;

	weigherType = FirstWeigher;

	flags = 0;
	minWeightToPrint = 0;
	model = WmNone;

	valueMax = 0;			// jhc valuemax - added default 0 value. Should I do this here?
	azsmMotionLimit				= 0;
}

void WeigherConfiguration::setConfigType(WgConfigType configType)
{
	this->configType = configType;
}

WgConfigType WeigherConfiguration::getConfigType() const
{
	return configType;
}

void WeigherConfiguration::setDeviceSignature(UINT8 *deviceSignature)
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		this->deviceSignature[i] = deviceSignature[i];
	}
}

void WeigherConfiguration::setCenterOfMaintenanceZone(UINT32 centerOfMaintenanceZone)
{
	this->centerOfMaintenanceZone = centerOfMaintenanceZone;
}

void WeigherConfiguration::setScaleFactor(UINT32 scaleFactor)
{
	Debug::trace("WeigherConfiguration", "setScaleFactor(): scaleFactor: " + HobString(Integer::toString((int)scaleFactor)));
	this->scaleFactor = scaleFactor;
}

void WeigherConfiguration::setMaxWeight(UINT32 maxWeight)
{
	this->maxWeight = maxWeight;
}

void WeigherConfiguration::setGainFactor(UINT32 gainFactor)
{
	this->gainFactor = gainFactor;
}

void WeigherConfiguration::setZeroReference(UINT32 zeroReference)
{
	this->zeroReference = zeroReference;
}

void WeigherConfiguration::setCalibrationReference(UINT32 calibrationReference)
{
	this->calibrationReference = calibrationReference;
}

void WeigherConfiguration::setRateOptionTime(UINT16 rateOptionTime)
{
	this->rateOptionTime = rateOptionTime;
}

void WeigherConfiguration::setAutoZeroMaintenanceLimit(UINT16 autoZeroMaintenanceLimit)
{
	this->autoZeroMaintenanceLimit = autoZeroMaintenanceLimit;
}

void WeigherConfiguration::setPrepackMotionCount(UINT16 prepackMotionCount)
{
	this->prepackMotionCount = prepackMotionCount;
}

void WeigherConfiguration::setInitializeZeroTime(UINT16 initializeZeroTime)
{
	this->initializeZeroTime = initializeZeroTime;
}

void WeigherConfiguration::setAutoZeroTime(UINT16 autoZeroTime)
{
	this->autoZeroTime = autoZeroTime;
}

void WeigherConfiguration::setRateOptionLimit(UINT16 rateOptionLimit)
{
	this->rateOptionLimit = rateOptionLimit;
}

void WeigherConfiguration::setZeroReferenceLimit(UINT16 zeroReferenceLimit)
{
	this->zeroReferenceLimit = zeroReferenceLimit;
}

void WeigherConfiguration::setCalibrationReferenceLimit(UINT16 calibrationReferenceLimit)
{
	this->calibrationReferenceLimit = calibrationReferenceLimit;
}

void WeigherConfiguration::setSmallMotionLimit(UINT16 smallMotionLimit)
{
	this->smallMotionLimit = smallMotionLimit;
}

void WeigherConfiguration::setLargeMotionLimit(UINT16 largeMotionLimit)
{
	this->largeMotionLimit = largeMotionLimit;
}

void WeigherConfiguration::setLargeMotionCount(UINT16 largeMotionCount)
{
	this->largeMotionCount = largeMotionCount;
}

void WeigherConfiguration::setSmallMotionCount(UINT16 smallMotionCount)
{
	this->smallMotionCount = smallMotionCount;
}

void WeigherConfiguration::setNoMotionCount(UINT16 noMotionCount)
{
	this->noMotionCount = noMotionCount;
}

void WeigherConfiguration::setZeroTrackingOption(UINT16 zeroTrackingOption)
{
	this->zeroTrackingOption = zeroTrackingOption;
}

void WeigherConfiguration::setNegativeWeightOption(UINT16 negativeWeightOption)
{
	this->negativeWeightOption = negativeWeightOption;
}

void WeigherConfiguration::setPowerupMaintenanceZoneLimit(UINT16 powerupMaintenanceZoneLimit)
{
	this->powerupMaintenanceZoneLimit = powerupMaintenanceZoneLimit;
}

void WeigherConfiguration::setOperateMaintenanceZoneLimit(UINT16 operateMaintenanceZoneLimit)
{
	this->operateMaintenanceZoneLimit = operateMaintenanceZoneLimit;
}

void WeigherConfiguration::setFilterSpeed(WgFilterSpeed filterSpeed)
{
	this->filterSpeed = filterSpeed;
}

void WeigherConfiguration::setReferenceVoltage(UINT8 referenceVoltage)
{
	this->referenceVoltage = referenceVoltage;
}

void WeigherConfiguration::setLastCalibrationDate(UINT32 lastCalibrationDate)
{
	this->lastCalibrationDate = lastCalibrationDate;
}

void WeigherConfiguration::setNumberOfCalibrations(UINT16 numberOfCalibrations)
{
	this->numberOfCalibrations = numberOfCalibrations;
}

void WeigherConfiguration::setLastConfigurationDate(UINT32 lastConfigurationDate)
{
	this->lastConfigurationDate = lastConfigurationDate;
}

void WeigherConfiguration::setNumberOfConfigurations(UINT16 numberOfConfigurations)
{
	this->numberOfConfigurations = numberOfConfigurations;
}

void WeigherConfiguration::setAzsmMotionLimit(UINT8 azsmMotionLimit)
{
	this->azsmMotionLimit = azsmMotionLimit;
}

void WeigherConfiguration::setWeigherType(WeigherType weigherType)
{
	this->weigherType = weigherType;
}

void WeigherConfiguration::setFlags(UINT8 flags)
{
	Debug::trace("WeigherConfiguration", "setFlags(): flags: " + HobString(Integer::toString(flags)) + "  current flag: " + HobString(Hex::toString(this->flags)));
	this->flags = flags;
	Debug::trace("WeigherConfiguration", "setFlags():  resulting flag: " + HobString(Hex::toString(this->flags)));
}
void WeigherConfiguration::setScaleWeighMode(WeighMode mode)
{
	Debug::trace("WeigherConfiguration", "setScaleWeighMode(): mode: " + HobString(Integer::toString(mode)) + "  current flag: " + HobString(Hex::toString(flags)));

	// Turn off any unused bits in the flags attribute.
	flags &= 0x0F;

	if (mode == WeighMode_Avoir)
		flags |= 0x01;
	else
		flags &= 0xFE;
	Debug::trace("WeigherConfiguration", "setScaleWeighMode():  resulting flag: " + HobString(Hex::toString(flags)));
}


	// masks used for setting/getting language values
#define GcdLanguage_Clear             0xF3   // 1111 0011
#define GcdLanguage_JustLanguageBits  0x0C   // 0000 1100

void WeigherConfiguration::setScaleGCDLanguage(int /*language*/)
{
	// These aren't used any more, so just clear them out!!!  Windux GCD R2_1 and Fastrack
	//Debug::trace("WeigherConfiguration", "setScaleGCDLanguage(): language: " + HobString(Integer::toString(language)) + "  current flag: " + HobString(Hex::toString(flags)));

	// Turn off any unused bits in the flags attribute.
	flags &= 0x0F;

	// clear all language bits in the flags attribute.
	flags &= GcdLanguage_Clear;

	// These aren't used any more, so just clear them out, don't set them!!!  Windux GCD R2_1 and Fastrack
	//flags |= language;

	Debug::trace("WeigherConfiguration", "setScaleGCDLanguage():  resulting flag: " + HobString(Hex::toString(flags)));
}

void WeigherConfiguration::setNetWeightDualRangeLimitCheck(bool usingNetWeight)
{
	bool usingNetWeightSetting = usingNetWeight;
	if ( (model == WgUsbApc) || (model == WgCanKpc) )
	{
		// For Fastrack, per FT-708, this should always be set to true (dual range limit based on net weight, not gross weight), and never be set to false.
		usingNetWeightSetting = true;
	}

	Debug::trace("WeigherConfiguration", "setNetWeightDualRangeLimitCheck(): usingNetWeight: " + HobString(Bool::toString(usingNetWeightSetting)) + "  current flag: " + HobString(Hex::toString(flags)));

	// Turn off any unused bits in the flags attribute.
	flags &= 0x1F;

	if (usingNetWeightSetting)
		flags |= 0x02;
	else
		flags &= 0xFD;
	Debug::trace("WeigherConfiguration", "setNetWeightDualRangeLimitCheck():  resulting flag: " + HobString(Hex::toString(flags)));
}

void WeigherConfiguration::setMinWeightToPrint(UINT32 minWeightToPrint)
{
	this->minWeightToPrint = minWeightToPrint;
}

void WeigherConfiguration::setWeigherModel(WeigherModel model)
{
	Debug::trace("WeigherConfiguration", "setWeigherModel(): model: " + HobString(Integer::toString(model)));
	this->model = model;
}


void WeigherConfiguration::getDeviceSignature(UINT8 *deviceSignature) const
{
	for (int i = 0; i < SystemDevSignatureSize; i++)
	{
		deviceSignature[i] = this->deviceSignature[i];
	}
}

UINT32 WeigherConfiguration::getCenterOfMaintenanceZone() const
{
	return centerOfMaintenanceZone;
}

UINT32 WeigherConfiguration::getScaleFactor() const
{
//	Debug::trace("WeigherConfiguration", "getScaleFactor(): scaleFactor: " + HobString(Integer::toString((int)scaleFactor)));
	return scaleFactor;
}

UINT32 WeigherConfiguration::getMaxWeight() const
{
	return maxWeight;
}

UINT32 WeigherConfiguration::getGainFactor() const
{
	return gainFactor;
}

UINT32 WeigherConfiguration::getZeroReference() const
{
	return zeroReference;
}

UINT32 WeigherConfiguration::getCalibrationReference() const
{
	return calibrationReference;
}

UINT16 WeigherConfiguration::getRateOptionTime() const
{
	return rateOptionTime;
}

UINT16 WeigherConfiguration::getAutoZeroMaintenanceLimit() const
{
	return autoZeroMaintenanceLimit;
}

UINT16 WeigherConfiguration::getPrepackMotionCount() const
{
	return prepackMotionCount;
}

UINT16 WeigherConfiguration::getInitializeZeroTime() const
{
	return initializeZeroTime;
}

UINT16 WeigherConfiguration::getAutoZeroTime() const
{
	return autoZeroTime;
}

UINT16 WeigherConfiguration::getRateOptionLimit() const
{
	return rateOptionLimit;
}

UINT16 WeigherConfiguration::getZeroReferenceLimit() const
{
	return zeroReferenceLimit;
}

UINT16 WeigherConfiguration::getCalibrationReferenceLimit() const
{
	return calibrationReferenceLimit;
}

UINT16 WeigherConfiguration::getSmallMotionLimit() const
{
	return smallMotionLimit;
}

UINT16 WeigherConfiguration::getLargeMotionLimit() const
{
	return largeMotionLimit;
}

UINT16 WeigherConfiguration::getLargeMotionCount() const
{
	return largeMotionCount;
}

UINT16 WeigherConfiguration::getSmallMotionCount() const
{
	return smallMotionCount;
}

UINT16 WeigherConfiguration::getNoMotionCount() const
{
	return noMotionCount;
}

UINT16 WeigherConfiguration::getZeroTrackingOption() const
{
	return zeroTrackingOption;
}

UINT16 WeigherConfiguration::getNegativeWeightOption() const
{
	return negativeWeightOption;
}

UINT16 WeigherConfiguration::getPowerupMaintenanceZoneLimit() const
{
	return powerupMaintenanceZoneLimit;
}

UINT16 WeigherConfiguration::getOperateMaintenanceZoneLimit() const
{
	return operateMaintenanceZoneLimit;
}

WgFilterSpeed WeigherConfiguration::getFilterSpeed() const
{
	return filterSpeed;
}

UINT8 WeigherConfiguration::getReferenceVoltage() const
{
	return referenceVoltage;
}

UINT32 WeigherConfiguration::getLastCalibrationDate() const
{
	return lastCalibrationDate;
}

UINT16 WeigherConfiguration::getNumberOfCalibrations() const
{
	return numberOfCalibrations;
}

UINT32 WeigherConfiguration::getLastConfigurationDate() const
{
	return lastConfigurationDate;
}

UINT16 WeigherConfiguration::getNumberOfConfigurations() const
{
	return numberOfConfigurations;
}

WeigherType WeigherConfiguration::getWeigherType()
{
	return weigherType;
}

UINT8 WeigherConfiguration::getFlags() const
{
	return flags;
}

WeighMode WeigherConfiguration::getScaleWeighMode() const
{
	WeighMode result = (isScaleAvoir()) ? WeighMode_Avoir : WeighMode_Metric;
	Debug::trace("WeigherConfiguration", "getScaleWeighMode(): result: " + HobString(Integer::toString(result)));
	return result;
}

bool WeigherConfiguration::isScaleMetric() const
{
	bool result = !isScaleAvoir();
	Debug::trace("WeigherConfiguration", "isScaleMetric(): result: " + HobString(Bool::toString(result)));
	return result;
}
bool WeigherConfiguration::isScaleAvoir() const
{
	bool result = (flags & 0x01) ? true : false;
	Debug::trace("WeigherConfiguration", "isScaleAvoir(): result: " + HobString(Bool::toString(result)));
	return result;
}

bool WeigherConfiguration::isNetWeightDualRangeLimitCheck() const
{
	/* SOF-1360 - Commenting out the check for model, since all weigher models are required to have the threshold set based on Net Weight, per W&M.
					This code caused a problem for the ARA board weigher, which isn't the WgUsbApc weigher model.
	bool usingNetWeightSetting = false;
	if ( (model == WgUsbApc) || (model == WgCanKpc) )
	{
		// For Fastrack, per FT-708, this should always be set to true (dual range limit based on net weight, not gross weight), and never be set to false.
		usingNetWeightSetting = true;
	}
	else
	{
		usingNetWeightSetting = (flags & 0x02) ? true : false;
		Debug::trace("WeigherConfiguration", "isNetWeightDualRangeLimitCheck(): result: " + HobString(Bool::toString(usingNetWeightSetting)));
	}
	*/
	return true;
}

UINT32 WeigherConfiguration::getMinWeightToPrint() const
{
	return minWeightToPrint;
}

WeigherModel WeigherConfiguration::getWeigherModel() const
{
	return model;
}

UINT8 WeigherConfiguration::getAzsmMotionLimit() const
{
	return azsmMotionLimit;
}


// static
UINT32 WeigherConfiguration::convertGainFactorToMetric(const UINT32 gainFactor)
{
	bool isTracing = Debug::isTracing("WeigherConfiguration");

	long gainL = (long)gainFactor;
	double gainD = (double)gainFactor;

	if (isTracing)
	{
		Debug::trace("WeigherConfiguration", "convertGainFactorToMetric(): input gain L: " + HobString(Long::toString(gainL)));
		Debug::trace("WeigherConfiguration", "convertGainFactorToMetric(): input gain D: " + util_string::doubletostring(gainD));
	}

	gainD = gainD * 2.0 * 0.45359237;

	if (isTracing)
		Debug::trace("WeigherConfiguration", "convertGainFactorToMetric(): new gain before rounding: " + util_string::doubletostring(gainD));

	gainD = Rounding::hobround(gainD);

	if (isTracing)
	{
		Debug::trace("WeigherConfiguration", "convertGainFactorToMetric(): output gain D: " + util_string::doubletostring(gainD));
		Debug::trace("WeigherConfiguration", "convertGainFactorToMetric(): output gain L: " + HobString(Long::toString((UINT32)gainD)));
	}

	return (UINT32)gainD;
}

// static
UINT32 WeigherConfiguration::convertGainFactorToAvoir(const UINT32 gainFactor)
{
	bool isTracing = Debug::isTracing("WeigherConfiguration");

	long gainL = (long)gainFactor;
	double gainD = (double)gainFactor;

	if (isTracing)
	{
		Debug::trace("WeigherConfiguration", "convertGainFactorToAvoir(): input gain L: " + HobString(Long::toString(gainL)));
		Debug::trace("WeigherConfiguration", "convertGainFactorToAvoir(): input gain D: " + util_string::doubletostring(gainD));
	}

	gainD = gainD / (2.0 * 0.45359237);

	if (isTracing)
		Debug::trace("WeigherConfiguration", "convertGainFactorToAvoir(): new gain before rounding: " + util_string::doubletostring(gainD));

	gainD = Rounding::hobround(gainD);

	if (isTracing)
	{
		Debug::trace("WeigherConfiguration", "convertGainFactorToAvoir(): output gain D:" + util_string::doubletostring(gainD));
		Debug::trace("WeigherConfiguration", "convertGainFactorToAvoir(): output gain L:" + HobString(Long::toString((UINT32)gainD)));
	}

	return (UINT32)gainD;
}


bool WeigherConfiguration::isUserConfigOutOfRange() const
{
	bool outOfRange = false;
	bool tracing = Debug::isTracing("WeigherConfiguration");
/*
	if (centerOfMaintenanceZone > )
		outOfRange = true;
	if (gainFactor > )
		outOfRange = true;
	if (zeroReference > )
		outOfRange = true;
*/

	// Not checked for out of range: lastCalibrationDate, numberOfCalibrations, lastConfigurationDate, numberOfConfigurations

	// This value is used for both metric and avoir modes.
	if (maxWeight != (UINT32)300900)
	{
		if (tracing)
			Debug::trace("WeigherConfiguration", "isUserConfigOutOfRange(): maxWeight: " + util_string::doubletostring(maxWeight));
		outOfRange = true;
	}

	// Really, 1 is only valid for pounds mode with a non dual range weigher.  We'll let it pass for now since the Access supports this.
	if (scaleFactor != 1 && scaleFactor != 2 && scaleFactor != 5)
	{
		if (tracing)
			Debug::trace("WeigherConfiguration", "isUserConfigOutOfRange(): scaleFactor: " + util_string::inttostring((int)scaleFactor));
		outOfRange = true;
	}

	if (filterSpeed != NormalFilterSpeed && filterSpeed != SlowFilterSpeed && filterSpeed != FastFilterSpeed)
	{
		if (tracing)
			Debug::trace("WeigherConfiguration", "isUserConfigOutOfRange(): filterSpeed: " + util_string::inttostring((int)filterSpeed));
		outOfRange = true;
	}

	// currently, only the first four bits are used; any other bit set indicates the flag field has not been set.
	if (flags & 0xF0)	//changed from 0xf0 to 0xe0 to mask the value max bit - 07/22/2019 - changed back to 0xf0 bc value max bit removed from the flag
	{
		if (tracing)
			Debug::trace("WeigherConfiguration", "isUserConfigOutOfRange(): flags: " + util_string::inttostring(flags));
		outOfRange = true;
	}

	if (minWeightToPrint > _Max_MinWeightToPrint_)
	{
		if (tracing)
			Debug::trace("WeigherConfiguration", "isUserConfigOutOfRange(): minWeightToPrint: " + util_string::doubletostring(minWeightToPrint));
		outOfRange = true;
	}

	if (model < WmNone || model >= endWeigherModel)
	{
		if (tracing)
			Debug::trace("WeigherConfiguration", "isUserConfigOutOfRange(): model: " + util_string::inttostring((int)model));
		outOfRange = true;
	}

	return outOfRange;
}

// jhc valuemax - moved these from the header file so I can step through them while debugging.
void WeigherConfiguration::setValueMaxEnabled(UINT8 enable )
{
	valueMax = enable;
}

UINT8 WeigherConfiguration::getValueMaxEnabled() const
{
	return valueMax;
}
