#include "EvolWrapperSaveCycleCount.h"
#include "HobString.h"
#include "Debug.h"
#include "Long.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperSaveCycleCount::_dataType_ = 0;
std::string EvolWrapperSaveCycleCount::_dataTypeName_("EvolWrapperSaveCycleCount"); 
#endif

EvolWrapperSaveCycleCount::EvolWrapperSaveCycleCount(Hobart::CEvolMsgReceiver *pHobThread, unsigned long cycleCount) : WrapperGeneric(WrEvolSaveCycleCount, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	wrapperCycleCount_	= cycleCount;
}


EvolWrapperSaveCycleCount::EvolWrapperSaveCycleCount(const char *buffer) : WrapperGeneric(buffer)
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

EvolWrapperSaveCycleCount::~EvolWrapperSaveCycleCount()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *EvolWrapperSaveCycleCount::serialize()const
{	
	WrapperGeneric::serialize();

	char *pChar = (char *) &wrapperCycleCount_;
	for (unsigned int i = 0; i < sizeof(wrapperCycleCount_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperSaveCycleCount::show()
{	
	Debug::trace("EvolWrapperSaveCycleCount", "EvolWrapperSaveCycleCount: ");
	Debug::trace("EvolWrapperSaveCycleCount", HobString("wrapperCycleCount_: ") + HobString(Long::toString(wrapperCycleCount_)));
}

