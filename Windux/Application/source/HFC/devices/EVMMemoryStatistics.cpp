#include "EVMMemoryStatistics.h"



#include "SessionEvent.h"
//#include "vxWorks.h"

#include "Debug.h"
#include "Integer.h"
#include "Hex.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EVMMemoryStatistics::dataType = 0;
std::string EVMMemoryStatistics::dataTypeName("EVMMemoryStatistics"); 
#endif


EVMMemoryStatistics::EVMMemoryStatistics(Hobart::CEvolMsgReceiver *pHobThread)
		: EvolMessage(SessionDev, SnWriteStatistics, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

    usedMemory_ = 0;
    freeMemory_ = 0;
}


EVMMemoryStatistics::EVMMemoryStatistics(const char *buffer)
		: EvolMessage(buffer)
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
	char *pChar = (char *) &usedMemory_;
	for (unsigned int i = 0; i < sizeof(usedMemory_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &freeMemory_;
	for (unsigned int i = 0; i < sizeof(freeMemory_); i++)
	{
		*pChar++ = buffer[j++];
	}
}


EVMMemoryStatistics::~EVMMemoryStatistics()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *EVMMemoryStatistics::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &usedMemory_;
	for (unsigned int i = 0; i < sizeof(usedMemory_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &freeMemory_;
	for (unsigned int i = 0; i < sizeof(freeMemory_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}


void EVMMemoryStatistics::show()
{
	bool isTracing = Debug::isTracing("EVMMemoryStatistics");
	if ( isTracing == false )
		Debug::addTraceCategory("EVMMemoryStatistics");


	Debug::trace("EVMMemoryStatistics", "EVMMemoryStatistics: ");

	EvolMessage::show();

	Debug::trace("EVMMemoryStatistics", "  used memory: " + HobString(Integer::toString(usedMemory_)) + "Kb");
	Debug::trace("EVMMemoryStatistics", "  free memory: " + HobString(Integer::toString(freeMemory_)) + "Kb");


	if ( isTracing == false )
		Debug::removeTraceCategory("EVMMemoryStatistics");
}


UINT32 EVMMemoryStatistics::getUsedMemory() const
{
	return usedMemory_;
}


void EVMMemoryStatistics::setUsedMemory(UINT32 usedMemory)
{
	this->usedMemory_ = usedMemory;
}


UINT32 EVMMemoryStatistics::getFreeMemory() const
{
	return freeMemory_;
}


void EVMMemoryStatistics::setFreeMemory(UINT32 freeMemory)
{
	this->freeMemory_ = freeMemory;
}
