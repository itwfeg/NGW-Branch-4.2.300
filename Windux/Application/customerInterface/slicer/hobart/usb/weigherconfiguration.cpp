#include "weigherconfiguration.h"
#include "hobartweigher.h"
#include "weigherconfiguration.h"
#include <endian.h>



WeigherConfiguration::WeigherConfiguration()
{
    qRegisterMetaType<WeigherConfiguration>("WeigherConfiguration&");
    /*
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
    zeroTrackingOption       	= 1;  //TRUE
    negativeWeightOption     	= 0; // FALSE
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
    */
}

WeigherConfiguration::WeigherConfiguration(const char *buffer)
{

    unsigned int j=4;


    // Now, for the derived class items
    /*
    char *pChar = (char *) &configType;

    for (unsigned int i = 0; i < sizeof(configType); i++)
    {
        *pChar++ = buffer[j++];
    }
    */

    for (unsigned int i = 0; i < SystemDevSignatureSize; i++)
    {
        deviceSignature[i] = buffer[j++];
    }

    char *pChar = (char *) &centerOfMaintenanceZone;
    for (unsigned int i = 0; i < sizeof(centerOfMaintenanceZone); i++)
    {
        *pChar++ = buffer[j++];
    }
    centerOfMaintenanceZone = be32toh(centerOfMaintenanceZone);

    pChar = (char *) &scaleFactor;
    for (unsigned int i = 0; i < sizeof(scaleFactor); i++)
    {
        *pChar++ = buffer[j++];
    }
    scaleFactor = be32toh(scaleFactor);

    pChar = (char *) &maxWeight;
    for (unsigned int i = 0; i < sizeof(maxWeight); i++)
    {
        *pChar++ = buffer[j++];
    }
    maxWeight = be32toh(maxWeight);

    pChar = (char *) &gainFactor;
    for (unsigned int i = 0; i < sizeof(gainFactor); i++)
    {
        *pChar++ = buffer[j++];
    }
    gainFactor = be32toh(gainFactor);

    pChar = (char *) &zeroReference;
    for (unsigned int i = 0; i < sizeof(zeroReference); i++)
    {
        *pChar++ = buffer[j++];
    }
    zeroReference = be32toh(zeroReference);

    pChar = (char *) &calibrationReference;
    for (unsigned int i = 0; i < sizeof(calibrationReference); i++)
    {
        *pChar++ = buffer[j++];
    }
    calibrationReference = be32toh(calibrationReference);

    pChar = (char *) &rateOptionTime;
    for (unsigned int i = 0; i < sizeof(rateOptionTime); i++)
    {
        *pChar++ = buffer[j++];
    }
    rateOptionTime = be16toh(rateOptionTime);

    pChar = (char *) &autoZeroMaintenanceLimit;
    for (unsigned int i = 0; i < sizeof(autoZeroMaintenanceLimit); i++)
    {
        *pChar++ = buffer[j++];
    }
    autoZeroMaintenanceLimit = be16toh(autoZeroMaintenanceLimit);

    pChar = (char *) &prepackMotionCount;
    for (unsigned int i = 0; i < sizeof(prepackMotionCount); i++)
    {
        *pChar++ = buffer[j++];
    }
    prepackMotionCount = be16toh(prepackMotionCount);


    pChar = (char *) &initializeZeroTime;
    for (unsigned int i = 0; i < sizeof(initializeZeroTime); i++)
    {
        *pChar++ = buffer[j++];
    }
    initializeZeroTime = be16toh(initializeZeroTime);

    pChar = (char *) &autoZeroTime;
    for (unsigned int i = 0; i < sizeof(autoZeroTime); i++)
    {
        *pChar++ = buffer[j++];
    }
    autoZeroTime = be16toh(autoZeroTime);

    pChar = (char *) &rateOptionLimit;
    for (unsigned int i = 0; i < sizeof(rateOptionLimit); i++)
    {
        *pChar++ = buffer[j++];
    }
    rateOptionLimit = be16toh(rateOptionLimit);

    pChar = (char *) &zeroReferenceLimit;
    for (unsigned int i = 0; i < sizeof(zeroReferenceLimit); i++)
    {
        *pChar++ = buffer[j++];
    }
    zeroReferenceLimit = be16toh(zeroReferenceLimit);

    pChar = (char *) &calibrationReferenceLimit;
    for (unsigned int i = 0; i < sizeof(calibrationReferenceLimit); i++)
    {
        *pChar++ = buffer[j++];
    }
    calibrationReferenceLimit = be16toh(calibrationReferenceLimit);

    pChar = (char *) &smallMotionLimit;
    for (unsigned int i = 0; i < sizeof(smallMotionLimit); i++)
    {
        *pChar++ = buffer[j++];
    }
    smallMotionLimit = be16toh(smallMotionLimit);

    pChar = (char *) &largeMotionLimit;
    for (unsigned int i = 0; i < sizeof(largeMotionLimit); i++)
    {
        *pChar++ = buffer[j++];
    }
    largeMotionLimit = be16toh(largeMotionLimit);

    pChar = (char *) &largeMotionCount;
    for (unsigned int i = 0; i < sizeof(largeMotionCount); i++)
    {
        *pChar++ = buffer[j++];
    }
    largeMotionCount = be16toh(largeMotionCount);

    pChar = (char *) &smallMotionCount;
    for (unsigned int i = 0; i < sizeof(smallMotionCount); i++)
    {
        *pChar++ = buffer[j++];
    }
    smallMotionCount = be16toh(smallMotionCount);

    pChar = (char *) &noMotionCount;
    for (unsigned int i = 0; i < sizeof(noMotionCount); i++)
    {
        *pChar++ = buffer[j++];
    }
    noMotionCount = be16toh(noMotionCount);

    pChar = (char *) &zeroTrackingOption;
    for (unsigned int i = 0; i < sizeof(zeroTrackingOption); i++)
    {
        *pChar++ = buffer[j++];
    }
    zeroTrackingOption = be16toh(zeroTrackingOption);

    pChar = (char *) &negativeWeightOption;
    for (unsigned int i = 0; i < sizeof(negativeWeightOption); i++)
    {
        *pChar++ = buffer[j++];
    }
    negativeWeightOption = be16toh(negativeWeightOption);

    pChar = (char *) &powerupMaintenanceZoneLimit;
    for (unsigned int i = 0; i < sizeof(powerupMaintenanceZoneLimit); i++)
    {
        *pChar++ = buffer[j++];
    }
    powerupMaintenanceZoneLimit = be16toh(powerupMaintenanceZoneLimit);

    pChar = (char *) &operateMaintenanceZoneLimit;
    for (unsigned int i = 0; i < sizeof(operateMaintenanceZoneLimit); i++)
    {
        *pChar++ = buffer[j++];
    }
    operateMaintenanceZoneLimit = be16toh(operateMaintenanceZoneLimit);

    pChar = (char *) &filterSpeed;
    for (unsigned int i = 0; i < sizeof(filterSpeed); i++)
    {
        *pChar++ = buffer[j++];
    }
    //single byte so no endian flip

    pChar = (char *) &referenceVoltage;
    for (unsigned int i = 0; i < sizeof(referenceVoltage); i++)
    {
        *pChar++ = buffer[j++];
    }
    //single byte so no endian flip

    pChar = (char *) &lastCalibrationDate;
    for (unsigned int i = 0; i < sizeof(lastCalibrationDate); i++)
    {
        *pChar++ = buffer[j++];
    }
    lastCalibrationDate = be32toh(lastCalibrationDate);

    pChar = (char *) &numberOfCalibrations;
    for (unsigned int i = 0; i < sizeof(numberOfCalibrations); i++)
    {
        *pChar++ = buffer[j++];
    }
    numberOfCalibrations = be16toh(numberOfCalibrations);

    pChar = (char *) &lastConfigurationDate;
    for (unsigned int i = 0; i < sizeof(lastConfigurationDate); i++)
    {
        *pChar++ = buffer[j++];
    }
    lastConfigurationDate = be32toh(lastConfigurationDate);

    pChar = (char *) &numberOfConfigurations;
    for (unsigned int i = 0; i < sizeof(numberOfConfigurations); i++)
    {
        *pChar++ = buffer[j++];
    }
    numberOfConfigurations = be16toh(numberOfConfigurations);

    for (unsigned int i = 0; i < NTEP_CCSIZE; i++)
    {
        ntep_cc[i] = buffer[j++];
    }

    pChar = (char *) &calibrationGainCoefficient;
    for (unsigned int i = 0; i < sizeof(calibrationGainCoefficient); i++)
    {
        *pChar++ = buffer[j++];
    }
    calibrationGainCoefficient = be32toh(calibrationGainCoefficient);

    pChar = (char *) &calibrationOffsetCoefficient;
    for (unsigned int i = 0; i < sizeof(calibrationOffsetCoefficient); i++)
    {
        *pChar++ = buffer[j++];
    }
    calibrationOffsetCoefficient = be32toh(calibrationOffsetCoefficient);

    pChar = (char *) &weigherType;
    for (unsigned int i = 0; i < sizeof(weigherType); i++)
    {
        *pChar++ = buffer[j++];
    }
    //single byte so no endian flip

    pChar = (char *) &loadCellId;
    for (unsigned int i = 0; i < sizeof(loadCellId); i++)
    {
        *pChar++ = buffer[j++];
    }
    //single byte so no endian flip

    pChar = (char *) &flags;
    for (unsigned int i = 0; i < sizeof(flags); i++)
    {
        *pChar++ = buffer[j++];
    }
    //single byte so no endian flip

    pChar = (char *) &minWeightToPrint;
    for (unsigned int i = 0; i < sizeof(minWeightToPrint); i++)
    {
        *pChar++ = buffer[j++];
    }
    minWeightToPrint = be32toh(minWeightToPrint);

    pChar = (char *) &model;
    for (unsigned int i = 0; i < sizeof(model); i++)
    {
        *pChar++ = buffer[j++];
    }

    // The flags field is used to indicate the weigh mode and the NetWeightDualRangeLimitCheck flag.
    // weighMode_Metric = 0x01
    // netWeightDualRangeLimitCheck = 0x02
    mode = static_cast<WeighMode>(flags & WeighMode_Metric);
}

