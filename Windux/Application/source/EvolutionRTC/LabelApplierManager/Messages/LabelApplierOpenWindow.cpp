//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierOpenWindow.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierOpenWindow::dataType = 0;
std::string LabelApplierOpenWindow::dataTypeName("LabelApplierOpenWindow"); 
#endif

LabelApplierOpenWindow::LabelApplierOpenWindow(Hobart::CEvolMsgReceiver *pHobThread, UINT8 packageId) : EvolMessage(LabelApplier, LaOpenWindow, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	this->packageId = packageId;
}

LabelApplierOpenWindow::LabelApplierOpenWindow(const char *buffer) : EvolMessage(buffer)
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

LabelApplierOpenWindow::~LabelApplierOpenWindow()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierOpenWindow::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierOpenWindow::show()
{
	if (Debug::isTracing("LabelApplierCloseWindow"))
	{
		Debug::trace("LabelApplierOpenWindow", "LabelApplierOpenWindow: ");

		EvolMessage::show();

		Debug::trace("LabelApplierOpenWindow", HobString("packageId: ") +
												HobString(Integer::toString(packageId)));
	}
}

UINT8 LabelApplierOpenWindow::getPackageId()
{
	return packageId;
}

void LabelApplierOpenWindow::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}
