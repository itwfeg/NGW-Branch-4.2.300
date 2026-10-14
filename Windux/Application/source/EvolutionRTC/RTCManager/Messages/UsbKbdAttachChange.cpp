//#include "vxWorks.h"

#include "UsbKbdAttachChange.h"
#include "SessionEvent.h"
//#include "HobThread.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int UsbKbdAttachChange::_dataType_ = 0;
std::string UsbKbdAttachChange::_dataTypeName_("UsbKbdAttachChange"); 
#endif

UsbKbdAttachChange::UsbKbdAttachChange(int attachStatus, Hobart::CEvolMsgReceiver *pHobThread) 
	: EvolMessage(SessionDev, SnUsbKbdAttachChange, pHobThread), attachStatus_(attachStatus)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

UsbKbdAttachChange::UsbKbdAttachChange(const char *buffer) : EvolMessage(buffer)
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

	char *pChar = (char *) &attachStatus_;
	for (unsigned int i = 0; i < sizeof(attachStatus_); i++)
	{
		*pChar++ = buffer[j++];
	}


}

UsbKbdAttachChange::~UsbKbdAttachChange()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}


const char *UsbKbdAttachChange::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items

	char *pChar = (char *) & attachStatus_;
	for (unsigned int i = 0; i < sizeof(attachStatus_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void UsbKbdAttachChange::show()
{
	EvolMessage::show();

	//cout << "attachStatus_: " << attachStatus_ <<endl;

}
