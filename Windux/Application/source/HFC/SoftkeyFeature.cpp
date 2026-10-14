/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SoftkeyFeature.h"
#include "SoftkeyTags.h"
#include "Configuration.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "Configuration.h"
#include "HobDateTime.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBSoftKeyFeaturesRM.h"
#include "DBSoftKeyFeature.h"

#include "AllDepts.h"

#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"
#include "Hex.h"

#include "Softkey.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SoftkeyFeature::dataType = 0;
std::string SoftkeyFeature::dataTypeName("SoftkeyFeature"); 
#endif

std::list<SoftkeyFeatureFieldOffsets> SoftkeyFeature::browserFieldList, SoftkeyFeature::softkeyGroupList, SoftkeyFeature::evolFieldList;


SoftkeyFeature::SoftkeyFeature(DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	dbAccess = access;
}


SoftkeyFeature::SoftkeyFeature(const char *data, int len, int formatType)
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
			std::list<SoftkeyFeatureFieldOffsets>::iterator fieldIter = browserFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != browserFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				updateField(offset, token);
				fieldIter++;
			}

			// Only for the UI, if we didn't receive the deptNum with the Class info, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetSKFDeptNum))
			{
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				setReceived(fieldOffsetSKFDeptNum, true);
			}

			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			SoftkeyFeatureTags softkeyFeatureTags;

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
					offset = (temp.size()) ? softkeyFeatureTags.getOffset(temp) : 0;
					Debug::trace("SoftkeyFeature", HobString("SoftkeyFeature(data,len,format) token: ") + temp + HobString("  offset:") + HobString(Integer::toString(offset)) + HobString("  data:") + token->getData());
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData());
					}
				}
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetSKFDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetSKFDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

SoftkeyFeature::SoftkeyFeature(DBSoftKeyFeature *dbFeatureRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	dbAccess = access;

	if (dbFeatureRec)
	{
		myDbRecord = dbFeatureRec;

		recordNumber = Long::valueOf(dbFeatureRec->getNumber());

		setDefaultHorizontalGridSize(dbFeatureRec->getDefaultHorizontalGridSize());
		setDefaultVerticalGridSize(dbFeatureRec->getDefaultVerticalGridSize());
		horizontalSpacing = dbFeatureRec->getDefaultHorizontalSpacing();
		verticalSpacing = dbFeatureRec->getDefaultVerticalSpacing();
		if ( !dbFeatureRec->isFixedRowColMethodNull() )
			fixedRowColMethod = (FixedRowColMethod)dbFeatureRec->getFixedRowColMethod();
		if (!dbFeatureRec->isDefaultButtonNull())
			defaultButton = dbFeatureRec->getDefaultButton();
		defaultGroupNumber = dbFeatureRec->getDefaultGroupNumber();
		styleNumber = dbFeatureRec->getSoftKeyStyleNumber();
		deptNum = Long::valueOf(dbFeatureRec->getDeptNumber());
	}
}

SoftkeyFeature::~SoftkeyFeature()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}

void SoftkeyFeature::init()
{
	//NOTE: in the VxWorks Wrapper code these defaults were used!
	//defaultHGridSize = 6;
	//defaultVGridSize = 4;
	//horizontalSpacing = 1;
	//verticalSpacing = 1;
	
	recordNumber = RECORD_NOT_FOUND;
	defaultHGridSize = 1;
	defaultVGridSize = 1;
	horizontalSpacing = 0;
	verticalSpacing = 0;
	fixedRowColMethod = FRCM_None;
	defaultButton = 0;
	defaultGroupNumber = RECORD_NOT_FOUND;
	styleNumber = RECORD_NOT_FOUND;
	deptNum = 0;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBSoftKeyFeature *)NULL;

	initFieldLists();
}

