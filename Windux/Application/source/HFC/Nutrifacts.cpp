/* Evolution Scale Project
 * COPYRIGHT (c) 2001.	All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Nutrifacts.h"
#include "NutrifactsDataTags.h"
#include "TextLinks.h"
#include "LabelTypeManager.h"
#include "LabelType.h"
#include "FormRecord.h"
#include "Configuration.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"		//added ats 10012002
#include "HobTokenTagged.h"
#include "HobList.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBNutritionsRM.h"
#include "DBNutrition.h"

#include "DBLabelsRM.h"
#include "DBProductsRM.h"
#include "DBROMLabelsRM.h"

#include "util/e_string/utilstring.h"
#include "Long.h"
#include "Hex.h"
#include "Integer.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Nutrifacts::dataType = 0;
std::string Nutrifacts::dataTypeName("Nutrifacts"); 
#endif

list<NutrifactsFieldOffsets> Nutrifacts::uiFieldList;
list<NutrifactsFieldOffsets> Nutrifacts::evolFieldList;


Nutrifacts::Nutrifacts()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

Nutrifacts::Nutrifacts(const char *data, int len, int formatType)
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
			std::list<NutrifactsFieldOffsets>::iterator fieldIter = uiFieldList.begin();
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
			if (!isReceived(fieldOffsetNFDeptNum))
			{
				setReceived(fieldOffsetNFDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			NutrifactsDataTags nutriDataTags;			

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
					offset = (temp.size()) ? nutriDataTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						temp = token->getData();

						updateField(offset, temp);
					}
				}	
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetNFDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetNFDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}	

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

Nutrifacts::Nutrifacts(DBNutrition *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Nutrifacts.ctor", string("Nutrifacts(DBNutrition, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		deptNum = Long::valueOf(dbRec->getDeptNumber());
		textNum = Long::valueOf(dbRec->getNumber());
		if(!dbRec->isNutritionColumn1Null())
			column1Data = dbRec->getNutritionColumn1();
		if(!dbRec->isNutritionColumn2Null())
			column2Data = dbRec->getNutritionColumn2();
		if(!dbRec->isNutritionColumn3Null())
			column3Data = dbRec->getNutritionColumn3();
		if(!dbRec->isNutritionColumn4Null())
			column4Data = dbRec->getNutritionColumn4();

		errorCode = RECORD_OK;
		//testIfUsedByMany();
	}
}


Nutrifacts::~Nutrifacts()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;

	Debug::trace("Nutrifacts.dtor", "    Nutrifacts: " + ptrtostring(this) + "  num " + longtostring(textNum) + "  deptNum " + longtostring(deptNum));
}


// private
void Nutrifacts::init()
{
	deptNum = 0;
	textNum = RECORD_NOT_FOUND;
	column1Data = "";
	column2Data = "";
	column3Data = "";
	column4Data = "";

	myDbRecord = (DBNutrition *)NULL;

	errorCode = RECORD_NOT_FOUND;
	usedByMany = false;

	initFieldLists();
}

// static
void Nutrifacts::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetNFTextNum);
		uiFieldList.push_back(fieldOffsetNFColumn1Data);
		uiFieldList.push_back(fieldOffsetNFColumn2Data);
		uiFieldList.push_back(fieldOffsetNFColumn3Data);
		uiFieldList.push_back(fieldOffsetNFColumn4Data);
	}
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetNFDeptNum);
		evolFieldList.push_back(fieldOffsetNFTextNum);
		evolFieldList.push_back(fieldOffsetNFColumn1Data);
		evolFieldList.push_back(fieldOffsetNFColumn2Data);
		evolFieldList.push_back(fieldOffsetNFColumn3Data);
		evolFieldList.push_back(fieldOffsetNFColumn4Data);

		NutrifactsDataTags nutrifactsDataTags((FieldOffsetsList *)&evolFieldList);
	}
}


HobRecordNum Nutrifacts::getDeptNum() const
{
	return deptNum;
}
void Nutrifacts::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

HobRecordNum Nutrifacts::getTextNum() const
{
	return textNum;
}
void Nutrifacts::setTextNum(HobRecordNum num)
{
	textNum = num;
}

HobString Nutrifacts::getColumn1Data() const
{
	return column1Data;
}
void Nutrifacts::setColumn1Data(const HobString &data)
{
	column1Data = data;
}

HobString Nutrifacts::getColumn2Data() const
{
	return column2Data;
}
void Nutrifacts::setColumn2Data(const HobString &data)
{
	column2Data = data;
}

HobString Nutrifacts::getColumn3Data() const
{
	return column3Data;
}
void Nutrifacts::setColumn3Data(const HobString &data)
{
	column3Data = data;
}

HobString Nutrifacts::getColumn4Data() const
{
	return column4Data;
}
void Nutrifacts::setColumn4Data(const HobString &data)
{
	column4Data = data;
}

HobErrorCode Nutrifacts::getErrorCode() const
{
	return errorCode;
}
void Nutrifacts::setErrorCode(HobErrorCode code)
{
	errorCode = code;
}


bool Nutrifacts::isUsedByMany() const
{
	return usedByMany;
}
void Nutrifacts::setUsedByMany(bool usedByMany)
{
	this->usedByMany = usedByMany;
}

void Nutrifacts::testIfUsedByMany()
{
	TextLinks *textLinks = new TextLinks(dbAccess, psttNutrifacts, myDbRecord);
	if (textLinks)
	{
		DBQuery<DBProduct>* linkList = textLinks->getLinkList();
		if (linkList)
		{
			if (linkList->size() > 1)
				setUsedByMany(true);
			else
				setUsedByMany(false);
		}
		delete textLinks;
	}
}


Nutrifacts *Nutrifacts::convertOldToNewData(DBAccess *dbAccess, const char *data, int len)
{
	LabelType *label = 0;
	HobString dataTag, labelTag, dataStr;
	LabelType::formContainer* formsList;
	LabelType::formContainerIterator labelIter;
	FieldType fieldType;

	HobString oldData(data, len);
	HobTokenizerUS tok(&oldData, Sub);
	oldData = "";
	HobTokenTagged *token = dynamic_cast<HobTokenTagged *> (HobToken::create(oldData, HobTaggedToken));

	Nutrifacts *nutrifacts = new Nutrifacts();

	while (tok.next(token) != -1)
	{
		dataTag = token->getTag();
		dataStr = token->getData();

		if (dataTag.compare("nt") == 0)
		{
			label = LabelType::findRecord((HobRecordNum)Long::valueOf(dataStr.c_str()), Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum), dbAccess);
			if (label)
			{
				formsList = label->getFormsList();
				labelIter = formsList->begin();
				break;
			}
		}
	}
	if (label && !formsList->empty())
	{
		while ((tok.next(token) != -1) && (labelIter != formsList->end()))
		{
			dataTag = token->getTag();
			dataStr = token->getData();

			if (isDataTagV1V2V3(dataTag))
			{
				while (labelIter != formsList->end())
				{
					fieldType = (*labelIter)->getFieldType();
					if (isFieldNutriColumn(fieldType))
					{
						labelTag = (*labelIter)->getTag();
						switch (fieldType)
						{
						case NUTRI_COLUMN_1:
							nutrifacts->getColumn1Data().append(labelTag);
							nutrifacts->getColumn1Data().append(dataStr);
							break;
						case NUTRI_COLUMN_2:
							nutrifacts->getColumn2Data().append(labelTag);
							nutrifacts->getColumn2Data().append(dataStr);
							break;
						case NUTRI_COLUMN_3:
							nutrifacts->getColumn3Data().append(labelTag);
							nutrifacts->getColumn3Data().append(dataStr);
							break;
						case NUTRI_COLUMN_4:
							nutrifacts->getColumn4Data().append(labelTag);
							nutrifacts->getColumn4Data().append(dataStr);
							break;
						default:
							break;
						}
						break;
					}
					labelIter++;
				}
			}
			labelIter++;
		}
	}
	return nutrifacts;
}

bool Nutrifacts::isDataTagV1V2V3(HobString& dataTag)
{
	bool returnValue = false;

	if (dataTag.compare("V1") == 0)
		returnValue = true;
	else if (dataTag.compare("V2") == 0)
		returnValue = true;
	else if (dataTag.compare("V3") == 0)
		returnValue = true;

	return returnValue;
}

bool Nutrifacts::isFieldNutriColumn(int fieldType)
{
	bool returnValue = false;

	if (fieldType == NUTRI_COLUMN_1)
		returnValue = true;
	else if (fieldType == NUTRI_COLUMN_2)
		returnValue = true;
	else if (fieldType == NUTRI_COLUMN_3)
		returnValue = true;
	else if (fieldType == NUTRI_COLUMN_4)
		returnValue = true;

	return returnValue;
}


void Nutrifacts::buildNutrifactsData(HobString& dbNutrifactsRec)
{
	if (dbNutrifactsRec.length())
	{
		HobString temp(dbNutrifactsRec.c_str());
		HobTokenizerCSV block(&temp, UnitSeparator);
		HobString token;

		while (block.next(&token) != -1)
		{
			parseNutridataBlock(token);
		}
	}
}

void Nutrifacts::parseNutridataBlock(HobString dataBlock)
{
	HobString column(dataBlock, 0, 2);
	HobString data(dataBlock, 2);

	if (column.compare(NutrifactsDataTags::nutriDataColumn1))
	{
		setColumn1Data(data);
	}
	else if (column.compare(NutrifactsDataTags::nutriDataColumn2))
	{
		setColumn2Data(data);
	}
	else if (column.compare(NutrifactsDataTags::nutriDataColumn3))
	{
		setColumn3Data(data);
	}
	else if (column.compare(NutrifactsDataTags::nutriDataColumn4))
	{
		setColumn4Data(data);
	}
}



bool Nutrifacts::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("Nutrifacts"))
		Debug::trace("Nutrifacts", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetNFDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetNFTextNum:
			setTextNum(Long::valueOf(newValue));
			break;
		case fieldOffsetNFColumn1Data:
			setColumn1Data(newValue);
			break;
		case fieldOffsetNFColumn2Data:
			setColumn2Data(newValue);
			break;
		case fieldOffsetNFColumn3Data:
			setColumn3Data(newValue);
			break;
		case fieldOffsetNFColumn4Data:
			setColumn4Data(newValue);
			break;
	}

	return true;
}

// Virtual override of updateText method in Text class.
void Nutrifacts::updateRecord(const Nutrifacts *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("Nutrifacts"))
	{
		Debug::trace("Nutrifacts", string("updateRecord() - this num ") + longtostring(textNum) + string("  this DeptNum ") + longtostring(deptNum));
		Debug::trace("Nutrifacts", string("updateRecord() - passed in num ") + longtostring(newData->getTextNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	short loopCnt = 1;
	while (loopCnt < endNutrifactsFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetNFDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetNFTextNum:
					setTextNum(newData->getTextNum());
					break;
				case fieldOffsetNFColumn1Data:
					setColumn1Data(newData->getColumn1Data());
					break;
				case fieldOffsetNFColumn2Data:
					setColumn2Data(newData->getColumn2Data());
					break;
				case fieldOffsetNFColumn3Data:
					setColumn3Data(newData->getColumn3Data());
					break;
				case fieldOffsetNFColumn4Data:
					setColumn4Data(newData->getColumn4Data());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool Nutrifacts::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetNFColumn1Data:
		case fieldOffsetNFColumn2Data:
		case fieldOffsetNFColumn3Data:
		case fieldOffsetNFColumn4Data:
			return true;
			break;
		default:
			break;
	}
	return false;
}

void Nutrifacts::getFieldAsString(int fieldOffset, HobString &result) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetNFDeptNum:
			result = longtostring(deptNum);
			break;
		case fieldOffsetNFTextNum:
			result = longtostring(textNum);
			break;
		case fieldOffsetNFColumn1Data:
			result = column1Data;
			break;
		case fieldOffsetNFColumn2Data:
			result = column2Data;
			break;
		case fieldOffsetNFColumn3Data:
			result = column3Data;
			break;
		case fieldOffsetNFColumn4Data:
			result = column4Data;
			break;
	}

	if (Debug::isTracing("Nutrifacts.getFieldAsString"))
		Debug::trace("Nutrifacts.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result);
}

const char *Nutrifacts::serialize(int format) const
{
	//	NOTE: We will not write nutrifacts data in the old Ultima/Quantum format.
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<NutrifactsFieldOffsets>::iterator fieldIter;
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

				getFieldAsString(*fieldIter, fieldData);
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
			NutrifactsDataTags nutridataTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = nutridataTags.getTag(*fieldIter);
				if (tag.size())
				{
					getFieldAsString(*fieldIter, fieldData);

					serializedData.append(tag); 
					serializedData.append(fieldData);
					serializedData.append(UnitSeparator);	
				}
				fieldIter++;
			}
			break;
		}

		default:
			serializedData = string("Nutrifacts::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


DBNutrition *Nutrifacts::getMyDbRecord() const
{
	return myDbRecord;
}

void Nutrifacts::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool Nutrifacts::isRecordValid() const
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int Nutrifacts::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)   //No need to delete if there is no record
	{
		DBNutritionsRM *dbRM = dbAccess->getDBNutritionsRM();
		if (dbRM)
		{
			Debug::trace("Nutrifacts", string("deleteRecord() - num ") + longtostring(textNum) + "  deptNum " + longtostring(deptNum));
			result = dbRM->deleteRecord(myDbRecord);
		}
	}
	return result;
}

void Nutrifacts::saveRecord()
{
	if (textNum > 0 && dbAccess)   //No need to save to the Database if the number is 0
	{
		DBNutritionsRM *dbNutritionsRM = dbAccess->getDBNutritionsRM();
		if (!dbNutritionsRM)
			return;

		if (!myDbRecord)
		{
			//	This must be a new record for the Database
			Debug::trace("Nutrifacts", HobString("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbNutritionsRM->createRecord();
		}

		if (myDbRecord)
		{
			myDbRecord->setDeptNumber(longtostring(deptNum));
			myDbRecord->setNumber(longtostring(textNum));

			if ( column1Data.empty() )
				myDbRecord->setNutritionColumn1Null();
			else
				myDbRecord->setNutritionColumn1(column1Data);
			if ( column2Data.empty() )
				myDbRecord->setNutritionColumn2Null();
			else
				myDbRecord->setNutritionColumn2(column2Data);
			if ( column3Data.empty() )
				myDbRecord->setNutritionColumn3Null();
			else
				myDbRecord->setNutritionColumn3(column3Data);
			if ( column4Data.empty() )
				myDbRecord->setNutritionColumn4Null();
			else
				myDbRecord->setNutritionColumn4(column4Data);

			try
			{
				dbNutritionsRM->saveRecord(myDbRecord);
				Debug::trace("Nutrifacts", string("saveRecord() - saved num ") + longtostring(textNum) + string("  deptNum ") + longtostring(deptNum));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Nutrifacts::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

// static
Nutrifacts *Nutrifacts::findRecord(HobRecordNum nutrifactsNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	Nutrifacts *rec = (Nutrifacts *)NULL;

	if (nutrifactsNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBNutritionsRM *dbRM = dbAccess->getDBNutritionsRM();
	if (!dbRM)
		return rec;

	try
	{
		DBNutrition *dbRec = dbRM->findNumber(longtostring(nutrifactsNum), longtostring(deptNum));
		if (dbRec)
		{
			rec = new Nutrifacts(dbRec, dbAccess);
			if (!rec)
				delete dbRec;
			else
			{
				if (Debug::isTracing("Nutrifacts"))
					Debug::trace("Nutrifacts", string("findRecord() - rec ptr: ") + ptrtostring(rec) + "  num " + longtostring(rec->getTextNum()) + "  deptNum " + longtostring(rec->getDeptNum()));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Nutrifacts::findRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}



/*
void Nutrifacts::buildFakeDBRecord()
{
	column1Data.append("SSzA(35g)");
	column1Data.append(Sub);
	column1Data.append("SPCA1");
	column1Data.append(Sub);
	column1Data.append("CalA120");
	column1Data.append(Sub);
	column1Data.append("CaFA0");
	column1Data.append(Sub);
	column1Data.append("FatA0g");
	column1Data.append(Sub);
	column1Data.append("FatP0%");
	column1Data.append(Sub);
	column1Data.append("SaFA0g");
	column1Data.append(Sub);
	column1Data.append("SaFP0%");
	column1Data.append(Sub);
	column1Data.append("ChoA0mg");
	column1Data.append(Sub);
	column1Data.append("ChoP0%");
	column1Data.append(Sub);
	column1Data.append("SodA0mg");
	column1Data.append(Sub);
	column1Data.append("SodP0%");
	column1Data.append(Sub);
	column1Data.append("PotA125mg");
	column1Data.append(Sub);
	column1Data.append("PotP4%");
	column1Data.append(Sub);
	column1Data.append("CarA29g");
	column1Data.append(Sub);
	column1Data.append("CarP10%");
	column1Data.append(Sub);
	column1Data.append("DiFA3g");
	column1Data.append(Sub);
	column1Data.append("DiFP12%");
	column1Data.append(Sub);
	column1Data.append("SugA8");
	column1Data.append(Sub);
	column1Data.append("ProA4g");
	column1Data.append(Sub);
	column1Data.append("ViAP0%");
	column1Data.append(Sub);
	column1Data.append("ViCP0%");
	column1Data.append(Sub);
	column1Data.append("ClcP0%");
	column1Data.append(Sub);
	column1Data.append("IrnP10%");
	column1Data.append(Sub);
	column1Data.append("ThiP10%");
	column1Data.append(Sub);
	column1Data.append("RibP30%");
	column1Data.append(Sub);
	column1Data.append("NiaP30%");
	column1Data.append(Sub);
	column1Data.append("VB6P30%");

	column2Data.append("SSzA(19g)");
	column2Data.append(Sub);
	column2Data.append("SPCA1");
	column2Data.append(Sub);
	column2Data.append("CalA70");
	column2Data.append(Sub);
	column2Data.append("CaFA0");
	column2Data.append(Sub);
	column2Data.append("FatA0g");
	column2Data.append(Sub);
	column2Data.append("FatP0%");
	column2Data.append(Sub);
	column2Data.append("SaFA0g");
	column2Data.append(Sub);
	column2Data.append("SaFP0%");
	column2Data.append(Sub);
	column2Data.append("ChoA0mg");
	column2Data.append(Sub);
	column2Data.append("ChoP0%");
	column2Data.append(Sub);
	column2Data.append("SodA200mg");
	column2Data.append(Sub);
	column2Data.append("SodP8%");
	column2Data.append(Sub);
	column2Data.append("PotA25mg");
	column2Data.append(Sub);
	column2Data.append("PotP1%");
	column2Data.append(Sub);
	column2Data.append("CarA17g");
	column2Data.append(Sub);
	column2Data.append("CarP6%");
	column2Data.append(Sub);
	column2Data.append("DiFA1g");
	column2Data.append(Sub);
	column2Data.append("DiFP4%");
	column2Data.append(Sub);
	column2Data.append("SugA6g");
	column2Data.append(Sub);
	column2Data.append("ProA1g");
	column2Data.append(Sub);
	column2Data.append("ViAP10%");
	column2Data.append(Sub);
	column2Data.append("ViCP15%");
	column2Data.append(Sub);
	column2Data.append("ClcP0%");
	column2Data.append(Sub);
	column2Data.append("IrnP6%");
	column2Data.append(Sub);
	column2Data.append("ThiP15%");
	column2Data.append(Sub);
	column2Data.append("RibP15%");
	column2Data.append(Sub);
	column2Data.append("NiaP15%");
	column2Data.append(Sub);
	column2Data.append("VB6P15%");

	column3Data.append("SSzA(27g)");
	column3Data.append(Sub);
	column3Data.append("SPCA1");
	column3Data.append(Sub);
	column3Data.append("CalA100");
	column3Data.append(Sub);
	column3Data.append("CaFA0");
	column3Data.append(Sub);
	column3Data.append("FatA0g");
	column3Data.append(Sub);
	column3Data.append("FatP0%");
	column3Data.append(Sub);
	column3Data.append("SaFA0g");
	column3Data.append(Sub);
	column3Data.append("SaFP0%");
	column3Data.append(Sub);
	column3Data.append("ChoA0mg");
	column3Data.append(Sub);
	column3Data.append("ChoP0%");
	column3Data.append(Sub);
	column3Data.append("SodA120mg");
	column3Data.append(Sub);
	column3Data.append("SodP5%");
	column3Data.append(Sub);
	column3Data.append("PotA30mg");
	column3Data.append(Sub);
	column3Data.append("PotP1%");
	column3Data.append(Sub);
	column3Data.append("CarA24g");
	column3Data.append(Sub);
	column3Data.append("CarP8%");
	column3Data.append(Sub);
	column3Data.append("DiFA1g");
	column3Data.append(Sub);
	column3Data.append("DiFP4%");
	column3Data.append(Sub);
	column3Data.append("SugA13g");
	column3Data.append(Sub);
	column3Data.append("ProA1g");
	column3Data.append(Sub);
	column3Data.append("ViAP10%");
	column3Data.append(Sub);
	column3Data.append("ViCP90%");
	column3Data.append(Sub);
	column3Data.append("ClcP0%");
	column3Data.append(Sub);
	column3Data.append("IrnP20%");
	column3Data.append(Sub);
	column3Data.append("ThiP20%");
	column3Data.append(Sub);
	column3Data.append("RibP20%");
	column3Data.append(Sub);
	column3Data.append("NiaP20%");
	column3Data.append(Sub);
	column3Data.append("VB6P20%");
}
*/
