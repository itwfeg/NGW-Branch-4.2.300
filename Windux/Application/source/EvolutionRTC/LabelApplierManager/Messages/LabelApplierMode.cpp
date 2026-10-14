//#include "vxWorks.h"
#include "LabelApplierEvent.h"
#include "LabelApplierMode.h"

#include "Debug.h"
#include "Integer.h"
#include "HobString.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int LabelApplierMode::_dataType_ = 0;
	std::string LabelApplierMode::_dataTypeName_("LabelApplierMode"); 
#endif

LabelApplierMode::LabelApplierMode(Hobart::CEvolMsgReceiver *pHobThread, LaOperatingMode mode)
	: EvolMessage(LabelApplier, LaMode, pHobThread)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	this->mode = mode;
}

LabelApplierMode::LabelApplierMode(const char *buffer)
	: EvolMessage(buffer)
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
	char *pChar = (char *) &mode;
	for (unsigned int i = 0; i < sizeof(mode); i++)
	{
		*pChar++ = buffer[j++];
	}
}

LabelApplierMode::~LabelApplierMode()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

const char *LabelApplierMode::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &mode;
	for (unsigned int i = 0; i < sizeof(mode); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void LabelApplierMode::show()
{
	Debug::addTraceCategory( std::string("LabelApplierMode") );

	Debug::trace("LabelApplierMode", "LabelApplierMode: ");

	EvolMessage::show();

	if (mode == Manual)
	{
		Debug::trace("LabelApplierMode", "mode: Manual");
	}
	else if (mode == SemiAuto)
	{
		Debug::trace("LabelApplierMode", "mode: SemiAuto");
	}
	else if (mode == Auto)
	{
		Debug::trace("LabelApplierMode", "mode: Auto");
	}
	else if (mode == Conveyor)
	{
		Debug::trace("LabelApplierMode", "mode: Conveyor");
	}
	else if (mode == Simulation)
	{
		Debug::trace("LabelApplierMode", "mode: Simulation");
	}
	else if (mode == CService)
	{
		Debug::trace("LabelApplierMode", "mode: CService");
	}
	else
	{
		Debug::trace("LabelApplierMode", HobString("mode: Unknown ") +
												  HobString(Integer::toString((UINT16) mode)));
	}
	Debug::removeTraceCategory( std::string("LabelApplierMode") );
}

void LabelApplierMode::setMode(LaOperatingMode mode)
{
	this->mode = mode;
}

LaOperatingMode LabelApplierMode::getMode()
{
	return mode;
}
