#include "POP3Parser.h"



#include "Debug.h"



#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace email
{


	#ifdef USE_ACCOUNTANT
		int POP3Parser::_dataType_ = 0;
		std::string POP3Parser::_dataTypeName_("email::POP3Parser"); 
	#endif


	POP3Parser::POP3Parser(const std::string& text)
			: text_(text)
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	POP3Parser::~POP3Parser()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


}
