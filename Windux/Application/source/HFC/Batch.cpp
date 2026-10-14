#include "Batch.h"


#include "BatchTags.h"	
#include "Scale.h"

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "Debug.h"

#include "util/checksums/checksum.h"
#include "util/nibble/nibble.h"

#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int Batch::_dataType_ = 0;
		std::string Batch::_dataTypeName_("hfc::Batch");
	#endif


	FieldOffsetsList Batch::_synchronizationFieldList_;
	FieldOffsetsList Batch::_dbindependentFieldList_;

	const std::string Batch::TypeUserName       ("User");
	const std::string Batch::TypeHQName         ("HQ");
	const std::string Batch::TypeWholeDBName    ("WholeDB");

	const std::string Batch::TypeUnknownName    ("Unknown");

	const std::string Batch::OriginLocalName    ("Local");
	const std::string Batch::OriginRemoteName   ("Remote");
	const std::string Batch::OriginUnknownName  ("Unknown");


	Batch::Batch()
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();
	}


	Batch::Batch(const char * data, int len, int formatType)
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
				BatchTags batchTags;

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
							offset = batchTags.getOffset(temp);
						}
						else
							offset = 0;

						if ( offset )
						{
							setReceived(offset, true);
							temp = token->getData();
							switch ( offset )
							{
								case BatchTags::fieldOffsetBatchNumber:
									setNumber(util_string::stringtoint(temp));
									break;
//								case BatchTags::fieldOffsetBatchScaleId:
//									setScaleId(util_string::stringtoint(temp));
//									break;
								case BatchTags::fieldOffsetBatchVersion:
									setVersion(util_string::stringtoint(temp));
									break;
								case BatchTags::fieldOffsetBatchRemoteVersion:
									setRemoteVersion(util_string::stringtoint(temp));
									break;
								case BatchTags::fieldOffsetBatchOrigin:
									setOrigin((BatchOrigin)util_string::stringtoint(temp));
									break;
								case BatchTags::fieldOffsetBatchType:
									setType((BatchType)util_string::stringtoint(temp));
									break;
								case BatchTags::fieldOffsetBatchCreatedDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setCreatedDateTime(hdt);
									break;
								}
								case BatchTags::fieldOffsetBatchCompletedDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setCompletedDateTime(hdt);
									break;
								}
								case BatchTags::fieldOffsetBatchSignature:
									setSignature(temp);
									break;
								case BatchTags::fieldOffsetBatchPendingApplication:
									setPendingApplication(util_string::stringtobool(temp));
									break;
								case BatchTags::fieldOffsetBatchAppliedDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setAppliedDateTime(hdt);
									break;
								}
								case BatchTags::fieldOffsetBatchPendingSynchronization:
									setPendingSynchronization(util_string::stringtobool(temp));
									break;
								case BatchTags::fieldOffsetBatchSynchronizedDateTime:
//									setSynchronizedDateTime(HobDateTime(temp.c_str(), temp.length(), ));
									break;
								case BatchTags::fieldOffsetBatchPendingDeletion:
									setPendingDeletion(util_string::stringtobool(temp));
									break;
								case BatchTags::fieldOffsetBatchDeletedDateTime:
