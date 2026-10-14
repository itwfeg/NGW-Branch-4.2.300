#include "OperatorChange.h"


#include "Product.h"
#include "HobOperator.h"
#include "Transaction.h"

#include "tags/OperatorChangeTags.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


#ifdef USE_ACCOUNTANT
	int OperatorChange::dataType = 0;
	std::string OperatorChange::dataTypeName("hfc::OperatorChange"); 
#endif

	FieldOffsetsList OperatorChange::_evolTaggedFieldList_;
	FieldOffsetsList OperatorChange::_dbIndependentFieldList_;


	OperatorChange::OperatorChange(Util::CSmartPtr<Transaction> trans, OperatorChange::OperatorChangeableField field)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		operatorId_.assign(trans->getOperatorId());
		operatorRFID_.assign(trans->getOperatorRFID());
		productNumber_ = trans->getProductNumber();
		changedField_ = field;
		newValue_ = trans->getOperatorChangeNewValue(field);
		oldValue_ = trans->getOperatorChangeOldValue(field);
		transType_ = trans->getTransactionTypeNumber();
		dateTime_ = trans->getCommittedDateTimeAsString();
		totalWeight_ = trans->getFinalNetWeight();
		totalPrice_ = trans->getFinalPrice();

		errorCode_ = RECORD_OK;
	}


	OperatorChange::~OperatorChange()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void OperatorChange::init()
	{
		errorCode_ = RECORD_NOT_FOUND;

		operatorId_.clear();
		operatorRFID_.clear();
		dateTime_.clear();
		transType_ = RECORD_NOT_FOUND;
		productNumber_ = RECORD_NOT_FOUND;
		totalPrice_ = 0;
		totalWeight_ = 0;
		changedField_ = OC_Unknown;
		newValue_.clear();
		oldValue_.clear();

		initFieldList();
	}


	void OperatorChange::initFieldList()
	{
		if ( _dbIndependentFieldList_.empty() )
		{
			_dbIndependentFieldList_.push_back(fieldOffsetOperatorId);
			_dbIndependentFieldList_.push_back(fieldOffsetOperatorRFID);
			_dbIndependentFieldList_.push_back(fieldOffsetDateTime);
			_dbIndependentFieldList_.push_back(fieldOffsetTransactionType);
			_dbIndependentFieldList_.push_back(fieldOffsetProductNumber);
			_dbIndependentFieldList_.push_back(fieldOffsetTotalPrice);
			_dbIndependentFieldList_.push_back(fieldOffsetTotalWeight);
			_dbIndependentFieldList_.push_back(fieldOffsetChangedField);
			_dbIndependentFieldList_.push_back(fieldOffsetNewValue);
			_dbIndependentFieldList_.push_back(fieldOffsetOldValue);

			OperatorChangeTags tags(&_dbIndependentFieldList_);
		}


		if ( _evolTaggedFieldList_.empty() )
		{
			_evolTaggedFieldList_.push_back(fieldOffsetOperatorId);
			_evolTaggedFieldList_.push_back(fieldOffsetOperatorRFID);
			_evolTaggedFieldList_.push_back(fieldOffsetDate);
			_evolTaggedFieldList_.push_back(fieldOffsetTime);
			_evolTaggedFieldList_.push_back(fieldOffsetTransactionType);
			_evolTaggedFieldList_.push_back(fieldOffsetProductNumber);
			_evolTaggedFieldList_.push_back(fieldOffsetTotalPrice);
			_evolTaggedFieldList_.push_back(fieldOffsetTotalWeight);
			_evolTaggedFieldList_.push_back(fieldOffsetChangedField);
			_evolTaggedFieldList_.push_back(fieldOffsetNewValue);
			_evolTaggedFieldList_.push_back(fieldOffsetOldValue);

			OperatorChangeTags tags(&_evolTaggedFieldList_);
		}
	}


	void OperatorChange::associateOperator(HobOperator* oper)
	{
		if ( oper )
		{
			setOperatorId(oper->getOpNum());
			if ( oper->getLoginMethod() == HobOperator::LOGINMETHOD_RFID )
				setOperatorRFID(oper->getOpRFID());
		}
	}


	void OperatorChange::getFieldAsString(int fieldCode, HobString &result, int UNUSED(format)) const
	{
	//	Right now we don't do anything with format but keep it in case we need it.
		result = "";

		switch (fieldCode)
		{
			case fieldOffsetOperatorId:
			{
				result = operatorId_;

				break;
			}
			case fieldOffsetOperatorRFID:
			{
				result = operatorRFID_;

				break;
			}
			case fieldOffsetDateTime:
			{
				result = getChangeDateTimeAsString();

				break;
			}
			case fieldOffsetDate:
			{
				result = getChangeDateTime().formatDate(FormatDate_MMDDYY_NoDelimiter);

				break;
			}
			case fieldOffsetTime:
			{
				result = getChangeDateTime().formatTime(FormatTime_HHMMSS_NoDelimiter, HourFormat_MILITARY);

				break;
			}
			case fieldOffsetTransactionType:
			{
				result = util_string::longtostring(transType_);

				break;
			}
			case fieldOffsetProductNumber:
			{
				result = util_string::longtostring(productNumber_);

				break;
			}
			case fieldOffsetTotalPrice:
			{
				result = util_string::longtostring(totalPrice_);

				break;
			}
			case fieldOffsetTotalWeight:
			{
				result = util_string::longtostring(totalWeight_);

				break;
			}
			case fieldOffsetChangedField:
			{
				result = util_string::inttostring(changedField_);

				break;
			}
			case fieldOffsetNewValue:
			{
				result = newValue_;

				break;
			}
			case fieldOffsetOldValue:
			{
				result = oldValue_;

				break;
			}
			default:
				break;
		}
	}


	const char *OperatorChange::serialize(int format) const
	{
		serializedData.clear();

		if (getFormatType() != UnknownFormat)
			format = getFormatType();

		switch ( format )
		{
			case BrowserFormat:
			{
				bool appendSeparator = false;
				HobString fieldData;
				FieldOffsetsList::iterator fieldIter = _evolTaggedFieldList_.begin();

				while ( fieldIter != _evolTaggedFieldList_.end() )
				{
					if (appendSeparator)
						serializedData.append(BrowserFormatSeparator);
					else
						appendSeparator = true;

					getFieldAsString(*fieldIter, fieldData, format);

					serializedData.append(fieldData);

					++fieldIter;
				}

				break;
			}
			case EvolutionTaggedFormat:
			{
				bool appendSeparator = false;
				HobString fieldData;
				OperatorChangeTags tags;
				HobString tag;

				for ( FieldOffsetsList::iterator fieldIter = _evolTaggedFieldList_.begin(); fieldIter != _evolTaggedFieldList_.end();  ++fieldIter )
				{
					tag = tags.getTag(*fieldIter);
					if ( tag.size() )
					{
						getFieldAsString(*fieldIter, fieldData, format);

						if ( appendSeparator )
							serializedData.append(UnitSeparator);
						else
							appendSeparator = true;

						serializedData.append(tag);
						serializedData.append(fieldData);
					}
				}

				break;
			}
			case DBIndependentFormatFormat:
			{
				bool appendSeparator = false;
				HobString fieldData;
				OperatorChangeTags tags;
				HobString tag;

				for ( FieldOffsetsList::iterator fieldIter = _dbIndependentFieldList_.begin(); fieldIter != _dbIndependentFieldList_.end();  ++fieldIter )
				{
					tag = tags.getTag(*fieldIter);
					if ( tag.size() )
					{
						getFieldAsString(*fieldIter, fieldData, format);

						if ( appendSeparator )
							serializedData.append(UnitSeparator);
						else
							appendSeparator = true;

						serializedData.append(tag);
						serializedData.append(fieldData);
					}
				}

				break;
			}
			default:
			{
				serializedData = "OperatorChange::serialize - unknown format:" + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}


}
