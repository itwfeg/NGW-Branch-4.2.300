//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "ILAOpenWindow.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int ILAOpenWindow::dataType = 0;
std::string ILAOpenWindow::dataTypeName("ILAOpenWindow"); 
#endif

ILAOpenWindow::ILAOpenWindow(Hobart::CEvolMsgReceiver *pHobThread, UINT8 packageId) : EvolMessage(LabelApplier, LaILAOpenWindow, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->packageId = packageId;
}

ILAOpenWindow::ILAOpenWindow(const char *buffer) : EvolMessage(buffer)
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

ILAOpenWindow::~ILAOpenWindow()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *ILAOpenWindow::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void ILAOpenWindow::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("ILAOpenWindow") );
#else
	Debug::addTraceCategory("ILAOpenWindow");
#endif
	Debug::trace("ILAOpenWindow", "ILAOpenWindow: ");

	EvolMessage::show();

	Debug::trace("ILAOpenWindow", HobString("packageId: ") + HobString(Integer::toString(packageId)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("ILAOpenWindow") );
#else
	Debug::removeTraceCategory("ILAOpenWindow");
#endif
}

UINT8 ILAOpenWindow::getPackageId()
{
	return packageId;
}

void ILAOpenWindow::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}
