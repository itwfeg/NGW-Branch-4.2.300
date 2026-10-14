/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Class.h"
#include "ClassTags.h"
#include "Configuration.h"
#include "HobDateTime.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBClassesRM.h"
#include "DBClass.h"

#include "MultiLanguageText.h"

#include "AllDepts.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"

using namespace std;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Class::dataType = 0;
string Class::dataTypeName("Class"); 
#endif

list<ClassFieldOffsets> Class::uiFieldList;
list<ClassFieldOffsets> Class::evolFieldList;


Class::Class()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

Class::Class(const char *data, int len, int formatType)
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
			std::list<ClassFieldOffsets>::iterator fieldIter = uiFieldList.begin();
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

			// Only for the UI, if we didn't receive the deptNum with the data, then set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetClassDeptNum))
			{
				setReceived(fieldOffsetClassDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			ClassTags classTags;

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
					offset = (temp.size()) ? classTags.getOffset(temp) : 0;
					if (Debug::isTracing("Class.ctor"))
						Debug::trace("Class.ctor", string("Class(data,len,format) token: ") + temp + string("  offset:") + inttostring(offset) + string("  data:") + token->getData());
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
			if (!isReceived(fieldOffsetClassDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetClassDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}		 

		default:
			break;
	}
}

Class::Class(DBClass *dbRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Class.ctor", string("Class(DBClass, DBAccess) - dbRecPtr passed in ") + ptrtostring(dbRec));
	init();

	dbAccess = access;

	if (dbRec)
	{
		myDbRecord = dbRec;

		deptNum           = Long::valueOf(dbRec->getDeptNumber());
		classNum          = Long::valueOf(dbRec->getNumber());
		className         = dbRec->getDescription();
		reportingClassNum = Long::valueOf(dbRec->getReportingClass());
		errorCode = RECORD_OK;
	}
}


Class::~Class()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("Class.dtor", "    Class: " + ptrtostring(this) + "  num " +	longtostring(classNum) + "  deptNum " + longtostring(deptNum));

	if (myDbRecord)
		delete myDbRecord;
}


//	private
void Class::init()
{
	deptNum = 0;
	classNum = RECORD_NOT_FOUND;
	className = "";
	reportingClassNum = GRAND_TOTALS_CLASS;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBClass *)NULL;

	errorCode = RECORD_NOT_FOUND;
	classLinkStatus = (ClassLinkStatus) CLASS_LINK_OK;
	badClass = RECORD_NOT_FOUND;
	badReportingClass = RECORD_NOT_FOUND;

	initFieldLists();
}

// static
void Class::initFieldLists()
{
	if (uiFieldList.empty())
	{
		uiFieldList.push_back(fieldOffsetClassNum);
		uiFieldList.push_back(fieldOffsetReportingClassNum);
		uiFieldList.push_back(fieldOffsetClassName);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetClassDeptNum);
		evolFieldList.push_back(fieldOffsetClassNum);
		evolFieldList.push_back(fieldOffsetReportingClassNum);
		evolFieldList.push_back(fieldOffsetClassName);

		ClassTags classTags((FieldOffsetsList *)&evolFieldList);
	}
}


HobRecordNum Class::getDeptNum() const
{
	return deptNum;
}
void Class::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

HobRecordNum Class::getClassNum() const
{
	return classNum;
}
void Class::setClassNum(HobRecordNum num)
{
	classNum = num;
}

HobString Class::getClassName() const
{
	return className;
}
void Class::setClassName(HobString name)
{
	className = name;
}

HobRecordNum Class::getReportingClassNum() const
{
	return reportingClassNum;
}
void Class::setReportingClassNum(HobRecordNum num)
{
	reportingClassNum = num;
}

HobErrorCode Class::getErrorCode() const
{
	return errorCode;
}
void Class::setErrorCode(HobErrorCode err)
{
	errorCode = err;
}


ClassLinkStatus Class::getClassLinkStatus() const
{
	return classLinkStatus;
}
void Class::setClassLinkStatus(ClassLinkStatus status)
{
	classLinkStatus = status;
}

