//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierLabelTaken.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierLabelTaken::dataType = 0;
std::string LabelApplierLabelTaken::dataTypeName("LabelApplierLabelTaken"); 
#endif

LabelApplierLabelTaken::LabelApplierLabelTaken(Hobart::CEvolMsgReceiver *pHobThread, LaStationID stationId, UINT8 packageId) : EvolMessage(LabelApplier, LaLabelTaken, pHobThread), 
																															   stationId(stationId), packageId(packageId), commSwitchToPrimary(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

LabelApplierLabelTaken::LabelApplierLabelTaken(const char *buffer) : EvolMessage(buffer), commSwitchToPrimary(false)
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
	char *pChar = (char *) &stationId;
	for (unsigned int i = 0; i < sizeof(stationId); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		*pChar++ = buffer[j++];
	}
}


LabelApplierLabelTaken::LabelApplierLabelTaken(const LabelApplierLabelTaken& lalt)
	: EvolMessage(LabelApplier, LaLabelTaken), stationId(lalt.stationId), packageId(lalt.packageId)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

LabelApplierLabelTaken::~LabelApplierLabelTaken()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierLabelTaken::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &stationId;
	for (unsigned int i = 0; i < sizeof(stationId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &packageId;
	for (unsigned int i = 0; i < sizeof(packageId); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierLabelTaken::show()
{
	if (Debug::isTracing("LabelApplierLabelTaken"))
	{
		Debug::trace("LabelApplierLabelTaken", "LabelApplierLabelTaken: ");

		EvolMessage::show();
		if (stationId == WeigherStation)
		{
			Debug::trace("LabelApplierLabelTaken", "transferStationType: WeigherStation");
		}
		else if (stationId == PrimaryPrinterStation)
		{
			Debug::trace("LabelApplierLabelTaken", "transferStationType: PrimaryPrinterStation");
		}
		else if (stationId == SecondaryPrinterStation)
		{
			Debug::trace("LabelApplierLabelTaken", "transferStationType: SecondaryPrinterStation");
		}
		else
		{
			Debug::trace("LabelApplierLabelTaken", "stationId: Unknown: " + HobString(Integer::toString((UINT16) stationId)));
		}

		Debug::trace("LabelApplierLabelTaken", "          packageId: " + HobString(Integer::toString(packageId)));
		Debug::trace("LabelApplierLabelTaken", "commSwitchToPrimary: " + HobString(Bool::toString(commSwitchToPrimary)));
	}
}

void LabelApplierLabelTaken::setStationId(LaStationID stationId)
{
	this->stationId = stationId;
}

LaStationID LabelApplierLabelTaken::getStationId()
{
	return stationId;
}

void LabelApplierLabelTaken::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}

UINT8 LabelApplierLabelTaken::getPackageId()
{
	return packageId;
}

bool LabelApplierLabelTaken::isCommSwitchToPrimary()
{
	return commSwitchToPrimary;
}

void LabelApplierLabelTaken::setCommSwitchToPrimary(bool value)
{
	commSwitchToPrimary = value;
}
