//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "CompactLabelApplierSensors.h"

#include "Debug.h"
#include "Bool.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CompactLabelApplierSensors::dataType = 0;
std::string CompactLabelApplierSensors::dataTypeName("CompactLabelApplierSensors"); 
#endif

CompactLabelApplierSensors::CompactLabelApplierSensors(Hobart::CEvolMsgReceiver *pHobThread, UINT16 sensorBits) : EvolMessage(LabelApplier, LaSensors, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	setSensors(sensorBits);
//	generic_switches = 0;
}

CompactLabelApplierSensors::CompactLabelApplierSensors(const char *buffer) : EvolMessage(buffer)
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
	char *pChar = (char *) &wake_up;
	for (unsigned int i = 0; i < sizeof(wake_up); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &weigher_entry;
	for (unsigned int i = 0; i < sizeof(weigher_entry); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &CLA_entry;
	for (unsigned int i = 0; i < sizeof(CLA_entry); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &CLA_pkg_edge_detect;
	for (unsigned int i = 0; i < sizeof(CLA_pkg_edge_detect); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &start_switch;
	for (unsigned int i = 0; i < sizeof(start_switch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &stop_switch;
	for (unsigned int i = 0; i < sizeof(stop_switch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &CLA_gate_switch;
	for (unsigned int i = 0; i < sizeof(CLA_gate_switch); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &CLA_printer_post_switch;
	for (unsigned int i = 0; i < sizeof(CLA_printer_post_switch); i++)
	{
		*pChar++ = buffer[j++];
	}
/*
	pChar = (char *) &generic_switches;
	for (unsigned int i = 0; i < sizeof(generic_switches); i++)
	{
		*pChar++ = buffer[j++];
	}*/
}

CompactLabelApplierSensors::~CompactLabelApplierSensors()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

const char *CompactLabelApplierSensors::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &wake_up;
	for (unsigned int i = 0; i < sizeof(wake_up); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &weigher_entry;
	for (unsigned int i = 0; i < sizeof(weigher_entry); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &CLA_entry;
	for (unsigned int i = 0; i < sizeof(CLA_entry); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &CLA_pkg_edge_detect;
	for (unsigned int i = 0; i < sizeof(CLA_pkg_edge_detect); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &start_switch;
	for (unsigned int i = 0; i < sizeof(start_switch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &stop_switch;
	for (unsigned int i = 0; i < sizeof(stop_switch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &CLA_gate_switch;
	for (unsigned int i = 0; i < sizeof(CLA_gate_switch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &CLA_printer_post_switch;
	for (unsigned int i = 0; i < sizeof(CLA_printer_post_switch); i++)
	{
		serializedData.append(1, pChar[i]);
	}
/*
	pChar = (char *) &generic_switches;
	for (unsigned int i = 0; i < sizeof(generic_switches); i++)
	{
		serializedData.append(1, pChar[i]);
	}
*/
	return serializedData.data();
}

void CompactLabelApplierSensors::show()
{
#if defined(LINUX_FIX)
	Debug::addTraceCategory( std::string("CompactLabelApplierSensors") );
#else
	Debug::addTraceCategory("CompactLabelApplierSensors");
#endif
	Debug::trace("CompactLabelApplierSensors", "CompactLabelApplierSensors: ");

	EvolMessage::show();

	Debug::trace("CompactLabelApplierSensors", HobString("WakeUp: ") +
									   HobString(Bool::toString(wake_up)));
	Debug::trace("CompactLabelApplierSensors", HobString("WeigherEntry: ") +
									   HobString(Bool::toString(weigher_entry)));
	Debug::trace("CompactLabelApplierSensors", HobString("CLAEntry: ") +
									   HobString(Bool::toString(CLA_entry)));
	Debug::trace("CompactLabelApplierSensors", HobString("CLAPackageDetect: ") +
									   HobString(Bool::toString(CLA_pkg_edge_detect)));
	Debug::trace("CompactLabelApplierSensors", HobString("StartSwitch: ") +
									   HobString(Bool::toString(start_switch)));
	Debug::trace("CompactLabelApplierSensors", HobString("StopSwitch: ") +
									   HobString(Bool::toString(stop_switch)));
	Debug::trace("CompactLabelApplierSensors", HobString("CLAGateSwitch: ") +
									   HobString(Bool::toString(CLA_gate_switch)));
	Debug::trace("CompactLabelApplierSensors", HobString("CLAPrinterPostSwitch: ") +
									   HobString(Bool::toString(CLA_printer_post_switch)));
	/*Debug::trace("CompactLabelApplierSensors", HobString("genericSwitches: ") +
									   HobString(Integer::toString(generic_switches)));*/

#if defined(LINUX_FIX)
	Debug::removeTraceCategory( std::string("CompactLabelApplierSensors") );
#else
	Debug::removeTraceCategory("CompactLabelApplierSensors");
#endif
}

bool CompactLabelApplierSensors::getWakeUp() const
{
	return wake_up;
}

void CompactLabelApplierSensors::setWakeUp(bool wakeUp)
{
	wake_up = wakeUp;
}

bool CompactLabelApplierSensors::getWeigherEntry() const
{
	return weigher_entry;
}

void CompactLabelApplierSensors::setWeigherEntry(bool weigherEntry)
{
	weigher_entry = weigherEntry;
}

bool CompactLabelApplierSensors::getCLAEntry() const
{
	return CLA_entry;
}

void CompactLabelApplierSensors::setCLAEntry(bool claEntry)
{
	CLA_entry = claEntry;
}

bool CompactLabelApplierSensors::getCLAPkgEdgeDetect() const
{
	return CLA_pkg_edge_detect;
}

void CompactLabelApplierSensors::setCLAPkgEdgeDetect(bool pkgEdgeDetect)
{
	CLA_pkg_edge_detect = pkgEdgeDetect;
}

bool CompactLabelApplierSensors::getStartSwitch() const
{
	return start_switch;
}

void CompactLabelApplierSensors::setStartSwitch(bool startSwitch)
{
	start_switch = startSwitch;
}

bool CompactLabelApplierSensors::getStopSwitch() const
{
	return stop_switch;
}

void CompactLabelApplierSensors::setStopSwitch(bool stopSwitch)
{
	stop_switch = stopSwitch;
}

bool CompactLabelApplierSensors::getCLAGateSwitch() const
{
	return CLA_gate_switch;
}

void CompactLabelApplierSensors::setCLAGateSwitch(bool gateSwitch)
{
	CLA_gate_switch = gateSwitch;
}

bool CompactLabelApplierSensors::getCLAPrinterPostSwitch() const
{
	return CLA_printer_post_switch;
}

void CompactLabelApplierSensors::setCLAPrinterPostSwitch(bool postSwitch)
{
	CLA_printer_post_switch = postSwitch;
}
//Getters and setters for the spares
bool CompactLabelApplierSensors::getSpare1() const
{
	return Spare1;
}

void CompactLabelApplierSensors::setSpare1(bool spare)
{
	Spare1 = spare;
}
bool CompactLabelApplierSensors::getSpare2() const
{
	return Spare2;
}

void CompactLabelApplierSensors::setSpare2(bool spare)
{
	Spare2 = spare;
}
bool CompactLabelApplierSensors::getSpare3() const
{
	return Spare3;
}

void CompactLabelApplierSensors::setSpare3(bool spare)
{
	Spare3 = spare;
}
bool CompactLabelApplierSensors::getSpare4() const
{
	return Spare4;
}

void CompactLabelApplierSensors::setSpare4(bool spare)
{
	Spare4 = spare;
}
bool CompactLabelApplierSensors::getSpare5() const
{
	return Spare5;
}

void CompactLabelApplierSensors::setSpare5(bool spare)
{
	Spare5 = spare;
}
bool CompactLabelApplierSensors::getSpare6() const
{
	return Spare6;
}

void CompactLabelApplierSensors::setSpare6(bool spare)
{
	Spare6 = spare;
}
bool CompactLabelApplierSensors::getSpare7() const
{
	return Spare7;
}

void CompactLabelApplierSensors::setSpare7(bool spare)
{
	Spare7 = spare;
}
bool CompactLabelApplierSensors::getSpare8() const
{
	return Spare8;
}

void CompactLabelApplierSensors::setSpare8(bool spare)
{
	Spare8 = spare;
}
/*
UINT16 CompactLabelApplierSensors::getGenericSwitches() const
{
	return generic_switches;
}

void CompactLabelApplierSensors::setGenericSwitches(UINT16 genSwitch)
{
	generic_switches = genSwitch;
}
*/
bool CompactLabelApplierSensors::equals(CompactLabelApplierSensors &laSensors)
{
	bool rv = false;

	if	((wake_up == laSensors.getWakeUp()) &&
		(weigher_entry == laSensors.getWeigherEntry()) &&
		(CLA_entry == laSensors.getCLAEntry()) &&
		(CLA_pkg_edge_detect == laSensors.getCLAPkgEdgeDetect()) &&
		(start_switch == laSensors.getStartSwitch()) &&
		(stop_switch == laSensors.getStopSwitch()) &&
		(CLA_gate_switch == laSensors.getCLAGateSwitch()) &&
		(CLA_printer_post_switch == laSensors.getCLAPrinterPostSwitch()) &&
		(Spare1 == laSensors.getSpare1()) &&
		(Spare2 == laSensors.getSpare2()) &&
		(Spare3 == laSensors.getSpare3()) &&
		(Spare4 == laSensors.getSpare4()) &&
		(Spare5 == laSensors.getSpare5()) &&
		(Spare6 == laSensors.getSpare6()) &&
		(Spare7 == laSensors.getSpare7()) &&
		(Spare8 == laSensors.getSpare8()))
	{
		rv = true;
	}

	return rv;
}

void CompactLabelApplierSensors::setSensors(UINT16 sensorBits)
{
	wake_up =		(((sensorBits & WakeUpMask) == WakeUpMask) ? true : false);
	weigher_entry =	(((sensorBits & WeigherEntryMask) == WeigherEntryMask) ? true : false);
	CLA_entry =	(((sensorBits & CLAEntryMask) == CLAEntryMask) ? true : false);
	CLA_pkg_edge_detect =	(((sensorBits & CLAPkgEdgeDetectMask) == CLAPkgEdgeDetectMask) ? true : false);
	start_switch =	(((sensorBits & StartSwitchMask) == StartSwitchMask) ? true : false);
	stop_switch =	(((sensorBits & StopSwitchMask) == StopSwitchMask) ? true : false);
	CLA_gate_switch =	(((sensorBits & CLAGateSwitchMask) == CLAGateSwitchMask) ? true : false);
	CLA_printer_post_switch =	(((sensorBits & CLAPrinterPostSwitchMask) == CLAPrinterPostSwitchMask) ? true : false);
	Spare1 = (((sensorBits & Spare1Mask) == Spare1Mask) ? true : false);
	Spare2 = (((sensorBits & Spare2Mask) == Spare2Mask) ? true : false);
	Spare3 = (((sensorBits & Spare3Mask) == Spare3Mask) ? true : false);
	Spare4 = (((sensorBits & Spare4Mask) == Spare4Mask) ? true : false);
	Spare5 = (((sensorBits & Spare5Mask) == Spare5Mask) ? true : false);
	Spare6 = (((sensorBits & Spare6Mask) == Spare6Mask) ? true : false);
	Spare7 = (((sensorBits & Spare7Mask) == Spare7Mask) ? true : false);
	Spare8 = (((sensorBits & Spare8Mask) == Spare8Mask) ? true : false);
}