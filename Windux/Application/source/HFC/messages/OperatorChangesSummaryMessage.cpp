#include "OperatorChangesSummaryMessage.h"


#include "OperatorChangeSummary.h"

#include "Configuration.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int OperatorChangesSummaryMessage::_dataType_ = 0;
	std::string OperatorChangesSummaryMessage::_dataTypeName_("hfc::OperatorChangesSummaryMessage");
#endif



	OperatorChangesSummaryMessage::OperatorChangesSummaryMessage()
	{
#ifdef USE_ACCOUNTANT
		if (_dataType_ == 0)
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif
	}


	OperatorChangesSummaryMessage::~OperatorChangesSummaryMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif
	}


	void OperatorChangesSummaryMessage::addOperatorSummary(Util::CSmartPtr<OperatorChangeSummary> summary)
	{
		operatorIds_.push_back(summary->getOperatorId());
		operatorNames_.push_back(summary->getOperatorName());
		transQuantities_.push_back(util_string::inttostring(summary->getTransactionQuantity()));
		oldTotals_.push_back(Configuration::getInstance()->formatHobPrice(summary->getOldTotal()));
		deltas_.push_back(Configuration::getInstance()->formatHobPrice(summary->getDelta(), true, Configuration::SignedValuePFFlag));
	}


	const char * OperatorChangesSummaryMessage::serialize(int format) const
	{
		serializedData.clear();

		switch ( format )
		{
			case BrowserFormat:
			{
				serializedData.append(operatorIds_.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(operatorNames_.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(transQuantities_.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(oldTotals_.serialize(format));
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(deltas_.serialize(format));

				break;
			}
			default:
				serializedData = "OperatorChangesSummaryMessage::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
		}

		return serializedData.c_str();
	}



}
