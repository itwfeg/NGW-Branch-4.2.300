//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierPrint.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierPrint::dataType = 0;
std::string LabelApplierPrint::dataTypeName("LabelApplierPrint"); 
#endif

LabelApplierPrint::LabelApplierPrint(Hobart::CEvolMsgReceiver *pHobThread, UINT8 packageId) : EvolMessage(LabelApplier, LaPrintPackage, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->packageId = packageId;
}

LabelApplierPrint::LabelApplierPrint(const char *buffer) : EvolMessage(buffer)
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

LabelApplierPrint::~LabelApplierPrint()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierPrint::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierPrint::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("LabelApplierPrint") );
#else
	Debug::addTraceCategory("LabelApplierPrint");
#endif
	Debug::trace("LabelApplierPrint", "LabelApplierPrint: ");

	EvolMessage::show();

	Debug::trace("LabelApplierPrint", HobString("packageId: ") + HobString(Integer::toString(packageId)));

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("LabelApplierPrint") );
#else
	Debug::removeTraceCategory("LabelApplierPrint");
#endif
}

UINT8 LabelApplierPrint::getPackageId()
{
	return packageId;
}

void LabelApplierPrint::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}
