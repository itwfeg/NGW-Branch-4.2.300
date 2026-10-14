#include "WriteTransactionGroupMessage.h"

#include "Transaction.h"
#include "Configuration.h"


#include "evolution.h"
#include "MessageFormat.h"


#include "Bool.h"
#include "Debug.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int WriteTransactionGroupMessage::_dataType_ = 0;
		std::string WriteTransactionGroupMessage::_dataTypeName_("hfc::WriteTransactionGroupMessage");
	#endif



	WriteTransactionGroupMessage::WriteTransactionGroupMessage(bool normalOrder)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		reverseOrder_ = !normalOrder;
		times_.setQuotingElements(true);
		weights_.setQuotingElements(true);
		prices_.setQuotingElements(true);
	}


	WriteTransactionGroupMessage::~WriteTransactionGroupMessage()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.WriteTransactionGroupMessage", "    WriteTransactionGroupMessage: " + util_string::ptrtostring(this));
	}


	void WriteTransactionGroupMessage::addTransaction(Util::CSmartPtr<Transaction> trans)
	{
		if ( trans.notNull() )
		{
			HobString dumbString;
			std::string datetime = trans->getCommittedDateTimeAsString();

			std::string::size_type position = datetime.find(',');

			if ( std::string::npos != position )
				datetime.replace( position, 1, 1, ' ' );

			if ( reverseOrder_ )
			{
				ids_.push_front(util_string::longtostring(trans->getTransactionId()));
				times_.push_front(datetime);
				if ( trans->getOperatorName().size() )
					operators_.push_front(trans->getOperatorName());
				else
					operators_.push_front(trans->getOperatorRFID());
            #if defined(LINUX_FIX)
				types_.push_front(util_string::longtostring(trans->getTransactionTypeNumber()));
				plus_.push_front(util_string::longtostring(trans->getProductNumber()));
            #else
				types_.push_front(util_string::inttostring(trans->getTransactionTypeNumber()));
				plus_.push_front(util_string::inttostring(trans->getProductNumber()));
            #endif
				Configuration::getInstance()->formatWeight(trans->getFinalNetWeight(), dumbString);
				weights_.push_front(dumbString);
				prices_.push_front(Configuration::getInstance()->formatHobPrice(trans->getFinalPrice()));
				items_.push_front(util_string::inttostring(trans->getNumberItems()));
			}
			else
			{
				ids_.push_back(util_string::longtostring(trans->getTransactionId()));
				times_.push_back(datetime);
				if ( trans->getOperatorName().size() )
					operators_.push_back(trans->getOperatorName());
				else
					operators_.push_back(trans->getOperatorRFID());
            #if defined(LINUX_FIX)
				types_.push_back(util_string::longtostring(trans->getTransactionTypeNumber()));
				plus_.push_back(util_string::longtostring(trans->getProductNumber()));
            #else
				types_.push_back(util_string::inttostring(trans->getTransactionTypeNumber()));
				plus_.push_back(util_string::inttostring(trans->getProductNumber()));
            #endif
				Configuration::getInstance()->formatWeight(trans->getFinalNetWeight(), dumbString);
				weights_.push_back(dumbString);
				prices_.push_back(Configuration::getInstance()->formatHobPrice(trans->getFinalPrice()));
				items_.push_back(util_string::inttostring(trans->getNumberItems()));
			}
		}
	}


	const char* WriteTransactionGroupMessage::serialize(int format) const
	{
		serializedData.clear();

		switch ( format )
		{
			case BrowserFormat:
			{
				for (int loopCnt = 1; loopCnt < endWriteTransactionGroupOffsets; loopCnt++)
				{
					if (loopCnt > 1)
						serializedData.append(BrowserFormatSeparator);

					switch ( loopCnt )
					{
						case fieldOffsetWriteTransactionGroupIds:
							serializedData.append(ids_.serialize(format));
							break;
						case fieldOffsetWriteTransactionGroupDateTimes:
							serializedData.append(times_.serialize(format));
							break;
						case fieldOffsetWriteTransactionGroupOperators:
							serializedData.append(operators_.serialize(format));
							break;
						case fieldOffsetWriteTransactionGroupTypes:
							serializedData.append(types_.serialize(format));
							break;
						case fieldOffsetWriteTransactionGroupPLUs:
							serializedData.append(plus_.serialize(format));
							break;
						case fieldOffsetWriteTransactionGroupWeights:
							serializedData.append(weights_.serialize(format));
							break;
						case fieldOffsetWriteTransactionGroupPrices:
							serializedData.append(prices_.serialize(format));
							break;
						case fieldOffsetWriteTransactionGroupItems:
							serializedData.append(items_.serialize(format));
							break;
						case fieldOffsetWriteTransactionGroupPrevious:
							serializedData.append(Bool::toString(prevAvailable_));
							break;
						case fieldOffsetWriteTransactionGroupNext:
							serializedData.append(Bool::toString(nextAvailable_));
							break;
						default: break;
					}
				}

				break;	
			}
			default:
			{
				serializedData = "WriteTransactionGroupMessage::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}



}
