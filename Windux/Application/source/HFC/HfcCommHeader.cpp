#include "HfcCommHeader.h"


#include "HfcCommHeaderTags.h"	

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int HfcCommHeader::_dataType_ = 0;
		string HfcCommHeader::_dataTypeName_("hfc::HfcCommHeader");
	#endif


	FieldOffsetsList HfcCommHeader::_fieldList_;

	const std::string HfcCommHeader::_datatypeHQCommName_         ("HQComm");
	const std::string HfcCommHeader::_datatypeSyncCommName_       ("SyncComm");
	const std::string HfcCommHeader::_datatypeUnknownName_        ("Unknown");



	HfcCommHeader::HfcCommHeader()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();
	}


	HfcCommHeader::HfcCommHeader(const char * data, int len, int formatType)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();

		switch ( formatType )
		{
			case EvolutionTaggedFormat:
			{
				HfcCommHeaderTags headerTags;

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
								case HfcCommHeaderTags::fieldOffsetHeaderDataType:
									setDataType((DataType)util_string::stringtoint(temp));
									break;
								case HfcCommHeaderTags::fieldOffsetHeaderHostname:
									setHostname(temp);
									break;
								case HfcCommHeaderTags::fieldOffsetHeaderIPAddress:
//									setIPAddress(temp);
									break;
								case HfcCommHeaderTags::fieldOffsetHeaderCurrentTime:
//									setCurrentDateTime(HobDateTime(temp.c_str(), temp.length(), ));
									break;
								default:
									break;
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
		}
	}


	HfcCommHeader::~HfcCommHeader()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.HfcCommHeader", "    HfcCommHeader: " + util_string::ptrtostring(this));
	}


	void HfcCommHeader::init()
	{
		dataType_ = TypeInvalid;
		hostname_.clear();
		ipAddress_.clear();
		currentDateTime_ = HobDateTime();

		initFieldList();
	}


	void HfcCommHeader::initFieldList()
	{
		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(HfcCommHeaderTags::fieldOffsetHeaderDataType);	
			_fieldList_.push_back(HfcCommHeaderTags::fieldOffsetHeaderHostname);
			_fieldList_.push_back(HfcCommHeaderTags::fieldOffsetHeaderIPAddress);
			_fieldList_.push_back(HfcCommHeaderTags::fieldOffsetHeaderCurrentTime);

			HfcCommHeaderTags headerTags(&_fieldList_);
		}
	}


	const char *HfcCommHeader::serialize(int format) const
	{
		serializedData.clear();

		if ( getFormatType() != UnknownFormat )
			format = getFormatType();

		switch ( format )
		{
			case EvolutionTaggedFormat:
			{
				HfcCommHeaderTags headerTags;

				for ( int loopCnt = 1; loopCnt < HfcCommHeaderTags::endHfcCommHeaderFieldOffsets; ++loopCnt )
				{
					std::string fieldData, tag = headerTags.getTag(loopCnt);

					switch ( loopCnt )
					{
						case HfcCommHeaderTags::fieldOffsetHeaderDataType:
							fieldData = util_string::inttostring(getDataType());
							break;
						case HfcCommHeaderTags::fieldOffsetHeaderHostname:
							fieldData = getHostname();
							break;
						case HfcCommHeaderTags::fieldOffsetHeaderIPAddress:
//							fieldData = getIPAddress();
							break;
						case HfcCommHeaderTags::fieldOffsetHeaderCurrentTime:
//							fieldData = getCreatedDateTime();
							break;
						default:
							tag.clear();
							break;
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
				serializedData = "HfcCommHeader::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}


	const std::string& HfcCommHeader::getDataTypeName(DataType dt)
	{
		switch ( dt )
		{
			case TypeHQComm:           return _datatypeHQCommName_;
			case TypeSyncComm:         return _datatypeSyncCommName_;
			default:                   return _datatypeUnknownName_;
		}
	}


}
