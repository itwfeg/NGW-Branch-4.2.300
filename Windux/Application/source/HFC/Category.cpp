/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Category.h"
#include "CategoryTags.h"
#include "HfcError.h"
#include "Configuration.h"
#include "filesys/filename.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBCategoriesRM.h"
#include "DBCategory.h"

#include "MultiLanguageText.h"

#include "AllDepts.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Category::dataType = 0;
std::string Category::dataTypeName("Category"); 
#endif

list<CategoryFieldOffsets> Category::uiFieldList;
list<CategoryFieldOffsets> Category::evolFieldList;


Category::Category()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

Category::Category(const char *data, int len, int formatType)
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
			std::list<CategoryFieldOffsets>::iterator fieldIter = uiFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != uiFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				if (token.length())
				{
					token.unQuoteForCSV();
					updateField(offset, token);
				}
				fieldIter++;
			}

			// Only for the UI, if we didn't receive the deptNum with the data, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCategoryDeptNum))
			{
				setReceived(fieldOffsetCategoryDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			CategoryTags categoryTags;

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
					offset = (temp.size()) ? categoryTags.getOffset(temp) : 0;
					if (Debug::isTracing("Category.ctor"))
						Debug::trace("Category.ctor", string("Category(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData());
						temp = token->getData();
					}
				}
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetCategoryDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetCategoryDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			break;
	}
}

Category::Category(DBCategory *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Category.ctor", string("Category(DBCategory, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		deptNum = Long::valueOf(dbRec->getDeptNumber());
		categoryNum = Long::valueOf(dbRec->getNumber());
		parentCategoryNum = Long::valueOf(dbRec->getParentCategory());
		categoryName1 = dbRec->getName1();
		categoryName2 = dbRec->getName2();
		graphicName = dbRec->getImage();
		if (!dbRec->isBackgroundColorNull())
			backgroundColor = Hex::toHTMLColorString((long)dbRec->getBackgroundColor());
		if (!dbRec->isTextColorNull())
			textColor = Hex::toHTMLColorString((long)dbRec->getTextColor());
		if (!dbRec->isPlaylistNull())
			playlist = dbRec->getPlaylist();
		errorCode = RECORD_OK;
	}
}


Category::~Category()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("Category.dtor", "    Category: " + ptrtostring(this) + "  num " + longtostring(categoryNum) + "  deptNum " + longtostring(deptNum));

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void Category::init()
{
	deptNum = 0;
	categoryNum = (HobRecordNum) RECORD_NOT_FOUND;
	parentCategoryNum = RootCategoryNum;
	categoryName1 = "";
	categoryName2 = "";
	graphicName = "";
	backgroundColor = "FFFFFF";		//	White
	textColor = "000000";			//	Black
	playlist = "";

	dbAccess = 0;
	myDbRecord = 0;

	errorCode = RECORD_NOT_FOUND;

	initFieldLists();
}


// static
void Category::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetCategoryNum);
		uiFieldList.push_back(fieldOffsetParentCategoryNum);
		uiFieldList.push_back(fieldOffsetCategoryName1);
		uiFieldList.push_back(fieldOffsetCategoryName2);
		uiFieldList.push_back(fieldOffsetCategoryGraphicName);
		uiFieldList.push_back(fieldOffsetCategoryBgColor);
		uiFieldList.push_back(fieldOffsetCategoryTextColor);
		uiFieldList.push_back(fieldOffsetCategoryPlaylist);
	}
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetCategoryDeptNum);
		evolFieldList.push_back(fieldOffsetCategoryNum);
		evolFieldList.push_back(fieldOffsetParentCategoryNum);
		evolFieldList.push_back(fieldOffsetCategoryName1);
		evolFieldList.push_back(fieldOffsetCategoryName2);
		evolFieldList.push_back(fieldOffsetCategoryGraphicName);
		evolFieldList.push_back(fieldOffsetCategoryBgColor);
		evolFieldList.push_back(fieldOffsetCategoryTextColor);
		evolFieldList.push_back(fieldOffsetCategoryPlaylist);

		CategoryTags categoryTags((FieldOffsetsList *)&evolFieldList);
	}
}


HobRecordNum Category::getDeptNum() const
{
	return deptNum;
}
void Category::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}


HobRecordNum Category::getCategoryNum() const
{
	return categoryNum;
}
void Category::setCategoryNum(HobRecordNum num)
{
	if (num >= 0 && num <= 999999)
		categoryNum = num;
	else
		addError(HfcError::ERRORCODE_INVALIDRANGE, CategoryTags::categoryNum);
}


HobRecordNum Category::getParentCategoryNum() const
{
	return parentCategoryNum;
}
void Category::setParentCategoryNum(HobRecordNum parentNum)
{
	if (parentNum >= 0 && parentNum <= 999999)
		parentCategoryNum = parentNum;
	else
		addError(HfcError::ERRORCODE_INVALIDRANGE, CategoryTags::parentCategoryNum);
}


