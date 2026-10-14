#include "HfcCommFooter.h"


//#include "HfcCommFooterTags.h"	

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int HfcCommFooter::_dataType_ = 0;
		string HfcCommFooter::_dataTypeName_("hfc::HfcCommFooter");
	#endif


	FieldOffsetsList HfcCommFooter::_fieldList_;



	HfcCommFooter::HfcCommFooter()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();
	}


	HfcCommFooter::HfcCommFooter(const char * data, int len, int formatType)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();

/*		switch ( formatType )
		{
			case EvolutionTaggedFormat:
			{
				HfcCommFooterTags headerTags;

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


	HfcCommFooter::~HfcCommFooter()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.HfcCommFooter", "    HfcCommFooter: " + util_string::ptrtostring(this));
	}


	void HfcCommFooter::init()
	{
		initFieldList();
	}


	void HfcCommFooter::initFieldList()
	{
/*		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(HfcCommFooterTags::fieldOffsetHeaderDataType);	
			_fieldList_.push_back(HfcCommFooterTags::fieldOffsetHeaderHostname);
			_fieldList_.push_back(HfcCommFooterTags::fieldOffsetHeaderIPAddress);
			_fieldList_.push_back(HfcCommFooterTags::fieldOffsetHeaderCurrentTime);

			HfcCommFooterTags headerTags(&_fieldList_);
		} */
	}


	const char *HfcCommFooter::serialize(int format) const
	{
		serializedData.clear();

		if ( getFormatType() != UnknownFormat )
			format = getFormatType();

/*		switch ( format )
		{
			case EvolutionTaggedFormat:
			{
//				HfcCommFooterTags headerTags;

				for ( int loopCnt = 1; loopCnt < HfcCommFooterTags::endHfcCommFooterFieldOffsets; ++loopCnt )
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
				serializedData = "HfcCommFooter::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}
  */
		return serializedData.data();
	}


}
