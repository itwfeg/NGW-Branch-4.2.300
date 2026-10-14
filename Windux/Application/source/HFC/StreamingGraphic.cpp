/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "StreamingGraphic.h"
#include "StreamingGraphicTags.h"
#include "Configuration.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBStreamingGraphicsRM.h"
#include "DBStreamingGraphic.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int StreamingGraphic::dataType = 0;
std::string StreamingGraphic::dataTypeName("StreamingGraphic"); 
#endif

std::list<StreamingGraphicFieldOffsets> StreamingGraphic::fieldList;	


StreamingGraphic::StreamingGraphic()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

StreamingGraphic::StreamingGraphic(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;
			short loopCnt = 1;
			while (tok.next(&token) != -1)
			{
				setReceived(loopCnt, (token.length() == 0) ? false : true);
					
				switch (loopCnt)
				{
					case fieldOffsetSGNum:
						streamingGraphicNum = (HobRecordNum)Long::valueOf(token);
						break;
					case fieldOffsetSGNextNum:
						nextStreamingGraphicNum = (HobRecordNum)Long::valueOf(token);
						break;
					case fieldOffsetSGImage:
						imageFileName = token;
						break;
					case fieldOffsetSGDisplayTime:
						displayTime = Integer::valueOf(token);
						break;
					case fieldOffsetSGActionType:
						actionType = Integer::valueOf(token);
						break;
					case fieldOffsetSGActionParameter:
					{
						token.unQuoteForCSV();
						actionParameter = token;
						break;
					}
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			StreamingGraphicTags streamingGraphicTags;

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp,HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? streamingGraphicTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						switch (offset)
						{
							case fieldOffsetSGNum:
								setStreamingGraphicNum((HobRecordNum)Long::valueOf(temp));
								break;
							case fieldOffsetSGNextNum:
								setNextStreamingGraphicNum((HobRecordNum)Long::valueOf(temp));
								break;
							case fieldOffsetSGImage:
								setImageFileName(temp);
								break;
							case fieldOffsetSGDisplayTime:
								setDisplayTime(Integer::valueOf(temp));
								break;
							case fieldOffsetSGActionType:
								setActionType(Integer::valueOf(temp));
								break;
							case fieldOffsetSGActionParameter:
								setActionParameter(temp);
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
			// todo - throw exception("invalid format");
			break;
	}
}

StreamingGraphic::StreamingGraphic(DBStreamingGraphic *dbStreamingGraphic, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("StreamingGraphic", "Constructor passing in a db rec");
	init();

	dbAccess = access;

	if (dbStreamingGraphic)
	{
		Debug::trace("StreamingGraphic", HobString("Constructor passing in a db rec - dbrec is not null"));
		myDbRecord = dbStreamingGraphic;

		streamingGraphicNum = (HobRecordNum) Long::valueOf(dbStreamingGraphic->getNumber());
		nextStreamingGraphicNum = (HobRecordNum) Long::valueOf(dbStreamingGraphic->getNext());
		imageFileName = dbStreamingGraphic->getImage();
		displayTime = dbStreamingGraphic->getTime();
		actionType = dbStreamingGraphic->getActionType();
		actionParameter = dbStreamingGraphic->getActionParameter();
		errorCode = RECORD_OK;
	}
}


StreamingGraphic::~StreamingGraphic()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;

	Debug::trace("StreamingGraphic.Delete", HobString("    StreamingGraphic: ") + HobString(Hex::toString((long long) this)) + HobString("  StreamingGraphic number:") + HobString(Long::toString(streamingGraphicNum)));
}


//	private
void StreamingGraphic::init()
{
	streamingGraphicNum = (HobRecordNum) RECORD_NOT_FOUND;
	nextStreamingGraphicNum = (HobRecordNum) RECORD_NOT_FOUND;
	imageFileName = "";
	displayTime = 15;
	actionType = 0;
	actionParameter = "";

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBStreamingGraphic *)NULL;

	errorCode = RECORD_NOT_FOUND;

	initFieldList();
}


// static
void StreamingGraphic::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetSGNum);
		fieldList.push_back(fieldOffsetSGNextNum);
		fieldList.push_back(fieldOffsetSGImage);
		fieldList.push_back(fieldOffsetSGDisplayTime);
		fieldList.push_back(fieldOffsetSGActionType);
		fieldList.push_back(fieldOffsetSGActionParameter);

		StreamingGraphicTags streamingGraphicTags((FieldOffsetsList *)&fieldList);
	}
}


