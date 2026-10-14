#include "SynchronizationRecord.h"


#include "SynchronizationRecordTags.h"	

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int SynchronizationRecord::_dataType_ = 0;
		std::string SynchronizationRecord::_dataTypeName_("hfc::SynchronizationRecord");
	#endif


	FieldOffsetsList SynchronizationRecord::_fieldList_;

	const std::string SynchronizationRecord::_commandNotifyLocalBatchesName_         ("NotifyLocalBatches");
	const std::string SynchronizationRecord::_commandUnknownName_                    ("Unknown");



	SynchronizationRecord::SynchronizationRecord(SynchronizationCommand command)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();

		command_ = command;
	}


	SynchronizationRecord::SynchronizationRecord(const char * data, int len, int formatType)
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
				SynchronizationRecordTags synchronizationTags;

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
							offset = synchronizationTags.getOffset(temp);
						else
							offset = 0;

						if ( offset )
						{
							setReceived(offset, true);
							temp = token->getData();
							switch ( offset )
							{
								case SynchronizationRecordTags::fieldOffsetSynchronizationCommand:
									setCommand((SynchronizationCommand)util_string::stringtoint(temp));
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


	SynchronizationRecord::~SynchronizationRecord()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.SynchronizationRecord", "    SynchronizationRecord: " + util_string::ptrtostring(this));
	}


	void SynchronizationRecord::init()
	{
		command_ = CommandInvalid;

		initFieldList();
	}


	void SynchronizationRecord::initFieldList()
	{
		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(SynchronizationRecordTags::fieldOffsetSynchronizationCommand);	

			SynchronizationRecordTags synchronizationTags(&_fieldList_);
		}
	}


	const char *SynchronizationRecord::serialize(int format) const
	{
		serializedData.clear();

		if ( getFormatType() != UnknownFormat )
			format = getFormatType();

		switch ( format )
		{
			case SynchronizationTaggedFormat:
			{
				SynchronizationRecordTags synchronizationTags;

				for ( int loopCnt = 1; loopCnt < SynchronizationRecordTags::endSynchronizationRecordFieldOffsets; ++loopCnt )
				{
					std::string fieldData, tag = synchronizationTags.getTag(loopCnt);

					switch ( loopCnt )
					{
						case SynchronizationRecordTags::fieldOffsetSynchronizationCommand:
							fieldData = util_string::inttostring(getCommand());
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
				serializedData = "SynchronizationRecord::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}


	const std::string& SynchronizationRecord::getCommandName(SynchronizationCommand command)
	{
		switch ( command )
		{
			case CommandNotifyLocalBatches:           return _commandNotifyLocalBatchesName_;
			default:                                  return _commandUnknownName_;
		}
	}


}