WeigherConfiguration::WeigherConfiguration( const WeigherConfiguration & config)
{
        setDeviceSignature(config.getDeviceSignature());
        centerOfMaintenanceZone = config.getCenterOfMaintenanceZone();
        scaleFactor = config.getScaleFactor();
        maxWeight = config.getMaxWeight();
        gainFactor = config.getGainFactor();
        zeroReference = config.getZeroReference();
        calibrationReference = config.getCalibrationReference();
        rateOptionTime = config.getRateOptionLimit();
        autoZeroMaintenanceLimit = config.getAutoZeroMaintenanceLimit();
        prepackMotionCount = config.getPrepackMotionCount();
        initializeZeroTime = config.getInitializeZeroTime();
        autoZeroTime = config.getAutoZeroTime();
        rateOptionLimit = config.getRateOptionLimit();
        zeroReferenceLimit = config.getZeroReferenceLimit();
        calibrationReferenceLimit = config.getCalibrationReferenceLimit();
        smallMotionLimit = config.getSmallMotionLimit();
        largeMotionLimit = config.getLargeMotionLimit();
        largeMotionCount = config.getLargeMotionCount();
        smallMotionCount = config.getSmallMotionCount();
        noMotionCount = config.getNoMotionCount();
        zeroTrackingOption = config.getZeroTrackingOption();
        negativeWeightOption = config.getNegativeWeightOption();
        powerupMaintenanceZoneLimit = config.getPowerupMaintenanceZoneLimit();
        operateMaintenanceZoneLimit = config.getOperateMaintenanceZoneLimit();
        filterSpeed = config.getFilterSpeed();
        referenceVoltage = config.getReferenceVoltage();
        lastCalibrationDate = config.getLastCalibrationDate();
        numberOfCalibrations = config.getNumberOfCalibrations();
        lastConfigurationDate = config.getLastConfigurationDate();
        numberOfConfigurations = config.getNumberOfConfigurations();
        setNtep_cc( config.getNtep_cc());
        calibrationGainCoefficient = config.getCalibrationGainCoefficent();
        calibrationOffsetCoefficient = config.getCalibrationOffsetCoefficent();
        weigherType = config.getWeigherType();
        loadCellId = config.getLoadCellID();
        flags = config.getFlags();
        minWeightToPrint = config.getMinWeightToPrint();
        model = (WeigherModel) config.getWeigherModel();

}

