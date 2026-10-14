#include "SynchronizationChange.h"


#include "Batch.h"
#include "SynchronizationChangeTags.h"	

#include "SessionMessage.h"

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBSynchronizationChangesRM.h"
#include "DBSynchronizationChange.h"

#include "DataTypeEvent.h"

#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace hfc
{


#ifdef USE_ACCOUNTANT
	int SynchronizationChange::dataType = 0;
	std::string SynchronizationChange::dataTypeName("SynchronizationChange"); 
#endif


	FieldOffsetsList SynchronizationChange::_fieldList_;

	const std::string SynchronizationChange::ProductDevName              ("ProductDev");
	const std::string SynchronizationChange::OperatorDevName             ("OperatorDev");
	const std::string SynchronizationChange::GraphicDevName              ("GraphicDev");
	const std::string SynchronizationChange::ExpandedTextDevName         ("ExpandedTextDev");
	const std::string SynchronizationChange::SpecialMessageDevName       ("SpecialMessageDev");
	const std::string SynchronizationChange::ProductNotesDevName         ("ProductNotesDev");
	const std::string SynchronizationChange::ClassDevName                ("ClassDev");
	const std::string SynchronizationChange::LabelTypeDevName            ("LabelTypeDev");
	const std::string SynchronizationChange::ConfigurationDevName        ("ConfigurationDev");
	const std::string SynchronizationChange::FontDevName                 ("FontDev");
	const std::string SynchronizationChange::NutrifactsDevName           ("NutrifactsDev");
	const std::string SynchronizationChange::GenericTextDevName          ("GenericTextDev");
	const std::string SynchronizationChange::CategoryDevName             ("CategoryDev");
	const std::string SynchronizationChange::SpecialsDevName             ("SpecialsDev");
	const std::string SynchronizationChange::OperatorNotesDevName        ("OperatorNotesDev");
	const std::string SynchronizationChange::ProductDefaultsDevName      ("ProductDefaultsDev");
	const std::string SynchronizationChange::EmailAlertsDevName          ("EmailAlertsDev");
	const std::string SynchronizationChange::COOLDevName                 ("COOLDev");
	const std::string SynchronizationChange::SoftkeyDevName              ("SoftkeyDev");
	const std::string SynchronizationChange::MarqueeDevName              ("MarqueeDev");
	const std::string SynchronizationChange::UnknownDeviceName           ("UnknownDevice");


	SynchronizationChange::SynchronizationChange()
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();
	}


	SynchronizationChange::SynchronizationChange(int device, int evt, const std::string& change)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		device_ = device;
		event_ = evt;
		format_ = SynchronizationTaggedFormat;
		change_.assign(change);
	}


	SynchronizationChange::SynchronizationChange(int device, int evt, MessageFormat format, const std::string& change)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		device_ = device;
		event_ = evt;
		format_ = format;
		change_.assign(change);
	}


	SynchronizationChange::SynchronizationChange(const SessionMessage& message)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		const char* buffer = message.serialize();

		device_ = message.getDeviceType();
		event_ = message.getEventType();
		format_ = (MessageFormat)message.getFormatType();
		change_.assign(buffer, message.getSerializedLength());
	}


	SynchronizationChange::SynchronizationChange(const char * buffer)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		memcpy(&errorCode_, buffer, sizeof(errorCode_));
		buffer += sizeof(errorCode_);

		memcpy(&batchNumber_, buffer, sizeof(batchNumber_));
		buffer += sizeof(batchNumber_);

		memcpy(&device_, buffer, sizeof(device_));
		buffer += sizeof(device_);

		memcpy(&event_, buffer, sizeof(event_));
		buffer += sizeof(event_);

		memcpy(&format_, buffer, sizeof(format_));
		buffer += sizeof(format_);

		int stringSize;
		memcpy(&stringSize, buffer, sizeof(stringSize));
		buffer += sizeof(stringSize);

		if ( stringSize )
		{
			change_.assign(buffer, stringSize);
			buffer += stringSize;
		}
	}


	SynchronizationChange::SynchronizationChange(const char * data, int len, int formatType)
	{
#ifdef USE_ACCOUNTANT
		if (dataType == 0)
			dataType = util_acc::registerDataType(dataTypeName);

		util_acc::addInstance(dataType);
#endif

		init();

		switch ( formatType )
		{
			case SynchronizationTaggedFormat:
			case DBIndependentFormatFormat:
			{
				SynchronizationChangeTags changeTags;

				//convert data to HobString
				HobString evolData(data, len);

				//use the UnitSeparator to parse the string
				HobTokenizerUS tok(&evolData, UnitSeparator);
				HobString temp = "";

				int offset = 0;
				HobTokenTagged *token = dynamic_cast<HobTokenTagged *>(HobToken::create(temp, HobTaggedToken));
				if ( token )
				{
					while ( tok.next(token) != -1 )
					{
						//get the tag and find out what offset goes with the tag
			  	 		temp = token->getTag();
						if ( temp.size() )
							offset = changeTags.getOffset(temp);
						else
							offset = 0;

						if ( offset )
						{
							setReceived(offset, true);
							temp = token->getData();
							switch ( offset )
							{
//								case SynchronizationChangeTags::fieldOffsetSynchronizationChangeIdentifier:
//									setIdentifier(util_string::stringtoint(temp));
//									break;
								case SynchronizationChangeTags::fieldOffsetSynchronizationChangeBatchNumber:
									setBatchNumber(util_string::stringtoint(temp));
									break;
								case SynchronizationChangeTags::fieldOffsetSynchronizationChangeDevice:
									setDevice(util_string::stringtoint(temp));
									break;
								case SynchronizationChangeTags::fieldOffsetSynchronizationChangeEvent:
									setEvent(util_string::stringtoint(temp));
									break;
								case SynchronizationChangeTags::fieldOffsetSynchronizationChangeFormat:
									setFormat((MessageFormat)util_string::stringtoint(temp));
									break;
								case SynchronizationChangeTags::fieldOffsetSynchronizationChangeChange:
									std::replace(temp.begin(), temp.end(), '\xFF', '\x1F');
									setChange(temp);
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


	SynchronizationChange::~SynchronizationChange()
	{
#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(dataType);
#endif

		if ( myDbRecord_ )
			delete myDbRecord_;

		Debug::trace("Delete.SynchronizationChange", "    SynchronizationChange: " + util_string::ptrtostring(this) /*+
				"  SynchronizationChange identifier:" + util_string::inttostring(this->getIdentifier()) */);
	}


	void SynchronizationChange::init()
	{
		errorCode_ = RECORD_NOT_FOUND;

		identifier_ = -1;
		batchNumber_ = Batch::_invalidBatchNumber_;
		device_ = -1;
		event_ = -1;
		format_ = UnknownFormat;
		change_.clear();

		dbAccess_ = 0;
		myDbRecord_ = 0;

		initFieldList();
	}


	void SynchronizationChange::initFieldList()
	{
		if ( _fieldList_.empty() )
		{
//			_fieldList_.push_back(SynchronizationChangeTags::fieldOffsetSynchronizationChangeIdentifier);	
			_fieldList_.push_back(SynchronizationChangeTags::fieldOffsetSynchronizationChangeBatchNumber);
			_fieldList_.push_back(SynchronizationChangeTags::fieldOffsetSynchronizationChangeDevice);
			_fieldList_.push_back(SynchronizationChangeTags::fieldOffsetSynchronizationChangeEvent);
			_fieldList_.push_back(SynchronizationChangeTags::fieldOffsetSynchronizationChangeFormat);
			_fieldList_.push_back(SynchronizationChangeTags::fieldOffsetSynchronizationChangeChange);

			SynchronizationChangeTags batchTags(&_fieldList_);
		}
	}


	void SynchronizationChange::setErrorCode(HobErrorCode errorCode)
	{
		this->errorCode_ = errorCode;

		Debug::trace("SynchronizationChange", "setErrorCode():  errorCode = " +
				util_string::inttostring(this->errorCode_));
	}


	const std::string& SynchronizationChange::getDeviceName() const
	{
		switch ( device_ )
		{
			case ProductDev:         /* 101 */    return ProductDevName;
			case OperatorDev:        /* 102 */    return OperatorDevName;
			case GraphicDev:         /* 105 */    return GraphicDevName;
			case ExpandedTextDev:    /* 106 */    return ExpandedTextDevName;
			case SpecialMessageDev:  /* 107 */    return SpecialMessageDevName;
			case ProductNotesDev:    /* 108 */    return ProductNotesDevName;
			case ClassDev:           /* 110 */    return ClassDevName;
			case LabelTypeDev:       /* 111 */    return LabelTypeDevName;
			case ConfigurationDev:   /* 112 */    return ConfigurationDevName;
			case FontDev:            /* 113 */    return FontDevName;
			case NutrifactsDev:      /* 117 */    return NutrifactsDevName;
			case GenericTextDev:     /* 119 */    return GenericTextDevName;
			case CategoryDev:        /* 121 */    return CategoryDevName;
			case SpecialsDev:        /* 124 */    return SpecialsDevName;
			case OperatorNotesDev:   /* 130 */    return OperatorNotesDevName;
			case ProductDefaultsDev: /* 132 */    return ProductDefaultsDevName;
			case EmailAlertsDev:     /* 134 */    return EmailAlertsDevName;
			case COOLDev:            /* 135 */    return COOLDevName;
			case SoftkeyDev:         /* 136 */    return SoftkeyDevName;
			case Marquee:            /* 140 */    return MarqueeDevName;
			default:                 /* N/A */    return UnknownDeviceName;
		}
	}


	void SynchronizationChange::updateSynchronizationChange(SynchronizationChange * newSynchronizationChange)
	{
		if (!newSynchronizationChange)
			return;

		for ( int loopCnt = 1; loopCnt < SynchronizationChangeTags::endSynchronizationChangeFieldOffsets; ++loopCnt )
		{
			if ( newSynchronizationChange->isReceived(loopCnt) )
			{
				switch ( loopCnt )
				{
//					case SynchronizationChangeTags::fieldOffsetSynchronizationChangeIdentifier:
//						setIdentifier(newSynchronizationChange->getIdentifier());
//						break;
					case SynchronizationChangeTags::fieldOffsetSynchronizationChangeBatchNumber:
						setBatchNumber(newSynchronizationChange->getBatchNumber());
						break;
					case SynchronizationChangeTags::fieldOffsetSynchronizationChangeDevice:
						setDevice(newSynchronizationChange->getDevice());
						break;
					case SynchronizationChangeTags::fieldOffsetSynchronizationChangeEvent:
						setEvent(newSynchronizationChange->getEvent());
						break;
					case SynchronizationChangeTags::fieldOffsetSynchronizationChangeFormat:
						setFormat(newSynchronizationChange->getFormat());
						break;
					case SynchronizationChangeTags::fieldOffsetSynchronizationChangeChange:
						setChange(newSynchronizationChange->getChange());
						break;
					case SynchronizationChangeTags::fieldOffsetSynchronizationChangeErrorCode:
						setErrorCode(newSynchronizationChange->getErrorCode());
						break;
					default:
						break;
				}
			}
		}
	}


	const char *SynchronizationChange::serialize() const
	{
		serializedData.clear();

		serializedData.append((const char *)&errorCode_, sizeof(errorCode_));

		serializedData.append((const char *)&batchNumber_, sizeof(batchNumber_));

		serializedData.append((const char *)&device_, sizeof(device_));

		serializedData.append((const char *)&event_, sizeof(event_));

		serializedData.append((const char *)&format_, sizeof(format_));

		int stringSize = (int)change_.size();
		serializedData.append((const char *)&stringSize, sizeof(stringSize));
		if ( stringSize )
			serializedData.append((const char *)change_.data(), stringSize);

		return serializedData.data();
	}


	const char *SynchronizationChange::serialize(int format) const
	{
		serializedData.clear();

		if ( getFormatType() != UnknownFormat )
			format = getFormatType();

		switch ( format )
		{
			case DBIndependentFormatFormat:
			{
				SynchronizationChangeTags batchTags;

				for ( int loopCnt = 1; loopCnt < SynchronizationChangeTags::endSynchronizationChangeFieldOffsets; ++loopCnt )
				{
					std::string fieldData, tag = batchTags.getTag(loopCnt);

					switch ( loopCnt )
					{
						case SynchronizationChangeTags::fieldOffsetSynchronizationChangeBatchNumber:
							fieldData = util_string::inttostring(getBatchNumber());
							break;
						case SynchronizationChangeTags::fieldOffsetSynchronizationChangeDevice:
							fieldData = util_string::inttostring(getDevice());
							break;
						case SynchronizationChangeTags::fieldOffsetSynchronizationChangeEvent:
							fieldData = util_string::inttostring(getEvent());
							break;
						case SynchronizationChangeTags::fieldOffsetSynchronizationChangeFormat:
							fieldData = util_string::inttostring(getFormat());
							break;
						case SynchronizationChangeTags::fieldOffsetSynchronizationChangeChange:
							fieldData = getChange();
							std::replace(fieldData.begin(), fieldData.end(), '\x1F', '\xFF');
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
			case SynchronizationTaggedFormat:
			{
				SynchronizationChangeTags batchTags;

				for ( int loopCnt = 1; loopCnt < SynchronizationChangeTags::endSynchronizationChangeFieldOffsets; ++loopCnt )
				{
					std::string fieldData, tag = batchTags.getTag(loopCnt);

					switch ( loopCnt )
					{
						case SynchronizationChangeTags::fieldOffsetSynchronizationChangeDevice:
							fieldData = util_string::inttostring(getDevice());
							break;
						case SynchronizationChangeTags::fieldOffsetSynchronizationChangeEvent:
							fieldData = util_string::inttostring(getEvent());
							break;
						case SynchronizationChangeTags::fieldOffsetSynchronizationChangeFormat:
							fieldData = util_string::inttostring(getFormat());
							break;
						case SynchronizationChangeTags::fieldOffsetSynchronizationChangeChange:
							fieldData = getChange();
							std::replace(fieldData.begin(), fieldData.end(), '\x1F', '\xFF');
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
				serializedData = "SynchronizationChange::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}

	bool SynchronizationChange::isConsolidatedOnly()
	{

		if ((device_ == TransactionDev) && (event_ == DtWrite))
		{
			return true;
		}

		return false;
	}

/*	SynchronizationChange * SynchronizationChange::findSynchronizationChangeId(int id, DBAccess * dbAccess)
	{
		SynchronizationChange * readSynchronizationChange = 0;

		if ( id <= 0 || !dbAccess )
			return readSynchronizationChange;

		DBSynchronizationChangesRM *dbSynchronizationChangesRM = dbAccess->getDBSynchronizationChangesRM();
		if ( !dbSynchronizationChangesRM )
			return readSynchronizationChange;

		try
		{
			DBSynchronizationChange *dbSynchronizationChangeRec = dbSynchronizationChangesRM->findIdentifier(util_string::inttostring(id));
			if ( dbSynchronizationChangeRec )
			{
				Debug::trace("SynchronizationChange", "findSynchronizationChange() - found this record in the db");

				readSynchronizationChange = new SynchronizationChange(dbSynchronizationChangeRec, dbAccess);
				if ( !readSynchronizationChange )
					delete dbSynchronizationChangeRec;
				else
				{
					if ( Debug::isTracing("SynchronizationChange") )
						Debug::trace("SynchronizationChange", "findSynchronizationChange() - created the hfc SynchronizationChange object - SynchronizationChange: " +
								util_string::ptrtostring(readSynchronizationChange) + "  SynchronizationChange identifier:" +
								util_string::inttostring(readSynchronizationChange->getIdentifier()) +
								"  SynchronizationChange version:" + util_string::inttostring(readSynchronizationChange->getFunctionCode()));
				}
			}
		}
		catch ( DBException &dbe )
		{
			if ( Debug::isTracing("DBAccess.exception") )
			{
				Debug::trace("DBAccess.exception", "SynchronizationChange::findSynchronizationChange() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", "     DBException: type:" +
						util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}
		}

		return readSynchronizationChange;
	}


	SynchronizationChange * SynchronizationChange::findSynchronizationChangeFunctionCode(int version, DBAccess * dbAccess)
	{
		SynchronizationChange * readSynchronizationChange = 0;

		if ( version <= 0 || !dbAccess )
			return readSynchronizationChange;

		DBSynchronizationChangesRM *dbSynchronizationChangesRM = dbAccess->getDBSynchronizationChangesRM();
		if ( !dbSynchronizationChangesRM )
			return readSynchronizationChange;

		try
		{
			DBSynchronizationChange *dbSynchronizationChangeRec = dbSynchronizationChangesRM->findFunctionCode(util_string::inttostring(version));
			if ( dbSynchronizationChangeRec )
			{
				Debug::trace("SynchronizationChange", "findSynchronizationChange() - found this record in the db");

				readSynchronizationChange = new SynchronizationChange(dbSynchronizationChangeRec, dbAccess);
				if ( !readSynchronizationChange )
					delete dbSynchronizationChangeRec;
				else
				{
					if ( Debug::isTracing("SynchronizationChange") )
						Debug::trace("SynchronizationChange", "findSynchronizationChange() - created the hfc SynchronizationChange object - SynchronizationChange: " +
								util_string::ptrtostring(readSynchronizationChange) + "  SynchronizationChange identifier:" +
								util_string::inttostring(readSynchronizationChange->getIdentifier()) +
								"  SynchronizationChange version:" + util_string::inttostring(readSynchronizationChange->getFunctionCode()));
				}
			}
		}
		catch ( DBException &dbe )
		{
			if ( Debug::isTracing("DBAccess.exception") )
			{
				Debug::trace("DBAccess.exception", "SynchronizationChange::findSynchronizationChange() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", "     DBException: type:" +
						util_string::inttostring(dbe.getType()) + "  info:" + dbe.info());
			}
		}

		return readSynchronizationChange;
	}
*/

}
