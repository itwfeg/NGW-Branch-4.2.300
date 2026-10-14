#include "POP3Capability.h"



#include "Debug.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace email
{


	#ifdef USE_ACCOUNTANT
		int POP3Capability::_dataType_ = 0;
		std::string POP3Capability::_dataTypeName_("email::POP3Capability"); 
	#endif


	const std::string POP3Capability::_InvalidCapability_          ("Invalid");
	const std::string POP3Capability::_TopCapability_              ("TOP");
	const std::string POP3Capability::_UserCapability_             ("USER");
	const std::string POP3Capability::_AuthenticationCapability_   ("SASL");
	const std::string POP3Capability::_ResponseCodesCapability_    ("RESP-CODES");
	const std::string POP3Capability::_LoginDelayCapability_       ("LOGIN-DELAY");
	const std::string POP3Capability::_PipeliningCapability_       ("PIPELINING");
	const std::string POP3Capability::_ExpireCapability_           ("EXPIRE");
	const std::string POP3Capability::_UniqueIDListCapability_     ("UIDL");
	const std::string POP3Capability::_SecureTransportCapability_  ("STLS");
	const std::string POP3Capability::_ImplementationCapability_   ("IMPLEMENTATION");



	POP3Capability::POP3Capability(CapabilityType type)
			: type_(type)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	POP3Capability::~POP3Capability()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	const std::string& POP3Capability::getCapabilityName() const
	{
		switch ( type_ )
		{
			case Top:                    return _TopCapability_;
			case User:                   return _UserCapability_;
			case Authentication:         return _AuthenticationCapability_;
			case ResponseCodes:          return _ResponseCodesCapability_;
			case LoginDelay:             return _LoginDelayCapability_;
			case Pipelining:             return _PipeliningCapability_;
			case Expire:                 return _ExpireCapability_;
			case UniqueIDList:           return _UniqueIDListCapability_;
			case SecureTransport:           return _SecureTransportCapability_;
			case Implementation:         return _ImplementationCapability_;

			case Invalid:
			default:                     return _InvalidCapability_;
		}
	}


}
