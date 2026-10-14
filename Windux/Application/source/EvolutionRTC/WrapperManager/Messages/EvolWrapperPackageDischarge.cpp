#include "WrapperEvent.h"
#include "EvolWrapperPackageDischarge.h"

#include "Debug.h"
#include "Integer.h"
#include "UInteger.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperPackageDischarge::_dataType_ = 0;
std::string EvolWrapperPackageDischarge::_dataTypeName_("EvolWrapperPackageDischarge"); 
#endif

EvolWrapperPackageDischarge::EvolWrapperPackageDischarge(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrEvolPackageDischarge, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	packId = 0;
	Debug::trace("EvolWrapperPackageDischarge", "EvolWrapperPackageDischarge(*pHobThread):  packId = " + HobString(Integer::toString(packId)));
}

EvolWrapperPackageDischarge::EvolWrapperPackageDischarge(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items

	char *pChar = (char *) &packId;
	for (unsigned int i = 0; i < sizeof(packId); i++)
	{
		*pChar++ = buffer[j++];
	}
	Debug::trace("EvolWrapperPackageDischarge", "EvolWrapperPackageDischarge(*buffer):  packId = " + HobString(Integer::toString(packId)));
}


EvolWrapperPackageDischarge::EvolWrapperPackageDischarge(const EvolWrapperPackageDischarge &ewpd) : WrapperGeneric(WrEvolPackageDischarge)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	packId = ewpd.getPackId();
	Debug::trace("EvolWrapperPackageDischarge", "EvolWrapperPackageDischarge(&ewpd):  packId = " + HobString(Integer::toString(packId)));
}

EvolWrapperPackageDischarge::~EvolWrapperPackageDischarge()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
 
const char *EvolWrapperPackageDischarge::serialize()
{
	Debug::trace("EvolWrapperPackageDischarge", "serialize()");
	WrapperGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &packId;
	for (unsigned int i = 0; i < sizeof(packId); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}

void EvolWrapperPackageDischarge::show()
{
	if (Debug::isTracing("EvolWrapperPackageDischarge"))
	{
		Debug::trace("EvolWrapperPackageDischarge", "EvolWrapperPackageDischarge: ");
		Debug::trace("EvolWrapperPackageDischarge", "   packId: " + HobString(Integer::toString(packId)));
	}
}


UINT16 EvolWrapperPackageDischarge::getPackId() const
{
	return packId;
}
void EvolWrapperPackageDischarge::setPackId(UINT16 packId)
{
	this->packId = packId;
	Debug::trace("EvolWrapperPackageDischarge", "packId = " + HobString(Integer::toString(packId)));
}
