//#include "vxWorks.h"

#include "AccessKeyDeactivation.h"
#include "SessionEvent.h"
//#include "HobThread.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int AccessKeyDeactivation::_dataType_ = 0;
std::string AccessKeyDeactivation::_dataTypeName_("AccessKeyDeactivation"); 
#endif

AccessKeyDeactivation::AccessKeyDeactivation(int keys, Hobart::CEvolMsgReceiver *pHobThread) 
	: EvolMessage(SessionDev, SnKeyDeactivation, pHobThread), keys_(keys)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

AccessKeyDeactivation::AccessKeyDeactivation(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
unsigned int j = 0;

	EvolMessage::serialize();

	j = getSerializedLength();

	// Now, for the derived class items

	char *pChar = (char *) &keys_;
	for (unsigned int i = 0; i < sizeof(keys_); i++)
	{
		*pChar++ = buffer[j++];
	}


}

AccessKeyDeactivation::~AccessKeyDeactivation()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *AccessKeyDeactivation::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	char *pChar = (char *) & keys_;
	for (unsigned int i = 0; i < sizeof(keys_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void AccessKeyDeactivation::show()
{
	EvolMessage::show();

	//cout << "keys_: " << keys_ <<endl;

}
