/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "AllCoolTexts.h"
#include "Configuration.h"
#include "HobTokenizerCSV.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBCoolTextsRM.h"
#include "DBCoolText.h"
#include "DBCoolListsRM.h"
#include "DBCoolList.h"
#include "DBCoolListTextsRM.h"
#include "DBCoolListText.h"

#include "Product.h"
#include "CSmartPtr.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

using namespace std;
using namespace util_string;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int AllCoolTexts::dataType = 0;
std::string AllCoolTexts::dataTypeName("AllCoolTexts"); 
#endif


AllCoolTexts::AllCoolTexts()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

AllCoolTexts::AllCoolTexts(const char *data, int len, int /* formatType */)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	HobString temp(data, len);
	HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
	HobString token;
	short loopCnt = 1;
	while (tok.next(&token) != -1)
	{
		setReceived(loopCnt, (token.length() == 0) ? false : true);

		switch (loopCnt)
		{
			case 1:
			{
				Debug::trace("AllCoolTexts", "Setting ProductNum: " + token);
				productNum = Long::valueOf(token);
				break;
			}
			case 2:
			{
				Debug::trace("AllCoolTexts", "Setting addTextsIfMissing: " + token);
				addTextsIfMissing = Bool::valueOf(token);
				break;
			}
			default:
				break;
		}
		loopCnt++;
	}
}

AllCoolTexts::~AllCoolTexts()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void AllCoolTexts::init()
{
	productNum = RECORD_NOT_FOUND;
	addTextsIfMissing = false;

	//textList.setQuotingElements(true);
}


void AllCoolTexts::findAll(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	DBCoolTextsRM *dbRM = dbAccess->getDBCoolTextsRM();
	if (!dbRM)
		return;

	bool traceOn = Debug::isTracing("AllCoolTexts");
	try
	{
		DBQuery<DBCoolText>* allQuery = dbRM->retrieveAllForDept( Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum) );
		if (!allQuery)
			return;

		DBQuery<DBCoolText>::const_iterator iter = allQuery->begin();
		while (iter != allQuery->end())
		{
			if (traceOn)
				Debug::trace("AllCoolTexts", string("findAll - recordNum ") + iter->getNumber() + "  text " + iter->getText());
			//	Push the Cool text on the list
			recordNumList.push_back(iter->getNumber());
			textList.push_back(iter->getText());
			++iter;
		}
		delete allQuery;

		if (traceOn)
			Debug::trace("AllCoolTexts", string("findAll - recordNumList size ") + inttostring(recordNumList.size()) + "  textList size " + inttostring(textList.size()));
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllCoolTexts::findAll() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}


