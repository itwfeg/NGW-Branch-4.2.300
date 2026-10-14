//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierRequestCloseWindow.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierRequestCloseWindow::dataType = 0;
std::string LabelApplierRequestCloseWindow::dataTypeName("LabelApplierRequestCloseWindow"); 
#endif

LabelApplierRequestCloseWindow::LabelApplierRequestCloseWindow(Hobart::CEvolMsgReceiver *pHobThread, UINT8 packageId) : EvolMessage(LabelApplier, LaReqCloseWindow, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->packageId = packageId;
}

LabelApplierRequestCloseWindow::LabelApplierRequestCloseWindow(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierRequestCloseWindow::~LabelApplierRequestCloseWindow()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierRequestCloseWindow::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierRequestCloseWindow::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("LabelApplierRequestCloseWindow") );
#else
	Debug::addTraceCategory("LabelApplierRequestCloseWindow");
#endif
	Debug::trace("LabelApplierRequestCloseWindow", "LabelApplierRequestCloseWindow: ");

	EvolMessage::show();

	Debug::trace("LabelApplierRequestCloseWindow", HobString("packageId: ") + HobString(Integer::toString(packageId)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("LabelApplierRequestCloseWindow") );
#else
	Debug::removeTraceCategory("LabelApplierRequestCloseWindow");
#endif
}

UINT8 LabelApplierRequestCloseWindow::getPackageId()
{
	return packageId;
}

void LabelApplierRequestCloseWindow::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}
