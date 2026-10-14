#include "OperatorTotal.h"


#include "OperatorTotalTags.h"

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
	int OperatorTotal::dataType = 0;
	std::string OperatorTotal::dataTypeName("hfc::OperatorTotal"); 
#endif

	FieldOffsetsList OperatorTotal::_fieldList_;

	
	OperatorTotal::OperatorTotal(const std::string id)
		: Total()
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		operatorId_.assign(id);
	}


	OperatorTotal::~OperatorTotal()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif
	}


	void OperatorTotal::init()
	{
		operatorId_.clear();

		initFieldList();
	}


	void OperatorTotal::initFieldList()
	{
		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(fieldOffsetTotalOperatorNum);
			_fieldList_.push_back(fieldOffsetTotalFlags);
			_fieldList_.push_back(fieldOffsetTotalWeight);
			_fieldList_.push_back(fieldOffsetTotalPrice);
			_fieldList_.push_back(fieldOffsetTotalPackages);
			_fieldList_.push_back(fieldOffsetTotalRuns);
			_fieldList_.push_back(fieldOffsetTotalItems);

			OperatorTotalTags totalTags(&_fieldList_);
		}
	}


	void OperatorTotal::getFieldAsString(int fieldCode, HobString &result, int format) const
	{
	//	Right now we don't do anything with format but keep it in case we need it.
		result = "";

		switch (fieldCode)
		{
			case fieldOffsetTotalOperatorNum:
				result = operatorId_;
				break;

			default:
				Total::getFieldAsString(fieldCode, result, format);
				break;
		}
	}


	const char *OperatorTotal::serialize(int format) const
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
				OperatorTotalTags operatorTotalTags;
				HobString tag = "";

				for ( FieldOffsetsList::iterator fieldIter = _fieldList_.begin(); fieldIter != _fieldList_.end(); ++fieldIter )
				{
					tag = operatorTotalTags.getTag(*fieldIter);
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
				serializedData = "OperatorTotal::serialize - unknown format:" + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}


}
