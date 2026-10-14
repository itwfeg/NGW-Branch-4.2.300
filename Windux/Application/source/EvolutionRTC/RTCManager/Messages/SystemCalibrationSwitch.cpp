//#include "vxWorks.h"
#include "SystemEvent.h"
#include "SystemCalibrationSwitch.h"

#include "Debug.h"
#include "Bool.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemCalibrationSwitch::_dataType_ = 0;
std::string SystemCalibrationSwitch::_dataTypeName_("SystemCalibrationSwitch"); 
#endif

SystemCalibrationSwitch::SystemCalibrationSwitch(EvolDeviceType evolDevType, 
												 Hobart::CEvolMsgReceiver *pHobThread,
												 bool systemCalibrationSwitch) : EvolMessage(evolDevType, SysCalibSwitch, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
	this->systemCalibrationSwitch = systemCalibrationSwitch;
}

SystemCalibrationSwitch::SystemCalibrationSwitch(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	// index into buffer
	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &systemCalibrationSwitch;
	for (unsigned int i = 0; i < sizeof(systemCalibrationSwitch); i++)
	{
		*pChar++ = buffer[j++];
	}
}

SystemCalibrationSwitch::~SystemCalibrationSwitch()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *SystemCalibrationSwitch::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &systemCalibrationSwitch;
	for (unsigned int i = 0; i < sizeof(systemCalibrationSwitch); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void SystemCalibrationSwitch::show()
{
	Debug::addTraceCategory( std::string("SystemCalibrationSwitch") );
	Debug::trace("SystemCalibrationSwitch", "SystemCalibrationSwitch: ");

	EvolMessage::show();

	Debug::trace("SystemCalibrationSwitch", HobString("systemCalibrationSwitch: ") +
											HobString(Bool::toString(systemCalibrationSwitch)));

	Debug::removeTraceCategory( std::string("SystemCalibrationSwitch") );
}

void SystemCalibrationSwitch::setSystemCalibrationSwitch()
{
	systemCalibrationSwitch = true;
}

void SystemCalibrationSwitch::clearSystemCalibrationSwitch()
{
	systemCalibrationSwitch = false;
}

bool SystemCalibrationSwitch::isSystemCalibrationSwitch() const
{
	return systemCalibrationSwitch;
}
