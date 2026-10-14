//#include "vxWorks.h"
#include "WeigherStatus.h"
#include "WeigherEvent.h"

#include "Configuration.h"

#include "Debug.h"
#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WeigherStatus::dataType = 0;
std::string WeigherStatus::dataTypeName("WeigherStatus"); 
#endif

WeigherStatus::WeigherStatus(const WeigherStatus& newWS) : EvolMessage(newWS)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	statusBits = newWS.getStatusBits();
	
	//PTR #750 -> metric weights being cut in 1/2 here on tare change ( getCounts() divides by 2 for metric )
	counts = newWS.counts;
	
	rawCounts = newWS.getRawCounts();

	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	FilteredUncalibratedCounts = newWS.getFilteredUncalibratedCounts();
	ADCounts = newWS.getADCounts();

	accelX = newWS.getAccelX();
	accelY = newWS.getAccelY();
	accelZ = newWS.getAccelZ();

	tiltX = newWS.getTiltX();
	tiltY = newWS.getTiltY();

	locked = newWS.locked;

	peripheralComm = newWS.isPeripheralComm();
	managerDeleted = newWS.isManagerDeleted();

	scaleFactor = newWS.scaleFactor;
	dualRangeEnabled = newWS.dualRangeEnabled;
	dualRangeDirection = newWS.dualRangeDirection;

	weigherConfigOutOfRange = newWS.isWeigherConfigOutOfRange();
}

WeigherStatus::WeigherStatus(Hobart::CEvolMsgReceiver *pHobThread, UINT16 statusBits, UINT32 rawCounts, UINT32 counts) : EvolMessage(Weigher, WgStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->statusBits = statusBits;
	this->rawCounts = rawCounts;
	this->counts = counts;

	scaleFactor = 0;

	dualRangeEnabled = false;
	dualRangeDirection = DualRangeUp;

	locked = false;
	peripheralComm = true;
	managerDeleted = false;

	weigherConfigOutOfRange = false;

    FilteredUncalibratedCounts = 0;
    ADCounts = 0;

	accelX = 0;
	accelY = 0;
	accelZ = 0;

	tiltX = 0;
	tiltY = 0;
}

