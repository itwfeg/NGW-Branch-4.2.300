#include "LabelApplierEvent.h"
#include "CLARemoveUpgradeFiles.h"

#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CLARemoveUpgradeFiles::_dataType_ = 0;
std::string CLARemoveUpgradeFiles::_dataTypeName_("CLARemoveUpgradeFiles"); 
#endif


CLARemoveUpgradeFiles::CLARemoveUpgradeFiles(Hobart::CEvolMsgReceiver *pHobThread, CLARemoveUpgradeState state)
	: EvolMessage(LabelApplier, LaRemoveUpgradeFiles, pHobThread), state_(state)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

CLARemoveUpgradeFiles::CLARemoveUpgradeFiles(const char *buffer)
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

CLARemoveUpgradeFiles::~CLARemoveUpgradeFiles()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *CLARemoveUpgradeFiles::serialize()const
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

void CLARemoveUpgradeFiles::show()
{
	if (Debug::isTracing("CLARemoveUpgradeFiles"))
	{
		HobString msgDebug = "state_: ";

//		EvolMessage::show();

		msgDebug.append(Integer::toString(state_));
		Debug::trace("CLARemoveUpgradeFiles", msgDebug);
	}
}


CLARemoveUpgradeFiles::CLARemoveUpgradeState CLARemoveUpgradeFiles::getState()
{
	return state_;
}

void CLARemoveUpgradeFiles::setState(CLARemoveUpgradeState state)
{
	if ((state >= BeginRemoval) && (state <= RemovalComplete))
		state_ = state;
}
