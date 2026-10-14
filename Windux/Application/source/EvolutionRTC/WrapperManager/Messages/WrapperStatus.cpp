
#include "WrapperEvent.h"
#include "WrapperStatus.h"

#include "Debug.h"
#include "Integer.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int WrapperStatus::dataType = 0;
std::string WrapperStatus::dataTypeName("WrapperStatus"); 
#endif

WrapperStatus::WrapperStatus(Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(Wrapper, WrStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	operatingMode = WrOperateMode;
	wrapperEvent = WrapperCount000Event;

	peripheralComm = true;
	managerDeleted = false;
}

WrapperStatus::WrapperStatus(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	statusInfo = WrapperStatusInfo(&buffer[j]);

	statusInfo.serialize();
	j += statusInfo.getSerializedLength();

	errorInfo = WrapperErrorInfo(&buffer[j]);

	errorInfo.serialize();
	j += errorInfo.getSerializedLength();

	historyInfo = WrapperErrorInfo(&buffer[j]);

	historyInfo.serialize();
	j += historyInfo.getSerializedLength();

	char *pChar = (char *) &operatingMode;
	for (unsigned int i = 0; i < sizeof(operatingMode); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &wrapperEvent;
	for (unsigned int i = 0; i < sizeof(wrapperEvent); i++)
	{
		*pChar++ = buffer[j++];
	}

	heaterInfo = WrapperHeaterInfo(&buffer[j]);

	heaterInfo.serialize();
	j += heaterInfo.getSerializedLength();

	for (int i = 0; i < NumWrapperPositions; i++)
	{
		packageInfo[i] = WrapperPackageInfo(&buffer[j]);

		packageInfo[i].serialize();
		j += packageInfo[i].getSerializedLength();
	}

	pChar = (char *) &peripheralComm;
	for (unsigned int i = 0; i < sizeof(peripheralComm); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &managerDeleted;
	for (unsigned int i = 0; i < sizeof(managerDeleted); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapperStatus::~WrapperStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *WrapperStatus::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	const char *pCC = statusInfo.serialize();
	int mLength = statusInfo.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = errorInfo.serialize();
	mLength = errorInfo.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = historyInfo.serialize();
	mLength = historyInfo.getSerializedLength();

	serializedData.append(pCC, mLength);

	pCC = (char *) &operatingMode;
	for (unsigned int i = 0; i < sizeof(operatingMode); i++)
	{
		serializedData.append(1, pCC[i]);
	}

	pCC = (char *) &wrapperEvent;
	for (unsigned int i = 0; i < sizeof(wrapperEvent); i++)
	{
		serializedData.append(1, pCC[i]);
	}

	pCC = heaterInfo.serialize();
	mLength = heaterInfo.getSerializedLength();

	serializedData.append(pCC, mLength);

	for (int i = 0; i < NumWrapperPositions; i++)
	{
		pCC = packageInfo[i].serialize();
		mLength = packageInfo[i].getSerializedLength();

		serializedData.append(pCC, mLength);
	}

	pCC = (char *) &peripheralComm;
	for (unsigned int i = 0; i < sizeof(peripheralComm); i++)
	{
		serializedData.append(1, pCC[i]);
	}

	pCC = (char *) &managerDeleted;
	for (unsigned int i = 0; i < sizeof(managerDeleted); i++)
	{
		serializedData.append(1, pCC[i]);
	}

	return serializedData.data();
}

void WrapperStatus::show()
{
	Debug::addTraceCategory("WrapperStatus");
	Debug::trace("WrapperStatus", "WrapperStatus: ");

	EvolMessage::show();

	Debug::trace("WrapperStatus", "statusInfo: ");
	statusInfo.show();

	Debug::trace("WrapperStatus", "errorInfo: ");
	errorInfo.show();

	Debug::trace("WrapperStatus", "historyInfo: ");
	historyInfo.show();

	if (operatingMode == WrOperateMode)
	{
		Debug::trace("WrapperStatus", "operatingMode: WrOperateMode");
	}
	else if (operatingMode == WrServiceMode)
	{
		Debug::trace("WrapperStatus", "operatingMode: WrServiceMode");
	}
	else if (operatingMode == WrTestMode)
	{
		Debug::trace("WrapperStatus", "operatingMode: WrTestMode");
	}
	else if (operatingMode == WrMfgMode)
	{
		Debug::trace("WrapperStatus", "operatingMode: WrMfgMode");
	}
	else if (operatingMode == WrContinuousMode)
	{
		Debug::trace("WrapperStatus", "operatingMode: WrContinuousMode");
	}
	else if (operatingMode == WrBoardMfgMode)
	{
		Debug::trace("WrapperStatus", "operatingMode: WrBoardMfgMode");
	}
	else
	{
		Debug::trace("WrapperStatus", HobString("operatingMode: Unknown ") +
									  HobString(Integer::toString((UINT16) operatingMode)));
	}

	Debug::trace("WrapperStatus", "heaterInfo: ");
	heaterInfo.show();

	Debug::trace("WrapperStatus", HobString("IntakePosition: "));
	packageInfo[0].show();
	Debug::trace("WrapperStatus", HobString("ElevatorPosition: "));
	packageInfo[1].show();
	Debug::trace("WrapperStatus", HobString("SealerBeltPosition: "));
	packageInfo[2].show();
	Debug::trace("WrapperStatus", HobString("DischargePosition: "));
	packageInfo[3].show();

	Debug::trace("WrapperStatus", HobString("peripheralComm: ") + HobString(Bool::toString(peripheralComm)));
	Debug::trace("WrapperStatus", HobString("managerDeleted: ") + HobString(Bool::toString(managerDeleted)));

	Debug::removeTraceCategory("WrapperStatus");
}

void WrapperStatus::setStatusInfo(WrapperStatusInfo &statusInfo)
{
	this->statusInfo = statusInfo;
}

WrapperStatusInfo &WrapperStatus::getStatusInfo()
{
	return statusInfo;
}

const WrapperStatusInfo& WrapperStatus::getStatusInfo() const
{
	return statusInfo;
}

void WrapperStatus::setErrorInfo(WrapperErrorInfo &errorInfo)
{
	this->errorInfo = errorInfo;
}

WrapperErrorInfo &WrapperStatus::getErrorInfo()
{
	return errorInfo;
}

void WrapperStatus::setHistoryInfo(WrapperErrorInfo &historyInfo)
{
	this->historyInfo = historyInfo;
}

WrapperErrorInfo &WrapperStatus::getHistoryInfo()
{
	return historyInfo;
}

void WrapperStatus::setOperatingMode(WrOpMode operatingMode)
{
	this->operatingMode = operatingMode;
}

WrOpMode WrapperStatus::getOperatingMode()
{
	return operatingMode;
}

void WrapperStatus::setWrapperEvent(WrapperEvent wrapperEvent)
{
	this->wrapperEvent = wrapperEvent;
}

WrapperEvent WrapperStatus::getWrapperEvent()
{
	return wrapperEvent;
}

void WrapperStatus::setHeaterInfo(WrapperHeaterInfo &heaterInfo)
{
	this->heaterInfo = heaterInfo;
}

WrapperHeaterInfo &WrapperStatus::getHeaterInfo()
{
	return heaterInfo;
}

const WrapperHeaterInfo &WrapperStatus::getHeaterInfo() const
{
	return heaterInfo;
}

void WrapperStatus::setPackageInfo(WrapperPackageInfo &packageInfo, UINT16 index)
{
	this->packageInfo[index] = packageInfo;
}

WrapperPackageInfo &WrapperStatus::getPackageInfo(UINT16 index)
{
		return packageInfo[index];
}

bool WrapperStatus::isPeripheralComm()
{
	return peripheralComm;
}

void WrapperStatus::setPeripheralComm(bool value)
{
	peripheralComm = value;
}

void WrapperStatus::clearPeripheralComm()
{
	peripheralComm = false;
}

bool WrapperStatus::isManagerDeleted()
{
	return managerDeleted;
}

void WrapperStatus::setManagerDeleted()
{
	managerDeleted = true;
}

void WrapperStatus::clearManagerDeleted()
{
	managerDeleted = false;
}

bool WrapperStatus::equals(WrapperStatus &wrStatus)
{
	bool rv = true;

	if ((statusInfo.equals(wrStatus.getStatusInfo()) == false) ||
		(errorInfo.equals(wrStatus.getErrorInfo()) == false) ||
		(historyInfo.equals(wrStatus.getHistoryInfo()) == false) ||
		(operatingMode != wrStatus.getOperatingMode()) ||
		(wrapperEvent != wrStatus.getWrapperEvent()) ||
		(heaterInfo.equals(wrStatus.getHeaterInfo()) == false) ||
		(peripheralComm != wrStatus.isPeripheralComm()) ||
		(managerDeleted != wrStatus.isManagerDeleted()))
	{
		rv = false;
	}

	UINT16 i = 0;
	while ((i < NumWrapperPositions) && (rv == true))
	{
		if (packageInfo[i].equals(wrStatus.getPackageInfo(i)) == false)
		{
			rv = false;
		}
		i++;
	}

	return rv;
}

