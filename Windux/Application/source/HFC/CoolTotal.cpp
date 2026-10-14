#include "CoolTotal.h"


#include "COOLTotalTags.h"

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
	int COOLTotal::dataType = 0;
	std::string COOLTotal::dataTypeName("hfc::COOLTotal"); 
#endif

	FieldOffsetsList COOLTotal::_fieldList_, COOLTotal::_ultimaFieldList_;


	COOLTotal::COOLTotal(HobRecordNum pluNumber, HobRecordNum transTypeNumber, std::string trackingText, std::string coolText)
		: Total()
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		productNumber_ = pluNumber;
		transTypeNumber_ = transTypeNumber;
		trackingText_.assign(trackingText);
		coolText_.assign(coolText);
	}


	COOLTotal::~COOLTotal()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void COOLTotal::init()
	{
		productNumber_ = RECORD_NOT_FOUND;
		transTypeNumber_ = RECORD_NOT_FOUND;
		trackingText_.clear();
		firstDateTime_ = HobDateTime();
		lastDateTime_ = HobDateTime();
		coolText_.clear();
		coolTextVaries_ = false;

		initFieldList();
	}


	void COOLTotal::initFieldList()
	{
		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(fieldOffsetTotalProductNum);
			_fieldList_.push_back(fieldOffsetTotalTransactionTypeNum);
			_fieldList_.push_back(fieldOffsetTotalCOOLTrackingText);
			_fieldList_.push_back(fieldOffsetTotalCOOLFirstLogDate);
			_fieldList_.push_back(fieldOffsetTotalCOOLFirstLogTime);
			_fieldList_.push_back(fieldOffsetTotalCOOLText);
			_fieldList_.push_back(fieldOffsetTotalWeight);
			_fieldList_.push_back(fieldOffsetTotalPrice);
			_fieldList_.push_back(fieldOffsetTotalPackages);
			_fieldList_.push_back(fieldOffsetTotalRuns);
			_fieldList_.push_back(fieldOffsetTotalItems);

			COOLTotalTags totalTags(&_fieldList_);
		}

		if ( _ultimaFieldList_.empty() )
		{
			_ultimaFieldList_.push_back(fieldOffsetTotalProductNum);
			_ultimaFieldList_.push_back(fieldOffsetTotalCOOLFirstLogDate);
			_ultimaFieldList_.push_back(fieldOffsetTotalCOOLFirstLogTime);
			_ultimaFieldList_.push_back(fieldOffsetTotalCOOLLastLogDate);
			_ultimaFieldList_.push_back(fieldOffsetTotalCOOLLastLogTime);
			_ultimaFieldList_.push_back(fieldOffsetTotalCOOLText);
			_ultimaFieldList_.push_back(fieldOffsetTotalCOOLTrackingText);
			_ultimaFieldList_.push_back(fieldOffsetTotalWeight);
			_ultimaFieldList_.push_back(fieldOffsetTotalItems);
			_ultimaFieldList_.push_back(fieldOffsetTotalPackages);
			_ultimaFieldList_.push_back(fieldOffsetTotalPrice);
			_ultimaFieldList_.push_back(fieldOffsetTotalRuns);
			_ultimaFieldList_.push_back(fieldOffsetTotalTransactionTypeNum);

			COOLTotalTags coolTags(&_ultimaFieldList_);
		}
	}


	void COOLTotal::getFieldAsString(int fieldCode, HobString &result, int format) const
	{
		result.clear();

		switch ( fieldCode )
		{
			case fieldOffsetTotalProductNum:
				result = util_string::longtostring(productNumber_);
				break;

			case fieldOffsetTotalTransactionTypeNum:
				result = util_string::longtostring(transTypeNumber_);
				break;

			case fieldOffsetTotalCOOLTrackingText:
				result = trackingText_;
				break;

			case fieldOffsetTotalCOOLFirstLogDate:
			{
				char strDate[10] = {'\0'};
				sprintf(strDate, "%02d%02d%02d", firstDateTime_.getMonth(), firstDateTime_.getDay(), firstDateTime_.getYear2());
				result = HobString(strDate);
				break;
			}
			case fieldOffsetTotalCOOLFirstLogTime:
			{
				char strDate[10] = {'\0'};
				sprintf(strDate, "%02d%02d%02d", firstDateTime_.getHour24(), firstDateTime_.getMinute(), firstDateTime_.getSecond());
				result = HobString(strDate);
				break;
			}
			case fieldOffsetTotalCOOLLastLogDate:
			{
				char strDate[10] = {'\0'};
				sprintf(strDate, "%02d%02d%02d", lastDateTime_.getMonth(), lastDateTime_.getDay(), lastDateTime_.getYear2());
				result = HobString(strDate);
				break;
			}
			case fieldOffsetTotalCOOLLastLogTime:
			{
				char strDate[10] = {'\0'};
				sprintf(strDate, "%02d%02d%02d", lastDateTime_.getHour24(), lastDateTime_.getMinute(), lastDateTime_.getSecond());
				result = HobString(strDate);
				break;
			}
			case fieldOffsetTotalCOOLText:
				result = coolText_;
				break;

			default:
				Total::getFieldAsString(fieldCode, result, format);
				break;
		}
	}


	const char *COOLTotal::serialize(int format) const
	{
		serializedData = "";

		if (getFormatType() != UnknownFormat)
			format = getFormatType();

		switch ( format )
		{
			case EvolutionTaggedFormat:
			case DBIndependentFormatFormat:
			{
				bool appendSeparator = false;
				HobString fieldData;
				COOLTotalTags coolTags;
				HobString tag;

				for ( FieldOffsetsList::iterator fieldIter = _fieldList_.begin(); fieldIter != _fieldList_.end(); ++fieldIter )
				{
					tag = coolTags.getTag(*fieldIter);
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
			case UltimaTaggedFormat:
			{
				bool appendSeparator = false;
				HobString fieldData;
				COOLTotalTags coolTags;
				HobString tag;

				for ( FieldOffsetsList::iterator fieldIter = _ultimaFieldList_.begin(); fieldIter != _ultimaFieldList_.end(); ++fieldIter )
				{
					tag = coolTags.getTag(*fieldIter);
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
				serializedData = "COOLTotal::serialize - unknown format:" + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}


	void COOLTotal::show()
	{
		if ( Debug::isTracing("COOLTotal") )
		{
			Debug::trace("COOLTotal", "***  COOLTotal data for productNum: " + util_string::longtostring(getProductNumber()) +
				"  transactionTypeNum: " + util_string::longtostring(getTransactionTypeNumber()));
			Debug::trace("COOLTotal", "trackingText:  " + trackingText_);
			Debug::trace("COOLTotal", "firstDateTimeStamp: " + firstDateTime_.convertToDBFormat());
			Debug::trace("COOLTotal", "lastDateTimeStamp: " + lastDateTime_.convertToDBFormat());
			Debug::trace("COOLTotal", "coolText:      " + coolText_);
			Debug::trace("COOLTotal", "coolTextVaries: " + util_string::booltostring(coolTextVaries_));
			Debug::trace("COOLTotal", "Weight:        " + util_string::longtostring(getWeight()));
			Debug::trace("COOLTotal", "Price:         " + util_string::longtostring(getPrice()));
			Debug::trace("COOLTotal", "Packages:      " + util_string::longtostring(getPackages()));
			Debug::trace("COOLTotal", "Runs:          " + util_string::longtostring(getRuns()));
			Debug::trace("COOLTotal", "Items:         " + util_string::longtostring(getItems()));
		}
	}


}
