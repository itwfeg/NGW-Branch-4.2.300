//#include "vxWorks.h"
#include "PrinterStatus.h"

#include "Debug.h"
#include "Hex.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int PrinterStatus::dataType = 0;
std::string PrinterStatus::dataTypeName("PrinterStatus"); 
#endif

PrinterStatus::PrinterStatus(const PrinterStatus& obj)
	: PrinterGeneric(obj), 
	labelReady(obj.labelReady), labelTakenError(obj.labelTakenError), takeLabelError(obj.takeLabelError), labelSizing(obj.labelSizing),
	labelSizingFailed(obj.labelSizingFailed), peripheralComm(obj.peripheralComm), managerDeleted(obj.managerDeleted),
	state(obj.state), command(obj.command), error(obj.error), history(obj.history), sensor(obj.sensor), sensor2(obj.sensor2),
	user(obj.user), sensorMask(obj.sensorMask), sensor2Mask(obj.sensor2Mask), userMask(obj.userMask), counter(obj.counter)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

PrinterStatus::PrinterStatus(Hobart::CEvolMsgReceiver *pHobThread, PrinterType printerType)
	: PrinterGeneric(PrStatus, pHobThread, printerType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	labelReady        = false;
	labelTakenError   = false;
	takeLabelError    = false;
	labelSizing       = false;
	labelSizingFailed = false;
	peripheralComm    = true;
	managerDeleted    = false;

	state       = 0;
	command     = 0;
	error       = 0;
	history     = 0;
	sensor      = 0;
	sensor2     = 0;
	user        = 0;
	counter     = 0;
	sensorMask  = 0;
	sensor2Mask = 0;
	userMask    = 0;
}

