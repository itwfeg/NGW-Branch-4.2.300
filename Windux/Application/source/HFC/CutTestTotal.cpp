#include "CutTestTotal.h"
#include "CutTestTotalTags.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

//#include "DBAccess.h"
//#include "DBCutTestTotal.h"


#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



#ifdef USE_ACCOUNTANT
	int CutTestTotal::dataType = 0;
	std::string CutTestTotal::dataTypeName("CutTestTotal"); 
#endif

FieldOffsetsList CutTestTotal::_fieldList_;


CutTestTotal::CutTestTotal(HobRecordNum cutTestNumber, HobRecordNum pluNumber, HobRecordNum deptNum, HobRecordNum transTypeNumber)
	: Total()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	cutTestNumber_ = cutTestNumber;
	productNumber_ = pluNumber;
	deptNum_ = deptNum;
	transTypeNumber_ = transTypeNumber;
}


CutTestTotal::~CutTestTotal()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}


void CutTestTotal::init()
{
	cutTestNumber_ = RECORD_NOT_FOUND;
	productNumber_ = RECORD_NOT_FOUND;
	transTypeNumber_ = RECORD_NOT_FOUND;

	initFieldList();
}


void CutTestTotal::initFieldList()
{
	if ( _fieldList_.empty() )
	{
		_fieldList_.push_back(fieldOffsetTotalCutTestNum);
		_fieldList_.push_back(fieldOffsetTotalProductNum);
		_fieldList_.push_back(fieldOffsetTotalTransactionTypeNum);
		_fieldList_.push_back(fieldOffsetTotalFlags);
		_fieldList_.push_back(fieldOffsetTotalWeight);
		_fieldList_.push_back(fieldOffsetTotalPrice);
		_fieldList_.push_back(fieldOffsetTotalPackages);
		_fieldList_.push_back(fieldOffsetTotalRuns);
		_fieldList_.push_back(fieldOffsetTotalItems);

		hfc::CutTestTotalTags totalTags(&_fieldList_);
	}
}


void CutTestTotal::getFieldAsString(int fieldCode, HobString &result, int format) const
{
//	Right now we don't do anything with format but keep it in case we need it.
	result.clear();

	switch ( fieldCode )
	{
		case fieldOffsetTotalCutTestNum:
			result = util_string::longtostring(cutTestNumber_);
			break;

		case fieldOffsetTotalProductNum:
			result = util_string::longtostring(productNumber_);
			break;

		case fieldOffsetTotalTransactionTypeNum:
			result = util_string::longtostring(transTypeNumber_);
			break;

		default:
			Total::getFieldAsString(fieldCode, result, format);
			break;
	}
}


const char *CutTestTotal::serialize(int format) const
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
			FieldOffsetsList::iterator fieldIter = _fieldList_.begin();

			while ( fieldIter != _fieldList_.end() )
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
			hfc::CutTestTotalTags totalTags;
			HobString tag;

			for ( FieldOffsetsList::iterator fieldIter = _fieldList_.begin(); fieldIter != _fieldList_.end(); ++fieldIter )
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
			serializedData = "CutTestTotal::serialize - unknown format:" + util_string::inttostring(format);
			// todo - throw exception("invalid format");
			break;
		}
	}

	Debug::trace("CutTestTotal", HobString("serializedData = [") + serializedData + "]");
	return serializedData.data();
}


void CutTestTotal::show()
{
	if ( Debug::isTracing("CutTestTotal") )
	{
		Debug::trace("CutTestTotal", "***  CutTestTotal data for cutTestNumber" + util_string::longtostring(cutTestNumber_) + "  ***");
		Debug::trace("CutTestTotal", "productNumber_:    " + util_string::longtostring(productNumber_));
		Debug::trace("CutTestTotal", "totalWeight:   " + util_string::longtostring(getWeight()));
		Debug::trace("CutTestTotal", "totalPrice:    " + util_string::longtostring(getPrice()));
		Debug::trace("CutTestTotal", "totalPackages: " + util_string::longtostring(getPackages()));
		Debug::trace("CutTestTotal", "totalRuns:     " + util_string::longtostring(getRuns()));
		Debug::trace("CutTestTotal", "totalItems:    " + util_string::longtostring(getItems()));
	}
}
