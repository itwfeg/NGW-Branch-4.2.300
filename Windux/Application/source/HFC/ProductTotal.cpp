#include "ProductTotal.h"


#include "ProductTotalTags.h"

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
	int ProductTotal::dataType = 0;
	std::string ProductTotal::dataTypeName("hfc::ProductTotal"); 
#endif

	FieldOffsetsList ProductTotal::_uiFieldList_;
	FieldOffsetsList ProductTotal::_evolFieldList_;


	ProductTotal::ProductTotal(HobRecordNum deptNum, HobRecordNum pluNumber, HobRecordNum transTypeNumber)
		: Total()
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		deptNum_ = deptNum;
		productNumber_ = pluNumber;
		transTypeNumber_ = transTypeNumber;
	}


	ProductTotal::~ProductTotal()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void ProductTotal::init()
	{
		deptNum_ = RECORD_NOT_FOUND;
		productNumber_ = RECORD_NOT_FOUND;
		transTypeNumber_ = RECORD_NOT_FOUND;
		reportingClassNumber_ = RECORD_NOT_FOUND;

		initFieldLists();
	}


	void ProductTotal::initFieldLists()
	{
		if (_uiFieldList_.empty())
		{
			_uiFieldList_.push_back(fieldOffsetTotalProductNum);
			_uiFieldList_.push_back(fieldOffsetTotalReportingClassNum);
			_uiFieldList_.push_back(fieldOffsetTotalTransactionTypeNum);
			_uiFieldList_.push_back(fieldOffsetTotalFlags);
			_uiFieldList_.push_back(fieldOffsetTotalWeight);
			_uiFieldList_.push_back(fieldOffsetTotalPrice);
			_uiFieldList_.push_back(fieldOffsetTotalPackages);
			_uiFieldList_.push_back(fieldOffsetTotalRuns);
			_uiFieldList_.push_back(fieldOffsetTotalItems);
		}
		if (_evolFieldList_.empty() )
		{
			_evolFieldList_.push_back(fieldOffsetTotalDeptNum);
			_evolFieldList_.push_back(fieldOffsetTotalProductNum);
			_evolFieldList_.push_back(fieldOffsetTotalReportingClassNum);
			_evolFieldList_.push_back(fieldOffsetTotalTransactionTypeNum);
			_evolFieldList_.push_back(fieldOffsetTotalFlags);
			_evolFieldList_.push_back(fieldOffsetTotalWeight);
			_evolFieldList_.push_back(fieldOffsetTotalPrice);
			_evolFieldList_.push_back(fieldOffsetTotalPackages);
			_evolFieldList_.push_back(fieldOffsetTotalRuns);
			_evolFieldList_.push_back(fieldOffsetTotalItems);

			ProductTotalTags totalTags(&_evolFieldList_);
		}
	}


	void ProductTotal::getFieldAsString(int fieldCode, HobString &result, int format) const
	{
	//	Right now we don't do anything with format but keep it in case we need it.
		result.clear();

		switch ( fieldCode )
		{
			case fieldOffsetTotalDeptNum:
				result = util_string::longtostring(deptNum_);
				break;

			case fieldOffsetTotalProductNum:
				result = util_string::longtostring(productNumber_);
				break;

			case fieldOffsetTotalTransactionTypeNum:
				result = util_string::longtostring(transTypeNumber_);
				break;

			case fieldOffsetTotalReportingClassNum:
				result = util_string::longtostring(reportingClassNumber_);
				break;

			default:
				Total::getFieldAsString(fieldCode, result, format);
				break;
		}
	}


	const char *ProductTotal::serialize(int format) const
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
				FieldOffsetsList::iterator fieldIter = _uiFieldList_.begin();

				while ( fieldIter != _uiFieldList_.end() )
				{
					if (appendSeparator)
						serializedData.append(BrowserFormatSeparator);
					else
						appendSeparator = true;

					getFieldAsString(*fieldIter, fieldData, format);

					serializedData.append(fieldData);

					fieldIter++;
				}

				break;
			}
			case EvolutionTaggedFormat:
			case DBIndependentFormatFormat:
			{
				bool appendSeparator = false;
				HobString fieldData;
				ProductTotalTags totalTags;
				HobString tag;

				for ( FieldOffsetsList::iterator fieldIter = _evolFieldList_.begin(); fieldIter != _evolFieldList_.end(); ++fieldIter )
				{
					tag = totalTags.getTag(*fieldIter);
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
				serializedData = "ProductTotal::serialize - unknown format:" + util_string::inttostring(format);
				break;
			}
		}

		Debug::trace("ProductTotal", HobString("serializedData = [") + serializedData + "]");
		return serializedData.data();
	}


	void ProductTotal::show()
	{
		if ( Debug::isTracing("ProductTotal") )
		{
			Debug::trace("ProductTotal", "***  ProductTotal data for productNum" + util_string::longtostring(productNumber_) + "  deptNum" + util_string::longtostring(deptNum_) + "  ***");
			Debug::trace("ProductTotal", "totalWeight:   " + util_string::longtostring(getWeight()));
			Debug::trace("ProductTotal", "totalPrice:    " + util_string::longtostring(getPrice()));
			Debug::trace("ProductTotal", "totalPackages: " + util_string::longtostring(getPackages()));
			Debug::trace("ProductTotal", "totalRuns:     " + util_string::longtostring(getRuns()));
			Debug::trace("ProductTotal", "totalItems:    " + util_string::longtostring(getItems()));
		}
	}


}
