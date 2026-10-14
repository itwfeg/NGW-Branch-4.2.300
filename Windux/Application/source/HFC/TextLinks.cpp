/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "TextLinks.h"
#include "ProductText.h"

#include "DBAccess.h"
#include "DBProductsRM.h"
#include "DBProductText.h"

#include "Integer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int TextLinks::dataType = 0;
std::string TextLinks::dataTypeName("TextLinks"); 
#endif



TextLinks::TextLinks()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

TextLinks::TextLinks(DBAccess *dbAccess, ProdSupportTextType txtType, void *dbRecord)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	textType = txtType;

	if (dbAccess && dbRecord)
	{
		DBProductsRM *dbProductsRM = dbAccess->getDBProductsRM();
		if (dbProductsRM)
		{
			switch (textType)
			{
				case psttNutrifacts:
					linkList = dbProductsRM->retrieveLinkedtoNutrition((DBNutrition*) dbRecord);
					break;

				case psttExpandedText:
					linkList = dbProductsRM->retrieveLinkedtoText((DBProductText*) dbRecord);
					break;

				case psttSpecialMessage:
					linkList = dbProductsRM->retrieveLinkedtoMessage((DBProductText*) dbRecord);
					break;

				case psttProductNote:
					linkList = dbProductsRM->retrieveLinkedtoNote((DBProductText*) dbRecord);
					break;

				case psttMarqueeText:
					linkList = dbProductsRM->retrieveLinkedtoMarquee((DBProductText*) dbRecord);
					break;

				case psttProductText5:
					linkList = dbProductsRM->retrieveLinkedtoProductText5((DBProductText*) dbRecord);
					break;

				case psttProductText6:
					linkList = dbProductsRM->retrieveLinkedtoProductText6((DBProductText*) dbRecord);
					break;

				case psttProductText7:
					linkList = dbProductsRM->retrieveLinkedtoProductText7((DBProductText*) dbRecord);
					break;

				case psttProductText8:
					linkList = dbProductsRM->retrieveLinkedtoProductText8((DBProductText*) dbRecord);
					break;

				case psttProductText9:
					linkList = dbProductsRM->retrieveLinkedtoProductText9((DBProductText*) dbRecord);
					break;

				default:
					break;
			}
		}

		if (linkList)
		{
			HobString paddedStr;
			HobString spacer(" ");
			DBQuery<DBProduct>::const_iterator iter = linkList->begin();
			if (iter != linkList->end())
			{
				myList.push_back(iter->getPLU());
				++iter;
			}
			while (iter != linkList->end())
			{
				//	The Browser needs spaces in front of the remaining PLU numbers so that it can create a newLine to wrap text.
				paddedStr = spacer + iter->getPLU();
				myList.push_back(paddedStr);
				++iter;
			}
		}
	}
}


TextLinks::~TextLinks()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (linkList)
		delete linkList;
}


void TextLinks::init()
{
	textType = psttUnknown;
	linkList = NULL;
}


ProdSupportTextType TextLinks::getTextType() const
{
	return textType;
}


DBQuery<DBProduct>* TextLinks::getLinkList()
{
	return linkList;
}


/*
const char *TextLinks::serialize(int format)
{
	switch (format)
	{
		case BrowserFormat:
		{
			serializedData = Integer::toString(textType);

			DBQuery<DBProduct>::const_iterator iter = linkList->begin();
			while (iter != linkList->end())
			{
				serializedData.append(BrowserFormatSeparator);
				serializedData.append(iter->getPLU());
				++iter;
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.data();
}
*/
