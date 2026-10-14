/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SoftkeyBasics.h"
#include "SoftkeyStyle.h"
#include "SoftkeyTags.h"
#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBSoftKeyStylesRM.h"
#include "DBSoftKeyStyle.h"

#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SoftkeyStyle::dataType = 0;
std::string SoftkeyStyle::dataTypeName("SoftkeyStyle"); 
#endif

std::list<SoftkeyStyleFieldOffsets> SoftkeyStyle::browserFieldList, SoftkeyStyle::softkeyGroupList, SoftkeyStyle::evolFieldList;


SoftkeyStyle::SoftkeyStyle(DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	dbAccess = access;
}


SoftkeyStyle::SoftkeyStyle(const char *data, int len, int formatType)
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
			std::list<SoftkeyStyleFieldOffsets>::iterator fieldIter = browserFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != browserFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);

#if 0
				if (token.length())
				{
					if (isFieldQuoted(offset))
						token.unQuoteForCSV();

					updateField(offset, token);
				}
#else
				updateField(offset, token);
#endif
				fieldIter++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			SoftkeyStyleTags softkeyStyleTags;

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
					temp = token->getTag();
					offset = (temp.size()) ? softkeyStyleTags.getOffset(temp) : 0;
					Debug::trace("SoftkeyStyle", HobString("SoftkeyStyle(data,len,format) token: ") + temp + HobString("  offset:") + HobString(Integer::toString(offset)) + HobString("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData());
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

SoftkeyStyle::SoftkeyStyle(DBSoftKeyStyle *dbSoftkeyRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	dbAccess = access;

	if (dbSoftkeyRec)
	{
		myDbRecord = dbSoftkeyRec;

		styleNumber = (HobRecordNum) Long::valueOf(dbSoftkeyRec->getNumber());

		textVerticalAlignment	= (VerticalAlignment) dbSoftkeyRec->getDefaultTextLocation();
		if (!dbSoftkeyRec->isDefaultTextColorNull())
			textColor = Hex::toHTMLColorString((long) dbSoftkeyRec->getDefaultTextColor());
		if (!dbSoftkeyRec->isDefaultGrayedOutTextColorNull())
			grayedOutTextColor = Hex::toHTMLColorString((long) dbSoftkeyRec->getDefaultGrayedOutTextColor());
		if (!dbSoftkeyRec->isDefaultBackgroundColorNull())
			backgroundColor = Hex::toHTMLColorString((long) dbSoftkeyRec->getDefaultBackgroundColor());
		backgroundImage			= dbSoftkeyRec->getDefaultBackgroundImage();
		imageDirectory			= dbSoftkeyRec->getImagesDirectory();
		foregroundImageLocation = (AreaPosition) dbSoftkeyRec->getDefaultForegroundImageLocation();
	}
	show();
}

SoftkeyStyle::~SoftkeyStyle()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}

void SoftkeyStyle::init()
{
	styleNumber				= RECORD_NOT_FOUND;
	textVerticalAlignment	= VA_Center;
	textColor				= "";
	grayedOutTextColor		= "";
	backgroundColor			= "";
	backgroundImage			= "";
	imageDirectory			= "";
	foregroundImageLocation = AP_Center;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBSoftKeyStyle *)NULL;

	initFieldLists();
}

