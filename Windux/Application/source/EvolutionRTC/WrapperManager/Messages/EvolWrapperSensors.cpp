#include "EvolWrapperSensors.h"

#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperSensors::_dataType_ = 0;
std::string EvolWrapperSensors::_dataTypeName_("EvolWrapperSensors"); 
#endif


EvolWrapperSensors::EvolWrapperSensors(Hobart::CEvolMsgReceiver *pHobThread)
	: WrapperGeneric(WrEvolSensors, pHobThread)
	, genSensors()
	, interlockSensors()
	, genSensors_2()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	if (Debug::isTracing("EvolWrapperSensors"))
	{
		HobString msgDebug("Constructor with HobThread  device = ");
		msgDebug.append(Integer::toString(getDeviceType()));
		msgDebug.append("  event = ").append(Integer::toString(getEventType()));
		Debug::trace("EvolWrapperSensors", msgDebug);
	}
}

EvolWrapperSensors::EvolWrapperSensors(const char *buffer)
	: WrapperGeneric(buffer)
	, genSensors()
	, interlockSensors()
	, genSensors_2()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();
	genSensors = EvolWrapperGeneralSensors(&buffer[j]);
	genSensors.serialize();
	j += genSensors.getSerializedLength();

	interlockSensors = EvolWrapperInterlockSensors(&buffer[j]);
	interlockSensors.serialize();
	j += interlockSensors.getSerializedLength();

	genSensors_2 = EvolWrapperGeneralSensors_2(&buffer[j]);
}

EvolWrapperSensors::EvolWrapperSensors(const EvolWrapperSensors &ews) : WrapperGeneric(WrEvolSensors)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	genSensors = ews.getGeneralSensors();
	interlockSensors = ews.getInterlockSensors();
	genSensors_2 = ews.getGeneralSensors_2();
}

EvolWrapperSensors::~EvolWrapperSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperSensors::serialize() const
{
	Debug::trace("EvolWrapperSensors", "New EvolWrapperSensors serialize");

	bool traceOn = Debug::isTracing("EvolWrapperSensors.test");

	WrapperGeneric::serialize();
	if (traceOn)
	{
		HobString h = HobString("After WrapperGeneric::serialize(), serializedData is: ") + HobString(Hex::toString(serializedData.data(), (int)serializedData.size()));
		Debug::trace("EvolWrapperSensors.test", h);
	}
	const char *pCC = genSensors.serialize();
	int mLength = genSensors.getSerializedLength();
	serializedData.append(pCC, mLength);
	if (traceOn)
	{
		HobString h = HobString("After genSensors, serializedData is: ") + HobString(Hex::toString(serializedData.data(), (int)serializedData.size()));
		Debug::trace("EvolWrapperSensors.test", h);
	}

	pCC = interlockSensors.serialize();
	mLength = interlockSensors.getSerializedLength();
	serializedData.append(pCC, mLength);
	if (traceOn)
	{
		HobString h = HobString("After interlockSensors, serializedData is: ") + HobString(Hex::toString(serializedData.data(), (int)serializedData.size()));
		Debug::trace("EvolWrapperSensors.test", h);
	}

	pCC = genSensors_2.serialize();
	mLength = genSensors_2.getSerializedLength();
	serializedData.append(pCC, mLength);
	if (traceOn)
	{
		HobString h = HobString("After genSensors_2, serializedData is: ") + HobString(Hex::toString(serializedData.data(), (int)serializedData.size()));
		Debug::trace("EvolWrapperSensors.test", h);
	}

	return serializedData.data();
}

UINT32 EvolWrapperSensors::getSerializedLength() const
{
	return (UINT32)serializedData.length();
}

void EvolWrapperSensors::show()
{
	if (Debug::isTracing("EvolWrapperSensors"))
	{
		HobString headers("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");

		EvolMessage::show();
		Debug::trace("EvolWrapperSensors", headers + "  START");
		genSensors.show();
		interlockSensors.show();
		genSensors_2.show();
		Debug::trace("EvolWrapperSensors", headers + "  END");
	}
}

EvolWrapperGeneralSensors &EvolWrapperSensors::getGeneralSensors()
{
	return genSensors;
}

const EvolWrapperGeneralSensors &EvolWrapperSensors::getGeneralSensors()const
{
	return genSensors;
}


EvolWrapperInterlockSensors &EvolWrapperSensors::getInterlockSensors()
{
	return interlockSensors;
}

const EvolWrapperInterlockSensors &EvolWrapperSensors::getInterlockSensors()const
{
	return interlockSensors;
}

EvolWrapperGeneralSensors_2 &EvolWrapperSensors::getGeneralSensors_2()
{
	return genSensors_2;
}

const EvolWrapperGeneralSensors_2 &EvolWrapperSensors::getGeneralSensors_2()const
{
	return genSensors_2;
}

bool EvolWrapperSensors::equals(const EvolWrapperSensors &wrSensors)const
{
	bool rv = false;

	if (
		(genSensors.equals(wrSensors.getGeneralSensors())) &&
		(interlockSensors.equals(wrSensors.getInterlockSensors())) &&
		(genSensors_2.equals(wrSensors.getGeneralSensors_2()))
	)
	{
		rv = true;
	}
	HobString m(Bool::toString(rv));
	Debug::trace("EvolWrapperSensors", "equals = " + m);
	return rv;
}

void EvolWrapperSensors::operator=(EvolWrapperSensors &ews)
{
	genSensors = ews.getGeneralSensors();
	interlockSensors = ews.getInterlockSensors();
	genSensors_2 = ews.getGeneralSensors_2();
}