WeigherStatus::WeigherStatus(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &statusBits;
	for (unsigned int i = 0; i < sizeof(statusBits); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &counts;
	for (unsigned int i = 0; i < sizeof(counts); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rawCounts;
	for (unsigned int i = 0; i < sizeof(rawCounts); i++)
	{
		*pChar++ = buffer[j++];
	}

	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	pChar = (char *) &FilteredUncalibratedCounts;
	for (unsigned int i = 0; i < sizeof(FilteredUncalibratedCounts); i++)
	{
		*pChar++ = buffer[j++];
	}

	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	pChar = (char *) &ADCounts;
	for (unsigned int i = 0; i < sizeof(ADCounts); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &scaleFactor;
	for (unsigned int i = 0; i < sizeof(scaleFactor); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &dualRangeEnabled;
	for (unsigned int i = 0; i < sizeof(dualRangeEnabled); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &dualRangeDirection;
	for (unsigned int i = 0; i < sizeof(dualRangeDirection); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &locked;
	for (unsigned int i = 0; i < sizeof(locked); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &peripheralComm;
	for (unsigned int i = 0; i < sizeof(peripheralComm); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &managerDeleted;
	for (unsigned int i = 0; i < sizeof(managerDeleted); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &weigherConfigOutOfRange;
	for (unsigned int i = 0; i < sizeof(weigherConfigOutOfRange); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &accelX;
	for (unsigned int i = 0; i < sizeof(accelX); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &accelY;
	for (unsigned int i = 0; i < sizeof(accelY); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &accelZ;
	for (unsigned int i = 0; i < sizeof(accelZ); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &tiltX;
	for (unsigned int i = 0; i < sizeof(tiltX); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &tiltY;
	for (unsigned int i = 0; i < sizeof(tiltY); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherStatus::WeigherStatus() : EvolMessage()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	statusBits = 0;
	counts = 0;
	rawCounts = 0;

	locked = false;
	peripheralComm = false;
	managerDeleted = false;

	scaleFactor = 0;

	dualRangeEnabled = false;
	dualRangeDirection = DualRangeUp;

	weigherConfigOutOfRange = false;

	FilteredUncalibratedCounts = 0;
	ADCounts = 0;

	accelX = 0;
	accelY = 0;
	accelZ = 0;

	tiltX = 0;
	tiltY = 0;
}

WeigherStatus::~WeigherStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WeigherStatus::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &statusBits;
	for (unsigned int i = 0; i < sizeof(statusBits); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &counts;
	for (unsigned int i = 0; i < sizeof(counts); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &rawCounts;
	for (unsigned int i = 0; i < sizeof(rawCounts); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	pChar = (char *) &FilteredUncalibratedCounts;
	for (unsigned int i = 0; i < sizeof(FilteredUncalibratedCounts); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
	pChar = (char *) &ADCounts;
	for (unsigned int i = 0; i < sizeof(ADCounts); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &scaleFactor;
	for (unsigned int i = 0; i < sizeof(scaleFactor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &dualRangeEnabled;
	for (unsigned int i = 0; i < sizeof(dualRangeEnabled); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &dualRangeDirection;
	for (unsigned int i = 0; i < sizeof(dualRangeDirection); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &locked;
	for (unsigned int i = 0; i < sizeof(locked); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &peripheralComm;
	for (unsigned int i = 0; i < sizeof(peripheralComm); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &managerDeleted;
	for (unsigned int i = 0; i < sizeof(managerDeleted); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &weigherConfigOutOfRange;
	for (unsigned int i = 0; i < sizeof(weigherConfigOutOfRange); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &accelX;
	for (unsigned int i = 0; i < sizeof(accelX); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &accelY;
	for (unsigned int i = 0; i < sizeof(accelY); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &accelZ;
	for (unsigned int i = 0; i < sizeof(accelZ); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &tiltX;
	for (unsigned int i = 0; i < sizeof(tiltX); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &tiltY;
	for (unsigned int i = 0; i < sizeof(tiltY); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WeigherStatus::show() const
{
	const bool isTracing( Debug::isTracing("WeigherStatus") );

	if ( !isTracing )
		return;

	Debug::trace("WeigherStatus", "WeigherStatus: ");

	EvolMessage::show();

	Debug::trace("WeigherStatus", HobString("rawCounts: ") +
								  HobString(Integer::toString(getRawCounts())));
	Debug::trace("WeigherStatus", HobString("counts: ") +
								  HobString(Integer::toString(getCounts())));
	Debug::trace("WeigherStatus", HobString("ADCounts: ") +
								  HobString(Integer::toString(getADCounts())));
	Debug::trace("WeigherStatus", HobString("FilteredUncalibratedCounts: ") +
								  HobString(Integer::toString(getFilteredUncalibratedCounts())));

	Debug::trace("WeigherStatus", HobString("AccelX: ") +
								  HobString(Integer::toString(getAccelX())));
	Debug::trace("WeigherStatus", HobString("AccelY: ") +
								  HobString(Integer::toString(getAccelY())));
	Debug::trace("WeigherStatus", HobString("AccelZ: ") +
								  HobString(Integer::toString(getAccelZ())));

	Debug::trace("WeigherStatus", HobString("TiltX: ") +
								  HobString(Integer::toString(getTiltX())));
	Debug::trace("WeigherStatus", HobString("TiltY: ") +
								  HobString(Integer::toString(getTiltY())));

	Debug::trace("WeigherStatus", HobString("scaleFactor: ") +
								  HobString(Integer::toString(scaleFactor)));
	Debug::trace("WeigherStatus", HobString("dualRangeEnabled: ") +
								  HobString(Bool::toString(dualRangeEnabled)));
	Debug::trace("WeigherStatus", HobString("dualRangeDirection: ") +
								  HobString(Integer::toString((int)dualRangeDirection)));

	Debug::trace("WeigherStatus", HobString("isSmallMotion: ") +
								  HobString(Bool::toString(isSmallMotion())));
	Debug::trace("WeigherStatus", HobString("isLargeMotion: ") +
								  HobString(Bool::toString(isLargeMotion())));
	Debug::trace("WeigherStatus", HobString("isNegativeGrossWeight: ") +
								  HobString(Bool::toString(isNegativeGrossWeight())));
	Debug::trace("WeigherStatus", HobString("isWithinZeroQuarterGrad: ") +
								  HobString(Bool::toString(isWithinZeroQuarterGrad())));
	Debug::trace("WeigherStatus", HobString("isOverGrossWeightLimit: ") +
								  HobString(Bool::toString(isOverGrossWeightLimit())));
	Debug::trace("WeigherStatus", HobString("isZeroOutOfMaintZone: ") +
								  HobString(Bool::toString(isZeroOutOfMaintZone())));
	Debug::trace("WeigherStatus", HobString("isScaleWarmup: ") +
								  HobString(Bool::toString(isScaleWarmup())));
	Debug::trace("WeigherStatus", HobString("isNewZero: ") +
								  HobString(Bool::toString(isNewZero())));
	Debug::trace("WeigherStatus", HobString("isCalibFailed: ") +
								  HobString(Bool::toString(isCalibFailed())));
	Debug::trace("WeigherStatus", HobString("isEepReadFailed: ") +
								  HobString(Bool::toString(isEepReadFailed())));
	Debug::trace("WeigherStatus", HobString("isDisabled: ") +
								  HobString(Bool::toString(isDisabled())));
	Debug::trace("WeigherStatus", HobString("isKeyPressed: ") +
								  HobString(Bool::toString(isKeyPressed())));

	Debug::trace("WeigherStatus", HobString("peripheralComm: ") +
								  HobString(Bool::toString(peripheralComm)));
	Debug::trace("WeigherStatus", HobString("managerDeleted: ") +
								  HobString(Bool::toString(managerDeleted)));

	Debug::trace("WeigherStatus", HobString("weigherConfigOutOfRange: ") +
								  HobString(Bool::toString(weigherConfigOutOfRange)));

	// addedd for Val_MAX
	Debug::trace("WeigherStatus", HobString("isValMaxEepCommFailure: ") +
		HobString(Bool::toString(isValMaxEepCommFailure())));

	Debug::trace("WeigherStatus", HobString("isValMaxEepChecksumFailure: ") +
		HobString(Bool::toString(isValMaxEepChecksumFailure())));

	Debug::trace("WeigherStatus", HobString("isValMaxEepAcclCommFailure: ") +
		HobString(Bool::toString(isValMaxEepAcclCommFailure())));

	Debug::trace("WeigherStatus", HobString("isValMaxTiltReached: ") +
		HobString(Bool::toString(isValMaxTiltReached())));
}

bool WeigherStatus::isPositiveNonZeroStableWeight() const
{
	return ((isNegativeGrossWeight() == false) && 
			(isWithinZeroQuarterGrad() == false) &&
			(isScaleWarmup() == false) &&
			(isSmallMotion() == false) && (isLargeMotion() == false));
}

bool WeigherStatus::isZeroStableWeight() const
{
	return ((isWithinZeroQuarterGrad() == true) && (isScaleWarmup() == false) &&
			(isSmallMotion() == false) && (isLargeMotion() == false));
}

bool WeigherStatus::isSmallMotion() const
{
	return statusBits & SMALL_MOTION;
}

void WeigherStatus::setSmallMotion(bool value)
{
	if (value == true)
	{
		statusBits |= SMALL_MOTION;
	}
	else
	{
		statusBits &= ~SMALL_MOTION;
	}
}

void WeigherStatus::clearSmallMotion()
{
	statusBits &= ~SMALL_MOTION;
}

bool WeigherStatus::isLargeMotion() const
{
	if ( (statusBits & LARGE_MOTION) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setLargeMotion(bool value)
{
	if (value == true)
	{
		statusBits |= LARGE_MOTION;
	}
	else
	{
		statusBits &= ~LARGE_MOTION;
	}
}

void WeigherStatus::clearLargeMotion()
{
	statusBits &= ~LARGE_MOTION;
}

bool WeigherStatus::isNegativeGrossWeight() const
{
	if ( (statusBits & NEGATIVE_GROSS_WEIGHT) > 0)
		return  true;
	else
		return false;
}

void WeigherStatus::setNegativeGrossWeight(bool value)
{
	if (value == true)
	{
		statusBits |= NEGATIVE_GROSS_WEIGHT;
	}
	else
	{
		statusBits &= ~NEGATIVE_GROSS_WEIGHT;
	}
}

void WeigherStatus::clearNegativeGrossWeight()
{
	statusBits &= ~NEGATIVE_GROSS_WEIGHT;
}

bool WeigherStatus::isWithinZeroQuarterGrad() const
{
	if ( (statusBits & WITHIN_QUARTER_GRADUATION_OF_ZERO) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setWithinZeroQuarterGrad(bool value)
{
	if (value == true)
	{
		statusBits |= WITHIN_QUARTER_GRADUATION_OF_ZERO;
	}
	else
	{
		statusBits &= ~WITHIN_QUARTER_GRADUATION_OF_ZERO;
	}
}

void WeigherStatus::clearWithinZeroQuarterGrad()
{
	statusBits &= ~WITHIN_QUARTER_GRADUATION_OF_ZERO;
}

bool WeigherStatus::isOverGrossWeightLimit() const
{
	if ( (statusBits & OVER_GROSS_WEIGHT_LIMIT) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setOverGrossWeightLimit(bool value)
{
	if (value == true)
	{
		statusBits |= OVER_GROSS_WEIGHT_LIMIT;
	}
	else
	{
		statusBits &= ~OVER_GROSS_WEIGHT_LIMIT;
	}
}

void WeigherStatus::clearOverGrossWeightLimit()
{
	statusBits &= ~OVER_GROSS_WEIGHT_LIMIT;
}

bool WeigherStatus::isZeroOutOfMaintZone() const
{
	if ( (statusBits & ZERO_OUT_OF_MAINTENANCE_ZONE) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setZeroOutOfMaintZone(bool value)
{
	if (value == true)
	{
		statusBits |= ZERO_OUT_OF_MAINTENANCE_ZONE;
	}
	else
	{
		statusBits &= ~ZERO_OUT_OF_MAINTENANCE_ZONE;
	}
}

void WeigherStatus::clearZeroOutOfMaintZone()
{
	statusBits &= ~ZERO_OUT_OF_MAINTENANCE_ZONE;
}

bool WeigherStatus::isScaleWarmup() const
{
	if ( (statusBits & SCALE_WARMUP) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setScaleWarmup(bool value)
{
	if (value == true)
	{
		statusBits |= SCALE_WARMUP;
	}
	else
	{
		statusBits &= ~SCALE_WARMUP;
	}
}

void WeigherStatus::clearScaleWarmup()
{
	statusBits &= ~SCALE_WARMUP;
}

bool WeigherStatus::isNewZero() const
{
	if ( (statusBits & NEW_ZERO) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setNewZero(bool value)
{
	if (value == true)
	{
		statusBits |= NEW_ZERO;
	}
	else
	{
		statusBits &= ~NEW_ZERO;
	}
}

void WeigherStatus::clearNewZero()
{
	statusBits &= ~NEW_ZERO;
}

bool WeigherStatus::isCalibFailed() const
{
	if ( (statusBits & WEIGHER_CALIBRATION_FAILED) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setCalibFailed(bool value)
{
	if (value == true)
	{
		statusBits |= WEIGHER_CALIBRATION_FAILED;
	}
	else
	{
		statusBits &= ~WEIGHER_CALIBRATION_FAILED;
	}
}

void WeigherStatus::clearCalibFailed()
{
	statusBits &= ~WEIGHER_CALIBRATION_FAILED;
}

bool WeigherStatus::isEepReadFailed() const
{
	if ( (statusBits & WEIGHER_EEP_READ_FAILED) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setEepReadFailed(bool value)
{
	if (value == true)
	{
		statusBits |= WEIGHER_EEP_READ_FAILED;
	}
	else
	{
		statusBits &= ~WEIGHER_EEP_READ_FAILED;
	}
}

void WeigherStatus::clearEepReadFailed()
{
	statusBits &= ~WEIGHER_EEP_READ_FAILED;
}

bool WeigherStatus::isDisabled() const
{
	if ( (statusBits & WEIGHER_DISABLED) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setDisabled(bool value)
{
	if (value == true)
	{
		statusBits |= WEIGHER_DISABLED;
	}
	else
	{
		statusBits &= ~WEIGHER_DISABLED;
	}
}

void WeigherStatus::clearDisabled()
{
	statusBits &= ~WEIGHER_DISABLED;
}

bool WeigherStatus::isKeyPressed() const
{
	if ( (statusBits & WEIGHER_KEY_PRESSED) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setKeyPressed(bool value)
{
	if (value == true)
	{
		statusBits |= WEIGHER_KEY_PRESSED;
	}
	else
	{
		statusBits &= ~WEIGHER_KEY_PRESSED;
	}
}

void WeigherStatus::clearKeyPressed()
{
	statusBits &= ~WEIGHER_KEY_PRESSED;
}

// Added for ValueMax
bool WeigherStatus::isValMaxEepCommFailure() const
{
	if ((statusBits & WEIGHER_VM_EEP_COMM_FAILURE) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setValMaxEepCommFailure(bool value)
{
	if (value == true)
	{
		statusBits |= WEIGHER_VM_EEP_COMM_FAILURE;
	}
	else
	{
		statusBits &= ~WEIGHER_VM_EEP_COMM_FAILURE;
	}
}

void WeigherStatus::clearValMaxEepCommFailure()
{
	statusBits &= ~WEIGHER_VM_EEP_COMM_FAILURE;
}

bool WeigherStatus::isValMaxEepChecksumFailure() const
{
	if ((statusBits & WEIGHER_VM_EEP_CHECKSUM_FAILURE) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setValMaxEepChecksumFailure(bool value)
{
	if (value == true)
	{
		statusBits |= WEIGHER_VM_EEP_CHECKSUM_FAILURE;
	}
	else
	{
		statusBits &= ~WEIGHER_VM_EEP_CHECKSUM_FAILURE;
	}
}

void WeigherStatus::clearValMaxEepChecksumFailure()
{
	statusBits &= ~WEIGHER_VM_EEP_CHECKSUM_FAILURE;
}

bool WeigherStatus::isValMaxEepAcclCommFailure() const
{
	if ((statusBits & WEIGHER_VM_ACCL_COMM_FAILURE) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setValMaxAcclCommFailure(bool value)
{
	if (value == true)
	{
		statusBits |= WEIGHER_VM_ACCL_COMM_FAILURE;
	}
	else
	{
		statusBits &= ~WEIGHER_VM_ACCL_COMM_FAILURE;
	}
}

void WeigherStatus::clearValMaxAcclCommFailure()
{
	statusBits &= ~WEIGHER_VM_ACCL_COMM_FAILURE;
}

bool WeigherStatus::isValMaxTiltReached() const
{
	if ((statusBits & WEIGHER_VM_MAX_TILT_REACHED) > 0)
		return true;
	else
		return false;
}

void WeigherStatus::setValMaxTiltReached(bool value)
{
	if (value == true)
	{
		statusBits |= WEIGHER_VM_MAX_TILT_REACHED;
	}
	else
	{
		statusBits &= ~WEIGHER_VM_MAX_TILT_REACHED;
	}
}

void WeigherStatus::clearValMaxTiltReached()
{
	statusBits &= ~WEIGHER_VM_MAX_TILT_REACHED;
}


// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
void WeigherStatus::setADCounts(UINT32 ADCounts)
{
	this->ADCounts = ADCounts;
}

UINT32 WeigherStatus::getADCounts() const
{
	return ADCounts;
}

void WeigherStatus::setFilteredUncalibratedCounts(UINT32 FilteredUncalibratedCounts)
{
	this->FilteredUncalibratedCounts = FilteredUncalibratedCounts;
}

UINT32 WeigherStatus::getFilteredUncalibratedCounts() const
{
	return FilteredUncalibratedCounts;
}

void WeigherStatus::setRawCounts(UINT32 rawCounts)
{
	this->rawCounts = rawCounts;
}

UINT32 WeigherStatus::getRawCounts() const
{
	return rawCounts;
}

void WeigherStatus::setCounts(UINT32 counts)
{
	this->counts = counts;
}

UINT32 WeigherStatus::getCounts() const
{
	if (MetSig::isScaleMetric())
		return counts / 2;
	else
		return counts;
}

UINT32 WeigherStatus::getBaseCounts() const
{
	return counts;
}

INT32 WeigherStatus::getRelativeCounts() const
{
	INT32 rv = getCounts();

	if (isNegativeGrossWeight() == true)
	{
		rv = -rv;
	}

	return rv;
}

INT32 WeigherStatus::getAccelX() const
{
	return accelX;
}

void WeigherStatus::setAccelX( INT32 value )
{
	accelX = value;
}

INT32 WeigherStatus::getAccelY() const
{
	return accelY;
}

void WeigherStatus::setAccelY( INT32 value )
{
	accelY = value;
}

INT32 WeigherStatus::getAccelZ() const
{
	return accelZ;
}

void WeigherStatus::setAccelZ( INT32 value )
{
	accelZ = value;
}

INT16 WeigherStatus::getTiltX() const
{
	return tiltX;
}

void WeigherStatus::setTiltX( INT16 value )
{
	tiltX = value;
}

INT16 WeigherStatus::getTiltY() const
{
	return tiltY;
}

void WeigherStatus::setTiltY( INT16 value )
{
	tiltY = value;
}

UINT32 WeigherStatus::getScaleFactor() const
{
	return scaleFactor;
}
// This is called by the WeigherManager so that the WeigherStatus knows the scaleFactor,
// which it uses in the getWeight() method below.
void WeigherStatus::setScaleFactor(UINT32 scaleFactor)
{
	Debug::trace("WeigherStatus", string("setScaleFactor - scaleFactor = ") + longtostring(scaleFactor) );
	this->scaleFactor = scaleFactor;
}

UINT32 WeigherStatus::getEffectiveScaleFactor() const
{
	UINT32 effectiveScaleFactor = scaleFactor;
	Debug::trace("WeigherStatus", string("getEffectiveScaleFactor - scaleFactor = ") + inttostring(scaleFactor));
	if (MetSig::isScaleMetric())
	{
		if (dualRangeEnabled && dualRangeDirection == DualRangeDown)
		{
			Debug::trace("WeigherStatus", string("getEffectiveScaleFactor - metric and dualRangeDown "));
			effectiveScaleFactor = 5;
		}
	}
	else  // avoir
	{
		if (dualRangeEnabled && dualRangeDirection == DualRangeDown)
		{
			Debug::trace("WeigherStatus", string("getEffectiveScaleFactor - avoir and dualRangeDown "));
			effectiveScaleFactor = 1;
		}
	}
	Debug::trace("WeigherStatus", string("getEffectiveScaleFactor - effectiveScaleFactor = ") + inttostring(effectiveScaleFactor));
	return effectiveScaleFactor;
}

//Returns true if the dualRangeDirection has changed, false if not changed.
bool WeigherStatus::setDualRangeParameters(WeighMode mode, WgDualRangeDirection currentDualRangeDirection)
{
	Debug::trace("WeigherStatus", string("setDualRangeParameters - mode = ") + inttostring(mode) + "  current direction = " + inttostring(currentDualRangeDirection) );

	bool directionChanged = false;
	bool drEnabled = MetSig::isWeigherConfiguredForDualRange();
	setDualRangeEnabled(drEnabled);
	Debug::trace("WeigherStatus", string("setDualRangeParameters - drEnabled = ") + booltostring(drEnabled) );

	if (drEnabled)
	{
		setDualRangeDirection(currentDualRangeDirection);

		// We can reset the direction to DualRangeUp here, before allowing the ProductManager to handle the weight,
		// since we only have to consider a stable zero to switch from DualRangeDown to DualRangeUp (tare plays no role in this).
		if (currentDualRangeDirection == DualRangeDown)
		{
			if (isZeroStableWeight())
			{
				directionChanged = true;
				setDualRangeDirection(DualRangeUp);
				Debug::trace("WeigherStatus", "setDualRangeParameters - Switching from dual range down to dual range up");
			}
		}
		else  // == DualRangeUp
		{
			// if using gross weight to determine when to switch from dualRangeUp to dualRangeDown mode (when the weight goes over the dual range limit)
			// then we can make that switch from up to down right now.  This will help the rounding of tare when in the dualRangeDown mode;
			// the mode will be set correctly before total values are updated in the current product transaction.  If we are using net weight
			// to determine the switch over from dualRangeUp to dualRangeDown mode, that will have to be checked when the Product updates
			// the total values, a part of the logic in the productManager->handleWeight() call made below.

			HobWeight weight = getWeightUsingWeighMode(mode);
			Debug::trace("WeigherStatus", string("setDualRangeParameters - weight = ") + longtostring(weight));

			if (!Configuration::getInstance()->isNetWeightDualRangeLimitCheck())
			{
				// FT-1388: dual range limit is at or above
				if ((mode == WeighMode_Metric && weight >= Configuration::getInstance()->toIntCfgI(CfgInternal::DUALRANGELIMITMETRIC)) ||
					(mode == WeighMode_Avoir  && weight >= Configuration::getInstance()->toIntCfgI(CfgInternal::DUALRANGELIMITAVOIR)))
				{
					directionChanged = true;
					setDualRangeDirection(DualRangeDown);
					Debug::trace("WeigherStatus", "setDualRangeParameters - Switching from dual range up to dual range down");
				}
			}
		}
	}
	return directionChanged;
}

bool WeigherStatus::isDualRangeEnabled() const
{
	return dualRangeEnabled;
}
void WeigherStatus::setDualRangeEnabled(bool enabled)
{
	dualRangeEnabled = enabled;
}

WgDualRangeDirection WeigherStatus::getDualRangeDirection() const
{
	return dualRangeDirection;
}
void WeigherStatus::setDualRangeDirection(WgDualRangeDirection direction)
{
	if ( getEffectiveScaleFactor() == 1 )
		dualRangeDirection = DualRangeDown;
	else
		dualRangeDirection = direction;
}


void WeigherStatus::setupToDetermineMetricRuleWeight(UINT32 counts, WeighMode mode)
{
	Debug::trace("WeigherStatus", string("setupToDetermineMetricRuleWeight - counts = ") + longtostring(counts) + "  WeighMode = " + inttostring(mode));

	setCounts(counts);
	if (mode == WeighMode_Avoir)
		setScaleFactor((MetSig::isWeigherConfiguredForDualRange()) ? 5 : 1);
	else
		setScaleFactor((MetSig::isWeigherConfiguredForDualRange()) ? 2 : 5);
	setDualRangeParameters(mode);
	setSmallMotion(false);
	setLargeMotion(false);
	setWithinZeroQuarterGrad(false);
	setScaleWarmup(false);
}


long WeigherStatus::getWeight() const
{
	return getWeightUsingWeighMode((MetSig::isScaleMetric()) ? WeighMode_Metric : WeighMode_Avoir);
}

long WeigherStatus::getWeightUsingWeighMode(WeighMode mode) const
{
	long rv = 0;

	if (isZeroStableWeight() == false)
	{
		//HobWeight divisor = pow(10, Configuration::getInstance()->getWeightPrecision());
		//HobWeight result = ((HobWeight)counts()) / (divisor * 100);

		//HobWeight result = ((HobWeight)counts()) / 100;
		//return result;

		//Calculating the weight came from the wgt_proc.c (Ultima); determine_weight() and calculate_weight()

		bool isTracing = Debug::isTracing("WeigherStatus");

		UINT32 effectiveScaleFactor = scaleFactor;
		long weight, remain;
		UINT32 zeroedCounts = getCounts();

		if (mode == WeighMode_Metric)
		{
			if (dualRangeEnabled && dualRangeDirection == DualRangeDown)
				effectiveScaleFactor = 5;

			if (isTracing)
			{
				Debug::trace("WeigherStatus", HobString("getWeight() - metric - dual range direction is ") + HobString(Integer::toString(dualRangeDirection)));
				Debug::trace("WeigherStatus", HobString("getWeight() - metric - scale factor is ") + HobString(Integer::toString(scaleFactor))+ HobString(" - effective scale factor is ") + HobString(Integer::toString(effectiveScaleFactor)));
			}

			if (effectiveScaleFactor == 5)
			{
				// Adjust the weight to the scale factor; 5g increments
				remain = zeroedCounts % 50;
				if (remain > 25)
					zeroedCounts += (50 - remain);
				else
					zeroedCounts -= remain;
				weight = zeroedCounts / 10;
			}
			else if (effectiveScaleFactor == 2)
			{
				// Adjust the weight to the scale factor; 2g increments
				remain = zeroedCounts % 20;
				if (remain > 9)
					zeroedCounts += (20 - remain);
				else
					zeroedCounts -= remain;
				weight = zeroedCounts / 10;
			}
			else
			{
				weight = zeroedCounts / 10;
				remain = zeroedCounts % 10;
				bool isOdd = (weight % 2) ? true : false;

				if ((remain > 5) || ((remain == 5) && isOdd))
					weight = weight + 1L;
			}
		}
		else // avoir weighing
		{
			if (dualRangeEnabled && dualRangeDirection == DualRangeDown)
				effectiveScaleFactor = 1;

			if (isTracing)
			{
				Debug::trace("WeigherStatus", HobString("getWeight() - avoir - dual range direction is ") + HobString(Integer::toString(dualRangeDirection)));
				Debug::trace("WeigherStatus", HobString("getWeight() - avoir - scale factor is ") + HobString(Integer::toString(scaleFactor))+ HobString(" - effective scale factor is ") + HobString(Integer::toString(effectiveScaleFactor)));
			}

			if (effectiveScaleFactor == 5)
			{
				// Adjust the weight to the scale factor; 5g increments
				remain = zeroedCounts % 50;
				if (remain > 25)
					zeroedCounts += (50 - remain);
				else
					zeroedCounts -= remain;
				weight = zeroedCounts / 10;
			}
			else
			{
				// On 5/18/2016 - For HTi, once the service counter and prepack code became merged, we want to have any weight value to be
				// 3 implied decimal places; the database will always store 3 implied decimal places.  This was always the case since the
				// beginning of HLX (but then again, the HLX was always dual range).   Even for single range weighers, which exist for
				// wrapping systems, we want to store 3 decimal places.  Single range weighers and dual range weighers when the weight is
				// over the dual range limit (12 lbs) will have their effectiveScaleFactor = 1.  At the end of this 'else' block, we used to
				// multiply the weight by 10 to get back to the 3 implied decimal places, but only if we have a dual range weigher (and the
				// weight is over 12 lbs).  Now we are multiplying by 10 for single range weighers (effectively any time the effectiveScaleFactor = 1)
				// so that all weights in the system are 3 implied decimal places, not just dual range weighers.
				// 
				// used for dual range weight determination, when weight is over the dual range limit, or
				// when we were previously over the limit but we haven't returned to a stable zero weight (DualRangeDown).
				// We still want to keep 3 decimal places (the thousanths place will be 0), but will only display 2 when formatted for display.
				weight = zeroedCounts / 100;
				remain = zeroedCounts % 100;

				bool isOdd = (weight % 2) ? true : false;

				if ((remain > 50) || ((remain == 50) && isOdd))
					weight = weight + 1L;

				weight *= 10;
			}
		}

		// todo - need to get the scale factor from WeigherConfiguration!!!
		//weight = weight * (long) scale_factor;

		rv = weight;
		if (isTracing)
			Debug::trace("WeigherStatus", HobString("getWeight() - zeroed counts =  ") + Long::toString(zeroedCounts) + "  weight = " + Long::toString(rv));
	}

	return rv;
}


void WeigherStatus::setStatusBits(UINT16 statusBits)
{
	this->statusBits = statusBits;
}

UINT16 WeigherStatus::getStatusBits() const
{
	return statusBits;
}

bool WeigherStatus::isPeripheralComm() const
{
	return peripheralComm;
}

void WeigherStatus::setPeripheralComm(bool value)
{
	peripheralComm = value;
}

void WeigherStatus::clearPeripheralComm()
{
	peripheralComm = false;
}

bool WeigherStatus::isManagerDeleted() const
{
	return managerDeleted;
}

void WeigherStatus::setManagerDeleted(bool value)
{
	managerDeleted = value;
}

void WeigherStatus::clearManagerDeleted()
{
	managerDeleted = false;
}

bool WeigherStatus::isWeigherConfigOutOfRange() const
{
	return weigherConfigOutOfRange;
}
void WeigherStatus::setWeigherConfigOutOfRange(bool outOfRange)
{
	weigherConfigOutOfRange = outOfRange;
}

bool WeigherStatus::equals(const WeigherStatus &wgStatus) const
{
	bool rv = false;

	if ((statusBits == wgStatus.getStatusBits()) &&
		(getCounts() == wgStatus.getCounts()) &&
		(rawCounts == wgStatus.getRawCounts()) &&
		// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		(FilteredUncalibratedCounts == wgStatus.getFilteredUncalibratedCounts()) &&
		(ADCounts == wgStatus.getADCounts()) &&
		(accelX == wgStatus.getAccelX()) &&
		(accelY == wgStatus.getAccelY()) &&
		(accelZ == wgStatus.getAccelZ()) &&
		(tiltX == wgStatus.getTiltX()) &&
		(tiltY == wgStatus.getTiltY()) &&
		(peripheralComm == wgStatus.isPeripheralComm()) &&
		(managerDeleted == wgStatus.isManagerDeleted()) &&
		(weigherConfigOutOfRange == wgStatus.isWeigherConfigOutOfRange()))
	{
		rv = true;
	}

	return rv;
}
