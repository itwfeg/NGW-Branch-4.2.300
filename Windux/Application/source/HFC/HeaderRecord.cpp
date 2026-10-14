#include "HeaderRecord.h"


#include "HeaderRecordTags.h"	

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int HeaderRecord::_dataType_ = 0;
		std::string HeaderRecord::_dataTypeName_("hfc::HeaderRecord");
	#endif


	FieldOffsetsList HeaderRecord::_fieldList_;

	const std::string HeaderRecord::_datatypeHQCommName_         ("HQComm");
	const std::string HeaderRecord::_datatypeSyncCommName_       ("SyncComm");
	const std::string HeaderRecord::_datatypeUnknownName_        ("Unknown");



	HeaderRecord::HeaderRecord()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();
	}


	HeaderRecord::HeaderRecord(const char * data, int len, int formatType)
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
			case SynchronizationTaggedFormat:
			{
				HeaderRecordTags headerTags;

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
								case HeaderRecordTags::fieldOffsetHeaderDataType:
									setDataType((DataType)util_string::stringtoint(temp));
									break;
								case HeaderRecordTags::fieldOffsetHeaderScaleRole:
									setScaleRole((hfc::Scale::ScaleRole)util_string::stringtoint(temp));
									break;
								case HeaderRecordTags::fieldOffsetHeaderHostname:
									setHostName(temp);
									break;
								case HeaderRecordTags::fieldOffsetHeaderIPAddress:
									setIPAddress(temp);
									break;
								case HeaderRecordTags::fieldOffsetHeaderCurrentTime:
									setCurrentDateTime(temp);
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


	HeaderRecord::~HeaderRecord()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.HeaderRecord", "    HeaderRecord: " + util_string::ptrtostring(this));
	}


	void HeaderRecord::init()
	{
		dataType_ = TypeInvalid;
		scaleRole_ = hfc::Scale::RoleInvalid;
		hostname_.clear();
		ipAddress_.clear();
		currentDateTime_.clear();

		initFieldList();
	}


	void HeaderRecord::initFieldList()
	{
		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(HeaderRecordTags::fieldOffsetHeaderDataType);	
			_fieldList_.push_back(HeaderRecordTags::fieldOffsetHeaderScaleRole);
			_fieldList_.push_back(HeaderRecordTags::fieldOffsetHeaderHostname);
			_fieldList_.push_back(HeaderRecordTags::fieldOffsetHeaderIPAddress);
			_fieldList_.push_back(HeaderRecordTags::fieldOffsetHeaderCurrentTime);

			HeaderRecordTags headerTags(&_fieldList_);
		}
	}


	const char * HeaderRecord::serialize(int format) const
	{
		serializedData.clear();

		if ( getFormatType() != UnknownFormat )
			format = getFormatType();

		switch ( format )
		{
			case SynchronizationTaggedFormat:
			case EvolutionTaggedFormat:
			{
				HeaderRecordTags headerTags;

				for ( int loopCnt = 1; loopCnt < HeaderRecordTags::endHeaderRecordFieldOffsets; ++loopCnt )
				{
					std::string fieldData, tag = headerTags.getTag(loopCnt);

					switch ( loopCnt )
					{
						case HeaderRecordTags::fieldOffsetHeaderDataType:
							fieldData = util_string::inttostring(getDataType());
							break;
						case HeaderRecordTags::fieldOffsetHeaderScaleRole:
							fieldData = util_string::inttostring(getScaleRole());
							break;
						case HeaderRecordTags::fieldOffsetHeaderHostname:
							fieldData = getHostName();
							break;
						case HeaderRecordTags::fieldOffsetHeaderIPAddress:
							fieldData = getIPAddress();
							break;
						case HeaderRecordTags::fieldOffsetHeaderCurrentTime:
//							fieldData = getCurrentDateTime();
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
				serializedData = "HeaderRecord::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}


	const std::string& HeaderRecord::getDataTypeName(DataType dt)
	{
		switch ( dt )
		{
			case TypeHQComm:           return _datatypeHQCommName_;
			case TypeSyncComm:         return _datatypeSyncCommName_;
			default:                   return _datatypeUnknownName_;
		}
	}


}
