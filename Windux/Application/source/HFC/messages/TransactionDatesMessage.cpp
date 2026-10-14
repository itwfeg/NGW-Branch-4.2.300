#include "TransactionDatesMessage.h"


#include "Debug.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int TransactionDatesMessage::_dataType_ = 0;
		std::string TransactionDatesMessage::_dataTypeName_("hfc::TransactionDatesMessage");
	#endif



	TransactionDatesMessage::TransactionDatesMessage()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		datesDisplay_.setQuotingElements(true);
	}


	TransactionDatesMessage::~TransactionDatesMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.TransactionDatesMessage", "    TransactionDatesMessage: " + util_string::ptrtostring(this));
	}


	void TransactionDatesMessage::addWeekDay(int weekDay)
	{
		weekDays_.push_back(util_string::inttostring(weekDay));
	}


	void TransactionDatesMessage::addDateDisplay(const std::string& date)
	{
		datesDisplay_.push_back(date);
	}


	void TransactionDatesMessage::addDate(const std::string& date)
	{
		dates_.push_back(date);
	}


	const char* TransactionDatesMessage::serialize(int format) const
	{
		serializedData.clear();

		switch ( format )
		{
			case BrowserFormat:
			{
				serializedData.append(weekDays_.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(datesDisplay_.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(dates_.serialize(format));

				break;	
			}
			default:
			{
				serializedData = "TransactionDatesMessage::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}



}