void SoftkeyFeature::initFieldLists()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetSKFNumber);
		browserFieldList.push_back(fieldOffsetSKFDefSoftkeyGroup);
		browserFieldList.push_back(fieldOffsetSKFSoftkeyStyle);
		browserFieldList.push_back(fieldOffsetSKFHorizontalGridSize);
		browserFieldList.push_back(fieldOffsetSKFVerticalGridSize);
		browserFieldList.push_back(fieldOffsetSKFHorizontalSpacing);
		browserFieldList.push_back(fieldOffsetSKFVerticalSpacing);
		browserFieldList.push_back(fieldOffsetSKFFixedRowColMethod);
		browserFieldList.push_back(fieldOffsetSKFDefaultButton);
	}
	if (softkeyGroupList.empty())
	{
		softkeyGroupList.push_back(fieldOffsetSKFNumber);
		softkeyGroupList.push_back(fieldOffsetSKFHorizontalGridSize);
		softkeyGroupList.push_back(fieldOffsetSKFVerticalGridSize);
		softkeyGroupList.push_back(fieldOffsetSKFHorizontalSpacing);
		softkeyGroupList.push_back(fieldOffsetSKFVerticalSpacing);
	}
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetSKFDeptNum);
		evolFieldList.push_back(fieldOffsetSKFNumber);
		evolFieldList.push_back(fieldOffsetSKFDefSoftkeyGroup);
		evolFieldList.push_back(fieldOffsetSKFSoftkeyStyle);
		evolFieldList.push_back(fieldOffsetSKFHorizontalGridSize);
		evolFieldList.push_back(fieldOffsetSKFVerticalGridSize);
		evolFieldList.push_back(fieldOffsetSKFHorizontalSpacing);
		evolFieldList.push_back(fieldOffsetSKFVerticalSpacing);
		evolFieldList.push_back(fieldOffsetSKFFixedRowColMethod);
		evolFieldList.push_back(fieldOffsetSKFDefaultButton);

		SoftkeyFeatureTags softkeyFeatureTags((FieldOffsetsList *)&evolFieldList);
	}
}

HobRecordNum SoftkeyFeature::getDeptNum() const
{
	return deptNum;
}
void SoftkeyFeature::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

void SoftkeyFeature::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

DBSoftKeyFeature *SoftkeyFeature::getMyDbRecord()
{
	return myDbRecord;
}

bool SoftkeyFeature::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}


HobRecordNum SoftkeyFeature::getFeatureNumber() const
{
	return recordNumber;
}
void SoftkeyFeature::setFeatureNumber(HobRecordNum number)
{
	recordNumber = number;
}

int SoftkeyFeature::getDefaultHorizontalGridSize() const
{
	return defaultHGridSize;
}
void SoftkeyFeature::setDefaultHorizontalGridSize(int size)
{
	if (size > 0)
		defaultHGridSize = size;
}

int SoftkeyFeature::getDefaultVerticalGridSize() const
{
	return defaultVGridSize;
}
void SoftkeyFeature::setDefaultVerticalGridSize(int size)
{
	if (size > 0)
		defaultVGridSize = size;
}

int SoftkeyFeature::getHorizontalSpacing() const
{
	return horizontalSpacing;
}
void SoftkeyFeature::setHorizontalSpacing(int spacing)
{
	horizontalSpacing = spacing;
}

int SoftkeyFeature::getVerticalSpacing() const
{
	return verticalSpacing;
}
void SoftkeyFeature::setVerticalSpacing(int spacing)
{
	verticalSpacing = spacing;
}

FixedRowColMethod SoftkeyFeature::getFixedRowColMethod() const
{
	return fixedRowColMethod;
}
void SoftkeyFeature::setFixedRowColMethod(FixedRowColMethod frcm)
{
	fixedRowColMethod = frcm;
}

int SoftkeyFeature::getDefaultButton() const
{
	return defaultButton;
}
void SoftkeyFeature::setDefaultButton(int defButton)
{
	defaultButton = defButton;
}

HobRecordNum SoftkeyFeature::getDefaultGroupNumber() const
{
	return defaultGroupNumber;
}
void SoftkeyFeature::setDefaultGroupNumber(HobRecordNum groupNum)
{
	defaultGroupNumber = groupNum;
}

HobRecordNum SoftkeyFeature::getStyleNumber() const
{
	return styleNumber;
}
void SoftkeyFeature::setStyleNumber(HobRecordNum styleNum)
{
	styleNumber = styleNum;
}


