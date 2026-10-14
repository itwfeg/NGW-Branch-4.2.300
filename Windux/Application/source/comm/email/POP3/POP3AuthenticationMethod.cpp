#include "POP3AuthenticationMethod.h"


#include "POP3PlainAuthentication.h"


#include "Debug.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace email
{


	#ifdef USE_ACCOUNTANT
		int POP3AuthenticationMethod::_dataType_ = 0;
		std::string POP3AuthenticationMethod::_dataTypeName_("email::POP3AuthenticationMethod"); 
	#endif


	const std::string POP3AuthenticationMethod::_InvalidMethod_          ("Invalid");
	const std::string POP3AuthenticationMethod::_PlainPassword_          ("USER");
	const std::string POP3AuthenticationMethod::_PasswordDigest_         ("PASS");



	POP3AuthenticationMethod::POP3AuthenticationMethod(MethodType type)
			: type_(type)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	POP3AuthenticationMethod::~POP3AuthenticationMethod()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const std::string& POP3AuthenticationMethod::getMethodName() const
	{
		switch ( type_ )
		{
			case PlainPassword:              return _PlainPassword_;
			case PasswordDigest:             return _PasswordDigest_;

			case InvalidMethod:
			default:                         return _InvalidMethod_;
		}
	}


	Util::CSmartPtr<POP3AuthenticationMethod> POP3AuthenticationMethod::createMethod(MethodType type)
	{
		switch ( type )
		{
			case PlainPassword:              return Util::CSmartPtr<POP3AuthenticationMethod>(new POP3PlainAuthentication());
//			case PasswordDigest:             return Util::CSmartPtr<POP3AuthenticationMethod>(new POP3DigestAuthentication());

			case InvalidMethod:
			default:                         return Util::CSmartPtr<POP3AuthenticationMethod>(0);
		}
	}


	const std::string& POP3AuthenticationMethod::serialize() const
	{
		serialization_.assign(getMethodName());

		serialization_.append(1, '\r').append(1, '\n');

		return serialization_;
	}


}
