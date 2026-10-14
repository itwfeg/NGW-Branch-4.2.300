#include "SynchronizeSecondaryScalesLazilyMessage.h"


#include "SynchronizationDevEvent.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace synchronization
{


#ifdef USE_ACCOUNTANT
	int SynchronizeSecondaryScalesLazilyMessage::_dataType_ = 0;
	std::string SynchronizeSecondaryScalesLazilyMessage::_dataTypeName_("synchronization::SynchronizeSecondaryScalesLazilyMessage"); 
#endif


	SynchronizeSecondaryScalesLazilyMessage::SynchronizeSecondaryScalesLazilyMessage()
		: EvolMessage(SynchronizationDev, SynchronizeSecondaryScalesLazilyEvent)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	SynchronizeSecondaryScalesLazilyMessage::SynchronizeSecondaryScalesLazilyMessage(const char* buffer)
		: EvolMessage(buffer)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	SynchronizeSecondaryScalesLazilyMessage::~SynchronizeSecondaryScalesLazilyMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}



}