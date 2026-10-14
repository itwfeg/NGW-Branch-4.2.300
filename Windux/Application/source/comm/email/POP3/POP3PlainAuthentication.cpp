#include "POP3PlainAuthentication.h"



#include "Debug.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace email
{


	#ifdef USE_ACCOUNTANT
		int POP3PlainAuthentication::_dataType_ = 0;
		std::string POP3PlainAuthentication::_dataTypeName_("email::POP3PlainAuthentication"); 
	#endif



	POP3PlainAuthentication::POP3PlainAuthentication()
			: POP3AuthenticationMethod(POP3AuthenticationMethod::PlainPassword)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	POP3PlainAuthentication::~POP3PlainAuthentication()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const std::string& POP3PlainAuthentication::serialize() const
	{
		serialization_.assign(getMethodName());

		serialization_.append(1, '\r').append(1, '\n');

		return serialization_;
	}


}
