//#include "vxWorks.h"
#include "WrapperEvent.h"
#include "WrapperCycle.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperCycle::dataType = 0;
std::string WrapperCycle::dataTypeName("WrapperCycle"); 
#endif

WrapperCycle::WrapperCycle(Hobart::CEvolMsgReceiver *pHobThread,
						   UINT8 packageId,
						   bool pullFilm,
						   WrPackCode packCode) : EvolMessage(Wrapper, WrCycle, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->packageId = packageId;
	this->pullFilm = pullFilm;
	this->packCode = packCode;
}

WrapperCycle::WrapperCycle(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pullFilm;
	for (unsigned int i = 0; i < sizeof(pullFilm); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packCode;
	for (unsigned int i = 0; i < sizeof(packCode); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperCycle::~WrapperCycle()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
 
const char *WrapperCycle::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pullFilm;
	for (unsigned int i = 0; i < sizeof(pullFilm); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packCode;
	for (unsigned int i = 0; i < sizeof(packCode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void WrapperCycle::show()
{
	Debug::addTraceCategory("WrapperCycle");
	Debug::trace("WrapperCycle", "WrapperCycle: ");

	EvolMessage::show();

	Debug::trace("WrapperCycle", HobString("packageId: ") +
								 HobString(Integer::toString(packageId)));

    Debug::trace("WrapperCycle", HobString("pullFilm: ") +
								 HobString(Bool::toString(pullFilm)));

	if (packCode == PackCode0)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode0");
	}
	else if (packCode == PackCode1)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode1");
	}
	else if (packCode == PackCode2)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode2");
	}
	else if (packCode == PackCode3)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode3");
	}
	else if (packCode == PackCode4)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode4");
	}
	else if (packCode == PackCode5)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode5");
	}
	else if (packCode == PackCode6)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode6");
	}
	else if (packCode == PackCode7)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode7");
	}
	else if (packCode == PackCode8)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode8");
	}
	else if (packCode == PackCode9)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode9");
	}
	else if (packCode == PackCode10)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode10");
	}
	else if (packCode == UndefinedPackCode)
	{
		Debug::trace("WrapperCycle", "packCode: UndefinedPackCode");
	}
	else if (packCode == PackCode11)
	{
		Debug::trace("WrapperCycle", "packCode: PackCode11");
	}
	else
	{
		Debug::trace("WrapperCycle", HobString("packCode: Unknown ") +
									 HobString(Integer::toString((UINT16) packCode)));
	}

	Debug::removeTraceCategory("WrapperCycle");
}

UINT8 WrapperCycle::getPackageId()
{
	return packageId;
}

void WrapperCycle::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}

bool WrapperCycle::isPullFilm()
{
	return pullFilm;
}

void WrapperCycle::setPullFilm()
{
	pullFilm = true;
}

void WrapperCycle::clearPullFilm()
{
	pullFilm = false;
}

WrPackCode WrapperCycle::getWrPackCode()
{
	return packCode;
}

void WrapperCycle::setWrPackCode(WrPackCode packCode)
{
	this->packCode = packCode;
}
