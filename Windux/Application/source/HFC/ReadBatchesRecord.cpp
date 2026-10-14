#include "ReadBatchesRecord.h"


#include "ReadBatchesRecordTags.h"	

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int ReadBatchesRecord::_dataType_ = 0;
		std::string ReadBatchesRecord::_dataTypeName_("hfc::ReadBatchesRecord");
	#endif


	FieldOffsetsList ReadBatchesRecord::_fieldList_;

	const std::string ReadBatchesRecord::_batchTypeLocalName_         ("Local");
	const std::string ReadBatchesRecord::_batchTypeRemoteName_        ("Remote");
	const std::string ReadBatchesRecord::_batchTypeUnknownName_       ("Unknown");



	ReadBatchesRecord::ReadBatchesRecord(BatchType type, int referenceVersion)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();

		batchType_ = type;
		referenceVersion_ = referenceVersion;
	}


	ReadBatchesRecord::ReadBatchesRecord(const char * data, int len, int formatType)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();

		switch ( formatType )
		{
			case SynchronizationTaggedFormat:
			{
				ReadBatchesRecordTags headerTags;

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
								case ReadBatchesRecordTags::fieldOffsetReadBatchesBatchType:
									setBatchType((BatchType)util_string::stringtoint(temp));
									break;
								case ReadBatchesRecordTags::fieldOffsetReadBatchesReferenceVersion:
									setReferenceVersion(util_string::stringtoint(temp));
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


	ReadBatchesRecord::~ReadBatchesRecord()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.ReadBatchesRecord", "    ReadBatchesRecord: " + util_string::ptrtostring(this));
	}


	void ReadBatchesRecord::init()
	{
		batchType_ = TypeInvalid;
		referenceVersion_ = -1;

		initFieldList();
	}


	void ReadBatchesRecord::initFieldList()
	{
		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(ReadBatchesRecordTags::fieldOffsetReadBatchesBatchType);	
			_fieldList_.push_back(ReadBatchesRecordTags::fieldOffsetReadBatchesReferenceVersion);

			ReadBatchesRecordTags headerTags(&_fieldList_);
		}
	}


	const char *ReadBatchesRecord::serialize(int format) const
	{
		serializedData.clear();

		if ( getFormatType() != UnknownFormat )
			format = getFormatType();

		switch ( format )
		{
			case SynchronizationTaggedFormat:
			{
				ReadBatchesRecordTags headerTags;

				for ( int loopCnt = 1; loopCnt < ReadBatchesRecordTags::endReadBatchesRecordFieldOffsets; ++loopCnt )
				{
					std::string fieldData, tag = headerTags.getTag(loopCnt);

					switch ( loopCnt )
					{
						case ReadBatchesRecordTags::fieldOffsetReadBatchesBatchType:
							fieldData = util_string::inttostring(getBatchType());
							break;
						case ReadBatchesRecordTags::fieldOffsetReadBatchesReferenceVersion:
							fieldData = util_string::inttostring(getReferenceVersion());
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
				serializedData = "ReadBatchesRecord::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}


	const std::string& ReadBatchesRecord::getBatchTypeName(BatchType bt)
	{
		switch ( bt )
		{
			case TypeLocal:            return _batchTypeLocalName_;
			case TypeRemote:           return _batchTypeRemoteName_;
			default:                   return _batchTypeUnknownName_;
		}
	}


}
