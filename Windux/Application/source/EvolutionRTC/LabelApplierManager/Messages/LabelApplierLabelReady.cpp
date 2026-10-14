#include "LabelApplierEvent.h"
#include "LabelApplierLabelReady.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierLabelReady::dataType = 0;
std::string LabelApplierLabelReady::dataTypeName("LabelApplierLabelReady"); 
#endif

LabelApplierLabelReady::LabelApplierLabelReady(Hobart::CEvolMsgReceiver *pHobThread, LaStationID stationId, UINT8 packageId) : EvolMessage(LabelApplier, LaLabelReady, pHobThread), 
																															   stationId(stationId), packageId(packageId), commSwitchToPrimary(false)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

LabelApplierLabelReady::LabelApplierLabelReady(const char *buffer) : EvolMessage(buffer), commSwitchToPrimary(false)
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


LabelApplierLabelReady::LabelApplierLabelReady(const LabelApplierLabelReady& lalr)
	: EvolMessage(LabelApplier, LaLabelReady), stationId(lalr.stationId), packageId(lalr.packageId)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}


LabelApplierLabelReady::~LabelApplierLabelReady()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierLabelReady::serialize() const
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

void LabelApplierLabelReady::show()
{
	if (Debug::isTracing("LabelApplierLabelReady"))
	{
		Debug::trace("LabelApplierLabelReady", "LabelApplierLabelReady: ");

		EvolMessage::show();
		if (stationId == WeigherStation)
		{
			Debug::trace("LabelApplierLabelReady", "transferStationType: WeigherStation");
		}
		else if (stationId == PrimaryPrinterStation)
		{
			Debug::trace("LabelApplierLabelReady", "transferStationType: PrimaryPrinterStation");
		}
		else if (stationId == SecondaryPrinterStation)
		{
			Debug::trace("LabelApplierLabelReady", "transferStationType: SecondaryPrinterStation");
		}
		else
		{
			Debug::trace("LabelApplierLabelReady", "stationId: Unknown: " + HobString(Integer::toString((UINT16) stationId)));
		}

		Debug::trace("LabelApplierLabelReady", "          packageId: " + HobString(Integer::toString(packageId)));
		Debug::trace("LabelApplierLabelReady", "commSwitchToPrimary: " + HobString(Bool::toString(commSwitchToPrimary)));
	}
}

void LabelApplierLabelReady::setStationId(LaStationID stationId)
{
	this->stationId = stationId;
}

LaStationID LabelApplierLabelReady::getStationId()
{
	return stationId;
}

void LabelApplierLabelReady::setPackageId(UINT8 packageId)
{
	this->packageId = packageId;
}

UINT8 LabelApplierLabelReady::getPackageId()
{
	return packageId;
}


bool LabelApplierLabelReady::isCommSwitchToPrimary()
{
	return commSwitchToPrimary;
}

void LabelApplierLabelReady::setCommSwitchToPrimary(bool value)
{
	commSwitchToPrimary = value;
}