const HobString &Category::getCategoryName1() const
{
	return categoryName1;
}
void Category::setCategoryName1(const HobString &name)
{
	if (name != "Invalid")
		categoryName1 = name;
	else
		addError(HfcError::ERRORCODE_INVALIDRANGE, CategoryTags::categoryName1);
}


const HobString &Category::getCategoryName2() const
{
	return categoryName2;
}
void Category::setCategoryName2(const HobString &name)
{
	categoryName2 = name;
}


const HobString &Category::getGraphicName() const
{
	return graphicName;
}
void Category::setGraphicName(const HobString &name)
{
	graphicName = name;
}


const HobString &Category::getBackgroundColor() const
{
	return backgroundColor;
}
void Category::setBackgroundColor(const HobString &color)
{
	backgroundColor = color;
}


const HobString &Category::getTextColor() const
{
	return textColor;
}
void Category::setTextColor(const HobString &color)
{
	textColor = color;
}


const HobString &Category::getPlaylist() const
{
	return playlist;
}
void Category::setPlaylist(const HobString &list)
{
	playlist = list;
}


HobErrorCode Category::getErrorCode() const
{
	return errorCode;
}
void Category::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


// virtual, override from basic functionality in HfcObject
HobString Category::getMyKeyValue() const
{
	return HobString(Long::toString((long)categoryNum));
}

// virtual, override from basic functionality in HfcObject
EvolDeviceType Category::getMyEvolDevice() const
{
	return CategoryDev;
}


// Returns true if the field was updated, false if not
bool Category::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("Category"))
		Debug::trace("Category", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetCategoryDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCategoryNum:
			setCategoryNum(Long::valueOf(newValue));
			break;
		case fieldOffsetParentCategoryNum:
			setParentCategoryNum(Long::valueOf(newValue));
			break;
		case fieldOffsetCategoryName1:
			setCategoryName1(newValue);
			break;
		case fieldOffsetCategoryName2:
			setCategoryName2(newValue);
			break;
		case fieldOffsetCategoryGraphicName:
			setGraphicName(newValue);
			break;
		case fieldOffsetCategoryBgColor:
			setBackgroundColor(newValue);
			break;
		case fieldOffsetCategoryTextColor:
			setTextColor(newValue);
			break;
		case fieldOffsetCategoryPlaylist:
			setPlaylist(newValue);
			break;
		default:
			return false;
			break;
	}

	return true;
}