PrinterStatus::PrinterStatus(const char *buffer) : PrinterGeneric(buffer)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	// index into buffer
	PrinterGeneric::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &labelReady;
	for (unsigned int i = 0; i < sizeof(labelReady); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelTakenError;
	for (unsigned int i = 0; i < sizeof(labelTakenError); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &takeLabelError;
	for (unsigned int i = 0; i < sizeof(takeLabelError); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelSizing;
	for (unsigned int i = 0; i < sizeof(labelSizing); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &labelSizingFailed;
	for (unsigned int i = 0; i < sizeof(labelSizingFailed); i++)
	{
		*pChar++ = buffer[j++];
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

	pChar = (char *) &state;
	for (unsigned int i = 0; i < sizeof(state); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &command;
	for (unsigned int i = 0; i < sizeof(command); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &error;
	for (unsigned int i = 0; i < sizeof(error); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &history;
	for (unsigned int i = 0; i < sizeof(history); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sensor;
	for (unsigned int i = 0; i < sizeof(sensor); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sensor2;
	for (unsigned int i = 0; i < sizeof(sensor2); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &user;
	for (unsigned int i = 0; i < sizeof(user); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sensorMask;
	for (unsigned int i = 0; i < sizeof(sensorMask); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &sensor2Mask;
	for (unsigned int i = 0; i < sizeof(sensor2Mask); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &userMask;
	for (unsigned int i = 0; i < sizeof(userMask); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &counter;
	for (unsigned int i = 0; i < sizeof(counter); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PrinterStatus::~PrinterStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

PrinterStatus& PrinterStatus::operator=(const PrinterStatus& rhs)
{
	PrinterGeneric::operator=(rhs); 
	labelReady = rhs.labelReady;
	labelTakenError = rhs.labelTakenError;
	takeLabelError = rhs.takeLabelError;
	labelSizing = rhs.labelSizing;
	labelSizingFailed = rhs.labelSizingFailed;
	peripheralComm = rhs.peripheralComm;
	managerDeleted = rhs.managerDeleted;
	state = rhs.state;
	command = rhs.command;
	error = rhs.error;
	history = rhs.history;
	sensor = rhs.sensor;
	sensor2 = rhs.sensor2;
	user = rhs.user;
	sensorMask = rhs.sensorMask;
	sensor2Mask = rhs.sensor2Mask;
	userMask = rhs.userMask;
	counter = rhs.counter;
	return *this;
}


const char *PrinterStatus::serialize() const
{
	PrinterGeneric::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &labelReady;
	for (unsigned int i = 0; i < sizeof(labelReady); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelTakenError;
	for (unsigned int i = 0; i < sizeof(labelTakenError); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &takeLabelError;
	for (unsigned int i = 0; i < sizeof(takeLabelError); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelSizing;
	for (unsigned int i = 0; i < sizeof(labelSizing); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &labelSizingFailed;
	for (unsigned int i = 0; i < sizeof(labelSizingFailed); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &peripheralComm;
	for (unsigned int i = 0; i < sizeof(peripheralComm); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &managerDeleted;
	for (unsigned int i = 0; i < sizeof(managerDeleted); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &state;
	for (unsigned int i = 0; i < sizeof(state); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &command;
	for (unsigned int i = 0; i < sizeof(command); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &error;
	for (unsigned int i = 0; i < sizeof(error); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &history;
	for (unsigned int i = 0; i < sizeof(history); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sensor;
	for (unsigned int i = 0; i < sizeof(sensor); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sensor2;
	for (unsigned int i = 0; i < sizeof(sensor2); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &user;
	for (unsigned int i = 0; i < sizeof(user); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sensorMask;
	for (unsigned int i = 0; i < sizeof(sensorMask); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &sensor2Mask;
	for (unsigned int i = 0; i < sizeof(sensor2Mask); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &userMask;
	for (unsigned int i = 0; i < sizeof(userMask); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &counter;
	for (unsigned int i = 0; i < sizeof(counter); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PrinterStatus::show() const
{
	if (!Debug::isTracing( "PrinterStatus" ))
		return;

	Debug::trace("PrinterStatus", "PrinterStatus: ");

	PrinterGeneric::show();

	Debug::trace("PrinterStatus", HobString("labelReady: ") +
								  HobString(Bool::toString(labelReady)));
	Debug::trace("PrinterStatus", HobString("labelTakenError: ") +
								  HobString(Bool::toString(labelTakenError)));
	Debug::trace("PrinterStatus", HobString("takeLabelError: ") +
								  HobString(Bool::toString(takeLabelError)));
	Debug::trace("PrinterStatus", HobString("labelSizing: ") +
								  HobString(Bool::toString(labelSizing)));
	Debug::trace("PrinterStatus", HobString("labelSizingFailed: ") +
								  HobString(Bool::toString(labelSizingFailed)));
	Debug::trace("PrinterStatus", HobString("peripheralComm: ") +
								  HobString(Bool::toString(peripheralComm)));
	Debug::trace("PrinterStatus", HobString("managerDeleted: ") +
								  HobString(Bool::toString(managerDeleted)));

	Debug::trace("PrinterStatus", HobString("state: 0x") +
								  HobString(Hex::toString(state)));
	Debug::trace("PrinterStatus", HobString("command: 0x") +
								  HobString(Hex::toString(command)));
	Debug::trace("PrinterStatus", HobString("error: 0x") +
								  HobString(Hex::toString(error)));
	Debug::trace("PrinterStatus", HobString("history: 0x") +
								  HobString(Hex::toString(history)));
	Debug::trace("PrinterStatus", HobString("sensor: 0x") +
								  HobString(Hex::toString(sensor)));
	Debug::trace("PrinterStatus", HobString("sensor2: 0x") +
								  HobString(Hex::toString(sensor2)));
	Debug::trace("PrinterStatus", HobString("user: 0x") +
								  HobString(Hex::toString(user)));
	Debug::trace("PrinterStatus", HobString("counter: 0x") +
								  HobString(Hex::toString(counter)));
	Debug::trace("PrinterStatus", HobString("sensorMask: 0x") +
								  HobString(Hex::toString(sensorMask)));
	Debug::trace("PrinterStatus", HobString("sensor2Mask: 0x") +
								  HobString(Hex::toString(sensor2Mask)));
	Debug::trace("PrinterStatus", HobString("userMask: 0x") +
								  HobString(Hex::toString(userMask)));

	Debug::trace("PrinterStatus", HobString("WideLabelBit: ") +
								  HobString(Bool::toString(isWideLabel())));
	Debug::trace("PrinterStatus", HobString("takingBit: ") +
								  HobString(Bool::toString(isTaking())));
	Debug::trace("PrinterStatus", HobString("labelTakenBit: ") +
								  HobString(Bool::toString(isLabelTaken())));
	Debug::trace("PrinterStatus", HobString("labelMissingBit: ") +
								  HobString(Bool::toString(isLabelMissing())));
	Debug::trace("PrinterStatus", HobString("outOfMediaBit: ") +
								  HobString(Bool::toString(isOutOfMedia())));
	Debug::trace("PrinterStatus", HobString("backupCmdCompleta: ") +
								  HobString(Bool::toString(isBackupCmdComplete())));
	Debug::trace("PrinterStatus", HobString("advanceCmdComplete: ") +
								  HobString(Bool::toString(isAdvanceCmdComplete())));
	Debug::trace("PrinterStatus", HobString("printCmdComplete: ") +
								  HobString(Bool::toString(isPrintCmdComplete())));
	Debug::trace("PrinterStatus", HobString("ejectCmdComplete: ") +
								  HobString(Bool::toString(isEjectCmdComplete())));
	Debug::trace("PrinterStatus", HobString("syncCmdComplete: ") +
								  HobString(Bool::toString(isSyncCmdComplete())));
	Debug::trace("PrinterStatus", HobString("findCmdComplete: ") +
								  HobString(Bool::toString(isFindCmdComplete())));
	Debug::trace("PrinterStatus", HobString("sizeCmdComplete: ") +
								  HobString(Bool::toString(isSizeCmdComplete())));
	Debug::trace("PrinterStatus", HobString("resetCmdComplete: ") +
								  HobString(Bool::toString(isResetCmdComplete())));
	Debug::trace("PrinterStatus", HobString("calibrateCmdComplete: ") +
								  HobString(Bool::toString(isCalibrateCmdComplete())));
	Debug::trace("PrinterStatus", HobString("FindwoLabelTakenCmdComplete: ") +
								  HobString(Bool::toString(isFindwoLabelTakenCmdComplete())));
}

bool PrinterStatus::isLabelReady() const
{
	return labelReady;
}

void PrinterStatus::setLabelReady()
{
	labelReady = true;
}

void PrinterStatus::clearLabelReady()
{
	labelReady = false;
}

void PrinterStatus::setLabelTaken()
{
	sensor |= LabelTakenBit;
}

void PrinterStatus::clearLabelTaken()
{
	sensor &= ~LabelTakenBit;
}

bool PrinterStatus::isLabelTaken() const
{
	return ((sensor & LabelTakenBit) == LabelTakenBit);
}

bool PrinterStatus::isTaking() const
{
	return ((user & TakingBit) == TakingBit);
}

bool PrinterStatus::isPriorityError() const
{
	bool rv = false;

	if (/* (isLabelMissing() == true) || */
		(isHeadDisengaged() == true) ||
		(isThermalOverload() == true) ||
		(isThermalShutdown() == true) ||
		(isMediaJammed() == true) ||
		(isOutOfMedia() == true) ||
		(isMediaShutdown() == true) ||
		(isHardwareError() == true) ||
		(isCassetteMissing() == true))
	{
		rv = true;
	}

	return rv;
}

bool PrinterStatus::isBackupCmdComplete() const
{
	return (command == BackupCmdComplete);
}

bool PrinterStatus::isAdvanceCmdComplete() const
{
	return (command == AdvanceCmdComplete);
}

bool PrinterStatus::isPrintCmdComplete() const
{
	return (command == PrintCmdComplete);
}

bool PrinterStatus::isEjectCmdComplete() const
{
	return (command == EjectCmdComplete);
}

bool PrinterStatus::isSyncCmdComplete() const
{
	return (command == SyncCmdComplete);
}

bool PrinterStatus::isFindCmdComplete() const
{
	return (command == FindCmdComplete);
}

bool PrinterStatus::isFindwoLabelTakenCmdComplete() const
{
	return (command == FindwoLabelTakenCmdComplete);
}

bool PrinterStatus::isSizeCmdComplete() const
{
	return (command == SizeCmdComplete || command == CutRetractCmdComplete);
}

bool PrinterStatus::isResetCmdComplete() const
{
	return (command == ResetCmdComplete);
}

bool PrinterStatus::isCutRetractCmdComplete() const
{
	return (command == CutRetractCmdComplete );
}

bool PrinterStatus::isCalibrateCmdComplete() const
{
	return (command == CalibrateCmdComplete);
}

void PrinterStatus::setCalibrateCmdComplete()
{
	command = CalibrateCmdComplete;
}

bool PrinterStatus::isVirtualLabelComplete() const
{
	return ( (command == VirtualLabelTakenComplete ) || (command == 0 ) );
}	


// Printer Error Checks!!!
bool PrinterStatus::isHeadDisengaged() const
{
	return ((error & PrHeadUpBit) == PrHeadUpBit);
}

void PrinterStatus::setHeadDisengaged(bool disengaged)
{
	if (disengaged)
	{
		error |= PrHeadUpBit;
	}
	else
	{
		error &= ~PrHeadUpBit;
	}
}


bool PrinterStatus::isThermalShutdown() const
{
	return ((error & PrThermalShutdownBit) == PrThermalShutdownBit);
}

bool PrinterStatus::isMediaShutdown() const
{
	return ((error & PrMediaShutdownBit) == PrMediaShutdownBit);
}

bool PrinterStatus::isCassetteMissing() const
{
	return ((error & PrCassetteMissingBit) == PrCassetteMissingBit);
}

void PrinterStatus::setCassetteMissing(bool cassetteMissing)
{
	if (cassetteMissing)
	{
		error |= PrCassetteMissingBit;
	}
	else
	{
		error &= ~PrCassetteMissingBit;
	}
}

bool PrinterStatus::isHardwareError() const
{
	return ((error & PrHardwareErrorBit) == PrHardwareErrorBit);
}

bool PrinterStatus::isOutOfMedia() const
{
	Debug::trace("PrinterStatus", HobString("isOutOfMedia(): ") +
								  HobString(Bool::toString((sensor & OutOfMediaBit) == OutOfMediaBit)));
	return ((sensor & OutOfMediaBit) == OutOfMediaBit);
}

void PrinterStatus::setOutOfMedia(bool outOfMedia)
{
	if (outOfMedia)
	{
		sensor |= OutOfMediaBit;
	}
	else
	{
		sensor &= ~OutOfMediaBit;
	}
}

bool PrinterStatus::isMediaJammed() const
{
	Debug::trace("PrinterStatus", HobString("isMediaJammed(): ") +
								  HobString(Bool::toString((user & MediaJammedBit) == MediaJammedBit)));
	return ((user & MediaJammedBit) == MediaJammedBit);
}

void PrinterStatus::clearMediaJammed()
{
	user &= ~MediaJammedBit;
}

bool PrinterStatus::isLabelMissing() const
{
	return ((user & LabelMissingBit) == LabelMissingBit);
}

void PrinterStatus::clearLabelMissing()
{
	user &= ~LabelMissingBit;
}

bool PrinterStatus::isThermalOverload() const
{
	Debug::trace("PrinterStatus", HobString("isThermalOverload(): ") +
								  HobString(Bool::toString((sensor & ThermalOverloadBit) == ThermalOverloadBit)));
	return ((sensor & ThermalOverloadBit) == ThermalOverloadBit);
}

void PrinterStatus::setLabelWidth(LabelWidth labelWidth)
{
	if (labelWidth == WideLabel)
	{
		sensor |= WideLabelBit;
	}
	else if (labelWidth == NarrowLabel)
	{
		sensor &= ~WideLabelBit;
	}
}

bool PrinterStatus::isWideLabel() const
{
	return ((sensor & WideLabelBit) == WideLabelBit);
}

bool PrinterStatus::isNarrowLabel() const
{
	return ((sensor & WideLabelBit) != WideLabelBit);
}

bool PrinterStatus::isOutOfDataBuffers() const
{
	return ((sensor2 & OutOfDataBuffersBit) == OutOfDataBuffersBit);
}

bool PrinterStatus::isStockLow() const
{
	return ((sensor2 & LowStock) == LowStock);
}

bool PrinterStatus::isOutOfStock() const
{
	return ((sensor2 & OutOfStock) == OutOfStock);
}


bool PrinterStatus::isTakeLabelError() const
{
	return takeLabelError;
}

void PrinterStatus::setTakeLabelError()
{
	takeLabelError = true;
}

void PrinterStatus::clearTakeLabelError()
{
	takeLabelError = false;
}

bool PrinterStatus::isLabelTakenError() const
{
	return labelTakenError;
}

void PrinterStatus::setLabelTakenError()
{
	labelTakenError = true;
}

void PrinterStatus::clearLabelTakenError()
{
	labelTakenError = false;
}

bool PrinterStatus::isLabelSizing() const
{
	return labelSizing;
}

void PrinterStatus::setLabelSizing()
{
	labelSizing = true;
}

void PrinterStatus::clearLabelSizing()
{
	labelSizing = false;
}

bool PrinterStatus::isLabelSizingFailed() const
{
	return labelSizingFailed;
}

void PrinterStatus::setLabelSizingFailed()
{
	labelSizingFailed = true;
}

void PrinterStatus::clearLabelSizingFailed()
{
	labelSizingFailed = false;
}

bool PrinterStatus::isPeripheralComm() const
{
	return peripheralComm;
}

void PrinterStatus::setPeripheralComm()
{
	peripheralComm = true;
}

void PrinterStatus::clearPeripheralComm()
{
	peripheralComm = false;
}

bool PrinterStatus::isManagerDeleted() const
{
	return managerDeleted;
}

void PrinterStatus::setManagerDeleted()
{
	managerDeleted = true;
}

void PrinterStatus::clearManagerDeleted()
{
	managerDeleted = false;
}


UINT8 PrinterStatus::getState() const
{
	return state;
}

void PrinterStatus::setState(UINT8 state)
{
	this->state = state;
}

UINT8 PrinterStatus::getCommand() const
{
	return command;
}

void PrinterStatus::setCommand(UINT8 command)
{
	this->command = command;
}

UINT8 PrinterStatus::getError() const
{
	return error;
}

void PrinterStatus::setError(UINT8 error)
{
	this->error = error;
}

UINT8 PrinterStatus::getHistory() const
{
	return history;
}

void PrinterStatus::setHistory(UINT8 history)
{
	this->history = history;
}

UINT8 PrinterStatus::getSensor() const
{
	return sensor;
}

void PrinterStatus::setSensor(UINT8 sensor)
{
	this->sensor = sensor;
}

UINT8 PrinterStatus::getSensor2() const
{
	return sensor2;
}

void PrinterStatus::setSensor2(UINT8 sensor2)
{
	this->sensor2 = sensor2;
}

UINT8 PrinterStatus::getUser() const
{
	return user;
}

void PrinterStatus::setUser(UINT8 user)
{
	this->user = user;
}

UINT8 PrinterStatus::getSensorMask() const
{
	return sensorMask;
}

void PrinterStatus::setSensorMask(UINT8 sensorMask)
{
	this->sensorMask = sensorMask;
}

UINT8 PrinterStatus::getSensor2Mask() const
{
	return sensor2Mask;
}

void PrinterStatus::setSensor2Mask(UINT8 sensor2Mask)
{
	this->sensor2Mask = sensor2Mask;
}

UINT8 PrinterStatus::getUserMask() const
{
	return userMask;
}

void PrinterStatus::setUserMask(UINT8 userMask)
{
	this->userMask = userMask;
}

UINT16 PrinterStatus::getCounter() const
{
	return counter;
}

void PrinterStatus::setCounter(UINT16 counter)
{
	this->counter = counter;
}

bool PrinterStatus::equals(const PrinterStatus &prStatus) const
{
	bool rv = false;

	if ((labelReady == prStatus.isLabelReady()) &&
		(labelTakenError == prStatus.isLabelTakenError()) &&
		(takeLabelError == prStatus.isTakeLabelError()) &&
		(labelSizing == prStatus.isLabelSizing()) &&
		(labelSizingFailed == prStatus.isLabelSizingFailed()) &&
		(state == prStatus.getState()) &&
		(command == prStatus.getCommand()) &&
		(history == prStatus.getHistory()) &&
		(sensor == prStatus.getSensor()) &&
		(sensor2 == prStatus.getSensor2()) &&
		(user == prStatus.getUser()) &&
		(sensorMask == prStatus.getSensorMask()) &&
		(sensor2Mask == prStatus.getSensor2Mask()) &&
		(userMask == prStatus.getUserMask()) &&
		(counter == prStatus.getCounter()))
	{
		rv = true;
	}

	return rv;
}