void SoftkeyStyle::initFieldLists()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetSKSStyleNumber);
		browserFieldList.push_back(fieldOffsetSKSTextVerticalAlignment);
		browserFieldList.push_back(fieldOffsetSKSTextColor);
		browserFieldList.push_back(fieldOffsetSKSGrayedOutTextColor);
		browserFieldList.push_back(fieldOffsetSKSBackgroundColor);
		browserFieldList.push_back(fieldOffsetSKSBackgroundImage);
		browserFieldList.push_back(fieldOffsetSKSImageDirectory);
		browserFieldList.push_back(fieldOffsetSKSForegroundImageLocation);
	}
	if (softkeyGroupList.empty())
	{
		softkeyGroupList.push_back(fieldOffsetSKSTextVerticalAlignment);
		softkeyGroupList.push_back(fieldOffsetSKSTextColor);
		softkeyGroupList.push_back(fieldOffsetSKSGrayedOutTextColor);
		softkeyGroupList.push_back(fieldOffsetSKSBackgroundColor);
		softkeyGroupList.push_back(fieldOffsetSKSBackgroundImage);
		softkeyGroupList.push_back(fieldOffsetSKSForegroundImageLocation);
		softkeyGroupList.push_back(fieldOffsetSKSImageDirectory);
	}
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetSKSStyleNumber);
		evolFieldList.push_back(fieldOffsetSKSTextVerticalAlignment);
		evolFieldList.push_back(fieldOffsetSKSTextColor);
		evolFieldList.push_back(fieldOffsetSKSGrayedOutTextColor);
		evolFieldList.push_back(fieldOffsetSKSBackgroundColor);
		evolFieldList.push_back(fieldOffsetSKSBackgroundImage);
		evolFieldList.push_back(fieldOffsetSKSImageDirectory);
		evolFieldList.push_back(fieldOffsetSKSForegroundImageLocation);

		SoftkeyStyleTags softkeyStyleTags((FieldOffsetsList *)&evolFieldList);
	}
}

void SoftkeyStyle::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

DBSoftKeyStyle *SoftkeyStyle::getMyDbRecord()
{
	return myDbRecord;
}

bool SoftkeyStyle::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}


HobRecordNum SoftkeyStyle::getStyleNumber() const
{
	return styleNumber;
}
void SoftkeyStyle::setStyleNumber(HobRecordNum number)
{
	styleNumber = number;
}

VerticalAlignment SoftkeyStyle::getTextVerticalAlignment() const
{
	return textVerticalAlignment;
}
void SoftkeyStyle::setTextVerticalAlignment(VerticalAlignment position)
{
	textVerticalAlignment = position;
}

const HobString &SoftkeyStyle::getTextColor() const
{
	return textColor;
}
void SoftkeyStyle::setTextColor(const HobString &color)
{
	textColor = color;
}

const HobString &SoftkeyStyle::getGrayedOutTextColor() const
{
	return grayedOutTextColor;
}
void SoftkeyStyle::setGrayedOutTextColor(const HobString &color)
{
	grayedOutTextColor = color;
}

const HobString &SoftkeyStyle::getBackgroundColor() const
{
	return backgroundColor;
}
void SoftkeyStyle::setBackgroundColor(const HobString &color)
{
	backgroundColor = color;
}

const HobString &SoftkeyStyle::getBackgroundImage() const
{
	return backgroundImage;
}
void SoftkeyStyle::setBackgroundImage(const HobString &imageName)
{
	backgroundImage = imageName;
}

const HobString &SoftkeyStyle::getImageDirectory() const
{
	return imageDirectory;
}
void SoftkeyStyle::setImageDirectory(const HobString &dirName)
{
	imageDirectory = dirName;
}

AreaPosition SoftkeyStyle::getForegroundImageLocation() const
{
	return foregroundImageLocation;
}
void SoftkeyStyle::setForegroundImageLocation(AreaPosition location)
{
	foregroundImageLocation = location;
}


