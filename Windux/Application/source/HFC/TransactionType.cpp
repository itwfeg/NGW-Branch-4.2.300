#include "TransactionType.h"
#include "TransactionTypeTags.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Bool.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int TransactionType::_dataType_ = 0;
		std::string TransactionType::_dataTypeName_("hfc::TransactionType");
	#endif


	FieldOffsetsList TransactionType::_fieldList_;
	FieldOffsetsList TransactionType::_runModeList_;


	TransactionType::TransactionType()
	{
#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

		init();
	}


	TransactionType::TransactionType(const char *data, int len, int formatType)
	{
#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
#endif

		init();

		switch (formatType)
		{
			case BrowserFormat:
			{
				HobString temp(data, len);
				FieldOffsetsList::iterator fieldIter = _fieldList_.begin();
				int offset;

				HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
				HobString token;

				while ((tok.next(&token) != -1) && (fieldIter != _fieldList_.end()))
				{
					offset = *fieldIter;
					setReceived(offset, (token.length() == 0) ? false : true);

					if (token.length())
					{
						if (isFieldQuoted(offset))
							token.unQuoteForCSV();
						updateField(offset, token);
						if ( offset == TransactionTypeTags::fieldOffsetTransactionTypeFlags )
						{
							setReceived(TransactionTypeTags::fieldOffsetTransactionTypeAffectsProduction, true);
							setReceived(TransactionTypeTags::fieldOffsetTransactionTypeManualChangeUPCDigit, true);
							setReceived(TransactionTypeTags::fieldOffsetTransactionTypeAutomaticChangeUPCDigit, true);
							setReceived(TransactionTypeTags::fieldOffsetTransactionTypeAllowProductBackDating, true);
							setReceived(TransactionTypeTags::fieldOffsetTransactionTypeUseBackgroundColor, true);
						}
					}
					fieldIter++;
				}
				break;
			}

			case SynchronizationTaggedFormat:
			case EvolutionTaggedFormat:
			case DBIndependentFormatFormat:
			{
				TransactionTypeTags transactionTypeTags;

				//convert data to HobString
				HobString evolData(data, len);

				//use the UnitSeparator to parse the string
				HobTokenizerUS tok(&evolData, UnitSeparator);
				HobString temp = "";

				int offset = 0;
				HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp, HobTaggedToken));
				if (token)
				{
					while (tok.next(token) != -1)
					{
						//get the tag and find out what offset goes with the tag
						temp = token->getTag();
						offset = (temp.size()) ? transactionTypeTags.getOffset(temp) : 0;
						if (offset)
						{
							setReceived(offset, true);
							temp = token->getData();
							updateField(offset, temp);
							if ( offset == TransactionTypeTags::fieldOffsetTransactionTypeFlags )
							{
								setReceived(TransactionTypeTags::fieldOffsetTransactionTypeAffectsProduction, true);
								setReceived(TransactionTypeTags::fieldOffsetTransactionTypeManualChangeUPCDigit, true);
								setReceived(TransactionTypeTags::fieldOffsetTransactionTypeAutomaticChangeUPCDigit, true);
								setReceived(TransactionTypeTags::fieldOffsetTransactionTypeAllowProductBackDating, true);
								setReceived(TransactionTypeTags::fieldOffsetTransactionTypeUseBackgroundColor, true);
							}
						}
					}
					delete token;
				}
				break;
			}

			case UltimaTaggedFormat:
			{
				TransactionTypeTags transactionTypeTags;

				//convert data to HobString
				HobString evolData(data, len);

				//use the UnitSeparator to parse the string
				HobTokenizerUS tok(&evolData, UnitSeparator);
				HobString temp = "";

				int offset = 0;
				HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp, HobTaggedToken));
				if (token)
				{
					while (tok.next(token) != -1)
					{
						//get the tag and find out what offset goes with the tag
						temp = token->getTag();
						offset = (temp.size()) ? transactionTypeTags.getOffset(temp) : 0;
						if (offset)
						{
							setReceived(offset, true);
							temp = token->getData();
							updateField(offset, temp);
							if ( offset == TransactionTypeTags::fieldOffsetTransactionTypeFlags )
							{
								setReceived(TransactionTypeTags::fieldOffsetTransactionTypeAffectsProduction, true);
								setReceived(TransactionTypeTags::fieldOffsetTransactionTypeManualChangeUPCDigit, true);
								setReceived(TransactionTypeTags::fieldOffsetTransactionTypeAutomaticChangeUPCDigit, true);
								setReceived(TransactionTypeTags::fieldOffsetTransactionTypeAllowProductBackDating, true);
								setReceived(TransactionTypeTags::fieldOffsetTransactionTypeUseBackgroundColor, true);
							}
						}
					}
					delete token;
				}
				break;
			}

			default:
				// todo - throw exception("invalid format");
				break;
		}

		updateWithTransactionTypeFlag();
	}


	TransactionType::~TransactionType()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif
	}


	void TransactionType::init()
	{
		errorCode_ = RECORD_NOT_FOUND;

		number_ = (HobRecordNum)RECORD_NOT_FOUND;
		name1_.clear();
		name2_.clear();
		grossWeight_ = 0;
		affectsProduction_ = AffectsProductionPositively;
		automaticChangeUPCDigit_ = false;
		manualChangeUPCDigit_ = false;
		upcChangeDigit_ = "0";
		allowProductBackDating_ = false;
		useBackgroundColor_ = false;
		backgroundColor_ = 0;
		textForLabel_ = "";
		transactionTypeFlag_ = 0;

		initFieldList();
	}


	void TransactionType::initFieldList()
	{
		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeNum);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeName1);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeName2);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeGrossWeight);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeUpcChangeDigit);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeAffectsProduction);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeManualChangeUPCDigit);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeAutomaticChangeUPCDigit);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeAllowProductBackDating);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeFlags);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeUseBackgroundColor);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeBackgroundColor);
			_fieldList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeTextForLabel);

			TransactionTypeTags transactionTypeTags(&_fieldList_);
		}

		if ( _runModeList_.empty() )
		{
			_runModeList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeNum);
			_runModeList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeName1);
			_runModeList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeName2);
			_runModeList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeManualChangeUPCDigit);
			_runModeList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeAllowProductBackDating);
			_runModeList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeUseBackgroundColor);
			_runModeList_.push_back(TransactionTypeTags::fieldOffsetTransactionTypeBackgroundColor);

			TransactionTypeTags transactionTypeTags(&_runModeList_);
		}
	}


	void TransactionType::setErrorCode(HobErrorCode errorCode)
	{
		errorCode_ = errorCode;

		Debug::trace("TransactionType", "setErrorCode():  errorCode = " + util_string::inttostring(errorCode_));
	}


	void TransactionType::refreshTransactionTypeFlag() const
	{
		transactionTypeFlag_ = 0;

		if ( manualChangeUPCDigit_ )
			transactionTypeFlag_ |= TTManualChangeDigit;

		if ( automaticChangeUPCDigit_ )
			transactionTypeFlag_ |= TTAutoChangeDigit;

		if ( allowProductBackDating_ )
			transactionTypeFlag_ |= TTAllowBackDating;

		if ( affectsProduction_ == AffectsProductionPositively )
			transactionTypeFlag_ |= TTProductTotals;

		if ( useBackgroundColor_ )
			transactionTypeFlag_ |= TTUseBgColor;
	}


	void TransactionType::updateTransactionType(const TransactionType *newTransactionType)
	{
		if ( !newTransactionType ) return;


		for ( int loopCnt = 1; loopCnt < TransactionTypeTags::endTransactionTypeFieldOffsets; ++loopCnt )
		{
			if ( newTransactionType->isReceived(loopCnt) )
			{
				switch ( loopCnt )
				{
					case TransactionTypeTags::fieldOffsetTransactionTypeNum:
						setNumber(newTransactionType->getNumber());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeName1:
						setName1(newTransactionType->getName1());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeName2:
						setName2(newTransactionType->getName2());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeGrossWeight:
						setGrossWeight(newTransactionType->getGrossWeight());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeUpcChangeDigit:
						setUPCChangeDigit(newTransactionType->getUPCChangeDigit());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeAffectsProduction:
						setAffectsProduction(newTransactionType->getAffectsProduction());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeManualChangeUPCDigit:
						setManualChangeUPCDigit(newTransactionType->isManualChangeUPCDigit());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeAutomaticChangeUPCDigit:
						setAutomaticChangeUPCDigit(newTransactionType->isAutomaticChangeUPCDigit());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeAllowProductBackDating:
						setAllowProductBackDating(newTransactionType->isAllowProductBackDating());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeUseBackgroundColor:
						setUseBackgroundColor(newTransactionType->isUseBackgroundColor());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeBackgroundColor:
						setBackgroundColor(newTransactionType->getBackgroundColor());
						break;
					case TransactionTypeTags::fieldOffsetTransactionTypeTextForLabel:
						setTextForLabel(newTransactionType->getTextForLabel());
						break;
					default:
						break;
				}
			}
		}
	}


	void TransactionType::updateWithTransactionTypeFlag()
	{
		if ( isReceived(TransactionTypeTags::fieldOffsetTransactionTypeFlags) )
		{
			if ( !isReceived(TransactionTypeTags::fieldOffsetTransactionTypeAffectsProduction) && (transactionTypeFlag_ & TTProductTotals) )
				affectsProduction_ = AffectsProductionPositively;
			if ( !isReceived(TransactionTypeTags::fieldOffsetTransactionTypeManualChangeUPCDigit) && (transactionTypeFlag_ & TTManualChangeDigit) )
				manualChangeUPCDigit_ = true;
			if ( !isReceived(TransactionTypeTags::fieldOffsetTransactionTypeAutomaticChangeUPCDigit) && (transactionTypeFlag_ & TTAutoChangeDigit) )
				automaticChangeUPCDigit_ = true;
			if ( !isReceived(TransactionTypeTags::fieldOffsetTransactionTypeAllowProductBackDating) && (transactionTypeFlag_ & TTAllowBackDating) )
				allowProductBackDating_ =  true;
			if ( !isReceived(TransactionTypeTags::fieldOffsetTransactionTypeUseBackgroundColor) && (transactionTypeFlag_ & TTUseBgColor) )
				useBackgroundColor_ =  true;
		}
	}


	const char *TransactionType::serialize(int format) const
	{
		serializedData.clear();

		if ( getFormatType() != UnknownFormat )
			format = getFormatType();

		Debug::trace("TransactionType", "serialize():  format = " + util_string::inttostring(format));

		switch ( format )
		{
			case BrowserFormat:
			{
				bool firstTime = true;
				HobString fieldData = "";
				FieldOffsetsList::iterator fieldIter = _fieldList_.begin();

				while ( fieldIter != _fieldList_.end() )
				{
					if ( firstTime )
						firstTime = false;
					else
						serializedData.append(BrowserFormatSeparator);

					fieldData = getFieldAsString(*fieldIter);
					if ( isFieldQuoted(*fieldIter) )
						fieldData.quoteForCSV();

					serializedData.append(fieldData);

					++fieldIter;
				}

				break;
			}
			case CurrentRunFormat:
			{
				bool firstTime = true;
				HobString fieldData = "";
				FieldOffsetsList::iterator fieldIter = _runModeList_.begin();

				while ( fieldIter != _runModeList_.end() )
				{
					if ( firstTime )
						firstTime = false;
					else
						serializedData.append(BrowserFormatSeparator);

					fieldData = getFieldAsString(*fieldIter);
					if (isFieldQuoted(*fieldIter))
						fieldData.quoteForCSV();

					serializedData.append(fieldData);

					++fieldIter;
				}
				break;
			}

			case SynchronizationTaggedFormat:
			{
				HobString tag;
				HobString fieldData;
				TransactionTypeTags transactionTypeTags;

				FieldOffsetsList::iterator fieldIter = _fieldList_.begin();
				while ( fieldIter != _fieldList_.end() )
				{
					tag = transactionTypeTags.getTag(*fieldIter);
					fieldData = getFieldAsString(*fieldIter);

					if ( tag.size() )
					{
						serializedData.append(tag);
						serializedData.append(fieldData);
						serializedData.append(UnitSeparator);
					}

					fieldIter++;
				}

				break;
			}
			case EvolutionTaggedFormat:
			case DBIndependentFormatFormat:
			{
				HobString tag;
				HobString fieldData;
				TransactionTypeTags transactionTypeTags;

				FieldOffsetsList::iterator fieldIter = _fieldList_.begin();
				while ( fieldIter != _fieldList_.end() )
				{
					tag = transactionTypeTags.getTag(*fieldIter);
					fieldData = getFieldAsString(*fieldIter);

					if ( tag.size() )
					{
						serializedData.append(tag);
						serializedData.append(fieldData);
						serializedData.append(UnitSeparator);
					}

					fieldIter++;
				}

				break;
			}
			case UltimaTaggedFormat:
			{
				TransactionTypeTags transactionTypeTags;
				HobString tag = "";
				HobString fieldData = "";

				FieldOffsetsList::iterator fieldIter = _fieldList_.begin();
				while (fieldIter != _fieldList_.end())
				{
					tag = transactionTypeTags.getTag(*fieldIter);
					switch (*fieldIter)
					{
						case TransactionTypeTags::fieldOffsetTransactionTypeNum:
						case TransactionTypeTags::fieldOffsetTransactionTypeName1:
						case TransactionTypeTags::fieldOffsetTransactionTypeName2:
						case TransactionTypeTags::fieldOffsetTransactionTypeUpcChangeDigit:
						case TransactionTypeTags::fieldOffsetTransactionTypeFlags:
							fieldData = getFieldAsString(*fieldIter);
							break;
						default:
							tag = "";
							fieldData = "";
							break;
					}

					if (tag.size())
					{
						serializedData.append(tag);
						serializedData.append(fieldData);
						serializedData.append(UnitSeparator);
					}
					fieldIter++;
				}
				break;
			}

			default:
				serializedData = "TransactionType::serialize - unknown format:" + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
		}

		return serializedData.c_str();
	}


	std::string TransactionType::getFieldAsString(int fieldCode) const
	{
		switch (fieldCode)
		{
			case TransactionTypeTags::fieldOffsetTransactionTypeNum:
				return util_string::longtostring(getNumber());
			case TransactionTypeTags::fieldOffsetTransactionTypeName1:
				return getName1();
			case TransactionTypeTags::fieldOffsetTransactionTypeName2:
				return getName2();
			case TransactionTypeTags::fieldOffsetTransactionTypeGrossWeight:
				return util_string::longtostring(getGrossWeight());
			case TransactionTypeTags::fieldOffsetTransactionTypeUpcChangeDigit:
				return getUPCChangeDigit();
			case TransactionTypeTags::fieldOffsetTransactionTypeAffectsProduction:
				return util_string::inttostring(getAffectsProduction());
			case TransactionTypeTags::fieldOffsetTransactionTypeManualChangeUPCDigit:
				return Bool::toString(isManualChangeUPCDigit());
			case TransactionTypeTags::fieldOffsetTransactionTypeAutomaticChangeUPCDigit:
				return Bool::toString(isAutomaticChangeUPCDigit());
			case TransactionTypeTags::fieldOffsetTransactionTypeAllowProductBackDating:
				return Bool::toString(isAllowProductBackDating());
			case TransactionTypeTags::fieldOffsetTransactionTypeUseBackgroundColor:
				return Bool::toString(isUseBackgroundColor());
			case TransactionTypeTags::fieldOffsetTransactionTypeBackgroundColor:
				return util_string::longtostring(getBackgroundColor());
			case TransactionTypeTags::fieldOffsetTransactionTypeTextForLabel:
				return getTextForLabel();
			case TransactionTypeTags::fieldOffsetTransactionTypeFlags:
				return util_string::uinttostring(getTransactionTypeFlag());
			case TransactionTypeTags::fieldOffsetTransactionTypeErrorCode:
				return util_string::inttostring(getErrorCode());
			default:
				return "";
		}
	}


	bool TransactionType::isFieldQuoted(int fieldOffset) const
	{
		switch ( fieldOffset )
		{
			case TransactionTypeTags::fieldOffsetTransactionTypeName1:
			case TransactionTypeTags::fieldOffsetTransactionTypeName2:
			case TransactionTypeTags::fieldOffsetTransactionTypeTextForLabel:
				return true;

			default:
				return false;
		}
	}


	void TransactionType::updateField(int fieldCode, const HobString &token)
	{
		Debug::trace("TransactionType", "updateField() - fieldCode: " + util_string::inttostring(fieldCode) + "  data: " + token);

		switch ( fieldCode )
		{
			case TransactionTypeTags::fieldOffsetTransactionTypeNum:
				setNumber((HobRecordNum)util_string::stringtolong(token));
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeName1:
				setName1(token);
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeName2:
				setName2(token);
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeGrossWeight:
				setGrossWeight((HobWeight)util_string::stringtolong(token));
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeUpcChangeDigit:
				setUPCChangeDigit(token);
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeAffectsProduction:
				setAffectsProduction((AffectsProductionFlag)util_string::stringtoint(token));
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeManualChangeUPCDigit:
				setManualChangeUPCDigit(Bool::valueOf(token));
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeAutomaticChangeUPCDigit:
				setAutomaticChangeUPCDigit(Bool::valueOf(token));
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeAllowProductBackDating:
				setAllowProductBackDating(Bool::valueOf(token));
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeUseBackgroundColor:
				setUseBackgroundColor(Bool::valueOf(token));
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeBackgroundColor:
				setBackgroundColor(util_string::stringtolong(token));
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeTextForLabel:
				setTextForLabel(token);
				break;
			case TransactionTypeTags::fieldOffsetTransactionTypeFlags:
				setTransactionTypeFlag(util_string::stringtoint(token));
				break;
			default:
				break;
		}
	}


}
