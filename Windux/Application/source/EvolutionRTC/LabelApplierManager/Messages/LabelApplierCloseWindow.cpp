//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierCloseWindow.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierCloseWindow::dataType = 0;
std::string LabelApplierCloseWindow::dataTypeName("LabelApplierCloseWindow"); 
#endif

LabelApplierCloseWindow::LabelApplierCloseWindow(Hobart::CEvolMsgReceiver *pHobThread, UINT8 packageId) : EvolMessage(LabelApplier, LaCloseWindow, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->packageId = packageId;
}

LabelApplierCloseWindow::LabelApplierCloseWindow(const char *buffer) : EvolMessage(buffer)
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

LabelApplierCloseWindow::~LabelApplierCloseWindow()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierCloseWindow::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierCloseWindow::show()
{
	if (Debug::isTracing("LabelApplierCloseWindow"))
	{
		Debug::trace("LabelApplierCloseWindow", "LabelApplierCloseWindow: ");

		EvolMessage::show();

		Debug::trace("LabelApplierCloseWindow", HobString("packageId: ") +
												HobString(Integer::toString(packageId)));
	}
}

UINT8 LabelApplierCloseWindow::getPackageId()
{
	return packageId;
}

void LabelApplierCloseWindow::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}