//									setDeletedDateTime(HobDateTime(temp.c_str(), temp.length(), ));
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
			case DBIndependentFormatFormat:
			{
				BatchTags batchTags;

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
							offset = batchTags.getOffset(temp);
						}
						else
							offset = 0;

						if ( offset )
						{
							setReceived(offset, true);
							temp = token->getData();
							switch ( offset )
							{
								case BatchTags::fieldOffsetBatchNumber:
									setNumber(util_string::stringtoint(temp));
									break;
								case BatchTags::fieldOffsetBatchScaleId:
									setScaleId(util_string::stringtoint(temp));
									break;
								case BatchTags::fieldOffsetBatchVersion:
									setVersion(util_string::stringtoint(temp));
									break;
								case BatchTags::fieldOffsetBatchRemoteVersion:
									setRemoteVersion(util_string::stringtoint(temp));
									break;
								case BatchTags::fieldOffsetBatchOrigin:
									setOrigin((BatchOrigin)util_string::stringtoint(temp));
									break;
								case BatchTags::fieldOffsetBatchType:
									setType((BatchType)util_string::stringtoint(temp));
									break;
								case BatchTags::fieldOffsetBatchCreatedDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setCreatedDateTime(hdt);
									break;
								}
								case BatchTags::fieldOffsetBatchCompletedDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setCompletedDateTime(hdt);
									break;
								}
								case BatchTags::fieldOffsetBatchSignature:
									setSignature(temp);
									break;
								case BatchTags::fieldOffsetBatchPendingApplication:
									setPendingApplication(util_string::stringtobool(temp));
									break;
								case BatchTags::fieldOffsetBatchAppliedDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setAppliedDateTime(hdt);
									break;
								}
								case BatchTags::fieldOffsetBatchPendingSynchronization:
									setPendingSynchronization(util_string::stringtobool(temp));
									break;
								case BatchTags::fieldOffsetBatchSynchronizedDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setSynchronizedDateTime(hdt);
									break;
								}
								case BatchTags::fieldOffsetBatchPendingDeletion:
									setPendingDeletion(util_string::stringtobool(temp));
									break;
								case BatchTags::fieldOffsetBatchDeletedDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setDeletedDateTime(hdt);
									break;
								}
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


	Batch::~Batch()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.Batch", "    Batch: " + util_string::ptrtostring(this) +
				"  Batch number:" + util_string::inttostring(this->getNumber()) +
				"  Version:" + util_string::inttostring(this->getVersion()));
	}


	void Batch::init()
	{
		errorCode_ = RECORD_NOT_FOUND;

		number_ = _invalidBatchNumber_;
		scaleId_ = Scale::_invalidScaleId_;
		version_ = _invalidBatchVersion_;
		remoteVersion_ = _invalidBatchVersion_;
		type_ = TypeUnknown;
		origin_ = OriginUnknown;
		signature_ = Signature();
		synchronizedSignature_ = Signature();
		createdDateTime_ = "";
		completed_ = false;
		completedDateTime_ = "";
		pendingSynchronization_ = false;
		synchronized_ = false;
		synchronizedDateTime_ = "";
		pendingApplication_ = false;
		applied_ = false;
		appliedDateTime_ = "";
		pendingDeletion_ = false;
		deleted_ = false;
		deletedDateTime_ = "";

		initFieldList();
	}


	void Batch::initFieldList()
	{
		if ( _synchronizationFieldList_.empty() )
		{
			_synchronizationFieldList_.push_back(BatchTags::fieldOffsetBatchVersion);
			_synchronizationFieldList_.push_back(BatchTags::fieldOffsetBatchOrigin);
			_synchronizationFieldList_.push_back(BatchTags::fieldOffsetBatchType);
			_synchronizationFieldList_.push_back(BatchTags::fieldOffsetBatchCreatedDateTime);
			_synchronizationFieldList_.push_back(BatchTags::fieldOffsetBatchCompletedDateTime);
			_synchronizationFieldList_.push_back(BatchTags::fieldOffsetBatchSignature);

			BatchTags batchTags(&_synchronizationFieldList_);
		}

		if ( _dbindependentFieldList_.empty() )
		{
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchNumber);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchScaleId);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchVersion);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchRemoteVersion);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchOrigin);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchType);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchCreatedDateTime);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchCompletedDateTime);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchSignature);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchPendingApplication);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchAppliedDateTime);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchPendingSynchronization);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchSynchronizedDateTime);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchPendingDeletion);
			_dbindependentFieldList_.push_back(BatchTags::fieldOffsetBatchDeletedDateTime);

			BatchTags batchTags(&_dbindependentFieldList_);
		}
	}


	void Batch::setErrorCode(HobErrorCode errorCode)
	{
		this->errorCode_ = errorCode;

		Debug::trace("Batch", "setErrorCode():  errorCode = " +
				util_string::inttostring(this->errorCode_));
	}


	void Batch::updateBatch(const Batch * newBatch)
	{
		if (!newBatch)
			return;

		for ( int loopCnt = 1; loopCnt < BatchTags::endBatchFieldOffsets; ++loopCnt )
		{
			if ( newBatch->isReceived(loopCnt) )
			{
				switch ( loopCnt )
				{
//					case BatchTags::fieldOffsetBatchNumber:
//						setNumber(newBatch->getNumber());
//						break;
//					case BatchTags::fieldOffsetBatchScaleId:
//						setScaleId(newBatch->getScaleId());
//						break;
					case BatchTags::fieldOffsetBatchVersion:
						setVersion(newBatch->getVersion());
						break;
					case BatchTags::fieldOffsetBatchRemoteVersion:
						setRemoteVersion(newBatch->getRemoteVersion());
						break;
					case BatchTags::fieldOffsetBatchType:
						setType(newBatch->getType());
						break;
					case BatchTags::fieldOffsetBatchOrigin:
						setOrigin(newBatch->getOrigin());
						break;
					case BatchTags::fieldOffsetBatchCreatedDateTime:
						setCreatedDateTime(newBatch->getCreatedDateTime());
						break;
					case BatchTags::fieldOffsetBatchCompletedDateTime:
						setCompletedDateTime(newBatch->getCompletedDateTime());
						break;
					case BatchTags::fieldOffsetBatchSignature:
						setSignature(newBatch->getSignature());
						break;
					case BatchTags::fieldOffsetBatchPendingApplication:
						setPendingApplication(newBatch->isPendingApplication());
						break;
					case BatchTags::fieldOffsetBatchAppliedDateTime:
						setAppliedDateTime(newBatch->getAppliedDateTime());
						break;
					case BatchTags::fieldOffsetBatchPendingSynchronization:
						setPendingSynchronization(newBatch->isPendingSynchronization());
						break;
					case BatchTags::fieldOffsetBatchSynchronizedDateTime:
						setSynchronizedDateTime(newBatch->getSynchronizedDateTime());
						break;
					case BatchTags::fieldOffsetBatchPendingDeletion:
						setPendingDeletion(newBatch->isPendingDeletion());
						break;
					case BatchTags::fieldOffsetBatchDeletedDateTime:
						setDeletedDateTime(newBatch->getDeletedDateTime());
						break;
					case BatchTags::fieldOffsetBatchErrorCode:
						setErrorCode(newBatch->getErrorCode());
						break;
					default:
						break;
				}
			}
		}
	}


	const char *Batch::serialize(int format) const
	{
		serializedData.clear();

		if ( getFormatType() != UnknownFormat )
			format = getFormatType();

		switch ( format )
		{
			case DBIndependentFormatFormat:
			{
				BatchTags batchTags;
				FieldOffsetsListIterator iter = _dbindependentFieldList_.begin();

				while ( iter != _dbindependentFieldList_.end() )
				{
					std::string fieldData, tag = batchTags.getTag(*iter);

					switch ( *iter )
					{
						case BatchTags::fieldOffsetBatchNumber:
							fieldData = util_string::inttostring(getNumber());
							break;
						case BatchTags::fieldOffsetBatchScaleId:
							fieldData = util_string::inttostring(getScaleId());
							break;
						case BatchTags::fieldOffsetBatchVersion:
							fieldData = util_string::inttostring(getVersion());
							break;
						case BatchTags::fieldOffsetBatchRemoteVersion:
							fieldData = util_string::inttostring(getRemoteVersion());
							break;
						case BatchTags::fieldOffsetBatchOrigin:
							fieldData = util_string::inttostring(getOrigin());
							break;
						case BatchTags::fieldOffsetBatchType:
							fieldData = util_string::inttostring(getType());
							break;
						case BatchTags::fieldOffsetBatchCreatedDateTime:
						{
							HobDateTime hdt = getCreatedDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case BatchTags::fieldOffsetBatchCompletedDateTime:
						{
							HobDateTime hdt = getCompletedDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case BatchTags::fieldOffsetBatchSignature:
							fieldData = getSignature();
							break;
						case BatchTags::fieldOffsetBatchPendingApplication:
							fieldData = util_string::inttostring(isPendingApplication() ? 1 : 0);
							break;
						case BatchTags::fieldOffsetBatchAppliedDateTime:
						{
							HobDateTime hdt = getAppliedDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case BatchTags::fieldOffsetBatchPendingSynchronization:
							fieldData = util_string::inttostring(isPendingSynchronization() ? 1 : 0);
							break;
						case BatchTags::fieldOffsetBatchSynchronizedDateTime:
						{
							HobDateTime hdt = getSynchronizedDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case BatchTags::fieldOffsetBatchPendingDeletion:
							fieldData = util_string::inttostring(isPendingDeletion() ? 1 : 0);
							break;
						case BatchTags::fieldOffsetBatchDeletedDateTime:
						{
							HobDateTime hdt = getDeletedDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
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

					++iter;
				}

				break;	
			}
			case SynchronizationTaggedFormat:
			{
				BatchTags batchTags;
				FieldOffsetsListIterator iter = _synchronizationFieldList_.begin();

				while ( iter != _synchronizationFieldList_.end() )
				{
					std::string fieldData, tag = batchTags.getTag(*iter);

					switch ( *iter )
					{
						case BatchTags::fieldOffsetBatchVersion:
							fieldData = util_string::inttostring(getVersion());
							break;
						case BatchTags::fieldOffsetBatchRemoteVersion:
							fieldData = util_string::inttostring(getRemoteVersion());
							break;
						case BatchTags::fieldOffsetBatchOrigin:
							fieldData = util_string::inttostring(getOrigin());
							break;
						case BatchTags::fieldOffsetBatchType:
							fieldData = util_string::inttostring(getType());
							break;
						case BatchTags::fieldOffsetBatchCreatedDateTime:
						{
							HobDateTime hdt = getCreatedDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case BatchTags::fieldOffsetBatchCompletedDateTime:
						{
							HobDateTime hdt = getCompletedDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case BatchTags::fieldOffsetBatchSignature:
							fieldData = getSignature();
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

					++iter;
				}

				break;
			}
			default:
			{
				serializedData = "Batch::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}


	void Batch::computeSignature(const std::string& change)
	{
		signature_.addChange(change);
	}

	void Batch::computeSynchronizedSignature(const std::string& change)
	{
		synchronizedSignature_.addChange(change);
	}


	const std::string& Batch::getTypeName(BatchType type)
	{
		switch ( type )
		{
			case TypeUser:        return TypeUserName;
			case TypeHQ:          return TypeHQName;
			case TypeWholeDB:     return TypeWholeDBName;
			default:              return TypeUnknownName;
		}
	}


	const std::string& Batch::getOriginName(BatchOrigin origin)
	{
		switch ( origin )
		{
			case OriginLocal:    return OriginLocalName;
			case OriginRemote:   return OriginRemoteName;
			default:             return OriginUnknownName;
		}
	}


	Batch::Signature::Signature(const std::string& buffer)
	{
		if ( buffer.size() )
		{
			const char* ptr = buffer.data();
			util_nibble::unnibblize(ptr, (char *)&crc_, sizeof(crc_));
			ptr += 2*sizeof(crc_);  // "2*" is because nibblized takes twice as many bytes
			util_nibble::unnibblize(ptr, (char *)&numRecords_, sizeof(numRecords_));
		}
		else
		{
			crc_ = 0;
			numRecords_ = 0;
		}
	}


	const std::string& Batch::Signature::serialize() const
	{
		serializedData_.clear();
		util_nibble::nibblize((const char *)&crc_, serializedData_, sizeof(crc_));
		util_nibble::nibblize((const char *)&numRecords_, serializedData_, sizeof(numRecords_));

		return serializedData_;
	}

	void Batch::Signature::addChange(const std::string& change)
	{

		crc_ = util_checksums::icrc(crc_, (unsigned char*)change.data(), change.length());

		addRecord();
	}

}
