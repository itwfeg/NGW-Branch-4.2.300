//#include "vxWorks.h"
#include "LabelApplierSensorsTracking.h"

#include "Debug.h"
#include "HobString.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierSensorsTracking::dataType = 0;
std::string LabelApplierSensorsTracking::dataTypeName("LabelApplierSensorsTracking"); 
#endif

LabelApplierSensorsTracking::LabelApplierSensorsTracking(Hobart::CEvolMsgReceiver *pHobThread,
											   			 bool sensorsTracking) : LabelApplierGeneric(LaSensorsTracking, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->sensorsTracking = sensorsTracking;
}

LabelApplierSensorsTracking::LabelApplierSensorsTracking(const char *buffer) : LabelApplierGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	LabelApplierGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &sensorsTracking;
	for (unsigned int i = 0; i < sizeof(sensorsTracking); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierSensorsTracking::~LabelApplierSensorsTracking()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierSensorsTracking::serialize() const
{
	LabelApplierGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &sensorsTracking;
	for (unsigned int i = 0; i < sizeof(sensorsTracking); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierSensorsTracking::show()
{
	bool isTracing = Debug::isTracing("LabelApplierSensorsTracking");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("LabelApplierSensorsTracking") );
#else
		Debug::addTraceCategory("LabelApplierSensorsTracking");
#endif
	}

	Debug::trace("LabelApplierSensorsTracking", "LabelApplierSensorsTracking: ");

	EvolMessage::show();

	Debug::trace("LabelApplierSensorsTracking", HobString("sensorsTracking: ") + HobString(Bool::toString(sensorsTracking)));


	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("LabelApplierSensorsTracking") );
#else
		Debug::removeTraceCategory("LabelApplierSensorsTracking");
#endif
	}
}

void LabelApplierSensorsTracking::setSensorsTracking(bool value)
{
	sensorsTracking = value;
}

void LabelApplierSensorsTracking::clearSensorsTracking()
{
	sensorsTracking = false;
}

bool LabelApplierSensorsTracking::isSensorsTracking()
{
	return sensorsTracking;
}

bool LabelApplierSensorsTracking::getSensorsTracking()
{
	return sensorsTracking;
}