int SoftkeyFeature::updateField(int fieldCode, const HobString &token)
{
	if (Debug::isTracing("SoftkeyFeature"))
	{
		HobString myDebug("updateField() - fieldCode: ");
		myDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("SoftkeyFeature", myDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetSKFNumber:
			setFeatureNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKFHorizontalGridSize:
			setDefaultHorizontalGridSize(Integer::valueOf(token));
			break;
		case fieldOffsetSKFVerticalGridSize:
			setDefaultVerticalGridSize(Integer::valueOf(token));
			break;
		case fieldOffsetSKFHorizontalSpacing:
			setHorizontalSpacing(Integer::valueOf(token));
			break;
		case fieldOffsetSKFVerticalSpacing:
			setVerticalSpacing(Integer::valueOf(token));
			break;
		case fieldOffsetSKFFixedRowColMethod:
			setFixedRowColMethod((FixedRowColMethod)Integer::valueOf(token));
			break;
		case fieldOffsetSKFDefaultButton:
			setDefaultButton(Integer::valueOf(token));
			break;
		case fieldOffsetSKFDefSoftkeyGroup:
			setDefaultGroupNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKFSoftkeyStyle:
			setStyleNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKFDeptNum:
			setDeptNum((HobRecordNum)Long::valueOf(token));
			break;
		default:
			break;
	}

	return 0;
}


void SoftkeyFeature::update(const SoftkeyFeature *newSoftkey)
{
	if (!newSoftkey)
		return;

	Debug::trace("SoftkeyFeature", "Entering update ...");
	
	//	Test to prevent the 1x1 feature #999001. We still don't know why or how it's happening.
	bool oneBy1For999001 = false;
	if ((newSoftkey->getFeatureNumber() == Reserved_ProductGrid) && (newSoftkey->getDefaultHorizontalGridSize() == 1) && (newSoftkey->getDefaultVerticalGridSize() == 1))
	{
		//	Document it even if the Trace category is not on.
		bool debugIsOn = Debug::isTracing("SoftkeyFeature");
		if (!debugIsOn)
			Debug::addTraceCategory("SoftkeyFeature");
		Debug::trace("SoftkeyFeature", "update():  Size is 1x1 for feature #999001! We will NOT set this size information.");
		if (!debugIsOn)
			Debug::removeTraceCategory("SoftkeyFeature");
		oneBy1For999001 = true;
	}

	short loopCnt = 1;
	while (loopCnt < endSoftkeyFeatureFieldOffsets)
	{
		if (newSoftkey->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetSKFNumber:
					setFeatureNumber(newSoftkey->getFeatureNumber());
					break;
				case fieldOffsetSKFHorizontalGridSize:
					if (!oneBy1For999001)
						setDefaultHorizontalGridSize(newSoftkey->getDefaultHorizontalGridSize());
					break;
				case fieldOffsetSKFVerticalGridSize:
					if (!oneBy1For999001)
						setDefaultVerticalGridSize(newSoftkey->getDefaultVerticalGridSize());
					break;
				case fieldOffsetSKFHorizontalSpacing:
					setHorizontalSpacing(newSoftkey->getHorizontalSpacing());
					break;
				case fieldOffsetSKFVerticalSpacing:
					setVerticalSpacing(newSoftkey->getVerticalSpacing());
					break;
				case fieldOffsetSKFFixedRowColMethod:
					setFixedRowColMethod(newSoftkey->getFixedRowColMethod());
					break;
				case fieldOffsetSKFDefaultButton:
					setDefaultButton(newSoftkey->getDefaultButton());
					break;
				case fieldOffsetSKFDefSoftkeyGroup:
					setDefaultGroupNumber(newSoftkey->getDefaultGroupNumber());
					break;
				case fieldOffsetSKFSoftkeyStyle:
					setStyleNumber(newSoftkey->getStyleNumber());
					break;
				case fieldOffsetSKFDeptNum:
					setDeptNum(newSoftkey->getDeptNum());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
	Debug::trace("SoftkeyFeature", "Leaving update ...");
}


void SoftkeyFeature::getFieldAsString(int fieldCode, HobString &result, int UNUSED(format)) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetSKFNumber:
			if (recordNumber > 0)
				result = Long::toString(recordNumber);
			break;
		case fieldOffsetSKFDeptNum:
			result = Long::toString(deptNum);
			break;
		case fieldOffsetSKFHorizontalGridSize:
			result = Integer::toString(defaultHGridSize);
			break;
		case fieldOffsetSKFVerticalGridSize:
			result = Integer::toString(defaultVGridSize);
			break;
		case fieldOffsetSKFHorizontalSpacing:
			result = Integer::toString(horizontalSpacing);
			break;
		case fieldOffsetSKFVerticalSpacing:
			result = Integer::toString(verticalSpacing);
			break;
		case fieldOffsetSKFFixedRowColMethod:
			result = Integer::toString(fixedRowColMethod);
			break;
		case fieldOffsetSKFDefaultButton:
			result = Integer::toString(defaultButton);
			break;
		case fieldOffsetSKFDefSoftkeyGroup:
			result = Long::toString(defaultGroupNumber);
			break;
		case fieldOffsetSKFSoftkeyStyle:
			result = Long::toString(styleNumber);
			break;
		default:
			Debug::trace("SoftkeyFeature", "getFieldAsString():  Invalid field code!!!");
			break;
	}
	if (Debug::isTracing("SoftkeyFeature"))
	{
		HobString myDebug = "getFieldAsString():  fieldCode = ";
		myDebug.append(Integer::toString(fieldCode)).append("   result = [");
		myDebug.append(result).append("]");
		Debug::trace("SoftkeyFeature", myDebug);
	}
}

SoftkeyFeature *SoftkeyFeature::find(HobRecordNum recordNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	SoftkeyFeature *readSoftkeyFeature = (SoftkeyFeature *)NULL;

	if (recordNum <= 0 || !dbAccess)
		return readSoftkeyFeature;

	DBSoftKeyFeaturesRM *dbSoftkeyFeaturesRM = dbAccess->getDBSoftKeyFeaturesRM();
	if (!dbSoftkeyFeaturesRM)
		return readSoftkeyFeature;

	HobString num(Long::toString(recordNum));
	HobString dNum(Long::toString(deptNum));

	try
	{
		DBSoftKeyFeature *dbSoftkeyFeatureRec = dbSoftkeyFeaturesRM->find(num, dNum);
		if (dbSoftkeyFeatureRec)
		{
			Debug::trace("SoftkeyFeature", "find() - found the record in the db");
			readSoftkeyFeature = new SoftkeyFeature(dbSoftkeyFeatureRec, dbAccess);
			if (!readSoftkeyFeature)
			{
				Debug::trace("SoftkeyFeature", "find() - But I can't create a new SoftkeyFeature object!");
				delete dbSoftkeyFeatureRec;
			}
			else
			{
				if (Debug::isTracing("SoftkeyFeature"))
				{
					HobString myDebug("find() - created the hfc SoftkeyFeature object - SoftkeyFeature: ");
					myDebug.append(Hex::toString((long long) readSoftkeyFeature)).append("   recordNum: ").append(num).append("   deptNum: ").append(Long::toString(deptNum));
					Debug::trace("SoftkeyFeature", myDebug);
				}
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "SoftkeyFeature::find() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}

	return readSoftkeyFeature;
}

int SoftkeyFeature::deleteRecord()
{
	int recordsDeleted = 0;

	if (myDbRecord && dbAccess)
	{
		DBSoftKeyFeaturesRM *dbSoftkeyFeaturesRM = dbAccess->getDBSoftKeyFeaturesRM();
		if (dbSoftkeyFeaturesRM)
		{
			recordsDeleted = dbSoftkeyFeaturesRM->deleteRecord(myDbRecord);
		}
		else
		{
			Debug::trace("SoftkeyFeature", "Cannot delete this record!");
		}
	}
	return recordsDeleted;
}

void SoftkeyFeature::saveRecord()
{
	if (!dbAccess)
		return;

	HobString msgDebug, msgDebug2;
	bool saveDebug = Debug::isTracing("SoftkeyFeature.save");

	Debug::trace("SoftkeyFeature", "Entering saveRecord ...");

	DBSoftKeyFeaturesRM *dbSoftkeyFeaturesRM = dbAccess->getDBSoftKeyFeaturesRM();
	if (!dbSoftkeyFeaturesRM)
		return;

	if (recordNumber <= 0)		//	Not a valid record.
		return;

	if (!myDbRecord)
	{
		Debug::trace("SoftkeyFeature", "saveRecord() - don't have a db record, creating it...");
		myDbRecord = dbSoftkeyFeaturesRM->createRecord(Long::toString(recordNumber));
	}

	if (myDbRecord)
	{
		if (Debug::isTracing("SoftkeyFeature"))
		{
			Debug::trace("SoftkeyFeature", "Starting to save fields to db record ...");
			const char *z = serialize(BrowserFormat);
			msgDebug = "Before saving fields to db record ... ";
			msgDebug.append(z);
			Debug::trace("SoftkeyFeature", msgDebug);
		}

		//	Plug the changes back into the dbSoftkeyFeature Record.
		myDbRecord->setDeptNumber(Long::toString(deptNum));
		myDbRecord->setNumber(Integer::toString(recordNumber));
		myDbRecord->setDefaultHorizontalGridSize(defaultHGridSize);
		myDbRecord->setDefaultVerticalGridSize(defaultVGridSize);
		myDbRecord->setDefaultHorizontalSpacing(horizontalSpacing);
		myDbRecord->setDefaultVerticalSpacing(verticalSpacing);
		myDbRecord->setFixedRowColMethod(fixedRowColMethod);
		myDbRecord->setDefaultButton(defaultButton);
		myDbRecord->setDefaultGroupNumber(defaultGroupNumber);
		myDbRecord->setSoftKeyStyleNumber(styleNumber);

		//	Now save the SoftkeyFeature record to the database.
		if (saveDebug)
		{
			msgDebug = "Before saving to db ...  recordNumber = ";
			msgDebug2 = Long::toString(recordNumber);
			msgDebug.append(msgDebug2);
			Debug::trace("SoftkeyFeature.save", msgDebug);
		}

		try
		{
			dbSoftkeyFeaturesRM->saveRecord(myDbRecord);
		}
		catch (DBException &dbe)
		{
			Debug::trace("DBAccess.exception", "SoftkeyFeature::saveRecord() ********** database exception **********");
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
			msgDebug = "After saving to db ...  recordNumber = ";
			msgDebug.append(msgDebug2);
			Debug::trace("SoftkeyFeature.save", msgDebug);
		}
	}
}

const char *SoftkeyFeature::serialize(int format) const
{
	serializedData = "";
	int loopCount = 0;

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<SoftkeyFeatureFieldOffsets>::iterator fieldIter;
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
			SoftkeyFeatureTags softkeyFeatureTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<SoftkeyFeatureFieldOffsets>::iterator fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = softkeyFeatureTags.getTag(*fieldIter);
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
			serializedData = "SoftkeyFeature::serialize - unknown format: ";
			serializedData.append(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
		}
	}
	if (Debug::isTracing("SoftkeyFeature"))
	{
		HobString myDebug = "serialize():  serializedData = [";
		myDebug.append(serializedData).append("]");
		Debug::trace("SoftkeyFeature", myDebug);
	}
	return serializedData.c_str();
}


// static
void SoftkeyFeature::verifyReservedRecords(DBAccess *dbAccess)
{
	Debug::trace("SoftkeyFeature", "verifying all reserved Softkey Features ...");

	if (!dbAccess)
		return;

	// information for setting up records for all departments in order to loop thru them
	AllDepts allDepts;
	allDepts.findAll(dbAccess);
	HobList deptNumList = allDepts.getDeptNumList();
	HobString deptNumStr = "0";
	long deptNum = 0;

	while (!deptNumStr.empty())
	{
		deptNum = Long::valueOf(deptNumStr);

		//createReservedFeatureRecord(DBAccess *dbAccess, HobRecordNum deptNum, HobRecordNum featureNum, int hSize, int vSize, int hSpacing, int vSpacing)
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_ProductGrid, 6, 4, 1, 1);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_StartScreenButtons, 2, 4, 2, 2);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_RunScreenButtons, 5, 3, 2, 2);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_PresetTareGrid, 6, 4, 1, 1);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_CategoryButtons, 1, 4, 0, 24);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_MostCommonlyUsedItems, 6, 4, 2, 2);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, ReservedSK_SelfservCategoryButtons, 4, 2, 2, 2);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_StartScreenCutTestOff, 2, 4, 2, 2);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_RunScreenCutTestOff, 2, 6, 2, 2);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_StartScreenCutTestOn, 2, 4, 2, 2);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_RunScreenCutTestOn, 2, 6, 2, 2);
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_RunScreenCutTestOnOptionOne, 3, 3, 2, 2);

		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, ReservedSK_SelfservAZCategoryButtons, 2, 5, 2, 2);	//SELF SERVE -> Originally was 999012-999013 pre Access Port  // A-Z categories for Self Serve
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, ReservedSK_SelfservCategoryResultsButtons, 6, 4, 2, 2);// Category results feature for Self Serve
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, ReservedSK_SelfservProductGrid, 6, 4, 2, 2);			// Static product grid for Self-Serve mode
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, ReservedSK_SelfservAZCategoryResults, 7, 2, 0, 0);	// Category A-Z results feature for Self Serve

		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_RunScreenNumericKeypadButtons, 3, 3, 1, 1);	//	Run screen buttons when the numeric keypad is hidden (Fastrack).

		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_BuildAMealScreenButtons, 4, 4, 1, 1);		//	Programmable Buttons for the Build-A-Meal screen (Fastrack).
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_BuildAMealSideItemButtons, 3, 4, 1, 1);		//	Programmable Buttons for the Build-A-Meal Side Items (Fastrack).

		// NEW for Meat Spec Sheet and Buttons and Sides
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_MeatSpecSheetScreeenButtons, 3, 4, 1, 1);	//	Programmable Buttons for the Meat Specification Sheet Meals (Fastrack).
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_MeatSpecSheetSideItemButtons, 3, 4, 1, 1);	//	Programmable Buttons for the Meat Specification Sheet Side Items (Fastrack).

		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_FixedRowColGrid, 6, 4, 1, 1);				// Category results feature for Self Serve
		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_FreshServeButtons, 5, 3, 1, 1);				// FreshServe feature for Self Serve

		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_CameraSystemPredictions, 3, 2, 2, 2);		// CAMERA -> Edgify and Tiliter Camera Predictions for Self Service 

		SoftkeyFeature::createReservedFeatureRecord(dbAccess, deptNum, Reserved_ForcedCameraOverride, 4, 4, 1, 1);

		// Advance to the next department in the department list
		deptNumStr = "";
		if (deptNumList.next(deptNumStr) != -1 && deptNumStr == "0")
		{
			deptNumStr = "";
			deptNumList.next(deptNumStr);
		}
	}
}


