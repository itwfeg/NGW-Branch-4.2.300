#include "ClassTotal.h"


#include "ClassTotalTags.h"

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
	int ClassTotal::dataType = 0;
	std::string ClassTotal::dataTypeName("hfc::ClassTotal"); 
#endif

	FieldOffsetsList ClassTotal::_uiFieldList_;
	FieldOffsetsList ClassTotal::_evolFieldList_;


	ClassTotal::ClassTotal(HobRecordNum deptNum, HobRecordNum classNumber, HobRecordNum transTypeNumber)
		: Total()
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		deptNum_ = deptNum;
		classNumber_ = classNumber;
		transTypeNumber_ = transTypeNumber;
	}


	ClassTotal::~ClassTotal()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void ClassTotal::init()
	{
		deptNum_ = RECORD_NOT_FOUND;
		classNumber_ = RECORD_NOT_FOUND;
		transTypeNumber_ = RECORD_NOT_FOUND;
		reportingClassNumber_ = RECORD_NOT_FOUND;

		initFieldLists();
	}


	void ClassTotal::initFieldLists()
	{
		if (_uiFieldList_.empty() )
		{
			_uiFieldList_.push_back(fieldOffsetTotalClassNum);
			_uiFieldList_.push_back(fieldOffsetTotalReportingClassNum);
			_uiFieldList_.push_back(fieldOffsetTotalTransactionTypeNum);
			_uiFieldList_.push_back(fieldOffsetTotalFlags);
			_uiFieldList_.push_back(fieldOffsetTotalWeight);
			_uiFieldList_.push_back(fieldOffsetTotalPrice);
			_uiFieldList_.push_back(fieldOffsetTotalPackages);
			_uiFieldList_.push_back(fieldOffsetTotalRuns);
			_uiFieldList_.push_back(fieldOffsetTotalItems);
		}
		if (_evolFieldList_.empty())
		{
			_evolFieldList_.push_back(fieldOffsetTotalDeptNum);
			_evolFieldList_.push_back(fieldOffsetTotalClassNum);
			_evolFieldList_.push_back(fieldOffsetTotalReportingClassNum);
			_evolFieldList_.push_back(fieldOffsetTotalTransactionTypeNum);
			_evolFieldList_.push_back(fieldOffsetTotalFlags);
			_evolFieldList_.push_back(fieldOffsetTotalWeight);
			_evolFieldList_.push_back(fieldOffsetTotalPrice);
			_evolFieldList_.push_back(fieldOffsetTotalPackages);
			_evolFieldList_.push_back(fieldOffsetTotalRuns);
			_evolFieldList_.push_back(fieldOffsetTotalItems);

			ClassTotalTags totalTags(&_evolFieldList_);
		}
	}


	void ClassTotal::getFieldAsString(int fieldCode, HobString &result, int format) const
	{
	//	Right now we don't do anything with format but keep it in case we need it.
		result.clear();

		switch ( fieldCode )
		{
			case fieldOffsetTotalDeptNum:
				result = util_string::longtostring(deptNum_);
				break;
			case fieldOffsetTotalClassNum:
				result = util_string::longtostring(classNumber_);
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


	const char *ClassTotal::serialize(int format) const
	{
		serializedData.clear();

		if (getFormatType() != UnknownFormat)
			format = getFormatType();

		switch ( format )
		{
			case EvolutionTaggedFormat:
			case DBIndependentFormatFormat:
			{
				bool appendSeparator = false;
				HobString fieldData;
				ClassTotalTags totalTags;
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
				serializedData = "ClassTotal::serialize - unknown format:" + util_string::inttostring(format);
				break;
			}
		}

		return serializedData.data();
	}


}
