#ifndef WEIGHERCONFIGURATION_H
#define WEIGHERCONFIGURATION_H
#include <QObject>
#include <stdint.h>
#include <QMetaType>

#define SystemDevSignatureSize 6
#define SystemPartNumberSize 10
#define NTEP_CCSIZE 8
#define CONFIGMESSAGESIZE 106
#define WEIGHMODEMETRIC 0x01

typedef enum StatusFlags : uint16_t
{
SMALL_MOTION                           =0x0001,
LARGE_MOTION                           =0x0002,
NEGATIVE_GROSS_WEIGHT                  =0x0004,
WITHIN_QUARTER_GRADUATION_OF_ZERO      =0x0008,
OVER_GROSS_WEIGHT_LIMIT                =0x0010,
ZERO_OUT_OF_MAINTENANCE_ZONE           =0x0020,
SCALE_WARMUP                           =0x0040,
NEW_ZERO                               =0x0080,
WEIGHER_CALIBRATION_FAILED             =0x0100,
WEIGHER_EEP_READ_FAILED                =0x0200,
WEIGHER_DISABLED                       =0x0400,
WEIGHER_KEY_PRESSED                    =0x0800
} StatusFlags_t;

typedef enum WeighingMode : uint8_t
{
    Prepack,
    WeightTracking,
    Dumb,
    Manual
} WeighingMode_t;

typedef enum WeigherType : uint8_t
{
    FirstWeigher,
    SecondWeigher
} WeigherType_t;

typedef enum WeighMode : uint8_t
{
    WeighMode_Avoir = 0,
    WeighMode_Metric = 1
}   WeighMode_t;

typedef enum WeigherModel :uint8_t
{
    WmNone,
    WmESS,			//	was WmEVOSS
    WmU2000S1,
    WmU2000S3,
    WmUWS,
    WmQuantHC,		//	was WmEVOHC
    WmESC,
    WmEPSA,
    WmHLX1,
    WmEzUSB,		//	Cypress-based Ez-USB weigher for the AWS
    WmAWS,			//	Atmel-based USB weigher for the AWS, AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
    WmEPSA2,  		//	Atmel-based USB weigher for the AWS, AWS Weighing Device, Non-Dual-Range Load Cell (Atmel-based SAM-BA device) (haberrc)
    WmAWS2,		  	//	Atmel-based USB weigher for the AWS, AWS Weighing Device, Non-Dual-Range Load Cell (Atmel-based SAM-BA device) (haberrc)
    WmEPSAC2,		//	New USB Compact Label Applier
    WmSASCALE1,		//  SASCALE-1 for SASCALE
    WmSASCALE2,		//  SASCALE-2 for SASCALE
    endWeigherModel	// KEEP THIS LAST IN THE LIST
}   WeigherModel_t;

enum WeigherModels
{
    None = 0,	//	NS means these models are not supported in the HT series
    ESS,		//	NS
    U2000S1,	//	NS
    U2000S3,	//	NS
    UWS,		//	NS
    QuantHC,	//	NS
    ESC,		//	NS
    EPSA,		//	NS
    HLX1,       //  8  0.005 dual range
    EzUSB,		//	NS
    AWS,        //  10 0.005 dual range
    EPSA2,      //  11 0.01 single range
    AWS2,       //  12 0.01 single range
    EPSAC2,     //  New USB Compact Label Applier
    SASCALE1,   //  SASCALE-1 for SASCALE
    SASCALE2    //  SASCALE-2 for SASCALE: dual range
};

typedef enum WgFilterSpeed  : uint8_t
{
    NormalFilterSpeed,
    SlowFilterSpeed,
    FastFilterSpeed,
} WgFilterSpeed_t;

typedef enum WgConfigType
{
    WgPermanent = 0x0000,
    WgTemporary = 0x0001,
    WgDefault = 0x0002,
    // Bit definition used in the CONFIG message disposition to
    // indicate that weigher calibration is in progress.
    WgCalibInProgress = 0x8000
}   WgConfigType_t;


class WeigherConfiguration
{
public:
		WeigherConfiguration();
		WeigherConfiguration(const char *buffer);
        WeigherConfiguration(const WeigherConfiguration &config);
        

		virtual ~WeigherConfiguration();
        
        void setConfigType(WgConfigType configType);
        WgConfigType getConfigType() const;
        uint8_t * getDeviceSignature() const;
        uint8_t * getNtep_cc() const;

        int32_t getCenterOfMaintenanceZone() const;
        int32_t getScaleFactor() const;
        int32_t getMaxWeight() const;
        int32_t getGainFactor() const;
        int32_t getZeroReference() const;
        int32_t getCalibrationReference() const;
        uint16_t getRateOptionTime() const;
        uint16_t getAutoZeroMaintenanceLimit() const;
        uint16_t getPrepackMotionCount() const;
        uint16_t getInitializeZeroTime() const;
        uint16_t getAutoZeroTime() const;
        uint16_t getRateOptionLimit() const;
        uint16_t getZeroReferenceLimit() const;
        uint16_t getCalibrationReferenceLimit() const;
        uint16_t getSmallMotionLimit() const;
        uint16_t getLargeMotionLimit() const;
        uint16_t getLargeMotionCount() const;
        uint16_t getSmallMotionCount() const;
        uint16_t getNoMotionCount() const;
        uint16_t getZeroTrackingOption() const;
        uint16_t getNegativeWeightOption() const;
        uint16_t getPowerupMaintenanceZoneLimit() const;
        uint16_t getOperateMaintenanceZoneLimit() const;
        WgFilterSpeed getFilterSpeed() const;
        uint8_t getReferenceVoltage() const;
        uint32_t getLastCalibrationDate() const;
        uint16_t getNumberOfCalibrations() const;
        uint32_t getLastConfigurationDate() const;
        uint16_t getNumberOfConfigurations() const;
        uint8_t getLoadCellID() const;
        uint32_t getCalibrationOffsetCoefficent() const;
        uint32_t getCalibrationGainCoefficent() const;
        WeigherType getWeigherType() const;
        uint32_t getMinWeightToPrint() const;
        uint8_t getWeigherModel() const;
        uint8_t getFlags() const;
        uint8_t getScaleWeighMode() const;
        uint32_t getBufferLen() const;
        bool isScaleMetric() const;
        bool isScaleAvoir() const;

