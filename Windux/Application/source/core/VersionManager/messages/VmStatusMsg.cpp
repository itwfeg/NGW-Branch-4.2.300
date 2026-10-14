//#include "vxWorks.h"

#include "VmStatusMsg.h"
#include "VersionManagerEvent.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int VmStatusMsg::dataType = 0;
std::string VmStatusMsg::dataTypeName("VmStatusMsg"); 
#endif

VmStatusMsg::VmStatusMsg(Hobart::CEvolMsgReceiver *pHobThread,
						 VersionManagerDevError statusCode,
						 UINT16 responseType,
						 bool removeDialog) : EvolMessage(VersionManagerDev, FirmwareUpdateStatus, pHobThread),
											  statusCode(statusCode),
											  responseType(responseType),
											  removeDialog(removeDialog)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

VmStatusMsg::VmStatusMsg(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &statusCode;
	for (unsigned int i = 0; i < sizeof(statusCode); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &responseType;
	for (unsigned int i = 0; i < sizeof(responseType); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &removeDialog;
	for (unsigned int i = 0; i < sizeof(removeDialog); i++)
	{
		*pChar++ = buffer[j++];
	}
}

VmStatusMsg::~VmStatusMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const char *VmStatusMsg::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &statusCode;
	for (unsigned int i = 0; i < sizeof(statusCode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &responseType;
	for (unsigned int i = 0; i < sizeof(responseType); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &removeDialog;
	for (unsigned int i = 0; i < sizeof(removeDialog); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void VmStatusMsg::show()
{
	bool isTracing = Debug::isTracing("VmStatusMsg");

	if (isTracing == false)
	{
    #if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("VmStatusMsg") );
    #else
		Debug::addTraceCategory("VmStatusMsg");
    #endif
	}

	Debug::trace("VmStatusMsg", "VmStatusMsg: ");

	EvolMessage::show();

	Debug::trace("VmStatusMsg", HobString("statusCode: ") + HobString(Integer::toString(statusCode)));
	Debug::trace("VmStatusMsg", HobString("responseType: ") + HobString(Integer::toString(responseType)));
	Debug::trace("VmStatusMsg", HobString("removeDialog: ") + HobString(Bool::toString(removeDialog)));

	if (isTracing == false)
	{
    #if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("VmStatusMsg") );
    #else
		Debug::removeTraceCategory("VmStatusMsg");
    #endif
	}
}

VersionManagerDevError VmStatusMsg::getStatusCode()
{
	return statusCode;
}

void VmStatusMsg::setStatusCode(VersionManagerDevError statusCode)
{
	this->statusCode = statusCode;
}

UINT16 VmStatusMsg::getResponseType()
{
	return responseType;
}

void VmStatusMsg::setResponseType(UINT16 responseType)
{
	this->responseType = responseType;
}

bool VmStatusMsg::isRemoveDialog()
{
	return removeDialog;
}

bool VmStatusMsg::getRemoveDialog()
{
	return removeDialog;
}

void VmStatusMsg::setRemoveDialog(bool removeDialog)
{
	this->removeDialog = removeDialog;
}

void VmStatusMsg::clearRemoveDialog()
{
	removeDialog = false;
}