WeigherConfiguration::~WeigherConfiguration()
{
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
}

void WeigherConfiguration::setConfigType(WgConfigType configType)
{
    this->configType = configType;
}

WgConfigType WeigherConfiguration::getConfigType() const
{
    return configType;
}

uint8_t * WeigherConfiguration::getDeviceSignature() const
{
    unsigned char * sig = new unsigned char[sizeof(ntep_cc)];
    for(unsigned int i = 0; i < sizeof(deviceSignature); i++){
        sig[i] = deviceSignature[i];
    }
    return sig;
}

uint8_t *WeigherConfiguration::getNtep_cc() const
{
    unsigned char * sig = new unsigned char[sizeof(ntep_cc)];
    for(unsigned int i = 0; i < sizeof(ntep_cc); i++){
        sig[i] = ntep_cc[i];
    }
    return sig;
}

int32_t WeigherConfiguration::getCenterOfMaintenanceZone() const
{
    return centerOfMaintenanceZone;
}

int32_t WeigherConfiguration::getScaleFactor() const
{
    return scaleFactor;
}

int32_t WeigherConfiguration::getMaxWeight() const
{
    return maxWeight;
}

int32_t WeigherConfiguration::getGainFactor() const
{
    return gainFactor;
}

int32_t WeigherConfiguration::getZeroReference() const
{
    return zeroReference;
}

