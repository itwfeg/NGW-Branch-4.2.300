//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierPackageReady.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierPackageReady::dataType = 0;
std::string LabelApplierPackageReady::dataTypeName("LabelApplierPackageReady"); 
#endif

LabelApplierPackageReady::LabelApplierPackageReady(Hobart::CEvolMsgReceiver *pHobThread, UINT8 packageId) : EvolMessage(LabelApplier, LaPackageReady, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->packageId = packageId;
}

LabelApplierPackageReady::LabelApplierPackageReady(const char *buffer) : EvolMessage(buffer)
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

LabelApplierPackageReady::~LabelApplierPackageReady()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierPackageReady::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierPackageReady::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("LabelApplierPackageReady") );
#else
	Debug::addTraceCategory("LabelApplierPackageReady");
#endif
	Debug::trace("LabelApplierPackageReady", "LabelApplierPackageReady: ");

	EvolMessage::show();

	Debug::trace("LabelApplierPackageReady", HobString("packageId: ") + HobString(Integer::toString(packageId)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("LabelApplierPackageReady") );
#else
	Debug::removeTraceCategory("LabelApplierPackageReady");
#endif
}

void LabelApplierPackageReady::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}

UINT8 LabelApplierPackageReady::getPackageId()
{
	return packageId;
}
