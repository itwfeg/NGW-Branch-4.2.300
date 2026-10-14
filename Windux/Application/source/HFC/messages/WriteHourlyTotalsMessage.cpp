#include "WriteHourlyTotalsMessage.h"

#include "HourlyTotal.h"
#include "Configuration.h"


#include "Debug.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int WriteHourlyTotalsMessage::_dataType_ = 0;
		std::string WriteHourlyTotalsMessage::_dataTypeName_("hfc::WriteHourlyTotalsMessage");
	#endif



	WriteHourlyTotalsMessage::WriteHourlyTotalsMessage()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif
		weights_.setQuotingElements(true);
		prices_.setQuotingElements(true);
	}


	WriteHourlyTotalsMessage::~WriteHourlyTotalsMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.WriteHourlyTotalsMessage", "    WriteHourlyTotalsMessage: " + util_string::ptrtostring(this));
	}


	void WriteHourlyTotalsMessage::addHourlyTotal(Util::CSmartPtr<HourlyTotal> total)
	{
		if ( total.notNull() )
		{
			HobString dumbString;

			hours_.push_back(util_string::longtostring(total->getHour()));
			Configuration::getInstance()->formatWeight(total->getWeight(), dumbString);
			weights_.push_back(dumbString);
			prices_.push_back(Configuration::getInstance()->formatHobPrice(total->getPrice(), true, Configuration::SignedValuePFFlag));
			items_.push_back(util_string::inttostring(total->getItems()));
			packages_.push_back(util_string::inttostring(total->getPackages()));
		}
	}


	const char* WriteHourlyTotalsMessage::serialize(int format) const
	{
		serializedData.clear();

		switch ( format )
		{
			case BrowserFormat:
			{
				for (int loopCnt = 1; loopCnt < endWriteHourlyTotalsOffsets; loopCnt++)
				{
					if (loopCnt > 1)
						serializedData.append(BrowserFormatSeparator);

					switch ( loopCnt )
					{
						case fieldOffsetWriteHourlyTotalsHour:
							serializedData.append(hours_.serialize(format));
							break;
						case fieldOffsetWriteHourlyTotalsWeights:
							serializedData.append(weights_.serialize(format));
							break;
						case fieldOffsetWriteHourlyTotalsPrices:
							serializedData.append(prices_.serialize(format));
							break;
						case fieldOffsetWriteHourlyTotalsItems:
							serializedData.append(items_.serialize(format));
							break;
						case fieldOffsetWriteHourlyTotalsPackages:
							serializedData.append(packages_.serialize(format));
							break;
						default: break;
					}
				}

				break;	
			}
			default:
			{
				serializedData = "WriteHourlyTotalsMessage::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}



}