int32_t WeigherConfiguration::getCalibrationReference() const
{
    return calibrationReference;
}

uint16_t WeigherConfiguration::getRateOptionTime() const
{
    return rateOptionTime;
}

uint16_t WeigherConfiguration::getAutoZeroMaintenanceLimit() const
{
    return autoZeroMaintenanceLimit;
}

uint16_t WeigherConfiguration::getPrepackMotionCount() const
{
    return prepackMotionCount;
}

uint16_t WeigherConfiguration::getInitializeZeroTime() const
{
    return initializeZeroTime;
}

uint16_t WeigherConfiguration::getAutoZeroTime() const
{
    return autoZeroTime;
}

uint16_t WeigherConfiguration::getRateOptionLimit() const
{
    return rateOptionLimit;
}

uint16_t WeigherConfiguration::getZeroReferenceLimit() const
{
    return zeroReferenceLimit;
}

uint16_t WeigherConfiguration::getCalibrationReferenceLimit() const
{
    return calibrationReferenceLimit;
}

uint16_t WeigherConfiguration::getSmallMotionLimit() const
{
    return  smallMotionLimit;
}

uint16_t WeigherConfiguration::getLargeMotionLimit() const
{
    return largeMotionLimit;
}

uint16_t WeigherConfiguration::getLargeMotionCount() const
{
    return largeMotionCount;
}

uint16_t WeigherConfiguration::getSmallMotionCount() const
{
    return smallMotionCount;
}

uint16_t WeigherConfiguration::getNoMotionCount() const
{
    return noMotionCount;
}

uint16_t WeigherConfiguration::getZeroTrackingOption() const
{
    return zeroTrackingOption;
}

uint16_t WeigherConfiguration::getNegativeWeightOption() const
{
    return negativeWeightOption;
}

uint16_t WeigherConfiguration::getPowerupMaintenanceZoneLimit() const
{
    return powerupMaintenanceZoneLimit;
}

uint16_t WeigherConfiguration::getOperateMaintenanceZoneLimit() const
{
    return operateMaintenanceZoneLimit;
}

WgFilterSpeed WeigherConfiguration::getFilterSpeed() const
{
    return (WgFilterSpeed) filterSpeed;
}

uint8_t WeigherConfiguration::getReferenceVoltage() const
{
    return referenceVoltage;
}

uint32_t WeigherConfiguration::getLastCalibrationDate() const
{
    return lastCalibrationDate;
}

uint16_t WeigherConfiguration::getNumberOfCalibrations() const
{
    return numberOfCalibrations;
}

uint32_t WeigherConfiguration::getLastConfigurationDate() const
{
    return lastConfigurationDate;
}

uint16_t WeigherConfiguration::getNumberOfConfigurations() const
{
    return numberOfConfigurations;
}

uint32_t WeigherConfiguration::getCalibrationGainCoefficent() const
{
    return calibrationGainCoefficient;
}

WeigherType WeigherConfiguration::getWeigherType() const
{
    return weigherType;
}

uint32_t WeigherConfiguration::getMinWeightToPrint() const
{
    return minWeightToPrint;
}

uint8_t WeigherConfiguration::getWeigherModel() const
{
    return model;
}

uint8_t WeigherConfiguration::getFlags() const
{
    return flags;
}

uint8_t WeigherConfiguration::getScaleWeighMode() const
{
    return mode;
}

uint32_t WeigherConfiguration::getBufferLen() const
{
    return bufferLen;
}

uint32_t WeigherConfiguration::getCalibrationOffsetCoefficent() const
{
    return calibrationOffsetCoefficient;
}

uint8_t WeigherConfiguration::getLoadCellID() const
{
    return loadCellId;
}

void WeigherConfiguration::setDeviceSignature(uint8_t *deviceSignature)
{
    for (int i = 0; i < SystemDevSignatureSize; i++)
    {
        this->deviceSignature[i] = deviceSignature[i];
    }
}

