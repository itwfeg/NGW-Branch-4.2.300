#include "GoingOffLineMessage.h"
#include "SynchronizationDevEvent.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace synchronization
{

#ifdef USE_ACCOUNTANT
	int GoingOffLineMessage::_dataType_ = 0;
	std::string GoingOffLineMessage::_dataTypeName_("synchronization::GoingOffLineMessage"); 
#endif

	GoingOffLineMessage::GoingOffLineMessage()
		: EvolMessage(SynchronizationDev, GoingOffLineEvent)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	GoingOffLineMessage::GoingOffLineMessage(const char* buffer)
		: EvolMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	GoingOffLineMessage::~GoingOffLineMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}