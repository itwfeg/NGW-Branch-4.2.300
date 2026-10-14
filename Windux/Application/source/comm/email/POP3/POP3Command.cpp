#include "POP3Command.h"



#include "Debug.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace email
{


	#ifdef USE_ACCOUNTANT
		int POP3Command::_dataType_ = 0;
		std::string POP3Command::_dataTypeName_("email::POP3Command"); 
	#endif


	const std::string POP3Command::_InvalidCommand_          ("Invalid");
	const std::string POP3Command::_StatisticsCommand_       ("STAT");
	const std::string POP3Command::_ListCommand_             ("LIST");
	const std::string POP3Command::_RetrieveCommand_         ("RETR");
	const std::string POP3Command::_DeleteCommand_           ("DELE");
	const std::string POP3Command::_NoOperationCommand_      ("NOOP");
	const std::string POP3Command::_ResetCommand_            ("RSET");
	const std::string POP3Command::_QuitCommand_             ("QUIT");
	const std::string POP3Command::_CapabilityCommand_       ("CAPA");
	const std::string POP3Command::_AuthenticationCommand_   ("AUTH");
	const std::string POP3Command::_UserCommand_             ("USER");
	const std::string POP3Command::_PasswordCommand_         ("PASS");
	const std::string POP3Command::_DigestCommand_           ("APOP");
	const std::string POP3Command::_TopCommand_              ("TOP");
	const std::string POP3Command::_UniqueIDListCommand_     ("UIDL");
	const std::string POP3Command::_SecureTransportCommand_  ("STLS");

	const std::string POP3Command::_noParameter_             ("Parameter not assigned");

	const int POP3Command::_maxParameters_[LastCommand]      = { 0, 0, 1, 1, 1, 0, 0, 0, 0, 2, 1, 1, 2, 2, 1, 0};
	const int POP3Command::_optionalParameters_[LastCommand] = { 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};



	POP3Command::POP3Command(CommandType type)
			: type_(type), numParameters_(0), parameters_(_maxParameters_[type])
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	POP3Command::~POP3Command()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	void POP3Command::setParameter(const std::string& param)
	{
		if ( _maxParameters_[type_] )
		{
			parameters_[0] = param;
			numParameters_ = numParameters_ ? numParameters_ : 1;
		}
	}


	void POP3Command::setParameter2(const std::string& param)
	{
		if ( _maxParameters_[type_] >= 2 )
		{
			parameters_[1] = param;
			numParameters_ = numParameters_ >= 2 ? numParameters_ : 2;
		}
	}


	const std::string& POP3Command::getCommandName() const
	{
		switch ( type_ )
		{
			case Statistics:             return _StatisticsCommand_;
			case List:                   return _ListCommand_;
			case Retrieve:               return _RetrieveCommand_;
			case Delete:                 return _DeleteCommand_;
			case NoOperation:            return _NoOperationCommand_;
			case Reset:                  return _ResetCommand_;
			case Quit:                   return _QuitCommand_;

			case Capability:             return _CapabilityCommand_;
			case Authentication:         return _AuthenticationCommand_;
			case User:                   return _UserCommand_;
			case Password:               return _PasswordCommand_;
			case Digest:                 return _DigestCommand_;
			case Top:                    return _TopCommand_;
			case UniqueIDList:           return _UniqueIDListCommand_;
			case SecureTransport:        return _SecureTransportCommand_;

			case Invalid:
			default:                     return _InvalidCommand_;
		}
	}


	const std::string& POP3Command::serialize() const
	{
		serialization_.assign(getCommandName());

		for ( int i = 0; i < _maxParameters_[type_]; ++i )
		{
			if ( i < numParameters_ )
				serialization_.append(1, ' ').append(parameters_[i]);
			else
			{
				// If we are missing a required parameter then return an empty serialization
				if ( i + _optionalParameters_[type_] < _maxParameters_[type_] )
				{
					serialization_.clear();
					return serialization_;
				}

				// If there are no more parameters then complete the serialization
				break;
			}
		}

		serialization_.append(1, '\r').append(1, '\n');

		return serialization_;
	}


}