HobRecordNum Class::getBadClass() const
{
	return badClass;
}
void Class::setBadClass(HobRecordNum num)
{
	badClass = num;
}

HobRecordNum Class::getBadReportingClass() const
{
	return badReportingClass;
}
void Class::setBadReportingClass(HobRecordNum num)
{
	badReportingClass = num;
}


void Class::associateItem(HobRecordNum /* itemNum */)
{
}

/*
void Class::associateItem(HobRecordNum &[] itemNums)
{
}
*/


/*
	THIS IS MY ATTEMPT TO CREATE A NEW VERSION OF THE Ultima/Quantum FUNCTION test_class_tree().
*/
void Class::checkClassTree(DBAccess *dbAccess)
{
//  CHECK WHAT WE HAVE BEFORE ACCESSING THE DATABASE FURTHER
    if((classNum == GRAND_TOTALS_CLASS) || (reportingClassNum == GRAND_TOTALS_CLASS))
    {
	//	THE CLASS EITHER IS OR REPORTS TO THE GRAND_TOTALS_CLASS SO WE ARE DONE HERE
        setClassLinkStatus(CLASS_LINK_OK);			
    }
    else
    if(reportingClassNum == classNum)
    {
        setClassLinkStatus(CLASS_REPORTS_TO_SELF);	//	SELF EXPLANATORY
		setBadClass(classNum);
		setBadReportingClass(reportingClassNum);
    }
	else
	{
	//  ALLOCATE A LIST FOR THE LINKS SO WE CAN SEARCH FOR LOOPING
		HobRecordNum *classList = new HobRecordNum[MAX_CLASS_LINKS];
	    if(classList == NULL)
	    {
	        setClassLinkStatus(INSUFFICIENT_CLASS_LIST_MEMORY);	//	Not enough memory to allocate list.
	    }
		else
		{
		//  STARTING WITH THE REPORTING CLASS NUMBER, READ THAT CLASS, THEN READ IN
		//  EACH SUCCESSIVE REPORTING CLASS, TESTING FOR LOOPING,
		//  UNTIL WE REACH GRAND TOTALS OR UNTIL AN ERROR IS FOUND.
			Class *classRecord			  = NULL;
		    ClassLinkStatus classLinkTest = CLASS_LINK_OK;
		    HobRecordNum tempClassNum	  = getReportingClassNum();
		    int numberOfLinks			  = 0;

			//	Keep track of the current class and reporting class so we know for which reporting class an error, if any, occurs.
			//	We will reset these values at the end of this method if the links are OK.
			setBadClass(getClassNum());
			setBadReportingClass(tempClassNum);
				
		    while((numberOfLinks < MAX_CLASS_LINKS) && (classLinkTest == CLASS_LINK_OK))
		    {
		    //  ADD THIS Reporting CLASS NUMBER TO THE LIST
		        classList[numberOfLinks++] = tempClassNum;

		    //  EXIT IF WE ARE AT GRAND_TOTALS_CLASS
		        if(tempClassNum == GRAND_TOTALS_CLASS)
		        {
		            break;
		        }

		    //  NOW TRY READING THE REPORTING CLASS
		        classRecord = Class::findRecord(tempClassNum, deptNum, dbAccess);
		        if(classRecord == NULL)
		        {
		            if(numberOfLinks > 1)
		            {
		                classLinkTest = MISSING_CLASS_IN_LINK;
		            }
		            else
		            {
						//	This case should not happen in this function.
		                classLinkTest = CLASS_DOES_NOT_EXIST;
		            }
		        }
		        else
		        {
		        //  THE DATABASE READ WAS SUCCESSFUL SO NOW WE CAN MAKE SURE THE CLASS DOES NOT REPORT TO ITSELF.
		            if(tempClassNum == classRecord->getReportingClassNum())
		            {
		                classLinkTest = CLASS_REPORTS_TO_SELF;
						setBadClass(tempClassNum);
						setBadReportingClass(tempClassNum);
		            }
		            else
		            {
	                //  SEE IF THE REPORTING CLASS MATCHES ONE ALREADY IN THE LIST.
	                    if(numberOfLinks > 1)
	                    {
	                    //  Check first to make sure the reportingClassNum is not the original class number.
							if(classNum == classRecord->getReportingClassNum())
							{
                                classLinkTest = LOOP_IN_CLASS_LINK;
							}
							else
							{
		                    //  WE'LL SEARCH FROM THE END TO THE BEGINNING of the list.
		    		            int index;
							    HobRecordNum tempReportingClass	= classRecord->getReportingClassNum();
		                        for(index = numberOfLinks - 1; index >= 0; index--)
		                        {
		                            if(tempReportingClass == classList[index])
		                            {
		                                classLinkTest = LOOP_IN_CLASS_LINK;
		                                break;
		                            }
		                        }
							}
	                    }
		            }
		        }

		    //  IF THINGS ARE OK SO FAR - SET UP TO LOOK AT THE NEXT CLASS
		        if(classLinkTest == CLASS_LINK_OK)
		        {
				//	The bad class becomes the current class.
					setBadClass(tempClassNum);

		        //  tempClassNum becomes the current Reporting Class.
		            tempClassNum = classRecord->getReportingClassNum();

				//	And the bad reporting class becomes the current Reporting Class.
					setBadReportingClass(tempClassNum);
		        }

		        if(classRecord != NULL)
		        {
		            delete classRecord;
		            classRecord = NULL;
		        }
		    }
		    if((numberOfLinks == MAX_CLASS_LINKS) &&  (classLinkTest == CLASS_LINK_OK))
		    {
		        classLinkTest = TOO_MANY_CLASS_LINKS;
				//	Set the badClass to the original class number
				setBadClass(getClassNum());
				//	Set the badReportingClass to the original reporting class number
				setBadReportingClass(getReportingClassNum());
		    }
	        setClassLinkStatus(classLinkTest);	//	SELF EXPLANATORY

		//  FREE THE CLASS LIST
		    delete [] classList;
	    }
    }
    if(getClassLinkStatus() == CLASS_LINK_OK)
	{
        setBadClass(0);
	    setBadReportingClass(0);
	}
}


