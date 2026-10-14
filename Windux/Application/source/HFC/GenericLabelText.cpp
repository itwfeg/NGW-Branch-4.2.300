/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "GenericLabelText.h"
#include "GenericLabelTextTags.h"
#include "Configuration.h"
#include "ConfigurationEvent.h"
#include "SessionMessage.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "HobToken.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBLabelTextsRM.h"
#include "DBLabelText.h"

#include "MultiLanguageText.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int GenericLabelText::dataType = 0;
std::string GenericLabelText::dataTypeName("GenericLabelText"); 
#endif

std::list<GenericLabelTextOffsets> GenericLabelText::fieldList;


GenericLabelText::GenericLabelText()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

GenericLabelText::GenericLabelText(const char *data, int len, int formatType)
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
					case fieldOffsetGenericTextNum:
						textNum = (HobRecordNum) Long::valueOf(token);
						break;
					case fieldOffsetGenericTextText:
					{
						token.unQuoteForCSV();
						text = token;
						break;
					}
					default:
						break;
				}
				loopCnt++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			GenericLabelTextTags genericLabelTextTags; 	

			//convert data to HobString
			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp = "";

			int offset = 0;
			HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(temp, HobTaggedToken));
			if (token)
			{
				while (tok.next(token) != -1)
				{
					//get the tag and find out what offset goes with the tag
					temp = token->getTag();
					offset = (temp.size()) ? genericLabelTextTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();
						switch (offset)
						{
							case fieldOffsetGenericTextNum:
							 	setTextNum((HobRecordNum)Long::valueOf(temp));
								break;	
							case fieldOffsetGenericTextText: 
								setText(temp);
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

GenericLabelText::GenericLabelText(DBLabelText *dbLabelText, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("GenericLabelText", "Constructor passing in a db rec");
	init();

	dbAccess = access;

	if (dbLabelText)
	{
		if (dbLabelText->isRecordValid() && !dbLabelText->isTextNull())
		{
			myDbRecord = dbLabelText;

			textNum = (HobRecordNum) Long::valueOf(dbLabelText->getNumber());
			text = HobString(dbLabelText->getText());
			errorCode = RECORD_OK;
		}
	}
}


GenericLabelText::~GenericLabelText()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}


//private
void GenericLabelText::init()
{
	textNum = RECORD_NOT_FOUND;
	text = "";
	errorCode = RECORD_NOT_FOUND;
	maxLabelTexts = Configuration::getInstance()->getMaxLabelTexts();

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBLabelText *)NULL;

	initFieldList();
}


// static
void GenericLabelText::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetGenericTextNum);
		fieldList.push_back(fieldOffsetGenericTextText);

		GenericLabelTextTags genericLabelTextTags((FieldOffsetsList *)&fieldList);
	}
}


HobRecordNum GenericLabelText::getTextNum() const
{
	return textNum;
}
void GenericLabelText::setTextNum(HobRecordNum textNumber)
{
	if ((textNumber > 0) && (textNumber <= maxLabelTexts))
		textNum = textNumber;
}


const HobString &GenericLabelText::getText() const
{
	return text;
}
void GenericLabelText::setText(const HobString &txt)
{
	if (txt.length() > 50)
	{
		text.assign(txt, 0, 50);
		Debug::trace("ClipText", "GenericLabelText: text clipped to [" + text + "]");
	}
	else
	{
		text = txt;
		Debug::trace("ClipText", "GenericLabelText: text is [" + text + "]");
	}
}


HobErrorCode GenericLabelText::getErrorCode() const
{
	return errorCode;
}
void GenericLabelText::setErrorCode(HobErrorCode errorCode)
{
	this->errorCode = errorCode;
}


