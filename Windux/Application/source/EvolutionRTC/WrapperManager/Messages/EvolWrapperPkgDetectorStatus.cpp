#include "EvolWrapperPkgDetectorStatus.h"
#include "HobString.h"
#include "Debug.h"
#include "Integer.h"
#include "Bool.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperPkgDetectorStatus::_dataType_ = 0;
std::string EvolWrapperPkgDetectorStatus::_dataTypeName_("EvolWrapperPkgDetectorStatus"); 
#endif

EvolWrapperPkgDetectorStatus::EvolWrapperPkgDetectorStatus(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolPkgDetectorStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	sensorLocation_		= NoLocation;

	detectorType_		= Detector455kHz;
	emitterPowerLevel_	= 0;	
	sensorOperational_	= false;

	isOldDetector_		= false;

	detectorDescription_ = "";
	detectorDescriptionA = "56kHz package detector ";
	detectorDescriptionB = "455kHz package detector";
}


EvolWrapperPkgDetectorStatus::EvolWrapperPkgDetectorStatus(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &sensorLocation_;
	for (unsigned int i = 0; i < sizeof(sensorLocation_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &detectorType_;
	for (unsigned int i = 0; i < sizeof(detectorType_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &emitterPowerLevel_;
	for (unsigned int i = 0; i < sizeof(emitterPowerLevel_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sensorOperational_;
	for (unsigned int i = 0; i < sizeof(sensorOperational_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &isOldDetector_;
	for (unsigned int i = 0; i < sizeof(isOldDetector_); i++)
	{
		*pChar++ = buffer[j++];
	}


	for (unsigned int i = 0; i < detectorDescriptionA.size(); i++)
	{
		detectorDescription_.append(1, buffer[j++]);
	}
}

EvolWrapperPkgDetectorStatus::~EvolWrapperPkgDetectorStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *EvolWrapperPkgDetectorStatus::serialize()const
{	
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sensorLocation_;
	for (unsigned int i = 0; i < sizeof(sensorLocation_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &detectorType_;
	for (unsigned int i = 0; i < sizeof(detectorType_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &emitterPowerLevel_;
	for (unsigned int i = 0; i < sizeof(emitterPowerLevel_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sensorOperational_;
	for (unsigned int i = 0; i < sizeof(sensorOperational_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &isOldDetector_;
	for (unsigned int i = 0; i < sizeof(isOldDetector_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	serializedData += detectorDescription_;

	return serializedData.data();
}


void EvolWrapperPkgDetectorStatus::setDetectorType(UINT16 detectorType)
{
	detectorType_ = (DetectorType)detectorType;

	if(detectorType_ == Detector455kHz)
	{
		detectorDescription_ = detectorDescriptionB;
	}
	else
	{
		detectorDescription_ = detectorDescriptionA;
	}
}

void EvolWrapperPkgDetectorStatus::show()
{	
	Debug::trace("EvolWrapperPkgDetectorStatus", "EvolWrapperPkgDetectorStatus: ");
	Debug::trace("EvolWrapperPkgDetectorStatus", HobString("sensorLocation_: ") + HobString(Integer::toString(sensorLocation_)));
	Debug::trace("EvolWrapperPkgDetectorStatus", HobString("detectorType_: ") + HobString(Integer::toString(detectorType_)));
	Debug::trace("EvolWrapperPkgDetectorStatus", HobString("emitterPowerLevel_: ") + HobString(Integer::toString(emitterPowerLevel_)));
	Debug::trace("EvolWrapperPkgDetectorStatus", HobString("sensorOperational_: ") + HobString(Integer::toString(sensorOperational_)));
	Debug::trace("EvolWrapperPkgDetectorStatus", HobString("isOldDetector_: ") + HobString(Integer::toString(isOldDetector_)));
	Debug::trace("EvolWrapperPkgDetectorStatus", HobString("detectorDescription_: ") + detectorDescription_);

}