bool Class::isReservedClass()
{
	return (classNum == GRAND_TOTALS_CLASS || classNum == VOID_TOTALS_CLASS || classNum == PRICE_CHANGE_CLASS) ? true : false;
}


// Returns true if the field was updated, false if not
bool Class::updateField(int fieldOffset, const HobString &newValue)
{
	if (Debug::isTracing("Class"))
		Debug::trace("Class", string("updateField() - fieldOffset ") + inttostring(fieldOffset) + "  data " + newValue);

	if (fieldOffset < 0)
		return false;

	switch (fieldOffset)
	{
		case fieldOffsetClassDeptNum:
			setDeptNum(Long::valueOf(newValue));
			break;
		case fieldOffsetClassNum:
			setClassNum(Long::valueOf(newValue));
			break;
		case fieldOffsetClassName:
			setClassName(newValue);
			break;
		case fieldOffsetReportingClassNum:
			setReportingClassNum(Long::valueOf(newValue));
			break;
		default:
			return false;
			break;
	}

	return true;
}

void Class::updateRecord(const Class *newData)
{
	if (!newData)
		return;

	if (Debug::isTracing("Class"))
	{
		Debug::trace("Class", string("updateRecord() - this num ") + longtostring(classNum) + string("  this DeptNum ") + longtostring(deptNum));
		Debug::trace("Class", string("updateRecord() - passed in num ") + longtostring(newData->getClassNum()) + string("  passed in DeptNum ") + longtostring(newData->getDeptNum()));
	}

	short loopCnt = 1;
	while (loopCnt < endClassFieldOffsets)
	{
		if (newData->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetClassDeptNum:
					setDeptNum(newData->getDeptNum());
					break;
				case fieldOffsetClassNum:
					setClassNum(newData->getClassNum());
					break;
				case fieldOffsetClassName:
					setClassName(newData->getClassName());
					break;
				case fieldOffsetReportingClassNum:
					setReportingClassNum(newData->getReportingClassNum());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
}


bool Class::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetClassName:
			return true;
			break;
		default:
			break;
	}
	return false;
}

