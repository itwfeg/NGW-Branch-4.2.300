#include "TransactionTypeNamesMessage.h"



#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int TransactionTypeNamesMessage::_dataType_ = 0;
	std::string TransactionTypeNamesMessage::_dataTypeName_("hfc::TransactionTypeNamesMessage");
#endif



	TransactionTypeNamesMessage::TransactionTypeNamesMessage()
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	TransactionTypeNamesMessage::~TransactionTypeNamesMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	void TransactionTypeNamesMessage::addTransactionType(const std::string& number, const std::string& name1, const std::string& name2)
	{
		typeNumbers_.push_back(number);
		namesPrimary_.push_back(name1);
		namesSecondary_.push_back(name2);
	}


	const char * TransactionTypeNamesMessage::serialize(int format) const
	{
		serializedData.clear();

		switch ( format )
		{
			case BrowserFormat:
			{
				serializedData.append(typeNumbers_.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(namesPrimary_.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(namesSecondary_.serialize(format));

				break;
			}
			default:
				serializedData = "TransactionTypeNamesMessage::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
		}

		return serializedData.c_str();
	}



}
