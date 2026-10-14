#include "NotifyLocalBatches.h"


#include "util/e_string/utilstring.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int NotifyLocalBatches::_dataType_ = 0;
		std::string NotifyLocalBatches::_dataTypeName_("hfc::NotifyLocalBatches");
	#endif



	NotifyLocalBatches::NotifyLocalBatches()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	NotifyLocalBatches::~NotifyLocalBatches()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.NotifyLocalBatches", "    NotifyLocalBatches: " + util_string::ptrtostring(this));
	}


	const char * NotifyLocalBatches::serialize(int UNUSED(format)) const
	{
		serializedData.clear();

		return serializedData.data();
	}


}