void WeigherConfiguration::setNtep_cc(uint8_t *ntep_cc)
{
    for (int i = 0; i < NTEP_CCSIZE; i++)
    {
        this->ntep_cc[i] = ntep_cc[i];
    }
}

void WeigherConfiguration::setFlags(uint8_t flags)
{
    this->flags = flags;
}

void WeigherConfiguration::setScaleWeighMode(WeighMode mode)
{
    this->mode = mode;
}

void WeigherConfiguration::setMinWeightToPrint(int32_t minWeightToPrint)
{
    this->minWeightToPrint = minWeightToPrint;
}

void WeigherConfiguration::setWeigherModel(WeigherModel model)
{
    this->model = model;
}

WeigherType WeigherConfiguration::getWeigherType()
{
    return this->weigherType;
}


void WeigherConfiguration::setCenterOfMaintenanceZone(int32_t centerOfMaintenanceZone)
{
    this->centerOfMaintenanceZone = centerOfMaintenanceZone;
}

void WeigherConfiguration::setScaleFactor(int32_t scaleFactor)
{
    this->scaleFactor = scaleFactor;
}

void WeigherConfiguration::setMaxWeight(int32_t maxWeight)
{
    this->maxWeight = maxWeight;
}

void WeigherConfiguration::setGainFactor(int32_t gainFactor)
{
    this->gainFactor = gainFactor;
}

void WeigherConfiguration::setZeroReference(int32_t zeroReference)
{
    this->zeroReference = zeroReference;
}

void WeigherConfiguration::setCalibrationReference(int32_t calibrationReference)
{
    this->calibrationReference = calibrationReference;
}

void WeigherConfiguration::setRateOptionTime(uint16_t rateOptionTime)
{
    this->rateOptionTime = rateOptionTime;
}

void WeigherConfiguration::setAutoZeroMaintenanceLimit(uint16_t autoZeroMaintenanceLimit)
{
    this->autoZeroMaintenanceLimit = autoZeroMaintenanceLimit;
}

void WeigherConfiguration::setPrepackMotionCount(uint16_t prepackMotionCount)
{
    this->prepackMotionCount = prepackMotionCount;
}

void WeigherConfiguration::setInitializeZeroTime(uint16_t initializeZeroTime)
{
    this->initializeZeroTime = initializeZeroTime;
}

void WeigherConfiguration::setAutoZeroTime(uint16_t autoZeroTime)
{
    this->autoZeroTime = autoZeroTime;
}

void WeigherConfiguration::setRateOptionLimit(uint16_t rateOptionLimit)
{
    this->rateOptionLimit = rateOptionLimit;
}

void WeigherConfiguration::setZeroReferenceLimit(uint16_t zeroReferenceLimit)
{
    this->zeroReferenceLimit = zeroReferenceLimit;
}

void WeigherConfiguration::setCalibrationReferenceLimit(uint16_t calibrationReferenceLimit)
{
    this->calibrationReferenceLimit = calibrationReferenceLimit;
}

void WeigherConfiguration::setSmallMotionLimit(uint16_t smallMotionLimit)
{
    this->smallMotionLimit = smallMotionLimit;
}

void WeigherConfiguration::setLargeMotionLimit(uint16_t largeMotionLimit)
{
    this->largeMotionLimit = largeMotionLimit;
}

void WeigherConfiguration::setLargeMotionCount(uint16_t largeMotionCount)
{
    this->largeMotionCount = largeMotionCount;
}

void WeigherConfiguration::setSmallMotionCount(uint16_t smallMotionCount)
{
    this->smallMotionCount = smallMotionCount;
}

void WeigherConfiguration::setNoMotionCount(uint16_t noMotionCount)
{
    this->noMotionCount = noMotionCount;
}

void WeigherConfiguration::setZeroTrackingOption(uint16_t zeroTrackingOption)
{
    this->zeroTrackingOption = zeroTrackingOption;
}

void WeigherConfiguration::setNegativeWeightOption(uint16_t negativeWeightOption)
{
    this->negativeWeightOption = negativeWeightOption;
}

void WeigherConfiguration::setPowerupMaintenanceZoneLimit(uint16_t powerupMaintenanceZoneLimit)
{
    this->powerupMaintenanceZoneLimit = powerupMaintenanceZoneLimit;
}

void WeigherConfiguration::setOperateMaintenanceZoneLimit(uint16_t operateMaintenanceZoneLimit)
{
    this->operateMaintenanceZoneLimit = operateMaintenanceZoneLimit;
}

