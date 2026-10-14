//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierSensors.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierSensors::dataType = 0;
std::string LabelApplierSensors::dataTypeName("LabelApplierSensors"); 
#endif

LabelApplierSensors::LabelApplierSensors(Hobart::CEvolMsgReceiver *pHobThread,
										 INT8 expectPackage,
										 INT8 weigherEntry,
										 INT8 pr1Entry,
										 INT8 pr1Gate,
										 INT8 pr1LabelPosition,
										 INT8 pr1Valid,
										 INT8 pr2Entry,
										 INT8 pr2Gate,
										 INT8 pr2LabelPosition,
										 INT8 pr2Valid) : EvolMessage(LabelApplier, LaSensors, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->expectPackage = expectPackage;
	this->weigherEntry = weigherEntry;
	this->pr1Entry = pr1Entry;
	this->pr1Gate = pr1Gate;
	this->pr1LabelPosition = pr1LabelPosition;
	this->pr1Valid = pr1Valid;
	this->pr2Entry = pr2Entry;
	this->pr2Gate = pr2Gate;
	this->pr2LabelPosition = pr2LabelPosition;
	this->pr2Valid = pr2Valid;
}

LabelApplierSensors::LabelApplierSensors(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &expectPackage;
	for (unsigned int i = 0; i < sizeof(expectPackage); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &weigherEntry;
	for (unsigned int i = 0; i < sizeof(weigherEntry); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pr1Entry;
	for (unsigned int i = 0; i < sizeof(pr1Entry); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pr1Gate;
	for (unsigned int i = 0; i < sizeof(pr1Gate); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pr1LabelPosition;
	for (unsigned int i = 0; i < sizeof(pr1LabelPosition); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pr1Valid;
	for (unsigned int i = 0; i < sizeof(pr1Valid); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pr2Entry;
	for (unsigned int i = 0; i < sizeof(pr2Entry); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pr2Gate;
	for (unsigned int i = 0; i < sizeof(pr2Gate); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pr2LabelPosition;
	for (unsigned int i = 0; i < sizeof(pr2LabelPosition); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pr2Valid;
	for (unsigned int i = 0; i < sizeof(pr2Valid); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierSensors::~LabelApplierSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierSensors::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &expectPackage;
	for (unsigned int i = 0; i < sizeof(expectPackage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &weigherEntry;
	for (unsigned int i = 0; i < sizeof(weigherEntry); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pr1Entry;
	for (unsigned int i = 0; i < sizeof(pr1Entry); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pr1Gate;
	for (unsigned int i = 0; i < sizeof(pr1Gate); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pr1LabelPosition;
	for (unsigned int i = 0; i < sizeof(pr1LabelPosition); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pr1Valid;
	for (unsigned int i = 0; i < sizeof(pr1Valid); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pr2Entry;
	for (unsigned int i = 0; i < sizeof(pr2Entry); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pr2Gate;
	for (unsigned int i = 0; i < sizeof(pr2Gate); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pr2LabelPosition;
	for (unsigned int i = 0; i < sizeof(pr2LabelPosition); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pr2Valid;
	for (unsigned int i = 0; i < sizeof(pr2Valid); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierSensors::show()
{
	bool isTracing = Debug::isTracing("LabelApplierSensors");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("LabelApplierSensors") );
#else
		Debug::addTraceCategory("LabelApplierSensors");
#endif
	}

	Debug::trace("LabelApplierSensors", "LabelApplierSensors: ");

	EvolMessage::show();

	Debug::trace("LabelApplierSensors", HobString("expectPackage: ") +
										  HobString(Integer::toString(expectPackage)));
	Debug::trace("LabelApplierSensors", HobString("weigherEntry: ") +
										  HobString(Integer::toString(weigherEntry)));
	Debug::trace("LabelApplierSensors", HobString("pr1Entry: ") +
										  HobString(Integer::toString(pr1Entry)));
	Debug::trace("LabelApplierSensors", HobString("pr1Gate: ") +
										  HobString(Integer::toString(pr1Gate)));
	Debug::trace("LabelApplierSensors", HobString("pr1LabelPosition: ") +
										  HobString(Integer::toString(pr1LabelPosition)));
	Debug::trace("LabelApplierSensors", HobString("pr1Valid: ") +
										  HobString(Integer::toString(pr1Valid)));
	Debug::trace("LabelApplierSensors", HobString("pr2Entry: ") +
										  HobString(Integer::toString(pr2Entry)));
	Debug::trace("LabelApplierSensors", HobString("pr2Gate: ") +
										  HobString(Integer::toString(pr2Gate)));
	Debug::trace("LabelApplierSensors", HobString("pr2LabelPosition: ") +
										  HobString(Integer::toString(pr2LabelPosition)));
	Debug::trace("LabelApplierSensors", HobString("pr2Valid: ") +
										  HobString(Integer::toString(pr2Valid)));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("LabelApplierSensors") );
#else
		Debug::removeTraceCategory("LabelApplierSensors");
#endif
	}
}

INT8 LabelApplierSensors::getExpectPackage() const
{
	return expectPackage;
}

void LabelApplierSensors::setExpectPackage(INT8 expectPackage)
{
	this->expectPackage = expectPackage;
}

INT8 LabelApplierSensors::getWeigherEntry() const
{
	return weigherEntry;
}

void LabelApplierSensors::setWeigherEntry(INT8 weigherEntry)
{
	this->weigherEntry = weigherEntry;
}

INT8 LabelApplierSensors::getPr1Entry() const
{
	return pr1Entry;
}

void LabelApplierSensors::setPr1Entry(INT8 pr1Entry)
{
	this->pr1Entry = pr1Entry;
}

INT8 LabelApplierSensors::getPr1Gate() const
{
	return pr1Gate;
}

void LabelApplierSensors::setPr1Gate(INT8 pr1Gate)
{
	this->pr1Gate = pr1Gate;
}

INT8 LabelApplierSensors::getPr1LabelPosition() const
{
	return 	pr1LabelPosition;
}

void LabelApplierSensors::setPr1LabelPosition(INT8 pr1LabelPosition)
{
	this->pr1LabelPosition = pr1LabelPosition;
}

INT8 LabelApplierSensors::getPr1Valid() const
{
	return pr1Valid;
}

void LabelApplierSensors::setPr1Valid(INT8 pr1Valid)
{
	this->pr1Valid = pr1Valid;
}

INT8 LabelApplierSensors::getPr2Entry() const
{
	return pr2Entry;
}

void LabelApplierSensors::setPr2Entry(INT8 pr2Entry)
{
	this->pr2Entry = pr2Entry;
}

INT8 LabelApplierSensors::getPr2Gate() const
{
	return pr2Gate;
}

void LabelApplierSensors::setPr2Gate(INT8 pr2Gate)
{
	this->pr2Gate = pr2Gate;
}

INT8 LabelApplierSensors::getPr2LabelPosition() const
{
	return pr2LabelPosition;
}

void LabelApplierSensors::setPr2LabelPosition(INT8 pr2LabelPosition)
{
	this->pr2LabelPosition = pr2LabelPosition;
}

INT8 LabelApplierSensors::getPr2Valid() const
{
	return pr2Valid;
}

void LabelApplierSensors::setPr2Valid(INT8 pr2Valid)
{
	this->pr2Valid = pr2Valid;
}

bool LabelApplierSensors::equals(LabelApplierSensors &laSensors)
{
	bool rv = false;

	if	((expectPackage == laSensors.getExpectPackage()) &&
		 (weigherEntry == laSensors.getWeigherEntry()) &&
		 (pr1Entry == laSensors.getPr1Entry()) &&
		 (pr1Gate == laSensors.getPr1Gate()) &&
		 (pr1LabelPosition == laSensors.getPr1LabelPosition()) &&
		 (pr1Valid == laSensors.getPr1Valid()) &&
		 (pr2Entry == laSensors.getPr2Entry()) &&
		 (pr2Gate == laSensors.getPr2Gate()) &&
		 (pr2LabelPosition == laSensors.getPr2LabelPosition()) &&
		 (pr2Valid == laSensors.getPr2Valid()))
	{
		rv = true;
	}

	return rv;
}
