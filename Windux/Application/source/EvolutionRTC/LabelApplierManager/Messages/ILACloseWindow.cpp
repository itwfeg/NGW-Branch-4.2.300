//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "ILACloseWindow.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ILACloseWindow::dataType = 0;
std::string ILACloseWindow::dataTypeName("ILACloseWindow"); 
#endif

ILACloseWindow::ILACloseWindow(Hobart::CEvolMsgReceiver *pHobThread, UINT8 packageId) : EvolMessage(LabelApplier, LaILACloseWindow, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->packageId = packageId;
}

ILACloseWindow::ILACloseWindow(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		*pChar++ = buffer[j++];
	}
}

ILACloseWindow::~ILACloseWindow()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ILACloseWindow::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void ILACloseWindow::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("ILACloseWindow") );
#else
	Debug::addTraceCategory("ILACloseWindow");
#endif
	Debug::trace("ILACloseWindow", "ILACloseWindow: ");

	EvolMessage::show();

	Debug::trace("ILACloseWindow", HobString("packageId: ") + HobString(Integer::toString(packageId)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("ILACloseWindow") );
#else
	Debug::removeTraceCategory("ILACloseWindow");
#endif
}

UINT8 ILACloseWindow::getPackageId()
{
	return packageId;
}

void ILACloseWindow::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}
