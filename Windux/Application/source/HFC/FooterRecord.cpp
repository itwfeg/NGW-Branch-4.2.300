#include "FooterRecord.h"


//#include "FooterRecordTags.h"	

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int FooterRecord::_dataType_ = 0;
		std::string FooterRecord::_dataTypeName_("hfc::FooterRecord");
	#endif


	FieldOffsetsList FooterRecord::_fieldList_;



	FooterRecord::FooterRecord()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();
	}


	FooterRecord::FooterRecord(const char * /* data */, int /* len */, int /* formatType */)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();

/*		switch ( formatType )
		{
			case SynchronizationTaggedFormat:
			case EvolutionTaggedFormat:
			{
				FooterRecordTags headerTags;

				//convert data to HobString
				HobString evolData(data, len);

				//use the UnitSeparator to parse the string
				HobTokenizerUS tok(&evolData, UnitSeparator);
				HobString temp = "";

				int offset = 0;
				HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
				if ( token )
				{
					while ( tok.next(token) != -1 )
					{
						//get the tag and find out what offset goes with the tag
			  	 		temp = token->getTag();
						if ( temp.size() )
						{
							//if we can't find it in the expanded text tags then check in special message tags
							offset = headerTags.getOffset(temp);
						}
						else
							offset = 0;

						if ( offset )
						{
							setReceived(offset, true);
							temp = token->getData();
							switch ( offset )
							{
							}
						}
					}	

					delete token;
				}

				break;
			}
			default:
			{
				// todo - throw exception("invalid format");
				break;
			}
		} */
	}


	FooterRecord::~FooterRecord()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.FooterRecord", "    FooterRecord: " + util_string::ptrtostring(this));
	}


	void FooterRecord::init()
	{
		initFieldList();
	}


	void FooterRecord::initFieldList()
	{
/*		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(FooterRecordTags::fieldOffsetHeaderDataType);	
			_fieldList_.push_back(FooterRecordTags::fieldOffsetHeaderHostname);
			_fieldList_.push_back(FooterRecordTags::fieldOffsetHeaderIPAddress);
			_fieldList_.push_back(FooterRecordTags::fieldOffsetHeaderCurrentTime);

			FooterRecordTags headerTags(&_fieldList_);
		} */
	}


	const char *FooterRecord::serialize(int format) const
	{
		serializedData.clear();

		if ( getFormatType() != UnknownFormat )
			format = getFormatType();

/*		switch ( format )
		{
			case SynchronizationTaggedFormat:
			case EvolutionTaggedFormat:
			{
//				FooterRecordTags headerTags;

				for ( int loopCnt = 1; loopCnt < FooterRecordTags::endFooterRecordFieldOffsets; ++loopCnt )
				{
					std::string fieldData, tag = headerTags.getTag(loopCnt);

					switch ( loopCnt )
					{
					}

					if ( tag.size() )
					{
						serializedData.append(tag);
						serializedData.append(fieldData);
						serializedData.append(UnitSeparator);
					}
				}

				break;	
			}
			default:
			{
				serializedData = "FooterRecord::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}
  */
		return serializedData.data();
	}


}
