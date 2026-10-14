//#include "vxWorks.h"
#include "SystemEvent.h"
#include "SystemRead.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemRead::_dataType_ = 0;
std::string SystemRead::_dataTypeName_("SystemRead"); 
#endif

SystemRead::SystemRead(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread, UINT16 address, UINT8 data) : EvolMessage(evolDevType, SysRead, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	this->address = address;
	this->data = data;
}

SystemRead::SystemRead(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &address;
	for (unsigned int i = 0; i < sizeof(address); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &data;
	for (unsigned int i = 0; i < sizeof(data); i++)
	{
		*pChar++ = buffer[j++];
	}
	
}

SystemRead::~SystemRead()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
 
const char *SystemRead::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &address;
	for (unsigned int i = 0; i < sizeof(address); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &data;
	for (unsigned int i = 0; i < sizeof(data); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

UINT16 SystemRead::getAddress()
{
	return address;
}

void SystemRead::setAddress(UINT16 address)
{
	this->address = address;
}

UINT8 SystemRead::getData()
{
	return data;
}

void SystemRead::setData(UINT8 data)
{
	this->data = data;
}

