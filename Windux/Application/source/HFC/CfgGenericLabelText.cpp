/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgGenericLabelText.h"
#include "GenericLabelText.h"

#include "Debug.h"

#include "Integer.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgGenericLabelText::dataType = 0;
std::string CfgGenericLabelText::dataTypeName("CfgGenericLabelText"); 
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
CfgGenericLabelText::CfgGenericLabelText()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CfgGenericLabelText::~CfgGenericLabelText()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	// cleanup memory
	for (int i( maxLabelTexts ); i--; )
		setLabelText( i, NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CfgGenericLabelText::init()
{
	maxLabelTexts = 20;
	labelTexts.insert(labelTexts.begin(), maxLabelTexts, (GenericLabelText *) 0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CfgGenericLabelText::load(DBAccess *dbAccess)
{
	Debug::trace("Configuration.CfgGenericLabelText", "load() - entering");
	if (!dbAccess)
		return;

	HobString dbVal;
	if (loadField(dbAccess, "maxLabelTexts", dbVal))
		setMaxLabelTexts(dbAccess, Integer::valueOf(dbVal));
	else
		reloadLabelText(dbAccess);

	Debug::trace("Configuration.CfgGenericLabelText", "load() - leaving");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CfgGenericLabelText::setMaxLabelTexts(DBAccess *dbAccess, short max)
{
	maxLabelTexts = max;
	labelTexts.resize(maxLabelTexts);
	
	reloadLabelText(dbAccess);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CfgGenericLabelText::reloadLabelText(DBAccess *dbAccess)
{
	Debug::trace("Configuration.CfgGenericLabelText", HobString("reloadLabelText() - entering"));
	if (dbAccess)
	{
		Debug::trace("Configuration.CfgGenericLabelText", "CfgGenericLabelText::reloadLabelText(): maxCount = " + Integer::toString(maxLabelTexts));
			
		for (int i = 1; i <= maxLabelTexts; i++)
			setLabelText(i, GenericLabelText::findLabelText(i, dbAccess));

		show();
	}
	Debug::trace("Configuration.CfgGenericLabelText", HobString("reloadLabelText() - leaving"));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HobString CfgGenericLabelText::getLabelText(int labelTextNum) const
{
	if (labelTextNum < 1 || labelTextNum > maxLabelTexts)
		return HobString("");

	GenericLabelText *genericLabelText( labelTexts[labelTextNum - 1] );

	if (!genericLabelText)
		return HobString("");

	return genericLabelText->getText();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CfgGenericLabelText::setLabelText(int labelTextNum, GenericLabelText* labelText)
{
	if (labelTextNum > 0 && labelTextNum <= maxLabelTexts)
	{
		if ( labelTexts[labelTextNum - 1] )
			delete labelTexts[labelTextNum - 1];

		labelTexts[labelTextNum - 1] = labelText;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const char *CfgGenericLabelText::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	switch (format)
	{
		case AllGenericTextFormat:
		{
			HobString maxCount = HobString(Integer::toString(maxLabelTexts));
			Debug::trace("Clare", "CfgGenericLabelText::serialize(): maxCount = " + maxCount);
			serializedData.append(maxCount);

			HobString listData("");
			HobString tempString("");
			for (int loopCnt = 1; loopCnt <= maxLabelTexts; loopCnt++)
			{
				listData.append(BrowserFormatSeparator);

				tempString = getLabelText(loopCnt);
				tempString.quote();
				listData.append(tempString);
			}
			listData.quoteForCSV();
			serializedData.append(listData);
			break;
		}

		default:
			serializedData = HobString("CfgGenericLabelText::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CfgGenericLabelText::show()
{
	Debug::trace("Configuration", "*** CfgGenericLabelText parameters ***");
	Debug::trace("Configuration", HobString("maxLabelTexts:") + HobString(Integer::toString(maxLabelTexts)));

	for (int i = 1; i < maxLabelTexts; i++)
	{
		Debug::trace("Configuration", HobString("genericLabelText:") + HobString(Integer::toString(i)) +
												HobString(" = ") + getLabelText(i));
	}
}
