//#include "vxWorks.h"
#include "SystemGeneric.h"

#include "Debug.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
//#include "util/accountant/accountanti.h"
#include "accountant/accountanti.h"

int SystemGeneric::_dataType_ = 0;
std::string SystemGeneric::_dataTypeName_("SystemGeneric"); 
#endif

SystemGeneric::SystemGeneric(EvolDeviceType evolDevType, SysEvent sysEvent, Hobart::CEvolMsgReceiver *pHobThread) : EvolMessage(evolDevType, (UINT8)sysEvent, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemGeneric::SystemGeneric(const char *buffer) : EvolMessage(buffer)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

SystemGeneric::~SystemGeneric()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

void SystemGeneric::show() const
{
	bool isTracing = Debug::isTracing("SystemGeneric");

	if (isTracing == false)
	{
		Debug::addTraceCategory( std::string("SystemGeneric") );
	}

	Debug::trace("SystemGeneric", "SystemGeneric: ");

	EvolMessage::show();

	if (isTracing == false)
	{
		Debug::removeTraceCategory( std::string("SystemGeneric") );
	}
}
