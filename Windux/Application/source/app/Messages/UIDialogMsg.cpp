#include "UIDialogMsg.h"
#include "SessionEvent.h"

#include "Debug.h"
#include "HobString.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int UIDialogMsg::dataType = 0;
std::string UIDialogMsg::dataTypeName("UIDialogMsg"); 
#endif


UIDialogMsg::UIDialogMsg(Hobart::CEvolMsgReceiver *pHobThread,
								int errorDevice,
								int statusCode,
								UINT16 responseType,
								bool removeDialog) : EvolMessage(SessionDev, SnDisplayDialog, pHobThread),
															errorDevice(errorDevice),
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

UIDialogMsg::UIDialogMsg(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &errorDevice;
	for (unsigned int i = 0; i < sizeof(errorDevice); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &statusCode;
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

UIDialogMsg::~UIDialogMsg()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


const char *UIDialogMsg::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &errorDevice;
	for (unsigned int i = 0; i < sizeof(errorDevice); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &statusCode;
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

int UIDialogMsg::getErrorDevice()
{
	return errorDevice;
}
void UIDialogMsg::setErrorDevice(int device)
{
	errorDevice = device;
}

int UIDialogMsg::getStatusCode()
{
	return statusCode;
}
void UIDialogMsg::setStatusCode(int code)
{
	statusCode = code;
}

UINT16 UIDialogMsg::getResponseType()
{
	return responseType;
}
void UIDialogMsg::setResponseType(UINT16 type)
{
	responseType = type;
}

bool UIDialogMsg::isRemoveDialog()
{
	return removeDialog;
}
void UIDialogMsg::setRemoveDialog(bool remove)
{
	removeDialog = remove;
}

void UIDialogMsg::show()
{
	bool isTracing = Debug::isTracing("VmStatusMsg");
	if (isTracing == false)
		Debug::addTraceCategory("VmStatusMsg");

	Debug::trace("VmStatusMsg", "VmStatusMsg: ");

	EvolMessage::show();

	Debug::trace("VmStatusMsg", HobString("statusCode: ") + HobString(Integer::toString(statusCode)));
	Debug::trace("VmStatusMsg", HobString("responseType: ") + HobString(Integer::toString(responseType)));
	Debug::trace("VmStatusMsg", HobString("removeDialog: ") + HobString(Bool::toString(removeDialog)));

	if (isTracing == false)
		Debug::removeTraceCategory("VmStatusMsg");
}