HobRecordNum StreamingGraphic::getStreamingGraphicNum() const
{
	return streamingGraphicNum;
}
void StreamingGraphic::setStreamingGraphicNum(const HobRecordNum num)
{
	streamingGraphicNum = num;
}

HobRecordNum StreamingGraphic::getNextStreamingGraphicNum() const
{
	return nextStreamingGraphicNum;
}
void StreamingGraphic::setNextStreamingGraphicNum(const HobRecordNum num)
{
	nextStreamingGraphicNum = num;
}

const HobString &StreamingGraphic::getImageFileName() const
{
	return imageFileName;
}
void StreamingGraphic::setImageFileName(const HobString &name)
{
	imageFileName = name;
}

int StreamingGraphic::getDisplayTime() const
{
	return displayTime;
}
void StreamingGraphic::setDisplayTime(const int time)
{
	displayTime = time;
}

int StreamingGraphic::getActionType() const
{
	return actionType;
}
void StreamingGraphic::setActionType(const int type)
{
	actionType = type;
}

const HobString &StreamingGraphic::getActionParameter() const
{
	return actionParameter;
}
void StreamingGraphic::setActionParameter(const HobString &parameter)
{
	actionParameter = parameter;
}


HobErrorCode StreamingGraphic::getErrorCode() const
{
	return errorCode;
}
void StreamingGraphic::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