        bool isNetWeightDualRangeLimitCheck() const;

        void clear();
        void setDeviceSignature(uint8_t *deviceSignature);
        void setCenterOfMaintenanceZone(int32_t centerOfMaintenanceZone);
        void setScaleFactor(int32_t scaleFactor);
        void setMaxWeight(int32_t maxWeight);
        void setGainFactor(int32_t gainFactor);
        void setZeroReference(int32_t zeroReference);
        void setCalibrationReference(int32_t calibrationReference);
        void setRateOptionTime(uint16_t rateOptionTime);
        void setAutoZeroMaintenanceLimit(uint16_t autoZeroMaintenanceLimit);
        void setPrepackMotionCount(uint16_t prepackMotionCount);
        void setInitializeZeroTime(uint16_t initializeZeroTime);
        void setAutoZeroTime(uint16_t autoZeroTime);
        void setRateOptionLimit(uint16_t rateOptionLimit);
        void setZeroReferenceLimit(uint16_t zeroReferenceLimit);
        void setCalibrationReferenceLimit(uint16_t calibrationReferenceLimit);
        void setSmallMotionLimit(uint16_t smallMotionLimit);
        void setLargeMotionLimit(uint16_t largeMotionLimit);
        void setLargeMotionCount(uint16_t largeMotionCount);
        void setSmallMotionCount(uint16_t smallMotionCount);
        void setNoMotionCount(uint16_t noMotionCount);
        void setZeroTrackingOption(uint16_t zeroTrackingOption);
        void setNegativeWeightOption(uint16_t negativeWeightOption);
        void setPowerupMaintenanceZoneLimit(uint16_t powerupMaintenanceZoneLimit);
        void setOperateMaintenanceZoneLimit(uint16_t operateMaintenanceZoneLimit);
        void setFilterSpeed(WgFilterSpeed filterSpeed);
        void setReferenceVoltage(uint8_t referenceVoltage);
        void setLastCalibrationDate(uint32_t lastCalibrationDate);
        void setNumberOfCalibrations(uint16_t numberOfCalibrations);
        void setLastConfigurationDate(uint32_t lastConfigurationDate);
        void setNumberOfConfigurations(uint16_t numberOfConfigurations);
        void setNtep_cc(uint8_t * ntep_cc);

        void setFlags(uint8_t flags);

        void setScaleWeighMode(WeighMode mode);
        void setMinWeightToPrint(int32_t minWeightToPrint);
        void setWeigherModel(WeigherModel model);


        WeigherType getWeigherType();
        void setWeigherType(WeigherType weigherType);

        unsigned char* toWgConfigMessage();
        
private:

		// How to update the configuration on the periferal
		WgConfigType configType;

		// weigher configuration follows.
        uint8_t deviceSignature[SystemDevSignatureSize];
        int32_t centerOfMaintenanceZone;
        int32_t scaleFactor;
        int32_t maxWeight;
        int32_t gainFactor;
        int32_t zeroReference;
        int32_t calibrationReference;
        uint16_t rateOptionTime;
        uint16_t autoZeroMaintenanceLimit;
        uint16_t prepackMotionCount;
        uint16_t initializeZeroTime;
        uint16_t autoZeroTime;
        uint16_t rateOptionLimit;
        uint16_t zeroReferenceLimit;
        uint16_t calibrationReferenceLimit;
        uint16_t smallMotionLimit;
        uint16_t largeMotionLimit;
        uint16_t largeMotionCount;
        uint16_t smallMotionCount;
        uint16_t noMotionCount;
        uint16_t zeroTrackingOption;
        uint16_t negativeWeightOption;
        uint16_t powerupMaintenanceZoneLimit;
        uint16_t operateMaintenanceZoneLimit;
        WgFilterSpeed filterSpeed;
        uint8_t referenceVoltage;
        uint32_t lastCalibrationDate;
        uint16_t numberOfCalibrations;
        uint32_t lastConfigurationDate;
        uint16_t numberOfConfigurations;
        uint8_t ntep_cc[NTEP_CCSIZE];
        uint32_t calibrationGainCoefficient;    /* value for gain reg */
        uint32_t calibrationOffsetCoefficient;
        WeigherType weigherType;
        uint8_t loadCellId;

		// The flags field is used to indicate the weigh mode and the NetWeightDualRangeLimitCheck flag.
		// weighMode_Metric = 0x01
		// netWeightDualRangeLimitCheck = 0x02
        uint8_t flags;
        int32_t minWeightToPrint;
        WeigherModel model;
        WeighMode mode;
        uint32_t bufferLen = CONFIGMESSAGESIZE;

};

#endif // WEIGHERCONFIGURATION_H
