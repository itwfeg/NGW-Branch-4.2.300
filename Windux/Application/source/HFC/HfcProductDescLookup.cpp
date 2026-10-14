/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HfcProductDescLookup.h"
#include "HobTokenizerCSV.h"
#include "Configuration.h"
#include "DBProductsRM.h"
#include "DBProduct.h"
#include "DBQuery.h"
#include "Session.h"
#include "SessionManager.h"
#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HfcProductDescLookup::dataType = 0;
std::string HfcProductDescLookup::dataTypeName("HfcProductDescLookup"); 
#endif

HfcProductDescLookup::HfcProductDescLookup(const char *data, int len, int formatType)
	: lookupString(""), lookupMethod(1)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobString temp(data, len);
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			if (tok.next(&token) != -1 && token.size())
			{
				lookupString = token;
				Debug::trace("HfcProductDescLookup", HobString("lookup string is ") + lookupString);
			}
			if (tok.next(&token) != -1 && token.size())
			{
				lookupMethod = Integer::valueOf(token);
				Debug::trace("HfcProductDescLookup", HobString("lookup method is ") + HobString(Integer::toString(lookupMethod)));
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

HfcProductDescLookup::~HfcProductDescLookup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


const char *HfcProductDescLookup::serialize(int format) const
{
	switch (format)
	{
		case BrowserFormat:
		{
			serializedData = "";
			serializedData.append(productNumList.serialize(format));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(descriptionList.serialize(format));
		}
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}


void HfcProductDescLookup::lookupProductsByDesc(DBProductsRM *dbProductsRM, LanguageSelection language)
{
	if (!dbProductsRM || !lookupString.size())
		return;

	if (lookupMethod == 0)
	{
		int lastPLU = 0, counter = 0, total = 0;

		// Lookup by description
		Debug::trace("HfcProductDescLookup", "Looking up products by description: " + lookupString);

		util_time::Time t0 = util_time::Time();

		DBQuery<DBProduct> *productList = dbProductsRM->retrieveKeywordInDescription(lookupString, Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum));
		if (!productList)
			return;

		util_time::Time t1 = util_time::Time();
		Debug::trace("HfcProductDescLookup", 28, "Created query in " + 
				util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");


		DBQuery<DBProduct>::const_iterator iter = productList->begin();
		while (iter != productList->end())
		{
			int currentPLU = stringtoint(iter->getPLU());

			Debug::trace("HfcProductDescLookup", HobString("by description - found product: ") + iter->getPLU() + HobString("   desc: ") + iter->getDescription());
			if ( lastPLU != currentPLU )
			{
				productNumList.push_back(iter->getPLU());

				HobString descStr("");

				if (language == PrimaryLanguage)
				{
					descStr = iter->getName1();
					if (descStr.empty())
					{
						descStr = iter->getDescription();
						descStr.stripFontSequences();
					}
				}
				else
				{
					descStr = iter->getName2();
					if (descStr.empty())
					{
						descStr = iter->getDescription();
						descStr.stripFontSequences();
					}

				}

				descStr.quoteForCSV();
				descriptionList.push_back(descStr);

				lastPLU = currentPLU;
				++counter;
			}
			else
			{
				Debug::trace("HfcProductDescLookup", HobString("discarding PLU since it is repeated"));
			}

			++total;
			++iter;
		}

		util_time::Time t2 = util_time::Time();
		Debug::trace("HfcProductDescLookup", 28, "Retrieved " + inttostring(counter) +
				" of a total of " + inttostring(total) + " records in " + 
				util_string::inttostring((t2 - t1).getTotalMilliseconds()) + " milliseconds");

		delete productList;
	}
	else if (lookupMethod == 1)
	{
		// the range is specified by CfgPrepack's pluSearchRange.
		int range = Configuration::getInstance()->toIntCfg(fieldOffsetCfgPluSearchRange);

		// Lookup range of plu's before and after the plu number passed in
		Debug::trace("HfcProductDescLookup", "Looking up products in a range of " + inttostring(range) + " above and below Plu number " + lookupString);

		util_time::Time t0 = util_time::Time();

//		DBQuery<DBProduct> *productList = dbProductsRM->retrieveAll();
		DBQuery<DBProduct> *productList = dbProductsRM->retrieveNearestPLU(lookupString, Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum), range);
		if ( productList )
		{
			util_time::Time t1 = util_time::Time();
			Debug::trace("HfcProductDescLookup", 28, "Created query and checked for non-empty in " + 
					util_string::inttostring((t1 - t0).getTotalMilliseconds()) + " milliseconds");

			DBQuery<DBProduct>::const_iterator iter = productList->begin();
			while ( iter != productList->end() )
			{
				Debug::trace("HfcProductDescLookup", 28, "Retrieved Product number " + iter->getPLU() +
						" with description " + iter->getDescription());

				productNumList.push_back(iter->getPLU());

				HobString descStr = iter->getDescription();
				descStr.stripFontSequences();	//	Strip all font sequences from this text.
				descStr.quoteForCSV();
				descriptionList.push_back(descStr);

				++iter;
			}

			util_time::Time t2 = util_time::Time();
			Debug::trace("HfcProductDescLookup", 28, "Retrieved all desired PLU records in " + 
					util_string::inttostring((t2 - t1).getTotalMilliseconds()) + " milliseconds");

			delete productList;
		}
		else
			Debug::trace("HfcProductDescLookup", "No products found - exiting!");
	}
	else
		Debug::trace("HfcProductDescLookup", HobString("INVALID LOOKUP METHOD: ") + HobString(Integer::toString(lookupMethod)));

}
