
#include "EvolLabelApplierConfiguration.h"

#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolLabelApplierConfiguration::_dataType_ = 0;
std::string EvolLabelApplierConfiguration::_dataTypeName_("EvolLabelApplierConfiguration"); 
#endif

EvolLabelApplierConfiguration::EvolLabelApplierConfiguration(ULONG checksum)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	this->checksum = checksum;
}

EvolLabelApplierConfiguration::EvolLabelApplierConfiguration(const char *buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	int j = 0;

	char *pChar = (char *) &checksum;
	for (unsigned int i = 0; i < sizeof(checksum); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &autoRotation;
	for (unsigned int i = 0; i < sizeof(autoRotation); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priLabelSizeDefault;
	for (unsigned int i = 0; i < sizeof(priLabelSizeDefault); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priLabelWidth;
	for (unsigned int i = 0; i < sizeof(priLabelWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priLabelRotationDefault;
	for (unsigned int i = 0; i < sizeof(priLabelRotationDefault); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priLabelLocationDefault;
	for (unsigned int i = 0; i < sizeof(priLabelLocationDefault); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priLabelOffsetOnPkg;
	for (unsigned int i = 0; i < sizeof(priLabelOffsetOnPkg); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priRotationCalibrationOffset;
	for (unsigned int i = 0; i < sizeof(priRotationCalibrationOffset); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secLabelSizeDefault;
	for (unsigned int i = 0; i < sizeof(secLabelSizeDefault); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secLabelWidth;
	for (unsigned int i = 0; i < sizeof(secLabelWidth); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secLabelRotationDefault;
	for (unsigned int i = 0; i < sizeof(secLabelRotationDefault); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secLabelLocationDefault;
	for (unsigned int i = 0; i < sizeof(secLabelLocationDefault); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secLabelOffsetOnPkg;
	for (unsigned int i = 0; i < sizeof(secLabelOffsetOnPkg); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secRotationCalibrationOffset;
	for (unsigned int i = 0; i < sizeof(secRotationCalibrationOffset); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secondCLAAttached;
	for (unsigned int i = 0; i < sizeof(secondCLAAttached); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sideDischargeType;
	for (unsigned int i = 0; i < sizeof(sideDischargeType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priWandCalibrationOffset;
	for (unsigned int i = 0; i < sizeof(priWandCalibrationOffset); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secWandCalibrationOffset;
	for (unsigned int i = 0; i < sizeof(secWandCalibrationOffset); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priRotationCalibrationOffset_90Deg;
	for (unsigned int i = 0; i < sizeof(priRotationCalibrationOffset_90Deg); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priRotationCalibrationOffset_180Deg;
	for (unsigned int i = 0; i < sizeof(priRotationCalibrationOffset_180Deg); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priRotationCalibrationOffset_270Deg;
	for (unsigned int i = 0; i < sizeof(priRotationCalibrationOffset_270Deg); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secRotationCalibrationOffset_90Deg;
	for (unsigned int i = 0; i < sizeof(secRotationCalibrationOffset_90Deg); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secRotationCalibrationOffset_180Deg;
	for (unsigned int i = 0; i < sizeof(secRotationCalibrationOffset_180Deg); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secRotationCalibrationOffset_270Deg;
	for (unsigned int i = 0; i < sizeof(secRotationCalibrationOffset_270Deg); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priApplierHandAdjust;
	for (unsigned int i = 0; i < sizeof(priApplierHandAdjust); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secApplierHandAdjust;
		for (unsigned int i = 0; i < sizeof(secApplierHandAdjust); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolLabelApplierConfiguration::EvolLabelApplierConfiguration(const EvolLabelApplierConfiguration &elac)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	checksum = elac.getChecksum();
	autoRotation = elac.doesHaveAutoRotation();

	priLabelSizeDefault = elac.getPriLabelSizeDefault();
	priLabelWidth = elac.getPriLabelWidth();
	priLabelRotationDefault = elac.getPriLabelRotationDefault();
	priLabelLocationDefault = elac.getPriLabelLocationDefault();
	priLabelOffsetOnPkg = elac.getPriLabelOffsetOnPkg();
	priRotationCalibrationOffset = elac.getPriRotationCalibrationOffset();
	priWandCalibrationOffset = elac.getPriWandCalibrationOffset();

	secLabelSizeDefault = elac.getSecLabelSizeDefault();
	secLabelWidth = elac.getSecLabelWidth();
	secLabelRotationDefault = elac.getSecLabelRotationDefault();
	secLabelLocationDefault = elac.getSecLabelLocationDefault();
	secLabelOffsetOnPkg = elac.getSecLabelOffsetOnPkg();
	secRotationCalibrationOffset = elac.getSecRotationCalibrationOffset();
	secWandCalibrationOffset = elac.getSecWandCalibrationOffset();

	secondCLAAttached = elac.isSecondCLAAttached();
	sideDischargeType = elac.getSideDischargeType();

	priRotationCalibrationOffset_90Deg	= elac.getPriRotationCalibrationOffset_90Deg();
	priRotationCalibrationOffset_180Deg 	= elac.getPriRotationCalibrationOffset_180Deg();
	priRotationCalibrationOffset_270Deg 	= elac.getPriRotationCalibrationOffset_270Deg();

	secRotationCalibrationOffset_90Deg		= elac.getSecRotationCalibrationOffset_90Deg();
	secRotationCalibrationOffset_180Deg 	= elac.getSecRotationCalibrationOffset_180Deg();
	secRotationCalibrationOffset_270Deg 	= elac.getSecRotationCalibrationOffset_270Deg();

	priApplierHandAdjust = elac.getPriApplierHandAdjust();
	secApplierHandAdjust = elac.getSecApplierHandAdjust();
}

EvolLabelApplierConfiguration::~EvolLabelApplierConfiguration()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolLabelApplierConfiguration::serialize() const 
{
	serializedData = "";

	char *pChar = (char *) &checksum;
	for (unsigned int i = 0; i < sizeof(checksum); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &autoRotation;
	for (unsigned int i = 0; i < sizeof(autoRotation); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priLabelSizeDefault;
	for (unsigned int i = 0; i < sizeof(priLabelSizeDefault); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priLabelWidth;
	for (unsigned int i = 0; i < sizeof(priLabelWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priLabelRotationDefault;
	for (unsigned int i = 0; i < sizeof(priLabelRotationDefault); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priLabelLocationDefault;
	for (unsigned int i = 0; i < sizeof(priLabelLocationDefault); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priLabelOffsetOnPkg;
	for (unsigned int i = 0; i < sizeof(priLabelOffsetOnPkg); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priRotationCalibrationOffset;
	for (unsigned int i = 0; i < sizeof(priRotationCalibrationOffset); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secLabelSizeDefault;
	for (unsigned int i = 0; i < sizeof(secLabelSizeDefault); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secLabelWidth;
	for (unsigned int i = 0; i < sizeof(secLabelWidth); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secLabelRotationDefault;
	for (unsigned int i = 0; i < sizeof(secLabelRotationDefault); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secLabelLocationDefault;
	for (unsigned int i = 0; i < sizeof(secLabelLocationDefault); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secLabelOffsetOnPkg;
	for (unsigned int i = 0; i < sizeof(secLabelOffsetOnPkg); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secRotationCalibrationOffset;
	for (unsigned int i = 0; i < sizeof(secRotationCalibrationOffset); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secondCLAAttached;
	for (unsigned int i = 0; i < sizeof(secondCLAAttached); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sideDischargeType;
	for (unsigned int i = 0; i < sizeof(sideDischargeType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priWandCalibrationOffset;
	for (unsigned int i = 0; i < sizeof(priWandCalibrationOffset); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secWandCalibrationOffset;
	for (unsigned int i = 0; i < sizeof(secWandCalibrationOffset); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priRotationCalibrationOffset_90Deg;
	for (unsigned int i = 0; i < sizeof(priRotationCalibrationOffset_90Deg); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priRotationCalibrationOffset_180Deg;
	for (unsigned int i = 0; i < sizeof(priRotationCalibrationOffset_180Deg); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priRotationCalibrationOffset_270Deg;
	for (unsigned int i = 0; i < sizeof(priRotationCalibrationOffset_270Deg); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secRotationCalibrationOffset_90Deg;
	for (unsigned int i = 0; i < sizeof(secRotationCalibrationOffset_90Deg); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secRotationCalibrationOffset_180Deg;
	for (unsigned int i = 0; i < sizeof(secRotationCalibrationOffset_180Deg); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secRotationCalibrationOffset_270Deg;
	for (unsigned int i = 0; i < sizeof(secRotationCalibrationOffset_270Deg); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &priApplierHandAdjust;
	for (unsigned int i = 0; i < sizeof(priApplierHandAdjust); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secApplierHandAdjust;
	for (unsigned int i = 0; i < sizeof(secApplierHandAdjust); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT32 EvolLabelApplierConfiguration::getSerializedLength()const
{
	return (UINT32)serializedData.length();
}

void EvolLabelApplierConfiguration::show()
{
	if (Debug::isTracing("EvolLabelApplierConfiguration"))
	{
//		Debug::addTraceCategory("EvolLabelApplierConfiguration");

		Debug::trace("EvolLabelApplierConfiguration", HobString("checksum = ") + HobString(Long::toString(checksum)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("autoRotation = ") + HobString(Bool::toString(autoRotation)));

		Debug::trace("EvolLabelApplierConfiguration", HobString("priLabelSizeDefault = ") + HobString(Integer::toString(priLabelSizeDefault)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("priLabelWidth = ") + HobString(Integer::toString(priLabelWidth)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("priLabelRotationDefault = ") + HobString(Integer::toString(priLabelRotationDefault)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("priLabelLocationDefault = ") + HobString(Integer::toString(priLabelLocationDefault)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("priLabelOffsetOnPkg = ") + HobString(Integer::toString(priLabelOffsetOnPkg)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("priRotationCalibrationOffset = ") + HobString(Integer::toString(priRotationCalibrationOffset)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("priWandCalibrationOffset = ") + HobString(Integer::toString(priWandCalibrationOffset)));

		Debug::trace("EvolLabelApplierConfiguration", HobString("secLabelSizeDefault = ") + HobString(Integer::toString(secLabelSizeDefault)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("secLabelWidth = ") + HobString(Integer::toString(secLabelWidth)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("secLabelRotationDefault = ") + HobString(Integer::toString(secLabelRotationDefault)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("secLabelLocationDefault = ") + HobString(Integer::toString(secLabelLocationDefault)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("secLabelOffsetOnPkg = ") + HobString(Integer::toString(secLabelOffsetOnPkg)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("secRotationCalibrationOffset = ") + HobString(Integer::toString(secRotationCalibrationOffset)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("secWandCalibrationOffset = ") + HobString(Integer::toString(secWandCalibrationOffset)));

		Debug::trace("EvolLabelApplierConfiguration", HobString("secondCLAAttached = ") + HobString(Bool::toString(secondCLAAttached)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("sideDischargeType = ") + HobString(Integer::toString(sideDischargeType)));

		Debug::trace("EvolLabelApplierConfiguration", HobString("priRotationCalibrationOffset_90 = ") + HobString(Integer::toString(priRotationCalibrationOffset_90Deg)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("priRotationCalibrationOffset_180 = ") + HobString(Integer::toString(priRotationCalibrationOffset_180Deg)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("priRotationCalibrationOffset_270 = ") + HobString(Integer::toString(priRotationCalibrationOffset_270Deg)));

		Debug::trace("EvolLabelApplierConfiguration", HobString("secRotationCalibrationOffset_90 = ") + HobString(Integer::toString(secRotationCalibrationOffset_90Deg)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("secRotationCalibrationOffset_180 = ") + HobString(Integer::toString(secRotationCalibrationOffset_180Deg)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("secRotationCalibrationOffset_270 = ") + HobString(Integer::toString(secRotationCalibrationOffset_270Deg)));

		Debug::trace("EvolLabelApplierConfiguration", HobString("priApplierHandAdjust = ") + HobString(Integer::toString(priApplierHandAdjust)));
		Debug::trace("EvolLabelApplierConfiguration", HobString("secApplierHandAdjust = ") + HobString(Integer::toString(secApplierHandAdjust)));

//		Debug::removeTraceCategory("EvolLabelApplierConfiguration");
	}
}

ULONG EvolLabelApplierConfiguration::getChecksum() const
{
	return checksum;
}

void EvolLabelApplierConfiguration::setChecksum(ULONG checksum)
{
	this->checksum = checksum;
}

bool EvolLabelApplierConfiguration::doesHaveAutoRotation() const
{
	Debug::trace("EvolLabelApplierConfiguration", "doesHaveAutoRotation():  autoRotation = " + HobString(Bool::toString(autoRotation)));
	return autoRotation;
}

bool EvolLabelApplierConfiguration::getHasAutoRotation() const
{
	Debug::trace("EvolLabelApplierConfiguration", "getHasAutoRotation():  autoRotation = " + HobString(Bool::toString(autoRotation)));
	return autoRotation;
}

void EvolLabelApplierConfiguration::setHasAutoRotation(bool value)
{
	autoRotation = value;
	Debug::trace("EvolLabelApplierConfiguration", "setHasAutoRotation():  autoRotation = " + HobString(Bool::toString(autoRotation)));
}

UINT16 EvolLabelApplierConfiguration::getPriLabelSizeDefault() const
{
	return priLabelSizeDefault;
}

void EvolLabelApplierConfiguration::setPriLabelSizeDefault(UINT16 size)
{
	priLabelSizeDefault = size;
}

LabelWidth EvolLabelApplierConfiguration::getPriLabelWidth() const
{
	return priLabelWidth;
}

void EvolLabelApplierConfiguration::setPriLabelWidth(LabelWidth size)
{
	priLabelWidth = size;
}

LaRotation EvolLabelApplierConfiguration::getPriLabelRotationDefault() const
{
	return priLabelRotationDefault;
}

void EvolLabelApplierConfiguration::setPriLabelRotationDefault(LaRotation value)
{
	priLabelRotationDefault = value;
}

LaPosLoc EvolLabelApplierConfiguration::getPriLabelLocationDefault() const
{
	return priLabelLocationDefault;
}

void EvolLabelApplierConfiguration::setPriLabelLocationDefault(LaPosLoc value)
{
	priLabelLocationDefault = value;
}

UINT16 EvolLabelApplierConfiguration::getPriLabelOffsetOnPkg() const
{
	return priLabelOffsetOnPkg;
}

void EvolLabelApplierConfiguration::setPriLabelOffsetOnPkg(UINT16 value)
{
	priLabelOffsetOnPkg = value;
}

UINT16 EvolLabelApplierConfiguration::getSecLabelSizeDefault() const
{
	return secLabelSizeDefault;
}

void EvolLabelApplierConfiguration::setSecLabelSizeDefault(UINT16 size)
{
	secLabelSizeDefault = size;
}

LabelWidth EvolLabelApplierConfiguration::getSecLabelWidth() const
{
	return secLabelWidth;
}

void EvolLabelApplierConfiguration::setSecLabelWidth(LabelWidth size)
{
	secLabelWidth = size;
}

LaRotation EvolLabelApplierConfiguration::getSecLabelRotationDefault() const
{
	return secLabelRotationDefault;
}

void EvolLabelApplierConfiguration::setSecLabelRotationDefault(LaRotation value)
{
	secLabelRotationDefault = value;
}

LaPosLoc EvolLabelApplierConfiguration::getSecLabelLocationDefault() const
{
	return secLabelLocationDefault;
}

void EvolLabelApplierConfiguration::setSecLabelLocationDefault(LaPosLoc value)
{
	secLabelLocationDefault = value;
}

UINT16 EvolLabelApplierConfiguration::getSecLabelOffsetOnPkg() const
{
	return secLabelOffsetOnPkg;
}

void EvolLabelApplierConfiguration::setSecLabelOffsetOnPkg(UINT16 value)
{
	secLabelOffsetOnPkg = value;
}

bool EvolLabelApplierConfiguration::isSecondCLAAttached() const
{
	return secondCLAAttached;
}

bool EvolLabelApplierConfiguration::getIsSecondCLAAttached() const
{
	return secondCLAAttached;
}

void EvolLabelApplierConfiguration::setIsSecondCLAAttached(bool value)
{
	secondCLAAttached = value;
}

LaSideDischarge EvolLabelApplierConfiguration::getSideDischargeType() const
{
	return sideDischargeType;
}

void EvolLabelApplierConfiguration::setSideDischargeType(LaSideDischarge type)
{
	sideDischargeType = type;
}


void EvolLabelApplierConfiguration::operator=(const EvolLabelApplierConfiguration &laConfig)
{
	checksum = laConfig.getChecksum();
	autoRotation = laConfig.getHasAutoRotation();

	priLabelSizeDefault = laConfig.getPriLabelSizeDefault();
	priLabelWidth = laConfig.getPriLabelWidth();
	priLabelRotationDefault = laConfig.getPriLabelRotationDefault();
	priLabelLocationDefault = laConfig.getPriLabelLocationDefault();
	priLabelOffsetOnPkg = laConfig.getPriLabelOffsetOnPkg();
	priRotationCalibrationOffset = laConfig.getPriRotationCalibrationOffset();
	priWandCalibrationOffset = laConfig.getPriWandCalibrationOffset();

	secLabelSizeDefault = laConfig.getSecLabelSizeDefault();
	secLabelWidth = laConfig.getSecLabelWidth();
	secLabelRotationDefault = laConfig.getSecLabelRotationDefault();
	secLabelLocationDefault = laConfig.getSecLabelLocationDefault();
	secLabelOffsetOnPkg = laConfig.getSecLabelOffsetOnPkg();
	secRotationCalibrationOffset = laConfig.getSecRotationCalibrationOffset();
	secWandCalibrationOffset = laConfig.getSecWandCalibrationOffset();

	secondCLAAttached = laConfig.getIsSecondCLAAttached();
	sideDischargeType = laConfig.getSideDischargeType();
	priRotationCalibrationOffset_90Deg	= laConfig.getPriRotationCalibrationOffset_90Deg();
	priRotationCalibrationOffset_180Deg 	= laConfig.getPriRotationCalibrationOffset_180Deg();
	priRotationCalibrationOffset_270Deg 	= laConfig.getPriRotationCalibrationOffset_270Deg();

	secRotationCalibrationOffset_90Deg		= laConfig.getSecRotationCalibrationOffset_90Deg();
	secRotationCalibrationOffset_180Deg 	= laConfig.getSecRotationCalibrationOffset_180Deg();
	secRotationCalibrationOffset_270Deg 	= laConfig.getSecRotationCalibrationOffset_270Deg();

	priApplierHandAdjust = laConfig.getPriApplierHandAdjust();
	secApplierHandAdjust = laConfig.getSecApplierHandAdjust();
}


UINT16 EvolLabelApplierConfiguration::getPriRotationCalibrationOffset() const
{
	return priRotationCalibrationOffset;
}

void EvolLabelApplierConfiguration::setPriRotationCalibrationOffset(UINT16 value)
{
	priRotationCalibrationOffset = value;
}

UINT16 EvolLabelApplierConfiguration::getPriRotationCalibrationOffset_90Deg() const
{
	return priRotationCalibrationOffset_90Deg;
}

void EvolLabelApplierConfiguration::setPriRotationCalibrationOffset_90Deg(UINT16 value)
{
	priRotationCalibrationOffset_90Deg = value;
}

UINT16 EvolLabelApplierConfiguration::getPriRotationCalibrationOffset_180Deg() const
{
	return priRotationCalibrationOffset_180Deg;
}

void EvolLabelApplierConfiguration::setPriRotationCalibrationOffset_180Deg(UINT16 value)
{
	priRotationCalibrationOffset_180Deg = value;
}

UINT16 EvolLabelApplierConfiguration::getPriRotationCalibrationOffset_270Deg() const
{
	return priRotationCalibrationOffset_270Deg;
}

void EvolLabelApplierConfiguration::setPriRotationCalibrationOffset_270Deg(UINT16 value)
{
	priRotationCalibrationOffset_270Deg = value;
}

UINT16 EvolLabelApplierConfiguration::getSecRotationCalibrationOffset() const
{
	return secRotationCalibrationOffset;
}

void EvolLabelApplierConfiguration::setSecRotationCalibrationOffset(UINT16 value)
{
	secRotationCalibrationOffset = value;
}

UINT16 EvolLabelApplierConfiguration::getPriWandCalibrationOffset() const
{
	return priWandCalibrationOffset;
}

void EvolLabelApplierConfiguration::setPriWandCalibrationOffset(UINT16 value)
{
	priWandCalibrationOffset = value;
}

UINT16 EvolLabelApplierConfiguration::getSecWandCalibrationOffset() const
{
	return secWandCalibrationOffset;
}

void EvolLabelApplierConfiguration::setSecWandCalibrationOffset(UINT16 value)
{
	secWandCalibrationOffset = value;
}