int SoftkeyStyle::updateField(int fieldCode, const HobString &token)
{
	if (Debug::isTracing("SoftkeyStyle"))
	{
		HobString myDebug("updateField() - fieldCode: ");
		myDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("SoftkeyStyle", myDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetSKSStyleNumber:
		{
			setStyleNumber((HobRecordNum)Long::valueOf(token));
			break;
		}
		case fieldOffsetSKSTextVerticalAlignment:
		{
			setTextVerticalAlignment((VerticalAlignment) Integer::valueOf(token));
			break;
		}
		case fieldOffsetSKSTextColor:
		{
			setTextColor(token);
			break;
		}
		case fieldOffsetSKSGrayedOutTextColor:
		{
			setGrayedOutTextColor(token);
			break;
		}
		case fieldOffsetSKSBackgroundColor:
		{
			setBackgroundColor(token);
			break;
		}
		case fieldOffsetSKSBackgroundImage:
		{
			setBackgroundImage(token);
			break;
		}
		case fieldOffsetSKSImageDirectory:
		{
			setImageDirectory(token);
			break;
		}
		case fieldOffsetSKSForegroundImageLocation:
		{
			setForegroundImageLocation((AreaPosition) Integer::valueOf(token));
			break;
		}
		default:
			break;
	}

	return 0;
}



void SoftkeyStyle::update(SoftkeyStyle *newSoftkey)
{
	if (!newSoftkey)
		return;

	Debug::trace("SoftkeyStyle", "Entering update ...");

	short loopCnt = 1;
	while (loopCnt < endSoftkeyStyleFieldOffsets)
	{
		if (newSoftkey->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetSKSStyleNumber:
					setStyleNumber(newSoftkey->getStyleNumber());
					break;
				case fieldOffsetSKSTextVerticalAlignment:
					setTextVerticalAlignment(newSoftkey->getTextVerticalAlignment());
					break;
				case fieldOffsetSKSTextColor:
					setTextColor(newSoftkey->getTextColor());
					break;
				case fieldOffsetSKSGrayedOutTextColor:
					setGrayedOutTextColor(newSoftkey->getGrayedOutTextColor());
					break;
				case fieldOffsetSKSBackgroundColor:
					setBackgroundColor(newSoftkey->getBackgroundColor());
					break;
				case fieldOffsetSKSBackgroundImage:
					setBackgroundImage(newSoftkey->getBackgroundImage());
					break;
				case fieldOffsetSKSImageDirectory:
					setImageDirectory(newSoftkey->getImageDirectory());
					break;
				case fieldOffsetSKSForegroundImageLocation:
					setForegroundImageLocation(newSoftkey->getForegroundImageLocation());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
	Debug::trace("SoftkeyStyle", "Leaving update ...");
}


void SoftkeyStyle::getFieldAsString(int fieldCode, HobString &result, int UNUSED(format)) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetSKSStyleNumber:
		{
			if (styleNumber > 0)
				result = Long::toString(styleNumber);
			break;
		}
		case fieldOffsetSKSTextVerticalAlignment:
			result = Integer::toString(textVerticalAlignment);
			break;
		case fieldOffsetSKSTextColor:
			result = textColor;
			break;
		case fieldOffsetSKSGrayedOutTextColor:
			result = grayedOutTextColor;
			break;
		case fieldOffsetSKSBackgroundColor:
			result = backgroundColor;
			break;
		case fieldOffsetSKSBackgroundImage:
			result = backgroundImage;
			break;
		case fieldOffsetSKSImageDirectory:
			result = imageDirectory;
			break;
		case fieldOffsetSKSForegroundImageLocation:
			result = Integer::toString(foregroundImageLocation);
			break;
		default:
			Debug::trace("SoftkeyStyle", "getFieldAsString():  Invalid field code!!!");
			break;
	}
	if (Debug::isTracing("SoftkeyStyle"))
	{
		HobString myDebug = "getFieldAsString():  fieldCode = ";
		myDebug.append(Integer::toString(fieldCode)).append("   result = [");
		myDebug.append(result).append("]");
		Debug::trace("SoftkeyStyle", myDebug);
	}
}

SoftkeyStyle *SoftkeyStyle::find(HobRecordNum styleNum, DBAccess *dbAccess)
{
	SoftkeyStyle *readSoftkeyStyle = (SoftkeyStyle *)NULL;

	if (styleNum <= 0 || !dbAccess)
		return readSoftkeyStyle;

	DBSoftKeyStylesRM *dbSoftkeyStylesRM = dbAccess->getDBSoftKeyStylesRM();
	if (!dbSoftkeyStylesRM)
		return readSoftkeyStyle;

	HobString num(Long::toString(styleNum));
	try
	{
		DBSoftKeyStyle *dbSoftkeyStyleRec = dbSoftkeyStylesRM->find(num);
		if (dbSoftkeyStyleRec)
		{
			Debug::trace("SoftkeyStyle", "find() - found the record in the db");
			readSoftkeyStyle = new SoftkeyStyle(dbSoftkeyStyleRec, dbAccess);
			if (!readSoftkeyStyle)
			{
				Debug::trace("SoftkeyStyle", "find() - But I can't create a new SoftkeyStyle object!");
				delete dbSoftkeyStyleRec;
			}
			else
			{
				if (Debug::isTracing("SoftkeyStyle"))
				{
					HobString myDebug("find() - created the hfc SoftkeyStyle object - SoftkeyStyle: ");
					myDebug.append(Hex::toString((long long) readSoftkeyStyle)).append("   styleNum: ").append(num);
					Debug::trace("SoftkeyStyle", myDebug);
				}
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "SoftkeyStyle::find() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}

	return readSoftkeyStyle;
}

int SoftkeyStyle::deleteRecord()
{
	int recordsDeleted = 0;

	if (myDbRecord && dbAccess)
	{
		DBSoftKeyStylesRM *dbSoftkeysRM = dbAccess->getDBSoftKeyStylesRM();
		if (dbSoftkeysRM)
		{
			recordsDeleted = dbSoftkeysRM->deleteRecord(myDbRecord);
		}
		else
		{
			Debug::trace("SoftkeyStyle", "Cannot Delete this softkey!");
		}
	}
	return recordsDeleted;
}

void SoftkeyStyle::saveRecord()
{
	if (!dbAccess)
		return;

	HobString msgDebug, msgDebug2;
	bool saveDebug = Debug::isTracing("SoftkeyStyle.save");

	Debug::trace("SoftkeyStyle", "Entering saveRecord ...");

	DBSoftKeyStylesRM *dbSoftkeysRM = dbAccess->getDBSoftKeyStylesRM();
	if (!dbSoftkeysRM)
		return;

	if (!myDbRecord)
	{
		Debug::trace("SoftkeyStyle", "saveRecord() - don't have a db record, creating it...");
		myDbRecord = dbSoftkeysRM->createRecord(Long::toString(styleNumber));
	}

	if (myDbRecord)
	{
		if (Debug::isTracing("SoftkeyStyle"))
		{
			Debug::trace("SoftkeyStyle", "Starting to save fields to db record ...");
			const char *z = serialize(BrowserFormat);
			msgDebug = "Before saving fields to db record ... ";
			msgDebug.append(z);
			Debug::trace("SoftkeyStyle", msgDebug);
		}

		//	Plug the changes back into the dbSoftkey Record.
		myDbRecord->setNumber(Long::toString(styleNumber));
		myDbRecord->setDefaultTextLocation(textVerticalAlignment);

		if (textColor == "")
			myDbRecord->setDefaultTextColorNull();
		else
			myDbRecord->setDefaultTextColor(Hex::longValueOf(textColor));

		if (grayedOutTextColor == "")
			myDbRecord->setDefaultGrayedOutTextColorNull();
		else
			myDbRecord->setDefaultGrayedOutTextColor(Hex::longValueOf(grayedOutTextColor));

		if (backgroundColor == "")
			myDbRecord->setDefaultBackgroundColorNull();
		else
			myDbRecord->setDefaultBackgroundColor(Hex::longValueOf(backgroundColor));

		myDbRecord->setDefaultBackgroundImage(backgroundImage);
		myDbRecord->setImagesDirectory(imageDirectory);
		myDbRecord->setDefaultForegroundImageLocation(foregroundImageLocation);

		//	Now save the SoftkeyStyle record to the database.
		if (saveDebug)
		{
			msgDebug = "Before saving to db ...  styleNumber = ";
			msgDebug2 = Long::toString(styleNumber);
			msgDebug.append(msgDebug2);
			Debug::trace("SoftkeyStyle.save", msgDebug);
		}

		try
		{
			dbSoftkeysRM->saveRecord(myDbRecord);
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "SoftkeyStyle::saveRecord() ********** database exception **********");
			DBException::Type t = dbe.getType();
			if (Debug::isTracing("DBAccess.exception"))
			{
				msgDebug = "   DBException: type: ";
				msgDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
				Debug::trace("DBAccess.exception", msgDebug);
			}
		}
		if (saveDebug)
		{
			msgDebug = "After saving to db ...  styleNumber = ";
			msgDebug.append(msgDebug2);
			Debug::trace("SoftkeyStyle.save", msgDebug);
		}
	}
}

const char *SoftkeyStyle::serialize(int format) const
{
	serializedData = "";
	int loopCount = 0;

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<SoftkeyStyleFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			HobString fieldData = "";
			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				if (loopCount == 0)
					++loopCount;
				else
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, BrowserFormat);
#if 0
				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();
#endif
				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case SoftkeyGroupFormat:
		{
			HobString fieldData = "";
			fieldIter = softkeyGroupList.begin();
			while (fieldIter != softkeyGroupList.end())
			{
				if (loopCount == 0)
					++loopCount;
				else
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, BrowserFormat);
				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			SoftkeyStyleTags softkeyStyleTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<SoftkeyStyleFieldOffsets>::iterator fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = softkeyStyleTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

					serializedData.append(tag);
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}

		default:
		{
			serializedData = "SoftkeyStyle::serialize - unknown format: ";
			serializedData.append(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
		}
	}
	if (Debug::isTracing("SoftkeyStyle"))
	{
		HobString myDebug = "serialize():  serializedData = [";
		myDebug.append(serializedData).append("]");
		Debug::trace("SoftkeyStyle", myDebug);
	}
	return serializedData.c_str();
}


void SoftkeyStyle::show()
{
	if (Debug::isTracing("SoftkeyStyle"))
	{
		HobString myDebug("show():");
		myDebug.append("\n*******************************************************");
		myDebug.append("\n   styleNumber:     ").append(Long::toString(styleNumber));
		myDebug.append("\n   textVAlign:      ").append(Integer::toString(textVerticalAlignment));
		myDebug.append("\n   textColor:       ").append(textColor);
		myDebug.append("\n   grayedTextColor: ").append(grayedOutTextColor);
		myDebug.append("\n   bgColor:         ").append(backgroundColor);
		myDebug.append("\n   bgImage:         ").append(backgroundImage);
		myDebug.append("\n   imageDir:        ").append(imageDirectory);
		myDebug.append("\n   fgImageLoc:      ").append(Integer::toString(foregroundImageLocation));
		myDebug.append("\n*******************************************************");
		Debug::trace("SoftkeyStyle", myDebug);
	}
}


// static
void SoftkeyStyle::verifyReservedRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("SoftkeyStyle", "verifying reserved SoftkeyStyles - dbAccess is NULL!");
		return;
	}

	Debug::trace("SoftkeyStyle", "verifying all reserved SoftkeyStyles ...");
	SoftkeyStyle *reservedSoftkeyStyle = (SoftkeyStyle *)NULL;
	HobRecordNum styleNum;

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999001 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/ 
	styleNum = Reserved_ProductGrid;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999001: Flash Keys");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999002 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/ 
	styleNum = Reserved_StartScreenButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999002: Programmable Buttons Main Screen");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999003 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_RunScreenButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999003: Programmable Buttons Run Screen");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999004 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/ 
	styleNum = Reserved_PresetTareGrid;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999004: Flash Keys");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("6699FF");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}
	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999006 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_CategoryButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999006: ESS Programmable Buttons Group");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = (SoftkeyStyle *)NULL;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999007 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_MostCommonlyUsedItems;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999007: Most Commonly Used Items");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Bottom);
			reservedSoftkeyStyle->setTextColor("FFFFFF");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setBackgroundImage("softkeys/blueGlass.png");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999008 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = ReservedSK_SelfservCategoryButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999008: Category Buttons");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999012 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_StartScreenCutTestOff;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999012: Prog Buttons for Start a run screen when Cutting test off");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}
	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999013 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_RunScreenCutTestOff;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999013: Prog Buttons for Run screen when Cutting test off");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}
	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

	// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999014 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_StartScreenCutTestOn;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999014: Prog Buttons for Start a run screen when Cutting test on");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}
	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999015 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_RunScreenCutTestOn;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999015: Prog Buttons for Run screen when Cutting test on");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}
	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999016 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	//SELF SERVE -> Originally was 999012-999013
	styleNum = ReservedSK_SelfservAZCategoryButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999016: SelfservAZCategoryButtons");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999017 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	//SELF SERVE -> Originally was 999012-999013 pre Access Port
	styleNum = ReservedSK_SelfservCategoryResultsButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999017: SelfservCategoryResultsButtons");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}
	
// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999018 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = ReservedSK_SelfservProductGrid;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999018: SelfservProductGrid");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}
	
	// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
	// 999020 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_FixedRowColGrid;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999020: FixedRowColGrid");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

    if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

    // SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
	// 999025 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/ 
	styleNum = Reserved_ForcedCameraOverride;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999025: ForcedCameraOverride");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("6699FF");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}
	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

	// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999030 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_RunScreenNumericKeypadButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999030: Numeric Keypad Buttons");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			// FT-918: this style should be same as 999003 (center pane)
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}
	
// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999031 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_BuildAMealScreenButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999031: Build-A-Meal Buttons");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Top);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_LowerCenter);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
// 999032 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_BuildAMealSideItemButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999032: Build-A-Meal Side Items");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Top);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_LowerCenter);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

	// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
    // 999033 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_MeatSpecSheetScreeenButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999033: Meat Specification Sheet Buttons");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Top);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_LowerCenter);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

	// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
    // 999034 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_MeatSpecSheetSideItemButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999034: Meat Specification Sheet Side Items");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Top);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_LowerCenter);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

	// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
    // 999035 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_RunScreenCutTestOnOptionOne;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999035: Prog Buttons for Run screen when Cutting test on for Option 1");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("FFFFFF");					//	White
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");			//	Gray
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("B0B0B0");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}
	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

	// CAMERA
	// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
	// 999040 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_CameraSystemPredictions;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 999040: Reserved_CameraSystemPredictions");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_Center);
			reservedSoftkeyStyle->setBackgroundColor("");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}

	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}

	// SSNUM  | SSDTEXTL | SSDTEXTC | SSDGRTXTC | SSDFIMGL | SSDBGCLR | SSDBGIMG | SSIMGDIR
	// 888050 |        1 |        0 |   8421504 |        0 | 11579568 |          | images/
	styleNum = Reserved_FreshServeButtons;
	reservedSoftkeyStyle = SoftkeyStyle::find(styleNum, dbAccess);
	if (!reservedSoftkeyStyle)
	{
		Debug::trace("SoftkeyStyle", "creating reserved SoftkeyStyle 888050: FixedRowColGrid");
		reservedSoftkeyStyle = new SoftkeyStyle(dbAccess);
		if (reservedSoftkeyStyle)
		{
			reservedSoftkeyStyle->setStyleNumber(styleNum);
			reservedSoftkeyStyle->setTextVerticalAlignment(VA_Center);
			reservedSoftkeyStyle->setTextColor("000000");
			reservedSoftkeyStyle->setGrayedOutTextColor("808080");
			reservedSoftkeyStyle->setForegroundImageLocation(AP_UpperLeft);
			reservedSoftkeyStyle->setBackgroundColor("");
			reservedSoftkeyStyle->setBackgroundImage("");
			reservedSoftkeyStyle->setImageDirectory("images/");
			reservedSoftkeyStyle->saveRecord();
		}
	}
	if (reservedSoftkeyStyle)
	{
		delete reservedSoftkeyStyle;
		reservedSoftkeyStyle = 0;
	}
}
