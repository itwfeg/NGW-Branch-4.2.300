#include "WriteOperatorTotalsMessage.h"

#include "OperatorTotal.h"
#include "Configuration.h"


#include "Debug.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
		int WriteOperatorTotalsMessage::_dataType_ = 0;
		std::string WriteOperatorTotalsMessage::_dataTypeName_("hfc::WriteOperatorTotalsMessage");
#endif



	WriteOperatorTotalsMessage::WriteOperatorTotalsMessage()
	{
#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
		weights_.setQuotingElements(true);
		prices_.setQuotingElements(true);
	}


	WriteOperatorTotalsMessage::~WriteOperatorTotalsMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

		Debug::trace("Delete.WriteOperatorTotalsMessage", "    WriteOperatorTotalsMessage: " + util_string::ptrtostring(this));
	}


	void WriteOperatorTotalsMessage::addOperatorTotal(Util::CSmartPtr<OperatorTotal> total, const std::string& name)
	{
		if ( total.notNull() )
		{
			HobString dumbString;

			names_.push_back(name);
			Configuration::getInstance()->formatWeight(total->getWeight(), dumbString);
			weights_.push_back(dumbString);
			prices_.push_back(Configuration::getInstance()->formatHobPrice(total->getPrice(), true, Configuration::SignedValuePFFlag));
			items_.push_back(util_string::inttostring(total->getItems()));
			packages_.push_back(util_string::inttostring(total->getPackages()));
		}
	}


	const char* WriteOperatorTotalsMessage::serialize(int format) const
	{
		serializedData.clear();

		switch ( format )
		{
			case BrowserFormat:
			{
				for (int loopCnt = 1; loopCnt < endWriteOperatorTotalsOffsets; loopCnt++)
				{
					if (loopCnt > 1)
						serializedData.append(BrowserFormatSeparator);

					switch ( loopCnt )
					{
						case fieldOffsetWriteOperatorTotalsNames:
							serializedData.append(names_.serialize(format));
							break;
						case fieldOffsetWriteOperatorTotalsWeights:
							serializedData.append(weights_.serialize(format));
							break;
						case fieldOffsetWriteOperatorTotalsPrices:
							serializedData.append(prices_.serialize(format));
							break;
						case fieldOffsetWriteOperatorTotalsItems:
							serializedData.append(items_.serialize(format));
							break;
						case fieldOffsetWriteOperatorTotalsPackages:
							serializedData.append(packages_.serialize(format));
							break;
						default: break;
					}
				}

				break;	
			}
			default:
			{
				serializedData = "WriteOperatorTotalsMessage::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}



}