void WeigherConfiguration::setFilterSpeed(WgFilterSpeed filterSpeed)
{
    this->filterSpeed = filterSpeed;
}

void WeigherConfiguration::setReferenceVoltage(uint8_t referenceVoltage)
{
    this->referenceVoltage = referenceVoltage;
}

void WeigherConfiguration::setLastCalibrationDate(uint32_t lastCalibrationDate)
{
    this->lastCalibrationDate = lastCalibrationDate;
}

void WeigherConfiguration::setNumberOfCalibrations(uint16_t numberOfCalibrations)
{
    this->numberOfCalibrations = numberOfCalibrations;
}

void WeigherConfiguration::setLastConfigurationDate(uint32_t lastConfigurationDate)
{
    this->lastConfigurationDate = lastConfigurationDate;
}

void WeigherConfiguration::setNumberOfConfigurations(uint16_t numberOfConfigurations)
{
    this->numberOfConfigurations = numberOfConfigurations;
}

void WeigherConfiguration::setWeigherType(WeigherType weigherType)
{
    this->weigherType = weigherType;
}

unsigned char * WeigherConfiguration::toWgConfigMessage()
{
    unsigned char * buffer = new unsigned char[CONFIGMESSAGESIZE]; //
    unsigned int index = 0;
    uint32_t temp32 = 0; //for flipping endianess 32bit uint
    uint16_t temp16 = 0; //for flipping endianess 16bit uint

    //add header and padding
    buffer[index++] = hobartWeigher::wgWriteCfg;
    buffer[index++] = 0x00;
    buffer[index++] = 0x00;
    buffer[index++] = 0x00;

    //add weigher config
    for (unsigned int i = 0; i < SystemDevSignatureSize; i++)
    {
        buffer[index++] = deviceSignature[i];
    }

    temp32 = htobe32(centerOfMaintenanceZone);
    char *pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp32 = htobe32(scaleFactor);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp32 = htobe32(maxWeight);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp32 = htobe32(gainFactor);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp32 = htobe32(zeroReference);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp32 = htobe32(calibrationReference);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(rateOptionTime);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(autoZeroMaintenanceLimit);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(prepackMotionCount);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(initializeZeroTime);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(autoZeroTime);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(rateOptionLimit);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(zeroReferenceLimit);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(calibrationReferenceLimit);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(smallMotionLimit);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(largeMotionLimit);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(largeMotionCount);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(smallMotionCount);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(noMotionCount);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(zeroTrackingOption);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(negativeWeightOption);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(powerupMaintenanceZoneLimit);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(operateMaintenanceZoneLimit);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    pChar = (char *) &filterSpeed;
    for (unsigned int i = 0; i < sizeof(filterSpeed); i++)
    {
        buffer[index++] = *pChar++;
    }

    pChar = (char *) &referenceVoltage;
    for (unsigned int i = 0; i < sizeof(filterSpeed); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp32 = htobe32(lastCalibrationDate);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(numberOfCalibrations);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp32 = htobe32(lastConfigurationDate);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp16 = htobe16(numberOfConfigurations);
    pChar = (char *) &temp16;
    for (unsigned int i = 0; i < sizeof(temp16); i++)
    {
        buffer[index++] = *pChar++;
    }

    for (unsigned int i = 0; i < NTEP_CCSIZE; i++)
    {
        buffer[index++] = ntep_cc[i];
    }

    temp32 = htobe32(calibrationGainCoefficient);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp32 = htobe32(calibrationOffsetCoefficient);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    pChar = (char *) &weigherType;
    for (unsigned int i = 0; i < sizeof(weigherType); i++)
    {
        buffer[index++] = *pChar++;
    }

    pChar = (char *) &loadCellId;
    for (unsigned int i = 0; i < sizeof(loadCellId); i++)
    {
        buffer[index++] = *pChar++;
    }

    pChar = (char *) &flags;
    for (unsigned int i = 0; i < sizeof(flags); i++)
    {
        buffer[index++] = *pChar++;
    }

    temp32 = htobe32(minWeightToPrint);
    pChar = (char *) &temp32;
    for (unsigned int i = 0; i < sizeof(temp32); i++)
    {
        buffer[index++] = *pChar++;
    }

    pChar = (char *) &model;
    for (unsigned int i = 0; i < sizeof(model); i++)
    {
       buffer[index++] = *pChar++;
    }

    return buffer;
}