void Class::getFieldAsString(int fieldOffset, HobString &result, int format) const
{
	result = "";

	switch (fieldOffset)
	{
		case fieldOffsetClassDeptNum:
			result = longtostring(deptNum);
			break;
		case fieldOffsetClassNum:
			result = longtostring(classNum);
			break;
		case fieldOffsetClassName:
			result = className;
			break;
		case fieldOffsetReportingClassNum:
			result = longtostring(reportingClassNum);
			break;
		default:
			break;
	}

	if (Debug::isTracing("Class.getFieldAsString"))
		Debug::trace("Class.getFieldAsString", string("fieldOffset ") + inttostring(fieldOffset) + string("  result ") + result + string("  format ") + inttostring(format));
}

const char *Class::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	list<ClassFieldOffsets>::iterator fieldIter;
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
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		case SynchronizationTaggedFormat:
		{
			ClassTags classTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = classTags.getTag(*fieldIter);
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
			serializedData = string("Class::serialize - unknown format:") + inttostring(format);
			break;
	}
	return serializedData.data();
}


void Class::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

bool Class::isRecordValid() const
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}

int Class::deleteRecord()
{
	int result = 0;
	if (myDbRecord && dbAccess)
	{
		DBClassesRM	*dbRM = dbAccess->getDBClassesRM();
		if (dbRM)
		{
			Debug::trace("Class", string("deleteRecord() - num ") + longtostring(classNum) + "  deptNum " + longtostring(deptNum));
			result = dbRM->deleteRecord(myDbRecord);
		}
	}
	return result;							
}

void Class::saveRecord()
{
	if (classNum > 0 && dbAccess)
	{
		DBClassesRM	*dbRM = dbAccess->getDBClassesRM();
		if (!dbRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("Class", string("saveRecord() - don't have a db record, creating it..."));
			myDbRecord = dbRM->createRecord();
		}

		if (myDbRecord)
		{
			//	Plug the changes back into the dbClassRecord
			myDbRecord->setDeptNumber(longtostring(deptNum));
			myDbRecord->setNumber(longtostring(classNum));
			myDbRecord->setDescription(className);
			if (isReservedClass())
			   myDbRecord->setReportingClassNull();
			else if (reportingClassNum <= 0)
				myDbRecord->setReportingClass(longtostring(GRAND_TOTALS_CLASS));
			else
			   myDbRecord->setReportingClass(longtostring(reportingClassNum));

			try
			{
				dbRM->saveRecord(myDbRecord);		//save record to the database
				Debug::trace("Class", string("saveRecord() - saved num ") + longtostring(classNum) + string("  deptNum ") + longtostring(deptNum));
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Class::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
			}
		}
	}
}

