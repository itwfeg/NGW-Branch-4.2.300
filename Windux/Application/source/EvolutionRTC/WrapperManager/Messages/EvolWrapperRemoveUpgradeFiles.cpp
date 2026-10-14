#include "WrapperEvent.h"
#include "EvolWrapperRemoveUpgradeFiles.h"

#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int EvolWrapperRemoveUpgradeFiles::_dataType_ = 0;
std::string EvolWrapperRemoveUpgradeFiles::_dataTypeName_("EvolWrapperRemoveUpgradeFiles"); 
#endif


EvolWrapperRemoveUpgradeFiles::EvolWrapperRemoveUpgradeFiles(Hobart::CEvolMsgReceiver *pHobThread, WrEvolRemoveUpgradeState state)
	: EvolMessage(Wrapper, WrEvolRemoveUpgradeFiles, pHobThread), state_(state)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

EvolWrapperRemoveUpgradeFiles::EvolWrapperRemoveUpgradeFiles(const char *buffer)
	: EvolMessage(buffer), state_(BeginRemoval)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &state_;
	for (unsigned int i = 0; i < sizeof(state_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

EvolWrapperRemoveUpgradeFiles::~EvolWrapperRemoveUpgradeFiles()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *EvolWrapperRemoveUpgradeFiles::serialize()const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &state_;
	for (unsigned int i = 0; i < sizeof(state_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void EvolWrapperRemoveUpgradeFiles::show()
{
	if (Debug::isTracing("EvolWrapperRemoveUpgradeFiles"))
	{
		HobString msgDebug = "state_: ";

//		EvolMessage::show();

		msgDebug.append(Integer::toString(state_));
		Debug::trace("EvolWrapperRemoveUpgradeFiles", msgDebug);
	}
}


EvolWrapperRemoveUpgradeFiles::WrEvolRemoveUpgradeState EvolWrapperRemoveUpgradeFiles::getState()
{
	return state_;
}

void EvolWrapperRemoveUpgradeFiles::setState(WrEvolRemoveUpgradeState state)
{
	if ((state >= BeginRemoval) && (state <= RemovalComplete))
		state_ = state;
}
