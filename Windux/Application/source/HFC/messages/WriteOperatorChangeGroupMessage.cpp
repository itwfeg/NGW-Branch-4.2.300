#include "WriteOperatorChangeGroupMessage.h"

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
	int WriteOperatorChangeGroupMessage::_dataType_ = 0;
	std::string WriteOperatorChangeGroupMessage::_dataTypeName_("hfc::WriteOperatorChangeGroupMessage");
#endif



	WriteOperatorChangeGroupMessage::WriteOperatorChangeGroupMessage(const std::string& operatorName, bool normalOrder)
	{
#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

		originalValues_.setQuotingElements(true);
		newValues_.setQuotingElements(true);
		totalPrices_.setQuotingElements(true);
		dateTimes_.setQuotingElements(true);
		operatorName_.assign(operatorName);
		reverseOrder_ = !normalOrder;
	}


	WriteOperatorChangeGroupMessage::~WriteOperatorChangeGroupMessage()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
#endif

		Debug::trace("Delete.WriteOperatorChangeGroupMessage", "    WriteOperatorChangeGroupMessage: " + util_string::ptrtostring(this));
	}


	void WriteOperatorChangeGroupMessage::addOperatorChange(Util::CSmartPtr<Transaction> trans, OperatorChange::OperatorChangeableField field)
	{
		if ( trans.notNull() )
		{
			std::string datetime = trans->getCommittedDateTimeAsString();

			// FT-876 do not replace if nothing found, exception will occur
			std::string::size_type pos( datetime.find(',') );

			if ( std::string::npos != pos )
				datetime.replace( pos, 1, 1, ' ' );

			if ( reverseOrder_ )
			{
				transactionIds_.push_front(util_string::longtostring(trans->getTransactionId()));
				dateTimes_.push_front(datetime);
				transactionTypes_.push_front(util_string::longtostring(trans->getTransactionTypeNumber()));
            #if defined(LINUX_FIX)
				plus_.push_front(util_string::longtostring(trans->getProductNumber()));
            #else
				plus_.push_front(util_string::inttostring(trans->getProductNumber()));
            #endif
				totalPrices_.push_front(Configuration::getInstance()->formatHobPrice(trans->getFinalPrice()));
				changedFields_.push_front(util_string::inttostring(field));
				originalValues_.push_front(trans->getOperatorChangeOldValue(field));
				newValues_.push_front(trans->getOperatorChangeNewValue(field));
			}
			else
			{
				transactionIds_.push_back(util_string::longtostring(trans->getTransactionId()));
				dateTimes_.push_back(datetime);
            #if defined(LINUX_FIX)
				transactionTypes_.push_back(util_string::longtostring(trans->getTransactionTypeNumber()));
				plus_.push_back(util_string::longtostring(trans->getProductNumber()));
            #else
				transactionTypes_.push_back(util_string::inttostring(trans->getTransactionTypeNumber()));
				plus_.push_back(util_string::inttostring(trans->getProductNumber()));
            #endif
				totalPrices_.push_back(Configuration::getInstance()->formatHobPrice(trans->getFinalPrice()));
				changedFields_.push_back(util_string::inttostring(field));
				originalValues_.push_back(trans->getOperatorChangeOldValue(field));
				newValues_.push_back(trans->getOperatorChangeNewValue(field));
			}
		}
	}


	const char* WriteOperatorChangeGroupMessage::serialize(int format) const
	{
		serializedData.clear();

		switch ( format )
		{
			case BrowserFormat:
			{
				for (int loopCnt = 1; loopCnt < endFieldOffsets; loopCnt++)
				{
					if (loopCnt > 1)
						serializedData.append(BrowserFormatSeparator);

					switch ( loopCnt )
					{
						case fieldOffsetOperatorName:
							serializedData.append(operatorName_);
							break;
						case fieldOffsetTransactionIds:
							serializedData.append(transactionIds_.serialize(format));
							break;
						case fieldOffsetDateTimes:
							serializedData.append(dateTimes_.serialize(format));
							break;
						case fieldOffsetTransactionTypes:
							serializedData.append(transactionTypes_.serialize(format));
							break;
						case fieldOffsetPLUs:
							serializedData.append(plus_.serialize(format));
							break;
						case fieldOffsetTotalPrices:
							serializedData.append(totalPrices_.serialize(format));
							break;
						case fieldOffsetChangedFields:
							serializedData.append(changedFields_.serialize(format));
							break;
						case fieldOffsetOriginalValues:
							serializedData.append(originalValues_.serialize(format));
							break;
						case fieldOffsetNewValues:
							serializedData.append(newValues_.serialize(format));
							break;
						case fieldOffsetPrevious:
							serializedData.append(Bool::toString(prevAvailable_));
							break;
						case fieldOffsetNext:
							serializedData.append(Bool::toString(nextAvailable_));
							break;
						default: break;
					}
				}

				break;	
			}
			default:
			{
				serializedData = "WriteOperatorChangeGroupMessage::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}



}