// static
void SoftkeyFeature::createReservedFeatureRecord(DBAccess *dbAccess, HobRecordNum deptNum, HobRecordNum featureNum, int hSize, int vSize, int hSpacing, int vSpacing)
{
	if (!dbAccess)
		return;

	bool traceOn = Debug::isTracing("SoftkeyFeature");

	try
	{
		SoftkeyFeature *reservedSoftkeyFeature = SoftkeyFeature::find(featureNum, deptNum, dbAccess);
		if (!reservedSoftkeyFeature)
		{
			if (traceOn)
				Debug::trace("SoftkeyFeature", string("SoftkeyFeature::createReservedFeatureRecord() - creating Feature # ") + longtostring(featureNum) +
				"  hSpacing = " + inttostring(hSpacing) + "  vSpacing = " + inttostring(vSpacing) + "  hSize = " + inttostring(hSize) + "  vSize = " + inttostring(vSize));

			reservedSoftkeyFeature = new SoftkeyFeature(dbAccess);
			if (reservedSoftkeyFeature)
			{
				reservedSoftkeyFeature->setDeptNum(deptNum);
				reservedSoftkeyFeature->setFeatureNumber(featureNum);
				reservedSoftkeyFeature->setStyleNumber(featureNum);				//	Same as feature num
				reservedSoftkeyFeature->setHorizontalSpacing(hSpacing);
				reservedSoftkeyFeature->setVerticalSpacing(vSpacing);
				reservedSoftkeyFeature->setDefaultHorizontalGridSize(hSize);
				reservedSoftkeyFeature->setDefaultVerticalGridSize(vSize);
				reservedSoftkeyFeature->setDefaultGroupNumber(featureNum);		//	Same as feature num
				reservedSoftkeyFeature->saveRecord();
			}
		}

		if (reservedSoftkeyFeature)
		{
			delete reservedSoftkeyFeature;
			reservedSoftkeyFeature = 0;
		}
	}
	catch (DBException &dbe)
	{
		DBException::Type t = dbe.getType();
		Debug::trace("SoftkeyFeature", string("SoftkeyFeature::createReservedFeatureRecord() - ********** EXCEPTION **********   DBException: type: ") + inttostring(t) + "  info: " + dbe.info());
	}
}