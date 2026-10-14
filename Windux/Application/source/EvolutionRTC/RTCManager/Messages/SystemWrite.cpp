//#include "vxWorks.h"
#include "SystemEvent.h"
#include "SystemWrite.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemWrite::_dataType_ = 0;
std::string SystemWrite::_dataTypeName_("SystemWrite"); 
#endif

SystemWrite::SystemWrite(EvolDeviceType evolDevType, Hobart::CEvolMsgReceiver *pHobThread, UINT16 address, UINT8 data) : EvolMessage(evolDevType, SysWrite, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	this->address = address;
	this->data = data;
}

SystemWrite::SystemWrite(const char *buffer) : EvolMessage(buffer)
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

SystemWrite::~SystemWrite()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}
 
const char *SystemWrite::serialize() const
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

UINT16 SystemWrite::getAddress()
{
	return address;
}

void SystemWrite::setAddress(UINT16 address)
{
	this->address = address;
}

UINT8 SystemWrite::getData()
{
	return data;
}

void SystemWrite::setData(UINT8 data)
{
	this->data = data;
}