void AllCoolTexts::findAllLinkedToProduct(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	if (productNum == RECORD_NOT_FOUND)
		return;

	HobRecordNum deptNum = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);

	Util::CSmartPtr<Product> theProduct(Product::findRecord(productNum, deptNum, dbAccess));
	if (theProduct.null())
		return;

	DBCoolTextsRM *dbCoolTextsRM = dbAccess->getDBCoolTextsRM();
	if (!dbCoolTextsRM)
		return;

	DBCoolListsRM *dbCoolListRM = dbAccess->getDBCoolListsRM();
	if (!dbCoolListRM)
		return;

	DBCoolListTextsRM *dbCoolListTextsRM = dbAccess->getDBCoolListTextsRM();
	if (!dbCoolListTextsRM)
		return;

	HobRecordNum	currentCoolNum = RECORD_NOT_FOUND;
	HobRecordNum	defaultCoolNum = RECORD_NOT_FOUND;
	HobRecordNum	tempCoolNum = RECORD_NOT_FOUND;
	bool			defaultCoolInShortlist = false;
	bool			currentCoolInShortlist = false;
	
	bool traceOn = Debug::isTracing("AllCoolTexts");
	HobString myDebug = "findAllLinkedToProduct():  ";

	HobString recordNumStr;
	DBCoolText *dbCoolText = 0;

	//	Find the Cool list for this product.
	DBCoolList *dbCoolList = dbCoolListRM->find(theProduct->getMyDbRecord());
	if (!dbCoolList)
	{
		if (traceOn)
			Debug::trace("AllCoolTexts", myDebug + "No dbCoolList for productNum " + longtostring(productNum) + "  deptNum " + longtostring(deptNum));
		// return;
	}

	if (addTextsIfMissing)
	{
		defaultCoolNum	= theProduct->getCoolDefaultTextNum();
		currentCoolNum	= theProduct->getCoolTextNum();
	}
	if (traceOn)
		Debug::trace("AllCoolTexts", myDebug + "addTextsIfMissing = " + booltostring(addTextsIfMissing) + "  defaultCoolNum = " + longtostring(productNum) + ",  currentCoolNum = " + longtostring(currentCoolNum));

	try
	{
		DBQuery<DBCoolListText>* allCoolListTextsQuery = 0;
		if (dbCoolList)
		{
			allCoolListTextsQuery = dbCoolListTextsRM->retrieveLinkedtoCoolList(dbCoolList);
			delete dbCoolList;
		}

		if (!allCoolListTextsQuery)
		{
			if (traceOn)
				Debug::trace("AllCoolTexts", myDebug + "allCoolListTextsQuery is 0.");
			// return;
		}
		else
		{
			DBQuery<DBCoolListText>::const_iterator iter = allCoolListTextsQuery->begin();
			while (iter != allCoolListTextsQuery->end())
			{
				recordNumStr = iter->getCoolTextNumber();
				dbCoolText = dbCoolTextsRM->findNumber(recordNumStr, iter->getDeptNumber());
				if (!dbCoolText)
				{
					if (traceOn)
						Debug::trace("AllCoolTexts", myDebug + "  Could not find COOLText Number: " + recordNumStr);
				}
				else
				{
					if (traceOn)
						Debug::trace("AllCoolTexts", myDebug + "  COOLText Number: " + recordNumStr + ", [" + dbCoolText->getText() + "]");

					//	Push the COOL text number on the list
					recordNumList.push_back(dbCoolText->getNumber());
					//	Push the COOL text on the list. Just quote it first.
					HobString tempName(dbCoolText->getText());
					tempName.quoteForCSV();
					textList.push_back(tempName);

					if (addTextsIfMissing && (!defaultCoolInShortlist || !currentCoolInShortlist))
					{
						tempCoolNum = Long::valueOf(recordNumStr);
						if (tempCoolNum == defaultCoolNum)
							defaultCoolInShortlist = true;
						if (tempCoolNum == currentCoolNum)
							currentCoolInShortlist = true;
					}
					delete dbCoolText;
				}
				++iter;
			}
			delete allCoolListTextsQuery;
		}

		if (addTextsIfMissing)
		{
			if (!defaultCoolInShortlist && (defaultCoolNum != RECORD_NOT_FOUND))
			{
				recordNumStr = Long::toString(defaultCoolNum);
				if (traceOn)
					Debug::trace("AllCoolTexts", myDebug + "  DefaultCOOLText Number: " + recordNumStr + " is not in the Shortlist. Will add it temporarily to the Shortlist if it exists.");

				dbCoolText = dbCoolTextsRM->findNumber(recordNumStr, longtostring(deptNum));
				if (!dbCoolText)
				{
					if (traceOn)
						Debug::trace("AllCoolTexts", myDebug + "  Could not find DefaultCOOLText Number: " + recordNumStr);
				}
				else
				{
					if (traceOn)
						Debug::trace("AllCoolTexts", myDebug + "  Appending DefaultCOOLText Number: " + recordNumStr + ", [" + dbCoolText->getText() + "] to the list.");
					//	Push the COOL text number on the list
					recordNumList.push_back(dbCoolText->getNumber());
					//	Push the COOL text on the list. Just quote it first.
					HobString tempName(dbCoolText->getText());
					tempName.quoteForCSV();
					textList.push_back(tempName);

					delete dbCoolText;
				}
			}
			if (!currentCoolInShortlist && (currentCoolNum != defaultCoolNum) && (currentCoolNum != RECORD_NOT_FOUND))
			{
				recordNumStr = Long::toString(currentCoolNum);
				if (traceOn)
					Debug::trace("AllCoolTexts", myDebug + "  CurrentCOOLText Number: " + recordNumStr + " is not in the Shortlist. Will add it temporarily to the Shortlist if it exists.");

				dbCoolText = dbCoolTextsRM->findNumber(recordNumStr, longtostring(deptNum));
				if (!dbCoolText)
				{
					if (traceOn)
						Debug::trace("AllCoolTexts", myDebug + "  Could not find CurrentCOOLText Number: " + recordNumStr);
				}
				else
				{
					if (traceOn)
						Debug::trace("AllCoolTexts", myDebug + "  Appending CurrentCOOLText Number: " + recordNumStr + ", [" + dbCoolText->getText() + "] to the list.");

					//	Push the COOL text number on the list
					recordNumList.push_back(dbCoolText->getNumber());
					//	Push the COOL text on the list. Just quote it first.
					HobString tempName(dbCoolText->getText());
					tempName.quoteForCSV();
					textList.push_back(tempName);

					delete dbCoolText;
				}
			}
		}
	}
	catch (DBException &dbe)
	{
		Debug::trace("DBAccess.exception", "AllCoolTexts::findAllLinkedToProduct() ********** database exception **********");
		DBException::Type t = dbe.getType();
		Debug::trace("DBAccess.exception", string("     DBException: type:") + inttostring(t) + string("  info:") + dbe.info());
	}
}


const char *AllCoolTexts::serialize(int format) const
{
	serializedData = "";

	switch (format)
	{
		case BrowserFormat:
			serializedData.append(recordNumList.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(textList.serialize(format));
			break;

		default:
			break;
	}
	return serializedData.c_str();
}
