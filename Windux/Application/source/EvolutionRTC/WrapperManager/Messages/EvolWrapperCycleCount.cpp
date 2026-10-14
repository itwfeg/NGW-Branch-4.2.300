#include "EvolWrapperCycleCount.h"
#include "HobString.h"
#include "Debug.h"
#include "Long.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperCycleCount::_dataType_ = 0;
std::string EvolWrapperCycleCount::_dataTypeName_("EvolWrapperCycleCount"); 
#endif

EvolWrapperCycleCount::EvolWrapperCycleCount(Hobart::CEvolMsgReceiver *pHobThread, ULONG cycleCount) : WrapperGeneric(WrEvolCycleCount, pHobThread)
{
	wrapperCycleCount_	= cycleCount;
}


EvolWrapperCycleCount::EvolWrapperCycleCount(const char *buffer) : WrapperGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &wrapperCycleCount_;
	for (unsigned int i = 0; i < sizeof(wrapperCycleCount_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperCycleCount::~EvolWrapperCycleCount()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *EvolWrapperCycleCount::serialize()const
{	
	WrapperGeneric::serialize();

	char *pChar = (char *) &wrapperCycleCount_;
	for (unsigned int i = 0; i < sizeof(wrapperCycleCount_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperCycleCount::show()
{	
	Debug::trace("EvolWrapperCycleCount", "EvolWrapperCycleCount: ");
	Debug::trace("EvolWrapperCycleCount", HobString("wrapperCycleCount_: ") + HobString(Long::toString(wrapperCycleCount_)));
}

