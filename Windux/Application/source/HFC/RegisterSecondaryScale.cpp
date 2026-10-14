#include "RegisterSecondaryScale.h"


#include "util/e_string/utilstring.h"


#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int RegisterSecondaryScale::_dataType_ = 0;
		std::string RegisterSecondaryScale::_dataTypeName_("hfc::RegisterSecondaryScale");
	#endif



	RegisterSecondaryScale::RegisterSecondaryScale()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
	}


	RegisterSecondaryScale::~RegisterSecondaryScale()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.RegisterSecondaryScale", "    RegisterSecondaryScale: " + util_string::ptrtostring(this));
	}


	const char * RegisterSecondaryScale::serialize(int UNUSED(format)) const
	{
		serializedData.clear();

		return serializedData.data();
	}


}
