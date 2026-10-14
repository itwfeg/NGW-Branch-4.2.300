#include "WriteProductTotalsMessage.h"

#include "ProductTotal.h"
#include "Configuration.h"


#include "Debug.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
		int WriteProductTotalsMessage::_dataType_ = 0;
		std::string WriteProductTotalsMessage::_dataTypeName_("hfc::WriteProductTotalsMessage");
#endif



	WriteProductTotalsMessage::WriteProductTotalsMessage()
	{
#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
		weights_.setQuotingElements(true);
		prices_.setQuotingElements(true);
	}


	WriteProductTotalsMessage::~WriteProductTotalsMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

		Debug::trace("Delete.WriteProductTotalsMessage", "    WriteProductTotalsMessage: " + util_string::ptrtostring(this));
	}


	void WriteProductTotalsMessage::addProductTotal(Util::CSmartPtr<ProductTotal> total)
	{
		if ( total.notNull() )
		{
			HobString dumbString;

			transTypes_.push_back(util_string::longtostring(total->getTransactionTypeNumber()));
			Configuration::getInstance()->formatWeight(total->getWeight(), dumbString);
			weights_.push_back(dumbString);
			prices_.push_back(Configuration::getInstance()->formatHobPrice(total->getPrice(), true, Configuration::SignedValuePFFlag));
			items_.push_back(util_string::inttostring(total->getItems()));
			packages_.push_back(util_string::inttostring(total->getPackages()));
		}
	}


	const char* WriteProductTotalsMessage::serialize(int format) const
	{
		serializedData.clear();

		switch ( format )
		{
			case BrowserFormat:
			{
				for (int loopCnt = 1; loopCnt < endWriteProductTotalsOffsets; loopCnt++)
				{
					if (loopCnt > 1)
						serializedData.append(BrowserFormatSeparator);

					switch ( loopCnt )
					{
						case fieldOffsetWriteProductTotalsTransactionTypes:
							serializedData.append(transTypes_.serialize(format));
							break;
						case fieldOffsetWriteProductTotalsWeights:
							serializedData.append(weights_.serialize(format));
							break;
						case fieldOffsetWriteProductTotalsPrices:
							serializedData.append(prices_.serialize(format));
							break;
						case fieldOffsetWriteProductTotalsItems:
							serializedData.append(items_.serialize(format));
							break;
						case fieldOffsetWriteProductTotalsPackages:
							serializedData.append(packages_.serialize(format));
							break;
						default: break;
					}
				}

				break;	
			}
			default:
			{
				serializedData = "WriteProductTotalsMessage::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}



}
