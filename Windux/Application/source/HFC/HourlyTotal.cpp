#include "HourlyTotal.h"


#include "HourlyTotalTags.h"

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
	int HourlyTotal::dataType = 0;
	std::string HourlyTotal::dataTypeName("hfc::HourlyTotal"); 
#endif

	FieldOffsetsList HourlyTotal::_fieldList_;


	HourlyTotal::HourlyTotal(short hour)
		: Total()
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		hour_ = hour;
	}


	HourlyTotal::~HourlyTotal()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void HourlyTotal::init()
	{
		hour_ = 0;

		initFieldList();
	}


	void HourlyTotal::initFieldList()
	{
		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(HourlyTotal::fieldOffsetHourlyTotalNum);
			_fieldList_.push_back(HourlyTotal::fieldOffsetTotalFlags);
			_fieldList_.push_back(HourlyTotal::fieldOffsetTotalWeight);
			_fieldList_.push_back(HourlyTotal::fieldOffsetTotalPrice);
			_fieldList_.push_back(HourlyTotal::fieldOffsetTotalPackages);
			_fieldList_.push_back(HourlyTotal::fieldOffsetTotalRuns);
			_fieldList_.push_back(HourlyTotal::fieldOffsetTotalItems);

			HourlyTotalTags totalTags(&_fieldList_);
		}
	}


	void HourlyTotal::getFieldAsString(int fieldCode, HobString &result, int format) const
	{
	//	Right now we don't do anything with format but keep it in case we need it.
		result = "";

		switch (fieldCode)
		{
			case fieldOffsetHourlyTotalNum:
				result = util_string::inttostring(hour_);
				break;

			default:
				Total::getFieldAsString(fieldCode, result, format);
				break;
		}
	}


	const char *HourlyTotal::serialize(int format) const
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

					++fieldIter;
				}

				break;
			}
			case EvolutionTaggedFormat:
			case DBIndependentFormatFormat:
			{
				bool appendSeparator = false;
				HobString fieldData;
				HourlyTotalTags hourlyTotalTags;
				HobString tag;

				for ( FieldOffsetsList::iterator fieldIter = _fieldList_.begin(); fieldIter != _fieldList_.end();  ++fieldIter )
				{
					tag = hourlyTotalTags.getTag(*fieldIter);
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
				serializedData = "HourlyTotal::serialize - unknown format:" + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}


}
