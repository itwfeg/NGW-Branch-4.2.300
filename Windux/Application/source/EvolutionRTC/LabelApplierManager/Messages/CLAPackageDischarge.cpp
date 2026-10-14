#include "LabelApplierEvent.h"
#include "CLAPackageDischarge.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CLAPackageDischarge::dataType = 0;
std::string CLAPackageDischarge::dataTypeName("CLAPackageDischarge"); 
#endif

CLAPackageDischarge::CLAPackageDischarge(Hobart::CEvolMsgReceiver *pHobThread, UINT8 packageId) : EvolMessage(LabelApplier, LaPackageDischarge, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	this->packageId = packageId;
}

CLAPackageDischarge::CLAPackageDischarge(const char *buffer) : EvolMessage(buffer)
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

CLAPackageDischarge::~CLAPackageDischarge()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *CLAPackageDischarge::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void CLAPackageDischarge::show()
{
	if (Debug::isTracing("CLAPackageDischarge"))
	{
		Debug::trace("CLAPackageDischarge", "CLAPackageDischarge: ");

		EvolMessage::show();

		Debug::trace("CLAPackageDischarge", HobString("packageId: ") +
												HobString(Integer::toString(packageId)));
	}
}

UINT8 CLAPackageDischarge::getPackageId()
{
	return packageId;
}

void CLAPackageDischarge::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}
