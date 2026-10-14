//#include "vxWorks.h"
#include "PrinterRequestLabelTransfer.h"

#include "Debug.h"
#include "HobString.h"
#include "Hex.h"
#include "UInteger.h"
#include "Integer.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterRequestLabelTransfer::dataType = 0;
std::string PrinterRequestLabelTransfer::dataTypeName("PrinterRequestLabelTransfer"); 
#endif

PrinterRequestLabelTransfer::PrinterRequestLabelTransfer(Hobart::CEvolMsgReceiver *pHobThread,
														 PrinterType printerType,
														 UINT16 totalDataLength,
														 UINT16 actualDataLength,
														 UINT16 *pData,
														 bool sendSetupXferMessage,
														 bool cancelPendingTransfer) : PrinterGeneric(PrReqLabelTransfer, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if ((totalDataLength == 0) || (pData == NULL))
	{
		Debug::trace("PrinterManager.LabelTransfer", "PrinterRequestLabelTransfer(): totalDataLength = 0 ");
		this->totalDataLength = 0;
		this->actualDataLength = 0;
		this->pData = NULL;
	}
	else
	{
		Debug::trace("PrinterManager.LabelTransfer", "PrinterRequestLabelTransfer(): totalDataLength" + Integer::toString(totalDataLength));
		this->totalDataLength = totalDataLength;
		this->actualDataLength = actualDataLength;
		this->pData = pData;
	}

	this->sendSetupXferMessage = sendSetupXferMessage;
	this->cancelPendingTransfer = cancelPendingTransfer;
}

PrinterRequestLabelTransfer::PrinterRequestLabelTransfer(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &totalDataLength;
	for (unsigned int i = 0; i < sizeof(totalDataLength); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &actualDataLength;
	for (unsigned int i = 0; i < sizeof(actualDataLength); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pData;
	for (unsigned int i = 0; i < sizeof(pData); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sendSetupXferMessage;
	for (unsigned int i = 0; i < sizeof(sendSetupXferMessage); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &cancelPendingTransfer;
	for (unsigned int i = 0; i < sizeof(cancelPendingTransfer); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterRequestLabelTransfer::~PrinterRequestLabelTransfer()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}
 
const char *PrinterRequestLabelTransfer::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &totalDataLength;
	for (unsigned int i = 0; i < sizeof(totalDataLength); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
	pChar = (char *) &actualDataLength;
	for (unsigned int i = 0; i < sizeof(actualDataLength); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &pData;
	for (unsigned int i = 0; i < sizeof(pData); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sendSetupXferMessage;
	for (unsigned int i = 0; i < sizeof(sendSetupXferMessage); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &cancelPendingTransfer;
	for (unsigned int i = 0; i < sizeof(cancelPendingTransfer); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterRequestLabelTransfer::show()
{
	bool isTracing = Debug::isTracing("PrinterRequestLabelTransfer");

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::addTraceCategory( std::string("PrinterRequestLabelTransfer") );
#else
		Debug::addTraceCategory("PrinterRequestLabelTransfer");
#endif
	}

	Debug::trace("PrinterRequestLabelTransfer", "PrinterRequestLabelTransfer: ");

	PrinterGeneric::show();

	Debug::trace("PrinterRequestLabelTransfer", HobString("totalDataLength: ") + HobString(UInteger::toString(totalDataLength)));
	Debug::trace("PrinterRequestLabelTransfer", HobString("actualDataLength: ") + HobString(UInteger::toString(actualDataLength)));
	Debug::trace("PrinterRequestLabelTransfer", HobString("pData: 0x") + HobString(Hex::toString((long long) pData)));
	Debug::trace("PrinterRequestLabelTransfer", HobString("sendSetupXferMessage: ") + HobString(Bool::toString(sendSetupXferMessage)));
	Debug::trace("PrinterRequestLabelTransfer", HobString("cancelPendingTransfer: ") + HobString(Bool::toString(cancelPendingTransfer)));

	if (isTracing == false)
	{
#if defined(LINUX_FIX)
		Debug::removeTraceCategory( std::string("PrinterRequestLabelTransfer") );
#else
		Debug::removeTraceCategory("PrinterRequestLabelTransfer");
#endif
	}
}

UINT16 PrinterRequestLabelTransfer::getTotalDataLength()
{
	return totalDataLength;
}

void PrinterRequestLabelTransfer::setTotalDataLength(UINT16 totalDataLength)
{
	if (totalDataLength == 0)
	{
		this->totalDataLength = totalDataLength;
		this->pData = pData;
	}
	else
	{
		this->totalDataLength = totalDataLength;
	}
}

UINT16 PrinterRequestLabelTransfer::getActualDataLength()
{
	return actualDataLength;
}

void PrinterRequestLabelTransfer::setActualDataLength(UINT16 actualDataLength)
{
	if ((pData != NULL) && (totalDataLength <= actualDataLength))
	{
		this->actualDataLength = actualDataLength;
	}
	else
	{
		actualDataLength = 0;
	}
}

const UINT16 *PrinterRequestLabelTransfer::getDataPointer()
{
	return (const UINT16 *) pData;
}

void PrinterRequestLabelTransfer::setDataPointer(const UINT16 * pData)
{
	if (pData == NULL)
	{
		this->actualDataLength = 0;
		this->actualDataLength = 0;
		this->pData = NULL;
	}
	else
	{
		this->pData = (UINT16 *) pData;
	}
}

bool PrinterRequestLabelTransfer::isSendSetupXferMessage()
{
	return sendSetupXferMessage;
}

void PrinterRequestLabelTransfer::setSendSetupXferMessage()
{
	sendSetupXferMessage = true;
}

void PrinterRequestLabelTransfer::clearSendSetupXferMessage()
{
	sendSetupXferMessage = false;
}

bool PrinterRequestLabelTransfer::isCancelPendingTransfer()
{
	return cancelPendingTransfer;
}

void PrinterRequestLabelTransfer::setCancelPendingTransfer()
{
	cancelPendingTransfer = true;
}

void PrinterRequestLabelTransfer::clearCancelPendingTransfer()
{
	cancelPendingTransfer = false;
}
