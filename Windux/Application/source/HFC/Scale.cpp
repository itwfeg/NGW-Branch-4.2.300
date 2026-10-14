#include "Scale.h"


#include "ScaleTags.h"	
#include "Batch.h"

#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"


#include "util/e_string/utilstring.h"
#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace hfc
{


	#ifdef USE_ACCOUNTANT
		int Scale::_dataType_ = 0;
		std::string Scale::_dataTypeName_("hfc::Scale");
	#endif


	FieldOffsetsList Scale::_fieldList_;


	Scale::Scale(ScaleState state)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();
		state_ = state;
	}


	Scale::Scale(const char * data, int len, int formatType)
	{
		#ifdef USE_ACCOUNTANT
			if ( !_dataType_ )
				_dataType_ = util_acc::registerDataType(_dataTypeName_);

			util_acc::addInstance(_dataType_);
		#endif

		init();

		switch (formatType)
		{
			case DBIndependentFormatFormat:
			{
				ScaleTags scaleTags;

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
							offset = scaleTags.getOffset(temp);
						}
						else
							offset = 0;

						if ( offset )
						{
							setReceived(offset, true);
							temp = token->getData();
							switch ( offset )
							{
								case ScaleTags::fieldOffsetScaleNumber:
									setIdentifier(util_string::stringtoint(temp));
									break;
								case ScaleTags::fieldOffsetScaleRole:
									setRole((hfc::Scale::ScaleRole)util_string::stringtoint(temp));
									break;
								case ScaleTags::fieldOffsetScaleHostName:
							  		setHostName(temp);
									break;
								case ScaleTags::fieldOffsetScaleIPAddress:
							  		setIPAddress(temp);
									break;
								case ScaleTags::fieldOffsetScaleCreationDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setCreationDateTime(hdt);
									break;
								}
								case ScaleTags::fieldOffsetScaleState:
									setState((ScaleState)util_string::stringtoint(temp));
									break;
								case ScaleTags::fieldOffsetScaleStateChangedDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setStateChangedDateTime(hdt);
									break;
								}
								case ScaleTags::fieldOffsetScaleRegistering:
									setRegistering(util_string::stringtobool(temp));
									break;
								case ScaleTags::fieldOffsetScaleDeregistering:
									setDeregistering(util_string::stringtobool(temp));
									break;
								case ScaleTags::fieldOffsetScaleNotifiedBatches:
									setNotifiedBatches(util_string::stringtobool(temp));
									break;
								case ScaleTags::fieldOffsetScaleNotifiedBatchesDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setNotifiedBatchesDateTime(hdt);
									break;
								}
								case ScaleTags::fieldOffsetScaleNotifiedOnline:
									setNotifiedOnline(util_string::stringtobool(temp));
									break;
								case ScaleTags::fieldOffsetScaleNotifiedOnlineDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setNotifiedOnlineDateTime(hdt);
									break;
								}
								case ScaleTags::fieldOffsetScaleRetryCount:
									setRetryCount(util_string::stringtoint(temp));
									break;
								case ScaleTags::fieldOffsetScaleVersion:
									setVersion(util_string::stringtoint(temp));
									break;
								case ScaleTags::fieldOffsetScaleVersionReceived:
									setVersionReceived(util_string::stringtoint(temp));
									break;
								case ScaleTags::fieldOffsetScaleVersionReceivedDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setVersionReceivedDateTime(hdt);
									break;
								}
								case ScaleTags::fieldOffsetScaleLastCommSuccessful:
									setLastCommSuccessful(util_string::stringtobool(temp));
									break;
								case ScaleTags::fieldOffsetScaleLastSuccessfulCommDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
									setLastSuccessfulCommDateTime(hdt);
									break;
								}
								case ScaleTags::fieldOffsetScaleFirstFailedCommDateTime:
								{
									HobDateTime hdt;
									hdt.convertFromDBFormat(temp);
							  		setFirstFailedCommDateTime(hdt);
									break;
								}
								case ScaleTags::fieldOffsetScaleTotalSuccessfulCommunications:
									setTotalSuccessfulCommunications(util_string::stringtoint(temp));
									break;
								case ScaleTags::fieldOffsetScaleTotalFailedCommunications:
									setTotalFailedCommunications(util_string::stringtoint(temp));
									break;
								case ScaleTags::fieldOffsetScaleTotalBusyCommunications:
									setTotalBusyCommunications(util_string::stringtoint(temp));
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


	Scale::~Scale()
	{
		#ifdef USE_ACCOUNTANT
			util_acc::removeInstance(_dataType_);
		#endif

		Debug::trace("Delete.Scale", "    Scale: " + util_string::ptrtostring(this) +
			"  Scale hostname:" + this->getHostName() + "  IPAddress:" + this->getIPAddress());
	}


	void Scale::init()
	{
		errorCode_ = RECORD_NOT_FOUND;

		identifier_ = -1;
		role_ = RoleInvalid;
		hostname_.assign("");
		ipAddress_.assign("");
		creationDateTime_ = HobDateTime().convertToDBFormat();
		state_ = StateActive;
		stateChangedDateTime_.assign("");
		registering_ = false;
		deregistering_ = false;
		notifiedBatches_ = false;
		notifiedBatchesDateTime_.assign("");
		notifiedOnline_ = false;
		notifiedOnlineDateTime_.assign("");
		retryCount_ = 0;
		version_ = Batch::_invalidBatchVersion_;
		versionReceived_ = Batch::_invalidBatchVersion_; 
		versionReceivedDateTime_.assign("");
		lastCommSuccessful_ = false;
		lastSuccessfulCommDateTime_.assign("");
		firstFailedCommDateTime_.assign("");
		totalSuccessfulCommunications_ = 0;
		totalFailedCommunications_ = 0;
		totalBusyCommunications_ = 0;

		initFieldList();
	}


	void Scale::initFieldList()
	{
		if ( _fieldList_.empty() )
		{
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleNumber);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleRole);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleHostName);	
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleIPAddress);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleCreationDateTime);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleState);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleStateChangedDateTime);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleRegistering);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleDeregistering);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleNotifiedBatches);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleNotifiedBatchesDateTime);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleNotifiedOnline);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleNotifiedOnlineDateTime);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleRetryCount);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleVersion);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleVersionReceived);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleVersionReceivedDateTime);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleLastCommSuccessful);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleLastSuccessfulCommDateTime);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleFirstFailedCommDateTime);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleTotalSuccessfulCommunications);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleTotalFailedCommunications);
			_fieldList_.push_back(ScaleTags::fieldOffsetScaleTotalBusyCommunications);

			ScaleTags scaleTags(&_fieldList_);
		}
	}


	void Scale::setHostName(const std::string & hostname)
	{
		hostname_ = util_string::strtoupper(hostname);
	}


	void Scale::setState(ScaleState state)
	{
		if ( state_ != state )
		{
			setStateChangedDateTime();
			state_ = state;
		}
	}


	void Scale::setLastCommSuccessful(bool successful)
	{
		if ( successful )
		{
			lastCommSuccessful_ = successful;
			setLastSuccessfulCommDateTime(HobDateTime());
			incrementTotalSuccessfulCommunications();
		}
		else
		{
			if ( lastCommSuccessful_ )
			{
				lastCommSuccessful_ = successful;
				setFirstFailedCommDateTime(HobDateTime());
			}

			incrementTotalFailedCommunications();
		}
	}


	void Scale::setErrorCode(HobErrorCode errorCode)
	{
		this->errorCode_ = errorCode;

		Debug::trace("Scale", "setErrorCode():  errorCode = " +
				util_string::inttostring(this->errorCode_));
	}


	void Scale::updateScale(Scale * newScale)
	{
		if ( !newScale )
			return;

		for ( int loopCnt = 1; loopCnt < ScaleTags::endScaleFieldOffsets; ++loopCnt )
		{
			if ( newScale->isReceived(loopCnt) )
			{
				switch ( loopCnt )
				{
					case ScaleTags::fieldOffsetScaleRole:
						setRole(newScale->getRole());
						break;
					case ScaleTags::fieldOffsetScaleHostName:
						setHostName(newScale->getHostName());
						break;
					case ScaleTags::fieldOffsetScaleIPAddress:
						setIPAddress(newScale->getIPAddress());
						break;
					case ScaleTags::fieldOffsetScaleCreationDateTime:
						setCreationDateTime(newScale->getCreationDateTime());
						break;
					case ScaleTags::fieldOffsetScaleState:
						setState(newScale->getState());
						break;
					case ScaleTags::fieldOffsetScaleStateChangedDateTime:
						setStateChangedDateTime(newScale->getStateChangedDateTime());
						break;
					case ScaleTags::fieldOffsetScaleRegistering:
						setRegistering(newScale->isRegistering());
						break;
					case ScaleTags::fieldOffsetScaleDeregistering:
						setDeregistering(newScale->isDeregistering());
						break;
					case ScaleTags::fieldOffsetScaleNotifiedBatches:
						setNotifiedBatches(newScale->isNotifiedBatches());
						break;
					case ScaleTags::fieldOffsetScaleNotifiedBatchesDateTime:
						setNotifiedBatchesDateTime(newScale->getNotifiedBatchesDateTime());
						break;
					case ScaleTags::fieldOffsetScaleNotifiedOnline:
						setNotifiedOnline(newScale->isNotifiedOnline());
						break;
					case ScaleTags::fieldOffsetScaleNotifiedOnlineDateTime:
						setNotifiedOnlineDateTime(newScale->getNotifiedOnlineDateTime());
						break;
					case ScaleTags::fieldOffsetScaleRetryCount:
						setRetryCount(newScale->getRetryCount());
						break;
					case ScaleTags::fieldOffsetScaleVersion:
						setVersion(newScale->getVersion());
						break;
					case ScaleTags::fieldOffsetScaleVersionReceived:
						setVersionReceived(newScale->getVersionReceived());
						break;
					case ScaleTags::fieldOffsetScaleVersionReceivedDateTime:
						setVersionReceivedDateTime(newScale->getVersionReceivedDateTime());
						break;
					case ScaleTags::fieldOffsetScaleLastCommSuccessful:
						setLastCommSuccessful(newScale->isLastCommSuccessful());
						break;
					case ScaleTags::fieldOffsetScaleLastSuccessfulCommDateTime:
						setLastSuccessfulCommDateTime(newScale->getLastSuccessfulCommDateTime());
						break;
					case ScaleTags::fieldOffsetScaleFirstFailedCommDateTime:
						setFirstFailedCommDateTime(newScale->getFirstFailedCommDateTime());
						break;
					case ScaleTags::fieldOffsetScaleTotalSuccessfulCommunications:
						setTotalSuccessfulCommunications(newScale->getTotalSuccessfulCommunications());
						break;
					case ScaleTags::fieldOffsetScaleTotalFailedCommunications:
						setTotalFailedCommunications(newScale->getTotalFailedCommunications());
						break;
					case ScaleTags::fieldOffsetScaleTotalBusyCommunications:
						setTotalBusyCommunications(newScale->getTotalBusyCommunications());
						break;
					default:
						break;
				}
			}
		}
	}


	const char * Scale::serialize(int format) const
	{
		serializedData.clear();

		if ( getFormatType() != UnknownFormat )
			format = getFormatType();

		switch ( format )
		{
			case DBIndependentFormatFormat:
			{
				ScaleTags scaleTags;

				for ( int loopCnt = 1; loopCnt < ScaleTags::endScaleFieldOffsets; ++loopCnt )
				{
					std::string fieldData, tag = scaleTags.getTag(loopCnt);

					switch ( loopCnt )
					{
						case ScaleTags::fieldOffsetScaleNumber:
							fieldData = util_string::inttostring(getIdentifier());
							break;
						case ScaleTags::fieldOffsetScaleRole:
							fieldData = util_string::inttostring(getRole());
							break;
						case ScaleTags::fieldOffsetScaleHostName:
							fieldData = getHostName();
							break;
						case ScaleTags::fieldOffsetScaleIPAddress:
							fieldData = getIPAddress();
							break;
						case ScaleTags::fieldOffsetScaleCreationDateTime:
						{
							HobDateTime hdt = getCreationDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case ScaleTags::fieldOffsetScaleState:
							fieldData = util_string::inttostring(getState());
							break;
						case ScaleTags::fieldOffsetScaleStateChangedDateTime:
						{
							HobDateTime hdt = getStateChangedDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case ScaleTags::fieldOffsetScaleRegistering:
							fieldData = util_string::booltostring(isRegistering());
							break;
						case ScaleTags::fieldOffsetScaleDeregistering:
							fieldData = util_string::booltostring(isDeregistering());
							break;
						case ScaleTags::fieldOffsetScaleNotifiedBatches:
							fieldData = util_string::booltostring(isNotifiedBatches());
							break;
						case ScaleTags::fieldOffsetScaleNotifiedBatchesDateTime:
						{
							HobDateTime hdt = getNotifiedBatchesDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case ScaleTags::fieldOffsetScaleNotifiedOnline:
							fieldData = util_string::booltostring(isNotifiedOnline());
							break;
						case ScaleTags::fieldOffsetScaleNotifiedOnlineDateTime:
						{
							HobDateTime hdt = getNotifiedOnlineDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case ScaleTags::fieldOffsetScaleRetryCount:
							fieldData = util_string::inttostring(getRetryCount());
							break;
						case ScaleTags::fieldOffsetScaleVersion:
							fieldData = util_string::inttostring(getVersion());
							break;
						case ScaleTags::fieldOffsetScaleVersionReceived:
							fieldData = util_string::inttostring(getVersionReceived());
							break;
						case ScaleTags::fieldOffsetScaleVersionReceivedDateTime:
						{
							HobDateTime hdt = getVersionReceivedDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case ScaleTags::fieldOffsetScaleLastCommSuccessful:
							fieldData = util_string::booltostring(isLastCommSuccessful());
							break;
						case ScaleTags::fieldOffsetScaleLastSuccessfulCommDateTime:
						{
							HobDateTime hdt = getLastSuccessfulCommDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case ScaleTags::fieldOffsetScaleFirstFailedCommDateTime:
						{
							HobDateTime hdt = getFirstFailedCommDateTime();
							fieldData =	hdt.convertToDBFormat(FormatDateTime_Windux);
							break;
						}
						case ScaleTags::fieldOffsetScaleTotalSuccessfulCommunications:
							fieldData = util_string::inttostring(getTotalSuccessfulCommunications());
							break;
						case ScaleTags::fieldOffsetScaleTotalFailedCommunications:
							fieldData = util_string::inttostring(getTotalFailedCommunications());
							break;
						case ScaleTags::fieldOffsetScaleTotalBusyCommunications:
							fieldData = util_string::inttostring(getTotalBusyCommunications());
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
				serializedData = "Scale::serialize - unknown format: " + util_string::inttostring(format);
				// todo - throw exception("invalid format");
				break;
			}
		}

		return serializedData.data();
	}


}
