//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierStatus.h"

#include "Debug.h"
#include "Bool.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int LabelApplierStatus::dataType = 0;
std::string LabelApplierStatus::dataTypeName("LabelApplierStatus"); 
#endif

LabelApplierStatus::LabelApplierStatus(Hobart::CEvolMsgReceiver *pHobThread, UINT16 status) : EvolMessage(LabelApplier, LaStatus, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	setStatus(status);

	peripheralComm = true;
	managerDeleted = false;
	Debug::trace("LabelApplierStatus", "Constructor 1:  managerDeleted = " + HobString(Bool::toString(managerDeleted)));
}

LabelApplierStatus::LabelApplierStatus(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++)
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
}

LabelApplierStatus::~LabelApplierStatus()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *LabelApplierStatus::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &status;
	for (unsigned int i = 0; i < sizeof(status); i++)
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
	/*	END OF NEW ERRORS	*/

	return serializedData.data();
}

void LabelApplierStatus::show()
{
	if (Debug::isTracing("LabelApplierStatus"))
	{
		Debug::trace("LabelApplierStatus", "LabelApplierStatus: ");

		EvolMessage::show();

		if (status == NoLabelApplierError)
		{
			Debug::trace("LabelApplierStatus", HobString("    Error: None"));
		}
		else
		{
			HobString errorString;
			if (isHighVoltage())
				errorString = "High Voltage";
			else if (isPrinter1ArmOut())
				errorString = "Printer 1 ArmOut";
			else if (isPrinter2ArmOut())
				errorString = "Printer 2 ArmOut";
			else if (isPrinter1NoWakeup())
				errorString = "Printer 1 NoWakeup";
			else if (isPrinter2NoWakeup())
				errorString = "Printer 2 NoWakeup";
			else if (isWeigherHardware())
				errorString = "Weigher Hardware error";
			else if (isPrinter1Hardware())
				errorString = "Printer 1 Hardware error";
			else if (isPrinter2Hardware())
				errorString = "Printer 2 Hardware error";
			else if (isPackageOverrun())
				errorString = "Package Overrun";
			else if (isModeError())
				errorString = "Mode error";
			else if (isForcePrint())
				errorString = "Force Print error";
			else if (isForceVoid())
				errorString = "Force Void error";
		/*New debug */
			else if (isWeigherConveyorBadHall())
				errorString = "Weigher Conveyor Bad Hall";
			else if (isWeigherConveyorJammed())
				errorString = "Weigher Conveyor Jammed";
			else if (isWeigherConveyorStall())
				errorString = "Weigher Conveyor Stall error";
			else if (isWeigherConveyorPkgSensor())
				errorString = "Weigher Conveyor Pkg Sensor";
			else if (isConveyorBadHall())
				errorString = "Conveyor Bad Hall";
			else if (isConveyorJammed())
				errorString = "Conveyor Jammed";
			else if (isConveyorStall())
				errorString = "Conveyor Stall error";
			else if (isConveyorPkgSensor())
				errorString = "Conveyor Pkg Sensor error";
			else if (isConveyorPkgEdgeSensor())
				errorString = "Conveyor Pkg Edge Sensor error";
			else if (isMotor1FuseBlown())
				errorString = "Motor 1 Fuse blown";
			else if (isMotor2FuseBlown())
				errorString = "Motor 2 Fuse blown";
			else if (isPkgWakeUpSensor())
				errorString = "Package Wakeup Sensor error";
			else if (isPkgGateSensor())
				errorString = "Package Gate Sensor error";
			else
				errorString = "Unknown!";

			Debug::trace("LabelApplierStatus", HobString("    Error: ") + errorString);
		}

		Debug::trace("LabelApplierStatus", HobString("    peripheralComm: ") +
										   HobString(Bool::toString(peripheralComm)));
		Debug::trace("LabelApplierStatus", HobString("    managerDeleted: ") +
										   HobString(Bool::toString(managerDeleted)));
	}
}

UINT16 LabelApplierStatus::getStatus()
{
	return status;
}

void LabelApplierStatus::setStatus(UINT16 newStatus)
{
	status = newStatus;
	Debug::trace("LabelApplierStatus", HobString("setStatus( ") + HobString(Integer::toString(newStatus)) +
										HobString("):  status = ") + HobString(Integer::toString(status)));
}

bool LabelApplierStatus::isError()
{
	return ((status > NoLabelApplierError) ? true : false);
}


bool LabelApplierStatus::isHighVoltage()
{
	return ((status == highVoltageError) ? true : false);
}

bool LabelApplierStatus::isPrinter1ArmOut()
{
	return ((status == printer1ArmOutError) ? true : false);
}


bool LabelApplierStatus::isPrinter2ArmOut()
{
	return ((status == printer2ArmOutError) ? true : false);
}

bool LabelApplierStatus::isPrinter1NoWakeup()
{
	return ((status == printer1NoWakeUpError) ? true : false);
}


bool LabelApplierStatus::isPrinter2NoWakeup()
{
	return ((status == printer2NoWakeUpError) ? true : false);
}


bool LabelApplierStatus::isWeigherHardware()
{
	return ((status == weigherHardwareError) ? true : false);
}