void StreamingGraphic::updateStreamingGraphic(const StreamingGraphic *newStreamingGraphic)
{
	if (!newStreamingGraphic)
		return;

	short loopCnt = 1;
	while (loopCnt < endStreamingGraphicFieldOffsets)
	{
		if (newStreamingGraphic->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetSGNum:
					setStreamingGraphicNum(newStreamingGraphic->getStreamingGraphicNum());
					break;
				case fieldOffsetSGNextNum:
					setNextStreamingGraphicNum(newStreamingGraphic->getNextStreamingGraphicNum());
					break;
				case fieldOffsetSGImage:
					setImageFileName(newStreamingGraphic->getImageFileName());
					break;
				case fieldOffsetSGDisplayTime:
					setDisplayTime(newStreamingGraphic->getDisplayTime());
					break;
				case fieldOffsetSGActionType:
					setActionType(newStreamingGraphic->getActionType());
					break;
				case fieldOffsetSGActionParameter:
					setActionParameter(newStreamingGraphic->getActionParameter());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


const char *StreamingGraphic::serialize(int format) const
{
	serializedData = "";
	Configuration *config = Configuration::getInstance();

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case EditSlideFormat:
		case BrowserFormat:
			for (int loopCnt = 1; loopCnt < endStreamingGraphicFieldOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetSGNum:
						serializedData.append(Long::toString((long)streamingGraphicNum));
						break;
					case fieldOffsetSGNextNum:
						serializedData.append(Long::toString((long)nextStreamingGraphicNum));
						break;
					case fieldOffsetSGImage:
					{
						HobString temp("");
						if (imageFileName.size())
						{
							if (format == EditSlideFormat)
								temp = imageFileName;
							else
							{
								temp = config->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDIR);
								temp.append(imageFileName);
								if (imageFileName.find('.') == std::string::npos)
									temp.append(config->toStringCfgI(CfgInternal::STREAMINGGRAPHICSDEFAULTEXTENSION));
							}
						}
						serializedData.append(temp);
						break;
					}
					case fieldOffsetSGDisplayTime:
						serializedData.append(Integer::toString(displayTime));
						break;
					case fieldOffsetSGActionType:
						serializedData.append(Integer::toString(actionType));
						break;
					case fieldOffsetSGActionParameter:
					{
						HobString temp(actionParameter);
						temp.quoteForCSV();
						serializedData.append(temp);
						break;
					}
					default:
						break;
				}
			}
			break;

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			StreamingGraphicTags streamingGraphicTags;
			HobString tag = "";
			HobString fieldData = "";
			
			std::list<StreamingGraphicFieldOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				tag = streamingGraphicTags.getTag(*fieldIter);
				switch (*fieldIter)
				{
					case fieldOffsetSGNum:
						fieldData =	Long::toString((long)streamingGraphicNum);
						break;
					case fieldOffsetSGNextNum:
						fieldData =	Long::toString((long)nextStreamingGraphicNum);
						break;
					case fieldOffsetSGImage:
						fieldData =	imageFileName;
						break;
					case fieldOffsetSGDisplayTime:
						fieldData =	Integer::toString(displayTime);
						break;
					case fieldOffsetSGActionType:
						fieldData =	Integer::toString(actionType);
						break;
					case fieldOffsetSGActionParameter:
						fieldData =	actionParameter;
						break;
					default:
						tag = "";
						fieldData = "";
						break;
				}
						
				if (tag.size())
				{
					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = HobString("StreamingGraphic::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void StreamingGraphic::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


StreamingGraphic *StreamingGraphic::findStreamingGraphic(HobRecordNum sgNum, DBAccess *dbAccess)
{
	StreamingGraphic *readStreamingGraphic = (StreamingGraphic *)NULL;

	if (sgNum <= 0 || !dbAccess)
		return readStreamingGraphic;

	DBStreamingGraphicsRM *dbStreamingGraphicsRM = dbAccess->getDBStreamingGraphicsRM();
	if (!dbStreamingGraphicsRM)
		return readStreamingGraphic;

	try
	{
		DBStreamingGraphic *dbStreamingGraphicRec = dbStreamingGraphicsRM->findNumber(HobString(Long::toString(sgNum)));
		if (dbStreamingGraphicRec)
		{
			readStreamingGraphic = new StreamingGraphic(dbStreamingGraphicRec, dbAccess);
			if (!readStreamingGraphic)
				delete dbStreamingGraphicRec;
			else
			{
				if (Debug::isTracing("StreamingGraphic"))
					Debug::trace("StreamingGraphic", HobString("findStreamingGraphic() - StreamingGraphic: ") + HobString(Hex::toString((long long) readStreamingGraphic)) +
														HobString("  StreamingGraphic number:") + HobString(Long::toString(readStreamingGraphic->getStreamingGraphicNum())));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("StreamingGraphic::findStreamingGraphic() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return readStreamingGraphic;
}


bool StreamingGraphic::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int StreamingGraphic::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBStreamingGraphicsRM *dbStreamingGraphicsRM = dbAccess->getDBStreamingGraphicsRM();
		if (dbStreamingGraphicsRM)
		{
			Debug::trace("StreamingGraphic", HobString("Deleting StreamingGraphic# ") + HobString(Long::toString(streamingGraphicNum)));
			result = dbStreamingGraphicsRM->deleteRecord(myDbRecord);	
		}
	}
	return result;							
}

void StreamingGraphic::saveRecord()
{
	if (Debug::isTracing("StreamingGraphic"))
	{
		Debug::trace("StreamingGraphic", HobString("saveRecord() - show what is getting saved."));
		show();
	}
	if (streamingGraphicNum > 0 && dbAccess)
	{
		DBStreamingGraphicsRM *dbStreamingGraphicsRM = dbAccess->getDBStreamingGraphicsRM();
		if (!dbStreamingGraphicsRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("StreamingGraphic", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbStreamingGraphicsRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbStreamingGraphicRecord
			myDbRecord->setNumber(Long::toString(streamingGraphicNum));
			myDbRecord->setNext(Long::toString(nextStreamingGraphicNum));
			myDbRecord->setImage(imageFileName);
			myDbRecord->setTime(displayTime);
			myDbRecord->setActionType(actionType);
			myDbRecord->setActionParameter(actionParameter);

			try
			{
				dbStreamingGraphicsRM->saveRecord(myDbRecord);		//save record to the database
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "StreamingGraphic::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

void StreamingGraphic::update(StreamingGraphic *streamingGraphic)
{
	if (!streamingGraphic)
		return;

	setStreamingGraphicNum(streamingGraphic->getStreamingGraphicNum());
	setNextStreamingGraphicNum(streamingGraphic->getNextStreamingGraphicNum());
	setImageFileName(streamingGraphic->getImageFileName());
	setDisplayTime(streamingGraphic->getDisplayTime());
	setActionType(streamingGraphic->getActionType());
	setActionParameter(streamingGraphic->getActionParameter());
}

void StreamingGraphic::show()
{
	if (Debug::isTracing("StreamingGraphic"))
	{
		Debug::trace("StreamingGraphic", "*** StreamingGraphic parameters ***");
		Debug::trace("StreamingGraphic", HobString("slideNumber:") + HobString(Long::toString(streamingGraphicNum)));
		Debug::trace("StreamingGraphic", HobString("nextSlideNumber:") + HobString(Long::toString(nextStreamingGraphicNum)));
		Debug::trace("StreamingGraphic", HobString("image:") + imageFileName);
		Debug::trace("StreamingGraphic", HobString("displayTime:") + HobString(Integer::toString(displayTime)));
		Debug::trace("StreamingGraphic", HobString("actionType:") + HobString(Integer::toString(actionType)));
		Debug::trace("StreamingGraphic", HobString("actionParameter:") + actionParameter);
	}
}