void GenericLabelText::updateText(const GenericLabelText *newGLT)
{
	if (!newGLT)
		return;

	short loopCnt = 1;
	while (loopCnt < endGenericLabelTextOffsets)
	{
		if (newGLT->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetGenericTextNum:
					setTextNum(newGLT->getTextNum());
					break;
				case fieldOffsetGenericTextText:
					setText(newGLT->getText());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


const char *GenericLabelText::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case BrowserFormat:
			for (int loopCnt = 1; loopCnt < endGenericLabelTextOffsets; loopCnt++)
			{
				if (loopCnt > 1)
					serializedData.append(BrowserFormatSeparator);
				switch (loopCnt)
				{
					case fieldOffsetGenericTextNum:
						serializedData.append(HobString(Long::toString(textNum)));
						break;

					case fieldOffsetGenericTextText:
					{
						HobString tempString(text);
						tempString.quoteForCSV();
						serializedData.append(tempString);
						break;
					}

					default:
						break;
				}
			}
			break;

		case SynchronizationTaggedFormat:
/*		{
			HobString tag, fieldData;
			GenericLabelTextTags genericLabelTextTags;

			std::list<GenericLabelTextOffsets>::iterator fieldIter = fieldList.begin();
			while ( fieldIter != fieldList.end() )
			{
				if ( isReceived(*fieldIter) )
				{
					tag = genericLabelTextTags.getTag(*fieldIter);
					switch ( *fieldIter )
					{
						case fieldOffsetGenericTextNum:
							fieldData = Long::toString(textNum); 				
							break;
						case fieldOffsetGenericTextText:
							fieldData = text;
							break;
						default:
							tag = "";
							fieldData = "";
							break;
					}

					if ( tag.size() )
					{
						serializedData.append(tag); 
						serializedData.append(fieldData);
						serializedData.append(UnitSeparator);	
					}
				}

				fieldIter++;
			}

			break;
		}
*/
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			GenericLabelTextTags genericLabelTextTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<GenericLabelTextOffsets>::iterator fieldIter = fieldList.begin();
			while (fieldIter != fieldList.end())
			{
				tag = genericLabelTextTags.getTag(*fieldIter);
				switch (*fieldIter)
				{
					case fieldOffsetGenericTextNum:
						fieldData = Long::toString(textNum); 				
						break;
					case fieldOffsetGenericTextText:
						fieldData = text;
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
			serializedData = HobString("GenericLabelText::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


void GenericLabelText::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}


GenericLabelText *GenericLabelText::findLabelText(HobRecordNum textNum, DBAccess *dbAccess)
{
	GenericLabelText *readGenericText = (GenericLabelText *)NULL;

	if (textNum <= 0 || !dbAccess)
		return readGenericText;

	DBLabelTextsRM *dbLabelTextsRM = dbAccess->getDBLabelTextsRM();
	if (!dbLabelTextsRM)
		return readGenericText;

	try
	{
		DBLabelText *dbLabelTextRec = dbLabelTextsRM->findNumber(HobString(Long::toString(textNum)));
		if (dbLabelTextRec)
		{
			Debug::trace("GenericLabelText", "findLabelText() - found this record in the db");
			readGenericText = new GenericLabelText(dbLabelTextRec, dbAccess);
			if (!readGenericText)
				delete dbLabelTextRec;
			else
			{
				if (Debug::isTracing("GenericLabelText"))
					Debug::trace("GenericLabelText", "findLabelText() - created the hfc GenericLabelText object - GenericLabelText: " +
							util_string::ptrtostring(readGenericText) + "  Text number:" + util_string::longtostring(readGenericText->getTextNum()));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("GenericLabelText::findLabelText() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return readGenericText;
}


bool GenericLabelText::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}


int GenericLabelText::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBLabelTextsRM *dbLabelTextsRM = dbAccess->getDBLabelTextsRM();
		if (dbLabelTextsRM)
		{
			Debug::trace("GenericLabelText", HobString("Deleting GenericLabelText# ") + HobString(Long::toString(textNum)));
			result = dbLabelTextsRM->deleteRecord(myDbRecord);	
		}
	}
	return result;							
}

void GenericLabelText::saveRecord()
{
	if (textNum > 0 && dbAccess)
	{ 
		DBLabelTextsRM *dbLabelTextsRM = dbAccess->getDBLabelTextsRM();
		if (!dbLabelTextsRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("GenericLabelText", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbLabelTextsRM->createRecord();
		}
		
		if (myDbRecord)
		{
			myDbRecord->setNumber(Long::toString(textNum));
			if (text.empty())
				myDbRecord->setTextNull();
			else
				myDbRecord->setText(text);

			//	Now save the Generic Text record to the database.
			Debug::trace("GenericLabelText", HobString("saveRecord() - saving record"));

			try
			{
				dbLabelTextsRM->saveRecord(myDbRecord);
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "GenericLabelText::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}


// static
/*
	This method will create the default GenericLabelText records if they do not exist.
*/
int GenericLabelText::verifyDefaultRecords(DBAccess *dbAccess)
{
	int updateCount = 0;
	if (!dbAccess)
	{
		Debug::trace("GenericLabelText", "verifyDefaultRecords - no dbAccess passed in!");
		return updateCount;
	}

	Debug::trace("GenericLabelText", "verifying all default GenericLabelTexts ...");
	GenericLabelText *reservedText = (GenericLabelText *)NULL;
	hfc::MultiLanguageText internationalize;

    //  Generic Text 1 - Packed On
	HobRecordNum recordNum = 1;
	reservedText = findLabelText(recordNum, dbAccess);
	if (!reservedText)
	{

		reservedText = new GenericLabelText();
		if (reservedText)
		{
			reservedText->setTextNum(recordNum);
			reservedText->setText(internationalize("LabelText.PackedOn"));
			reservedText->setDBAccess(dbAccess);
			reservedText->saveRecord();
			if (Debug::isTracing("GenericLabelText"))
				Debug::trace("GenericLabelText", HobString("vDR: Creating label text #") + HobString(Integer::toString(recordNum)) + HobString(" : ") + reservedText->getText());
			++updateCount;

			Configuration::getInstance()->setLabelText(reservedText->getTextNum(), reservedText);
		}
	}
	else
		delete reservedText;

    //  Generic Text 2 - Best Before
	recordNum = 2;
	reservedText = findLabelText(recordNum, dbAccess);
	if (!reservedText)
	{
		reservedText = new GenericLabelText();
		if (reservedText)
		{
			reservedText->setTextNum(recordNum);
			reservedText->setText(internationalize("LabelText.BestBefore"));
			reservedText->setDBAccess(dbAccess);
			reservedText->saveRecord();
			if (Debug::isTracing("GenericLabelText"))
				Debug::trace("GenericLabelText", HobString("vDR: Creating label text #") + HobString(Integer::toString(recordNum)) + HobString(" : ") + reservedText->getText());
			++updateCount;

			Configuration::getInstance()->setLabelText(reservedText->getTextNum(), reservedText);
		}
	}
	else
		delete reservedText;

    //  Generic Text 3 - Sell By
	recordNum = 3;
	reservedText = findLabelText(recordNum, dbAccess);
	if (!reservedText)
	{
		reservedText = new GenericLabelText();
		if (reservedText)
		{
			reservedText->setTextNum(recordNum);
			reservedText->setText(internationalize("LabelText.SellBy"));
			reservedText->setDBAccess(dbAccess);
			reservedText->saveRecord();
			if (Debug::isTracing("GenericLabelText"))
				Debug::trace("GenericLabelText", HobString("vDR: Creating label text #") + HobString(Integer::toString(recordNum)) + HobString(" : ") + reservedText->getText());
			++updateCount;

			Configuration::getInstance()->setLabelText(reservedText->getTextNum(), reservedText);
		}
	}
	else
		delete reservedText;

    //  Generic Text 4 - Net Wt/Ct
	recordNum = 4;
	reservedText = findLabelText(recordNum, dbAccess);
	if (!reservedText)
	{
		reservedText = new GenericLabelText();
		if (reservedText)
		{
			reservedText->setTextNum(recordNum);
			reservedText->setText(internationalize("LabelText.NetWeight"));
			reservedText->setDBAccess(dbAccess);
			reservedText->saveRecord();
			if (Debug::isTracing("GenericLabelText"))
				Debug::trace("GenericLabelText", HobString("vDR: Creating label text #") + HobString(Integer::toString(recordNum)) + HobString(" : ") + reservedText->getText());
			++updateCount;

			Configuration::getInstance()->setLabelText(reservedText->getTextNum(), reservedText);
		}
	}
	else
		delete reservedText;

    //  Generic Text 5 - Unit Price
	recordNum = 5;
	reservedText = findLabelText(recordNum, dbAccess);
	if (!reservedText)
	{
		reservedText = new GenericLabelText();
		if (reservedText)
		{
			reservedText->setTextNum(recordNum);
			reservedText->setText(internationalize("LabelText.UnitPrice"));
			reservedText->setDBAccess(dbAccess);
			reservedText->saveRecord();
			if (Debug::isTracing("GenericLabelText"))
				Debug::trace("GenericLabelText", HobString("vDR: Creating label text #") + HobString(Integer::toString(recordNum)) + HobString(" : ") + reservedText->getText());
			++updateCount;

			Configuration::getInstance()->setLabelText(reservedText->getTextNum(), reservedText);
		}
	}
	else
		delete reservedText;


    //  Generic Text 6 - Total Price
	recordNum = 6;
	reservedText = findLabelText(recordNum, dbAccess);
	if (!reservedText)
	{
		reservedText = new GenericLabelText();
		if (reservedText)
		{
			reservedText->setTextNum(recordNum);
			reservedText->setText(internationalize("LabelText.TotalPrice"));
			reservedText->setDBAccess(dbAccess);
			reservedText->saveRecord();
			if (Debug::isTracing("GenericLabelText"))
				Debug::trace("GenericLabelText", HobString("vDR: Creating label text #") + HobString(Integer::toString(recordNum)) + HobString(" : ") + reservedText->getText());
			++updateCount;

			Configuration::getInstance()->setLabelText(reservedText->getTextNum(), reservedText);
		}
	}
	else
		delete reservedText;


	return updateCount;
}