bool LabelApplierStatus::isPrinter1Hardware()
{
	return ((status == printer1HardwareError) ? true : false);
}


bool LabelApplierStatus::isPrinter2Hardware()
{
	return ((status == printer2HardwareError) ? true : false);
}

bool LabelApplierStatus::isPackageOverrun()
{
	return ((status == packageOverrunError) ? true : false);
}

bool LabelApplierStatus::isModeError()
{
	return ((status == modeErrorError) ? true : false);
}

bool LabelApplierStatus::isForcePrint()
{
	return ((status == reserved1Error) ? true : false);
}

bool LabelApplierStatus::isForceVoid()
{
	return ((status == reserved2Error) ? true : false);
}

/*	CLA new Errors 10/10 Alex Zavala	*/
bool LabelApplierStatus::isWeigherConveyorBadHall()
{
	return ((status == weigherConveyorBadHallError) ? true : false);
}

bool LabelApplierStatus::isWeigherConveyorJammed()
{
	return ((status == weigherConveyorJammedError) ? true : false);
}

bool LabelApplierStatus::isWeigherConveyorStall()
{
	return ((status == weigherConveyorStallError) ? true : false);
}

bool LabelApplierStatus::isWeigherConveyorPkgSensor()
{
	return ((status == weigherConveyorPkgSensorError) ? true : false);
}

bool LabelApplierStatus::isConveyorBadHall()
{
	return ((status == conveyorBadHallError) ? true : false);
}

bool LabelApplierStatus::isConveyorJammed()
{
	return ((status == conveyorJammedError) ? true : false);
}

bool LabelApplierStatus::isConveyorStall()
{
	return ((status == conveyorStallError) ? true : false);
}

bool LabelApplierStatus::isConveyorPkgSensor()
{
	return ((status == conveyorPkgSensorError) ? true : false);
}

bool LabelApplierStatus::isConveyorPkgEdgeSensor()
{
	return ((status == conveyorPkgEdgeSensorError) ? true : false);
}

bool LabelApplierStatus::isMotor1FuseBlown()
{
	return ((status == motor1FuseBlownError) ? true : false);
}

bool LabelApplierStatus::isMotor2FuseBlown()
{
	return ((status == motor2FuseBlownError) ? true : false);
}

bool LabelApplierStatus::isPkgWakeUpSensor()
{
	return ((status == pkgWakeUpSensorError) ? true : false);
}

bool LabelApplierStatus::isPkgGateSensor()
{
	return ((status == pkgGateSensorError) ? true : false);
}

/****************************************************
* CLA Operator Errors - Notify Operator, don't run. *
****************************************************/

bool LabelApplierStatus::isOperatorError()
{
	return (isPrinter1NoWakeup() || isPrinter2NoWakeup() ||
			isPrinter1ArmOut() || isPrinter2ArmOut() ||
			isHighVoltage() || isModeError() || isWeigherHardware() ||
			isPrinter1Hardware() || isPrinter2Hardware());
}

/**************************************************************
* CLA Hardware Errors - Notify Operator, but continue to run. *
**************************************************************/
	
bool LabelApplierStatus::isHardwareError()
{
	return (isPrinter1NoWakeup() || isPrinter2NoWakeup() ||
			isWeigherHardware() ||	isPrinter1Hardware() ||
			isPrinter2Hardware());
}


/*********************************************************************
* CLA Fatal Errors - Notify Operator, Operate as if no CLA attached. *
*********************************************************************/

bool LabelApplierStatus::isFatalError()
{
	return (isPrinter1NoWakeup() || isPrinter2NoWakeup());
}

/**************************************************************************
* CLA Operation Error - Notify Operator, machine will attempt to recover. *
**************************************************************************/
 
bool LabelApplierStatus::isActionError()
{
	return (isForcePrint() || isForceVoid());
}

bool LabelApplierStatus::isPeripheralComm()
{
	return peripheralComm;
}

void LabelApplierStatus::setPeripheralComm()
{
	peripheralComm = true;
}

void LabelApplierStatus::clearPeripheralComm()
{
	peripheralComm = false;
}

bool LabelApplierStatus::isManagerDeleted()
{
	Debug::trace("LabelApplierStatus", "isManagerDeleted():  " + HobString(Bool::toString(managerDeleted)));
	return managerDeleted;
}

void LabelApplierStatus::setManagerDeleted()
{
	managerDeleted = true;
	Debug::trace("LabelApplierStatus", "setManagerDeleted():  " + HobString(Bool::toString(managerDeleted)) + "!!!!!!");
}

void LabelApplierStatus::clearManagerDeleted()
{
	managerDeleted = false;
	Debug::trace("LabelApplierStatus", "clearManagerDeleted():  " + HobString(Bool::toString(managerDeleted)));
}

bool LabelApplierStatus::equals(LabelApplierStatus &laStatus)
{
	bool rv = false;

	if ((status == laStatus.getStatus()) &&
		(peripheralComm == laStatus.isPeripheralComm()) &&
		(managerDeleted == laStatus.isManagerDeleted()))
	{
		rv = true;
	}

	return rv;
}