// static
Class *Class::findRecord(HobRecordNum classNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	Class *rec = (Class *)NULL;

	// Debug::trace("Class", string("findRecord() - looking for num ") + longtostring(classNum) + "  deptNum " + longtostring(deptNum) + " in database");

	if (classNum <= 0 || deptNum < 0 || !dbAccess)
		return rec;

	DBClassesRM	*dbRM = dbAccess->getDBClassesRM();
	if (!dbRM)
		return rec;

	try
	{
		DBClass *dbRec = dbRM->findNumber(longtostring(classNum), longtostring(deptNum));
		if (dbRec)
		{
			rec = new Class(dbRec, dbAccess);
			if (!rec)
				delete dbRec;
			else
			{
				if (Debug::isTracing("Class"))
					Debug::trace("Class", string("findRecord() - rec ptr: ") + ptrtostring(rec) + "  num " + longtostring(rec->getClassNum()) + "  deptNum " + longtostring(rec->getDeptNum()));
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Class::findRecord() ********** database exception **********"));
			DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("     DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}

	return rec;
}


Class *Class::findClassValidateLinks(HobRecordNum classNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	Class *readClass = Class::findRecord(classNum, deptNum, dbAccess);

	if (readClass != NULL)
	{
		readClass->checkClassTree(dbAccess);
	}
	else
	{
		//	We need to know the reporting class number.
		//	Therefore, we need to keep a pointer to the class.
		readClass = new Class();
		readClass->setClassNum(classNum);
		readClass->setDeptNum(deptNum);
		readClass->setErrorCode(RECORD_NOT_FOUND);
		readClass->setClassLinkStatus(CLASS_DOES_NOT_EXIST);
		readClass->setBadClass(classNum);
		Debug::trace("New.Class", "    Class: " + ptrtostring(readClass) +
			"  Class number:" + longtostring(readClass->getClassNum()) + " - findClassValidateLinks()");
	}
	return readClass;
}


// static
void Class::verifyReservedRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("Class", "verifyReservedRecords - no dbAccess passed in!");
		return;
	}

	bool traceOn = Debug::isTracing("Class");
	HobString msgDebug = "verifyReservedRecords() - Created class ";
	if (traceOn)
		Debug::trace("Class", "verifying all reserved Classes ...");

	AllDepts allDepts;
	allDepts.findAll(dbAccess);
	HobList deptNumList = allDepts.getDeptNumList();

	HobString deptNumStr = "0";
	long deptNum = 0;

	Class *rec = (Class *)NULL;
	hfc::MultiLanguageText internationalize;

	HobDateTime todaysDate;
	HobString createdOn(internationalize("Class.Created"));
	createdOn.append(1, ' ').append(todaysDate.formatDate(Configuration::getInstance()->getDateDisplayFormat(), HobDateTime::_UsingPrimaryLanguageLocale_));
			
	while (!deptNumStr.empty())
	{
		deptNum = Long::valueOf(deptNumStr);

		rec = Class::findRecord(GRAND_TOTALS_CLASS, deptNum, dbAccess);
		if (!rec)
		{
			rec = new Class();
			if (rec)
			{
				HobString name(internationalize("Class.GrandTotals"));
				name.append(1, '\n').append(createdOn);

				rec->setDeptNum(deptNum);
				rec->setClassNum(GRAND_TOTALS_CLASS);
				rec->setClassName(name);
				rec->setDBAccess(dbAccess);
				rec->saveRecord();
				if (traceOn)
					Debug::trace("Class", msgDebug + longtostring(rec->getClassNum()) + "  deptNum " + deptNumStr);
			}
		}

		if (rec)
		{
			delete rec;
			rec = (Class *)NULL;
		}

		rec = Class::findRecord(VOID_TOTALS_CLASS, deptNum, dbAccess);
		if (!rec)
		{
			rec = new Class();
			if (rec)
			{
				HobString name(internationalize("Class.VoidTotals"));
				name.append(1, '\n').append(createdOn);

				rec->setDeptNum(deptNum);
				rec->setClassNum(VOID_TOTALS_CLASS);
				rec->setClassName(name);
				rec->setDBAccess(dbAccess);
				rec->saveRecord();
				if (traceOn)
					Debug::trace("Class", msgDebug + longtostring(rec->getClassNum()) + "  deptNum " + deptNumStr);
			}
		}

		if (rec)
		{
			delete rec;
			rec = (Class *)NULL;
		}

		rec = Class::findRecord(PRICE_CHANGE_CLASS, deptNum, dbAccess);
		if (!rec)
		{
			rec = new Class();
			if (rec)
			{
				HobString name(internationalize("Class.PriceChangeTotals"));
				name.append(1, '\n').append(createdOn);

				rec->setDeptNum(deptNum);
				rec->setClassNum(PRICE_CHANGE_CLASS);
				rec->setClassName(name);
				rec->setDBAccess(dbAccess);
				rec->saveRecord();
				if (traceOn)
					Debug::trace("Class", msgDebug + longtostring(rec->getClassNum()) + "  deptNum " + deptNumStr);
			}
		}

		if (rec)
		{
			delete rec;
			rec = (Class *)NULL;
		}

		rec = Class::findRecord(LOST_TOTALS_CLASS, deptNum, dbAccess);
		if (!rec)
		{
			rec = new Class();
			if (rec)
			{
				HobString name(internationalize("Class.LostPLUTotals"));
				name.append(1, '\n').append(createdOn);

				rec->setDeptNum(deptNum);
				rec->setClassNum(LOST_TOTALS_CLASS);
				rec->setClassName(name);
				rec->setReportingClassNum(GRAND_TOTALS_CLASS);
				rec->setDBAccess(dbAccess);
				rec->saveRecord();
				if (traceOn)
					Debug::trace("Class", msgDebug + longtostring(rec->getClassNum()) + "  deptNum " + deptNumStr);
			}
		}

		if (rec)
		{
			delete rec;
			rec = (Class *)NULL;
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

// static
void Class::recreateReservedRecords(DBAccess *dbAccess)
{
	if (!dbAccess)
	{
		Debug::trace("Class", "recreateReservedRecords - no dbAccess passed in!");
		return;
	}

	bool traceOn = Debug::isTracing("Class");
	HobString msgDebug = "recreateReservedRecords() - Deleting reserved class ";
	if (traceOn)
		Debug::trace("Class", "recreating all reserved classes ...");

	AllDepts allDepts;
	allDepts.findAll(dbAccess);
	HobList deptNumList = allDepts.getDeptNumList();

	HobString deptNumStr = "0";
	long deptNum = 0;

	Class *rec = (Class *)NULL;
			
	while (!deptNumStr.empty())
	{
		deptNum = Long::valueOf(deptNumStr);

		rec = Class::findRecord(GRAND_TOTALS_CLASS, deptNum, dbAccess);
		if (rec)
		{
			if (traceOn)
				Debug::trace("Class", msgDebug + "GRAND_TOTALS_CLASS  deptNum " + deptNumStr);
			rec->deleteRecord();
			delete rec;
			rec = 0;
		}

		rec = Class::findRecord(VOID_TOTALS_CLASS, deptNum, dbAccess);
		if (rec)
		{
			if (traceOn)
				Debug::trace("Class", msgDebug + "VOID_TOTALS_CLASS  deptNum " + deptNumStr);
			rec->deleteRecord();
			delete rec;
			rec = 0;
		}

		rec = Class::findRecord(PRICE_CHANGE_CLASS, deptNum, dbAccess);
		if (rec)
		{
			if (traceOn)
				Debug::trace("Class", msgDebug + "PRICE_CHANGE_CLASS  deptNum " + deptNumStr);
			rec->deleteRecord();
			delete rec;
			rec = 0;
		}

		rec = Class::findRecord(LOST_TOTALS_CLASS, deptNum, dbAccess);
		if (rec)
		{
			if (traceOn)
				Debug::trace("Class", msgDebug + "LOST_TOTALS_CLASS  deptNum " + deptNumStr);
			rec->deleteRecord();
			delete rec;
			rec = 0;
		}

		// Get the next dept record from the database.  If the next one is the default dept (Dept 0), skip it and get the next one, since we
		// automatically deleted the reserved records for dept 0 in the first pass through the while loop (since deptNumStr was initialized to "0").
		// If the database doesn't contain the default dept record, we still need to delete the reserved records for the default dept (Dept 0).
		deptNumStr = "";
		if (deptNumList.next(deptNumStr) != -1 && deptNumStr == "0")
		{
			deptNumStr = "";
			deptNumList.next(deptNumStr);
		}
	}

	if (traceOn)
		Debug::trace("Class", msgDebug + ", Now to recreate the reserved classes...");
	verifyReservedRecords(dbAccess);
}