void Category::updateRecord(const Category *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("Category"))
	{
		Debug::trace("Category", string("updateRecord() - this num ") + longtostring(categoryNum) + string("  this DeptNum ") + longtostring(deptNum));
		Debug::trace("Category", string("updateRecord() - passed in num ") + longtostring(newData->getCategoryNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	short loopCnt = 1;
	while (loopCnt < endCategoryFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetCategoryDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetCategoryNum:
					setCategoryNum(newData->getCategoryNum());
					break;
				case fieldOffsetParentCategoryNum:
					setParentCategoryNum(newData->getParentCategoryNum());
					break;
				case fieldOffsetCategoryName1:
					setCategoryName1(newData->getCategoryName1());
					break;
				case fieldOffsetCategoryName2:
					setCategoryName2(newData->getCategoryName2());
					break;
				case fieldOffsetCategoryGraphicName:
					setGraphicName(newData->getGraphicName());
					break;
				case fieldOffsetCategoryBgColor:
					setBackgroundColor(newData->getBackgroundColor());
					break;
				case fieldOffsetCategoryTextColor:
					setTextColor(newData->getTextColor());
					break;
				case fieldOffsetCategoryPlaylist:
					setPlaylist(newData->getPlaylist());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool Category::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetCategoryName1:
		case fieldOffsetCategoryName2:
		case fieldOffsetCategoryGraphicName:
		case fieldOffsetCategoryPlaylist:
			return true;
			break;
		default:
			break;
	}
	return false;
}

void Category::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetCategoryDeptNum:
			result = longtostring(deptNum);
			break;
		case fieldOffsetCategoryNum:
			result = longtostring(categoryNum);
			break;
		case fieldOffsetParentCategoryNum:
			if (parentCategoryNum != RECORD_NOT_FOUND && parentCategoryNum != RootCategoryNum)
				result = longtostring(parentCategoryNum);
			else
				result = "";
			break;
		case fieldOffsetCategoryName1:
			result = categoryName1;
			break;
		case fieldOffsetCategoryName2:
			result = categoryName2;
			break;
		case fieldOffsetCategoryGraphicName:
		{
			if (format == BrowserFormat)
			{
				HobString tempName(graphicName);
				Filename file1(tempName);
				if (!tempName.empty() && file1.getFileNameExtension().empty())
					tempName.append(Configuration::getInstance()->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDEFAULTEXTENSION));
				result = tempName;
			}
			else
				result = graphicName;
			break;
		}
		case fieldOffsetCategoryBgColor:
			result = backgroundColor;
			break;
		case fieldOffsetCategoryTextColor:
			result = textColor;
			break;
		case fieldOffsetCategoryPlaylist:
			result = playlist;
			break;
		default:
			break;
	}

	if (Debug::isTracing("Category.getFieldAsString"))
		Debug::trace("Category.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *Category::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<CategoryFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			bool firstTime = true;
			HobString fieldData = "";
			fieldIter = uiFieldList.begin();
			while (fieldIter != uiFieldList.end())
			{
				if (firstTime)
					firstTime = false;
				else
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, format);
				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			CategoryTags categoryTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = categoryTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData, EvolutionTaggedFormat);

					if (*fieldIter != fieldOffsetParentCategoryNum || !fieldData.empty())
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

		default:
			serializedData = string("Category::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


DBCategory *Category::getMyDbRecord() const
{
	return myDbRecord;
}

void Category::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool Category::isRecordValid() const
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int Category::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBCategoriesRM *dbRM = dbAccess->getDBCategoriesRM();
		if (dbRM)
		{
			Debug::trace("Category", string("deleteRecord() - num ") + longtostring(categoryNum) + "  deptNum " + longtostring(deptNum));
			result = dbRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void Category::saveRecord()
{
	if (categoryNum > 0 && dbAccess)
	{
		DBCategoriesRM	*dbRM = dbAccess->getDBCategoriesRM();
		if (!dbRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("Category", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbCategoryRecord
			myDbRecord->setDeptNumber(longtostring(deptNum));
			myDbRecord->setNumber(longtostring(categoryNum));
			// If parent category num not set up, set this category to be a root-level category.
			// todo - Do we want to enforce here the rule that the user can't have more than 5 root categories???????????
			myDbRecord->setParentCategory(longtostring((parentCategoryNum == RECORD_NOT_FOUND || parentCategoryNum == 0L) ? RootCategoryNum : parentCategoryNum));
			myDbRecord->setName1(categoryName1);
			myDbRecord->setName2(categoryName2);
			myDbRecord->setImage(graphicName);

			if (backgroundColor == "")
				myDbRecord->setBackgroundColorNull();
			else
				myDbRecord->setBackgroundColor(Hex::longValueOf(backgroundColor));

			if (textColor == "")
				myDbRecord->setTextColorNull();
			else
				myDbRecord->setTextColor(Hex::longValueOf(textColor));

			if (playlist == "")
				myDbRecord->setPlaylistNull();
			else
				myDbRecord->setPlaylist(playlist);

			try
			{
				dbRM->saveRecord(myDbRecord);		//	Save record to the database
				Debug::trace("Category", string("saveRecord() - saved num ") + longtostring(categoryNum) + string("  deptNum ") + longtostring(deptNum));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Category::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

// static
Category *Category::findRecord(HobRecordNum categoryNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	Category *rec = 0;

	if (categoryNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBCategoriesRM	*dbRM = dbAccess->getDBCategoriesRM();
	if (!dbRM)
		return rec;

	try
	{
		DBCategory *dbRec = dbRM->findNumber(longtostring(categoryNum), longtostring(deptNum));
		if (dbRec)
		{
			rec = new Category(dbRec, dbAccess);
			if (!rec)
				delete dbRec;
			else
			{
				if (Debug::isTracing("Category"))
					Debug::trace("Category", string("findRecord() - rec ptr: ") + ptrtostring(rec) + "  num " + longtostring(rec->getCategoryNum()) + "  deptNum " + longtostring(rec->getDeptNum()));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Category::findRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}


// static
void Category::verifyReservedRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("Category", "verifyReservedRecords - no dbAccess passed in!");
		return;
	}

	bool traceOn = Debug::isTracing("Category");
	if (traceOn)
		Debug::trace("Category", "verifying all reserved Categories ...");

	AllDepts allDepts;
	allDepts.findAll(dbAccess);
	HobList deptNumList = allDepts.getDeptNumList();

	HobString deptNumStr = "0";
	long deptNum = 0;

	Category *rec = 0;
	hfc::MultiLanguageText internationalize;

	while (!deptNumStr.empty())
	{
		deptNum = Long::valueOf(deptNumStr);

		rec = Category::findRecord(RootCategoryNum, deptNum, dbAccess);
		if (!rec)
		{
			rec = new Category();
			if (rec)
			{
				rec->setDeptNum(deptNum);
				rec->setCategoryNum(RootCategoryNum);
				rec->setParentCategoryNum(0);
				rec->setCategoryName1(internationalize("Category.Master"));
				rec->setCategoryName2(internationalize("Category.Master", SecondaryLanguage));
				rec->setDBAccess(dbAccess);
				rec->saveRecord();
				if (traceOn)
					Debug::trace("Category", "creating reserved Category RootCategoryNum (999999)  deptNum " + deptNumStr);
			}
		}
		if (rec)
		{
			delete rec;
			rec = 0;
		}

		// Get the next dept record from the database.  If the next one is the default dept (Dept 0), skip it and get the next one, since we
		// automatically created the reserved records for dept 0 in the first pass through the while loop (since deptNumStr was initialized to "0").
		// If the database doesn't contain the default dept record, we still need the reserved records for the default dept (Dept 0).
		deptNumStr = "";
		if (deptNumList.next(deptNumStr) != -1 && deptNumStr == "0")
		{
			deptNumStr = "";
			deptNumList.next(deptNumStr);
		}
	}
}
