/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Softkey.h"
#include "SoftkeyTags.h"
#include "SoftkeyFeature.h"
#include "SoftkeyStyle.h"
#include "SoftkeyScriptManager.h"

#include "SoftkeyGroup.h"

#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "Configuration.h"

#include "DBAccess.h"
#include "DBException.h"
#include "DBSoftKeysRM.h"
#include "DBSoftKey.h"
#include "DBProductsRM.h"
#include "DBProduct.h"
#include "DBCategoriesRM.h"
#include "DBCategory.h"
#include "DBOperatorsRM.h"
#include "DBOperator.h"

#include "DBQuery.h"

#include "MultiLanguageText.h"

#include "Special.h"
#include "filesys/filename.h"

#include "AllDepts.h"

#include "Integer.h"
#include "Long.h"
#include "Double.h"
#include "Bool.h"
#include "Hex.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Softkey::dataType = 0;
std::string Softkey::dataTypeName("Softkey"); 
#endif

std::list<SoftkeyFieldOffsets> Softkey::evolFieldList;
std::list<SoftkeyFieldOffsets> Softkey::ultimaFieldList;
std::list<SoftkeyFieldOffsets> Softkey::browserFieldList;
std::list<SoftkeyFieldOffsets> Softkey::executeSoftkeyFieldList;


Softkey::Softkey(DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Softkey", "Softkey constructor 1.");
	init();

	dbAccess = access;
}


Softkey::Softkey(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Softkey", "Softkey constructor 2.");
	init();

	switch (formatType)
	{
		case BrowserFormat:
		{
			Debug::trace("Softkey", "Constructor:  BrowserFormat");
			HobString temp(data, len);
			std::list<SoftkeyFieldOffsets>::iterator fieldIter = browserFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != browserFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				if ((offset == fieldOffsetSKeyFlashkeyNum) && isReceived(offset))
				{
					setReceived(fieldOffsetSKeyGroupNum, true);
					setReceived(fieldOffsetSKeyHorizontalPos, true);
					setReceived(fieldOffsetSKeyVerticalPos, true);
				}
				if (token.length() && isFieldQuoted(offset))
					token.unQuoteForCSV();

				if (Debug::isTracing("Softkey"))
				{
					HobString msgDebug("updateField(BrowserFormat) - fieldCode: ");
					msgDebug.append(Integer::toString(offset)).append("  data: ").append(token);
					Debug::trace("Softkey", msgDebug);
				}
				updateField(offset, token);
				fieldIter++;
			}

			// Only for the UI, if we didn't receive the deptNum with the Class info, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetSKeyDeptNumber))
			{
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				setReceived(fieldOffsetSKeyDeptNumber, true);
			}

			//	Parse the field returned from the Browser so the data is interpreted correctly, based on the action.
			parseActionParameter();
			break;
		}

		case ExecuteSoftkeyFormat:
		{
			Debug::trace("Softkey", "Constructor:  ExecuteSoftkeyFormat");
			HobString temp(data, len);
			std::list<SoftkeyFieldOffsets>::iterator fieldIter = executeSoftkeyFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != executeSoftkeyFieldList.end()))
			{
				offset = *fieldIter;
				setReceived(offset, (token.length() == 0) ? false : true);
				if (Debug::isTracing("Softkey"))
				{
					HobString msgDebug("Softkey(ExecuteSoftkeyFormat) - fieldCode: ");
					msgDebug.append(Integer::toString(offset)).append("  data: ").append(token);
					Debug::trace("Softkey", msgDebug);
				}
				updateField(offset, token);
				fieldIter++;
			}

			// Only for the UI, if we didn't receive the deptNum with the Class info, the set the deptNum to the currently active dept.
			// If we don't receive it via comm/merge/restore, or other mechanism in the subsequent cases, then it remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetSKeyDeptNumber))
			{
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				setReceived(fieldOffsetSKeyDeptNumber, true);
			}

			//	Parse the field returned from the Browser so the data is interpreted correctly, based on the action.
			parseActionParameter();
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case UltimaTaggedFormat:
		case DBIndependentFormatFormat:
		{
			SoftkeyTags softkeyTags;

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
					offset = (temp.size()) ? softkeyTags.getOffset(temp) : 0;
					if (offset)
					{
						setReceived(offset, true);
						updateField(offset, token->getData(), EvolutionTaggedFormat);
					}
				}
				delete token;
			}

			// For comm/backup/restore, if we didn't receive the deptNum with the data and the config parameter is set, set the deptNum to the currently active dept.
			// If we didn't receive the deptNum with the data and the config parameter is NOT set (normal case), deptNum remains its initial value of 0, the default dept.
			if (!isReceived(fieldOffsetSKeyDeptNumber) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetSKeyDeptNumber, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

Softkey::Softkey(DBSoftKey *dbSoftkeyRec, DBAccess *access, short languageSelection)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	Debug::trace("Softkey", "Softkey constructor with DBSoftKey record.");
	init();

	if (!access)
		return;

	dbAccess = access;

	if (dbSoftkeyRec)
	{
		DBProductsRM *dbProductsRM = (DBProductsRM*) NULL;
		DBProduct *dbProductRec = (DBProduct*) NULL;

		myDbRecord = dbSoftkeyRec;

		groupNumber = Long::valueOf(dbSoftkeyRec->getGroupNumber());
		deptNum = Long::valueOf(dbSoftkeyRec->getDeptNumber());


		horizontalPosition = Integer::valueOf(dbSoftkeyRec->getHorizontalPosition());
		verticalPosition = Integer::valueOf(dbSoftkeyRec->getVerticalPosition());
		horizontalSize = dbSoftkeyRec->getHorizontalSize();
		verticalSize = dbSoftkeyRec->getVerticalSize();

		if (!dbSoftkeyRec->isBackgroundColorNull())
			backgroundColor = Hex::toHTMLColorString((long) dbSoftkeyRec->getBackgroundColor());

		if (!dbSoftkeyRec->isToggledBackgroundColorNull())
			toggledBackgroundColor = Hex::toHTMLColorString((long)dbSoftkeyRec->getToggledBackgroundColor());

		if (!dbSoftkeyRec->isBackgroundImageNull())
			backgroundImage = dbSoftkeyRec->getBackgroundImage();
		if (backgroundImage.size() && validateBackgroundImageFile)
			backgroundImage = validateSoftkeyImage(backgroundImage, languageSelection);

		if (!dbSoftkeyRec->isToggledBackgroundImageNull())
			toggledBackgroundImage = dbSoftkeyRec->getToggledBackgroundImage();
		if (toggledBackgroundImage.size() && validateToggledBackgroundImageFile)
			toggledBackgroundImage = validateSoftkeyImage(toggledBackgroundImage, languageSelection);

		if (!dbSoftkeyRec->isTextColorNull())
			textColor = Hex::toHTMLColorString((long) dbSoftkeyRec->getTextColor());

		if (!dbSoftkeyRec->isName1Null())
			name1 = dbSoftkeyRec->getName1();
		if (!dbSoftkeyRec->isName2Null())
			name2 = dbSoftkeyRec->getName2();

		if (!dbSoftkeyRec->isActionNull())
			action = dbSoftkeyRec->getAction();

		if (!dbSoftkeyRec->isActionSubtypeNull())
			actionSubtype = dbSoftkeyRec->getActionSubtype();

		if (!dbSoftkeyRec->isActionIntegerParameterNull())
		{
			actionIntParam = dbSoftkeyRec->getActionIntegerParameter();
			Debug::trace("Softkey",HobString("dbSoftkeyRec->getActionIntegerParameter() = ") + Integer::toString(dbSoftkeyRec->getActionIntegerParameter()));
		}	

		if (!dbSoftkeyRec->isActionStringParameterNull())
			actionStringParam = dbSoftkeyRec->getActionStringParameter();

		if (!dbSoftkeyRec->isActionCategoryNumberNull())
			actionCategoryNumber = Long::valueOf(dbSoftkeyRec->getActionCategoryNumber());

		if (!dbSoftkeyRec->isActionOperatorNumberNull())
			actionOperatorNumber = dbSoftkeyRec->getActionOperatorNumber();

		if (!dbSoftkeyRec->isActionFeatureNumberNull())
			actionFeatureNumber = Long::valueOf(dbSoftkeyRec->getActionFeatureNumber());

		if (!dbSoftkeyRec->isActionProductNumberNull())
			actionProductNumber = Long::valueOf(dbSoftkeyRec->getActionProductNumber());

		if (action == Action_SelectProduct)
		{
			dbProductsRM = dbAccess->getDBProductsRM();
			if ( dbProductsRM && actionProductNumber != RECORD_NOT_FOUND )
			{
			#if defined(LINUX_FIX)
				Debug::trace("Softkey", "Trying to find linked product for PLUNUM = " + longtostring(actionProductNumber));
            #else
				Debug::trace("Softkey", "Trying to find linked product for PLUNUM = " + inttostring(actionProductNumber));
            #endif
				dbProductRec = dbProductsRM->findLinkedRecord(dbSoftkeyRec);
				if (!dbProductRec)
				{
					dbProductRec = dbProductsRM->findPLU(Long::toString(actionProductNumber), Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				}
				else
				{
					if (!dbProductRec->isPLUNull())
						actionProductNumber = Long::valueOf(dbProductRec->getPLU());
				}
			}
		}
		//	There is no foregroundImage for a Softkey. It comes only from the Special assigned to a Product.
		//	Use it, if there is one.
		if (dbProductRec)
		{
			defaultForegroundImage = getSpecialsImageFilename(Long::valueOf(dbProductRec->getDeptNumber()), Long::valueOf(dbProductRec->getSpecialNumber()), languageSelection, dbAccess);
			if (defaultForegroundImage.size())
				defaultForegroundImage = validateSpecialsImage(defaultForegroundImage, languageSelection);
		}

		if (!dbSoftkeyRec->isScriptNameNull())
			scriptName = dbSoftkeyRec->getScriptName();
		
		if (!dbSoftkeyRec->isScriptIntervalNull())
			scriptInterval = dbSoftkeyRec->getScriptInterval();

		if (!dbSoftkeyRec->isForceNoBackgroundColorNull())
			forceNoBgColor = dbSoftkeyRec->getForceNoBackgroundColor();
		if (!dbSoftkeyRec->isForceNoBackgroundImageNull())
			forceNoBgImage = dbSoftkeyRec->getForceNoBackgroundImage();
		if (!dbSoftkeyRec->isForceNoTextNull())
			forceNoText = dbSoftkeyRec->getForceNoText();
		if (Debug::isTracing("Softkey"))
		{
			HobString msgDebug("From the db:");
			msgDebug.append("  action = ").append(Integer::toString(action));
			msgDebug.append("  hSize = ").append(Integer::toString(dbSoftkeyRec->getHorizontalSize()));
			msgDebug.append("  vSize = ").append(Integer::toString(dbSoftkeyRec->getVerticalSize()));
			msgDebug.append("  forceNoBgColor = ").append(Bool::toString(dbSoftkeyRec->getForceNoBackgroundColor()));
			msgDebug.append("  forceNoBgImage = ").append(Bool::toString(dbSoftkeyRec->getForceNoBackgroundImage()));
			msgDebug.append("  forceNoText = ").append(Bool::toString(dbSoftkeyRec->getForceNoText()));
			Debug::trace("Softkey", msgDebug);
		}

		if (dbProductRec)
			delete dbProductRec;

		if (scriptName != "" && scriptInterval > 0)
		{
			SoftkeyScriptManager::getInstance().addScript(scriptName, scriptInterval);
		}
	}
}

Softkey::~Softkey()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (myDbRecord)
		delete myDbRecord;
}

void Softkey::init()
{
	groupNumber				= RECORD_NOT_FOUND;
	deptNum                 = 0;
	horizontalPosition		= 0;
	verticalPosition		= 0;
	horizontalSize			= 1;
	verticalSize			= 1;
	action					= Action_NoAction;
	actionSubtype			= 0;
	actionIntParam			= 0;
	actionStringParam		= "";
	actionCategoryNumber	= RECORD_NOT_FOUND;
	actionOperatorNumber	= "";
	actionProductNumber		= RECORD_NOT_FOUND;
	actionFeatureNumber		= RECORD_NOT_FOUND;

	featureNumber			= RECORD_NOT_FOUND;
	flashkeyNumber			= RECORD_NOT_FOUND;

	backgroundColor			= "";
	toggledBackgroundColor	= "";
	backgroundImage			= "";
	toggledBackgroundImage	= "";
	textColor				= "";
	name1					= "";
	name2					= "";

	defaultBackgroundColor	= "";
	defaultBackgroundImage	= "";
	defaultTextColor		= "";
	defaultName1			= "";
	defaultName2			= "";
	defaultForegroundImage	= "";

	useDefaultBgColor		= false;
	useDefaultBgImage		= false;
	useDefaultTextColor		= false;
	useDefaultName1			= false;
	useDefaultName2			= false;

	forceNoBgColor			= false;
	forceNoBgImage			= false;
	forceNoText				= false;

	softkeyEnabled			= true;
	bgImageExists			= false;
	fgImageExists			= false;

	scriptName              = "";
	scriptInterval          = 0;

	actionParameter			= "";

	validateBackgroundImageFile = true;
	validateToggledBackgroundImageFile = true;

	dbAccess = (DBAccess *)NULL;
	myDbRecord = (DBSoftKey *)NULL;

	initFieldLists();
}

void Softkey::initFieldLists()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetSKeyFeatureNum);			//	fg - Softkey group number
		browserFieldList.push_back(fieldOffsetSKeyGroupNum);			//	fg - Softkey group number
		browserFieldList.push_back(fieldOffsetSKeyHorizontalPos);		//	hp - Horizontal position
		browserFieldList.push_back(fieldOffsetSKeyVerticalPos);			//	vp - Vertical position
		browserFieldList.push_back(fieldOffsetSKeyHorizontalSize);		//	fh - Horizontal size in pixels
		browserFieldList.push_back(fieldOffsetSKeyVerticalSize);		//	fv - Vertical size in pixels
		browserFieldList.push_back(fieldOffsetSKeyAction);				//	ft
		browserFieldList.push_back(fieldOffsetSKeyActionParameter);		//	Used only in Browser messages.
		browserFieldList.push_back(fieldOffsetSKeyActionStringParam);
		browserFieldList.push_back(fieldOffsetSKeyActionFeatureNum);	//	??	- Do we have a tag for this one yet?

		browserFieldList.push_back(fieldOffsetSKeyForegroundImage);		//	This will come from a category

		browserFieldList.push_back(fieldOffsetSKeyBackgroundColor);		//	bC
		browserFieldList.push_back(fieldOffsetSKeyBgColorDefault);		//	No tag needed - for Browser only
		browserFieldList.push_back(fieldOffsetSKeyBgColorUseDefault);	//	No tag needed - for Browser only

		browserFieldList.push_back(fieldOffsetSKeyBackgroundImage);		//	bI
		browserFieldList.push_back(fieldOffsetSKeyBgImageDefault);		//	No tag needed - for Browser only
		browserFieldList.push_back(fieldOffsetSKeyBgImageUseDefault);	//	No tag needed - for Browser only

		browserFieldList.push_back(fieldOffsetSKeyTextColor);			//	??	- Do we have a tag for this one yet?
		browserFieldList.push_back(fieldOffsetSKeyTextColorDefault);	//	No tag needed - for Browser only
		browserFieldList.push_back(fieldOffsetSKeyTextColorUseDefault);	//	No tag needed - for Browser only

		browserFieldList.push_back(fieldOffsetSKeyName1);				//	kn
		browserFieldList.push_back(fieldOffsetSKeyName1Default);		//	No tag needed - for Browser only
		browserFieldList.push_back(fieldOffsetSKeyName1UseDefault);		//	No tag needed - for Browser only

		browserFieldList.push_back(fieldOffsetSKeyName2);				//	kN
		browserFieldList.push_back(fieldOffsetSKeyName2Default);		//	No tag needed - for Browser only
		browserFieldList.push_back(fieldOffsetSKeyName2UseDefault);		//	No tag needed - for Browser only

		browserFieldList.push_back(fieldOffsetSKeyForceNoText);			//	Tag ??
		browserFieldList.push_back(fieldOffsetSKeyForceNoBgColor);		//	Tag ??
		browserFieldList.push_back(fieldOffsetSKeyForceNoBgImage);		//	Tag ??

		browserFieldList.push_back(fieldOffsetSKeyScriptName);          // fx
		browserFieldList.push_back(fieldOffsetSKeyScriptInterval);      // fX

		browserFieldList.push_back(fieldOffsetSKeyToggledBackgroundColor);		//	bC
		browserFieldList.push_back(fieldOffsetSKeyToggledBackgroundImage);		//	bI
	}

	if (executeSoftkeyFieldList.empty())
	{
		executeSoftkeyFieldList.push_back(fieldOffsetSKeyAction);				//	ft
		executeSoftkeyFieldList.push_back(fieldOffsetSKeyActionParameter);		//	Used only in Browser messages.
		executeSoftkeyFieldList.push_back(fieldOffsetSKeyActionFeatureNum);	//	??
		executeSoftkeyFieldList.push_back(fieldOffsetSKeyScriptName);           // fx
		executeSoftkeyFieldList.push_back(fieldOffsetSKeyScriptInterval);       // fX
	}

	if (ultimaFieldList.empty())
	{
		ultimaFieldList.push_back(fieldOffsetSKeyFlashkeyNum);			//	f# - Flashkey number
		ultimaFieldList.push_back(fieldOffsetSKeyHorizontalSize);		//	fh - Horizontal size in units
		ultimaFieldList.push_back(fieldOffsetSKeyVerticalSize);			//	fv - Vertical size in units
		ultimaFieldList.push_back(fieldOffsetSKeyAction);				//	ft - Flashkey record type (PRODUCT_FLASHKEY, OPERATOR_FLASHKEY, MACRO_FLASHKEY OR GROUP_FLASHKEY.
		ultimaFieldList.push_back(fieldOffsetSKeyActionIntegerParam);	//	fn - Record number for the Flashkey record type
		ultimaFieldList.push_back(fieldOffsetSKeyFlashkeyName);			//	fd - Flashkey text
		SoftkeyTags softkeyTags((FieldOffsetsList *)&ultimaFieldList);
	}

	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetSKeyDeptNumber);
		evolFieldList.push_back(fieldOffsetSKeyGroupNum);			//	fg - Softkey group number
		evolFieldList.push_back(fieldOffsetSKeyHorizontalPos);		//	hp - Horizontal position
		evolFieldList.push_back(fieldOffsetSKeyVerticalPos);		//	vp - Vertical position
		evolFieldList.push_back(fieldOffsetSKeyHorizontalSize);		//	fh - Horizontal size in pixels
		evolFieldList.push_back(fieldOffsetSKeyVerticalSize);		//	fv - Vertical size in pixels
		evolFieldList.push_back(fieldOffsetSKeyBackgroundColor);	//	bC
		evolFieldList.push_back(fieldOffsetSKeyToggledBackgroundColor);	//	cB
		evolFieldList.push_back(fieldOffsetSKeyBackgroundImage);	//	bI
		evolFieldList.push_back(fieldOffsetSKeyToggledBackgroundImage);	//	iB
		evolFieldList.push_back(fieldOffsetSKeyName1);				//	kn
		evolFieldList.push_back(fieldOffsetSKeyName2);				//	kN
		evolFieldList.push_back(fieldOffsetSKeyAction);				//	ft
		evolFieldList.push_back(fieldOffsetSKeyActionIntegerParam);	//	fn
		evolFieldList.push_back(fieldOffsetSKeyActionStringParam);	//	fS
		evolFieldList.push_back(fieldOffsetSKeyActionCategoryNum);	//	From CategoryTags
		evolFieldList.push_back(fieldOffsetSKeyActionOperatorNum);	//	from OperatorTags
		evolFieldList.push_back(fieldOffsetSKeyActionProductNum);	//	from ProductTags
		evolFieldList.push_back(fieldOffsetSKeyActionFeatureNum);
		evolFieldList.push_back(fieldOffsetSKeyTextColor);
		evolFieldList.push_back(fieldOffsetSKeyScriptName);         // fx
		evolFieldList.push_back(fieldOffsetSKeyScriptInterval);     // fX
		
		SoftkeyTags softkeyTags((FieldOffsetsList *)&evolFieldList);
	}
}

void Softkey::setDBAccess(DBAccess *access)
{
	dbAccess = access;
}

DBSoftKey *Softkey::getMyDbRecord()
{
	return myDbRecord;
}

bool Softkey::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}


HobRecordNum Softkey::getGroupNumber() const
{
	return groupNumber;
}

void Softkey::setGroupNumber(HobRecordNum number)
{
	groupNumber = number;
}

HobRecordNum Softkey::getDeptNum() const
{
	return deptNum;
}
void Softkey::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

int Softkey::getHorizontalPosition() const
{
	return horizontalPosition;
}
void Softkey::setHorizontalPosition(int position)
{
	horizontalPosition = position;
}

int Softkey::getVerticalPosition() const
{
	return verticalPosition;
}
void Softkey::setVerticalPosition(int position)
{
	verticalPosition = position;
}

int Softkey::getHorizontalSize() const
{
	return horizontalSize;
}
void Softkey::setHorizontalSize(int size)
{
	horizontalSize = size;
}

int Softkey::getVerticalSize() const
{
	return verticalSize;
}
void Softkey::setVerticalSize(int size)
{
	verticalSize = size;
}

const HobString &Softkey::getBackgroundColor() const
{
	return backgroundColor;
}
void Softkey::setBackgroundColor(const HobString &color)
{
	backgroundColor = color;
}

const HobString &Softkey::getToggledBackgroundColor() const
{
	return toggledBackgroundColor;
}
void Softkey::setToggledBackgroundColor(const HobString &color)
{
	toggledBackgroundColor = color;
}

const HobString &Softkey::getBackgroundImage() const
{
	return backgroundImage;
}
void Softkey::setBackgroundImage(const HobString &graphicName)
{
	backgroundImage = graphicName;
}

const HobString &Softkey::getToggledBackgroundImage() const
{
	return toggledBackgroundImage;
}
void Softkey::setToggledBackgroundImage(const HobString &graphicName)
{
	toggledBackgroundImage = graphicName;
}

const HobString &Softkey::getName1() const
{
	return name1;
}
void Softkey::setName1(const HobString &name)
{
		if (name.length() > 80)
	{
		name1.assign(name, 0, 80);
		Debug::trace("ClipText", "Softkey: name1 clipped to [" + name1 + "]");
	}
	else
	{
		name1 = name;
		Debug::trace("ClipText", "Softkey: name1 is [" + name1 + "]");
	}
}

const HobString &Softkey::getName2() const
{
	return name2;
}
void Softkey::setName2(const HobString &name)
{
	if (name.length() > 80)
	{
		name2.assign(name, 0, 80);
		Debug::trace("ClipText", "Softkey: name2 clipped to [" + name2 + "]");
	}
	else
	{
		name2 = name;
		Debug::trace("ClipText", "Softkey: name2 is [" + name2 + "]");
	}
}

int Softkey::getAction() const
{
	return action;
}
void Softkey::setAction(int actionValue)
{
	action = actionValue;
}

int Softkey::getActionIntegerParam() const
{
	return actionIntParam;
}
void Softkey::setActionIntegerParam(int param)
{
	actionIntParam = param;
}

const HobString &Softkey::getActionStringParam() const
{
	return actionStringParam;
}
void Softkey::setActionStringParam(const HobString &param)
{
	actionStringParam = param;
}

HobRecordNum Softkey::getActionCategoryNumber() const
{
	return actionCategoryNumber;
}
void Softkey::setActionCategoryNumber(HobRecordNum number)
{
	actionCategoryNumber = number;
}

const HobString &Softkey::getActionOperatorNumber() const
{
	return actionOperatorNumber;
}
void Softkey::setActionOperatorNumber(const HobString &number)
{
	actionOperatorNumber = number;
}

HobRecordNum Softkey::getActionProductNumber() const
{
	return actionProductNumber;
}
void Softkey::setActionProductNumber(HobRecordNum number)
{
	actionProductNumber = number;
}

HobRecordNum Softkey::getActionFeatureNumber() const
{
	return actionFeatureNumber;
}
void Softkey::setActionFeatureNumber(HobRecordNum number)
{
	actionFeatureNumber = number;
}

const HobString &Softkey::getActionParameter() const
{
	//	This variable will contain either the actionIntParam, actionStringParam, actionCategoryNumber or actionOperatorNumber.
	//	Where it really goes depends on the action.
	return actionParameter;
}
void Softkey::setActionParameter(const HobString &param)
{
	//	This variable will contain either the actionIntParam, actionStringParam, actionCategoryNumber or actionOperatorNumber.
	//	Where it really goes depends on the action.
	actionParameter = param;

	if (Debug::isTracing("Softkey"))
		Debug::trace("Softkey.Tare", HobString("setActionParameter():  actionParameter = ") + actionParameter);
}

void Softkey::parseActionParameter()
{
	bool traceOn = Debug::isTracing("Softkey");
	HobString msgDebug("");

	if (traceOn)
	{
		msgDebug = "parseActionParameter():   action = ";
		msgDebug.append(Integer::toString(action)).append(",  ");
	}

	//	This method takes the actionParameter and processes the string according to the action.
	//	This is called after the message from the browser has been parsed.
	switch (action)
	{
		case Action_ApplyDiscount_PercentOff:
		{
			double newPercent = Double::valueOf(actionParameter);
			if (actionParameter.find(".") != std::string::npos)
				newPercent *= Power::power((double) 10.0, (double) 2.0);
			double newPercent2 = Rounding::hobround(newPercent);
			setActionIntegerParam((int)newPercent2);
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Action_ApplyDiscount_PercentOff, actionIntParam = ").append(Integer::toString(actionIntParam));
				Debug::trace("Softkey", msgDebug2);
			}
		}
		break;

		case Action_ApplyDiscount_CentsOff:
		{
			setActionIntegerParam( (int) MetSig::determineImplicitDecimalPrice(actionParameter) );
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Action_ApplyDiscount_CentsOff, actionIntParam = ").append(Integer::toString(actionIntParam));
				Debug::trace("Softkey", msgDebug2);
			}
		}
		break;
		case Action_Swipe:
		case Action_ChangeDepartment:
		case Action_SwitchTotalType:
		case Action_ChangeLanguage:			//	Number or string???
		case Action_ChangeScreen:			//	Screen numberf
		case Action_LoadSoftkeyGroup:		//	Group number
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("calling setActionIntegerParam");
				Debug::trace("Softkey", msgDebug2);
			}
			setActionIntegerParam(Long::valueOf(actionParameter));
			break;

		case Action_LoadCategory_First:
		case Action_LoadCategory_Next:
		case Action_LoadCategory_Previous:
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("calling setActionCategoryNumber");
				Debug::trace("Softkey", msgDebug2);
			}
			setActionCategoryNumber((HobRecordNum) Long::valueOf(actionParameter));
			break;

		case Action_LoginOperator:
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("calling setActionOperatorNumber");
				Debug::trace("Softkey", msgDebug2);
			}
			setActionOperatorNumber(actionParameter);
			break;

		case Action_SelectProduct:
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("calling setActionProductNumber");
				Debug::trace("Softkey", msgDebug2);
			}
			setActionProductNumber((HobRecordNum) Long::valueOf(actionParameter));
			break;

		case Action_BrowseWeb:
		case Action_FPP_URL:
		case Action_Application_URL:
		case Action_LoadSearch_First:
		case Action_LoadSearch_Next:
		case Action_LoadSearch_Previous:
		case Action_PlayVideo:
		case Action_LaunchExternalApp:
		case Action_ForcedCameraOverride:
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("calling setActionStringParam(");
				msgDebug2.append(actionParameter);
				msgDebug2.append(")");
				Debug::trace("Softkey", msgDebug2);
			}
			setActionStringParam(actionParameter);
			break;

		case Action_LoadAlphabeticSearch_First:
		case Action_LoadAlphabeticSearch_Next:
		case Action_LoadAlphabeticSearch_Previous:
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("calling setActionCategoryNumber");
				Debug::trace("Softkey", msgDebug2);
			}
			setActionStringParam(actionParameter);
			break;

		case Action_LoadTareSoftkey:
		{
			if (traceOn)
				Debug::trace("Softkey", msgDebug + "Action_LoadTareSoftkey");

			Debug::trace("Softkey", "Action_LoadTareSoftkey actionParameter: " + actionParameter);
			
			double newPercent = Double::valueOf(actionParameter);
			
			Debug::trace("Softkey", "Action_LoadTareSoftkey newPercent: " + util_string::doubletostring(newPercent));

			if (actionParameter.find(".") != std::string::npos)
#ifdef CHECK_FT
			// FT-541
			// Always store preset tare to 3 decimal precision... we will correct as per current
			// weight settings when flashkey is used.
				newPercent *= Power::power((double) 10.0, (double) 3);
#else
				newPercent *= Power::power((double) 10.0, (double) Configuration::getInstance()->getWeightPrecision());		//	GCDR-115
#endif
			Debug::trace("Softkey", "Action_LoadTareSoftkey newerPercent: " + util_string::doubletostring(newPercent));
			
			double newPercent2 = Rounding::hobround(newPercent);
			Debug::trace("Softkey", "Action_LoadTareSoftkey newPercent2: " + util_string::doubletostring(newPercent2));
			setActionIntegerParam((int)newPercent2);
		}
		break;

		case Action_ChangeLabelPlacementAndPosition:
		{
			if (traceOn)
				Debug::trace("Softkey.Placement", msgDebug + "Action_ChangeLabelPlacementAndPosition");
			if (isReceived(fieldOffsetSKeyActionStringParam))
			{
				setActionIntegerParam(getPlacementAndPositionAction(actionStringParam));
				//setSecondActionIntegerParam(getsecondPlacementAndPositionAction(secondActionStringParam));
			}
			else
			{
				setActionIntegerParam(Integer::valueOf(getActionParameter()));
				//setSecondActionIntegerParam(Integer::valueOf(getSecondActionParameter()));
			}
			break;
		}

		case Action_ToggleLabelType:
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Action_ToggleLabelType - actionStringParam = [");
				msgDebug2.append(actionStringParam);
				msgDebug2.append("]");
				Debug::trace("Softkey.Clare", msgDebug2);
			}
		default:
			break;
	}
}


void Softkey::determineActionParameter(HobString &result, int format) const
{
	bool traceOn = Debug::isTracing("Softkey.ActionParam");
	HobString msgDebug("");
	result = "";

	if (traceOn)
		msgDebug.append("determineActionParameter():   ");

	switch (action)
	{
		case Action_ApplyDiscount_PercentOff:
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Action_ApplyDiscount_PercentOff, actionIntParam = ").append(Integer::toString(actionIntParam));
				Debug::trace("Softkey.ActionParam", msgDebug2);
			}
			if ((format == BrowserFormat) || (format == BrowserFormatPLUOperateMode))
				Configuration::getInstance()->formatDecimalValue(actionIntParam, result, 2);
			else
				Configuration::getInstance()->formatDecimalValue(actionIntParam, result, 2, false);
		}
		break;

		case Action_ApplyDiscount_CentsOff:
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Action_ApplyDiscount_CentsOff, actionIntParam = ").append(Integer::toString(actionIntParam));
				Debug::trace("Softkey.ActionParam", msgDebug2);
			}
			if ((format == BrowserFormat) || (format == BrowserFormatPLUOperateMode))
				result = Configuration::getInstance()->formatHobPrice(actionIntParam);
			else
				result = Configuration::getInstance()->formatHobPrice(actionIntParam, false);
		}
		break;

        case Action_Swipe:
		case Action_ChangeLanguage:			//	Number or string???
		case Action_ChangeDepartment:
		case Action_ChangeScreen:			//	Screen number
		case Action_LoadSoftkeyGroup:		//	Group number
			result = Integer::toString(actionIntParam);
			break;

		case Action_LoadCategory_First:
		case Action_LoadCategory_Next:
		case Action_LoadCategory_Previous:
			result = Long::toString(actionCategoryNumber);
			break;

		case Action_LoginOperator:
			result = actionOperatorNumber;
			break;

		case Action_SelectProduct:
			result = Long::toString(actionProductNumber);
			break;

		case Action_SwitchTotalType:
			Debug::trace("Softkey",HobString("determineActionParameter ") + Integer::toString(actionIntParam));
			result = Integer::toString(actionIntParam);
			break;

		case Action_BrowseWeb:
		case Action_FPP_URL:
		case Action_Application_URL:
		case Action_LoadSearch_First:
		case Action_LoadSearch_Next:
		case Action_LoadSearch_Previous:
		case Action_PlayVideo:
		case Action_LaunchExternalApp:
		case Action_ForcedCameraOverride:
			result = actionStringParam;
			break;

		case Action_LoadAlphabeticSearch_First:
		case Action_LoadAlphabeticSearch_Next:
		case Action_LoadAlphabeticSearch_Previous:
			result = actionStringParam;
			break;
		
		case Action_LoadTareSoftkey:			// ALV - 04/2009
		{
			Debug::trace("Softkey.Tare",HobString("determineActionParameter(Action_LoadTareSoftkey):  actionParameter = ") + Integer::toString(actionIntParam));

			// FT-541
			// Correct stored tare to current tare precision.
			double tareValue = actionIntParam;
			Debug::trace("Softkey.Tare", "Incoming actionIntParam and make it a double: " + util_string::doubletostring(tareValue) );

			if ( Configuration::getInstance()->getEffectiveWeightPrecision() == 2 )
			{
				// Need to round the preset tare flashkey's value to 2 decimal places, then make it 3 implied decimal places again.
				// The subsequent call to formatWeight will get rid of the third decimal place.
				tareValue = MetSig::roundWeightTo2DecimalsThenBackTo3(tareValue);
				Debug::trace("SoftkeyManager.Tare", "Action_LoadTareSoftkey - weigher is in SINGLE RANGE mode.");
			}
			else
			{
				Debug::trace("SoftkeyManager.Tare", "Action_LoadTareSoftkey - weigher is in DUAL RANGE mode.");
			}

			long tareValueLong = (long) Rounding::hobround(tareValue);
			Debug::trace("Softkey.Tare", "Make a tareValueLong:" + Long::toString(tareValueLong) );

			if ((format == BrowserFormat) || (format == BrowserFormatPLUOperateMode))
			{
				Debug::trace("Softkey.Tare", "Call formatWeight IF");
				Configuration::getInstance()->formatWeight(tareValueLong, result);
			}
			else
			{
				Debug::trace("Softkey.Tare", "formatWeight ELSE");
				Configuration::getInstance()->formatWeight(tareValueLong, result, false);
			}
			Debug::trace("Softkey.Tare","determineActionParameter(Action_LoadTareSoftkey):  FINAL result = " + result);
		}
		break;

		default:
			break;
	}
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("action = ").append(Integer::toString(action));
		msgDebug2.append("   result = ").append(result);
		Debug::trace("Softkey.ActionParam", msgDebug2);
	}
}

const HobString &Softkey::getTextColor() const
{
	return textColor;
}
void Softkey::setTextColor(const HobString &color)
{
	textColor = color;
}

bool Softkey::getUseDefaultBackgroundColor() const
{
	return useDefaultBgColor;
}
void Softkey::setUseDefaultBackgroundColor(bool useDefault)
{
	useDefaultBgColor = useDefault;
}

bool Softkey::getUseDefaultBackgroundImage() const
{
	return useDefaultBgImage;
}
void Softkey::setUseDefaultBackgroundImage(bool useDefault)
{
	useDefaultBgImage = useDefault;
}

bool Softkey::getUseDefaultTextColor() const
{
	return useDefaultTextColor;
}
void Softkey::setUseDefaultTextColor(bool useDefault)
{
	useDefaultTextColor = useDefault;
}

bool Softkey::getUseDefaultName1() const
{
	return useDefaultName1;
}
void Softkey::setUseDefaultName1(bool useDefault)
{
	useDefaultName1 = useDefault;
}

bool Softkey::getUseDefaultName2() const
{
	return useDefaultName2;
}
void Softkey::setUseDefaultName2(bool useDefault)
{
	useDefaultName2 = useDefault;
}

bool Softkey::isSoftkeyEnabled() const
{
	return softkeyEnabled;
}
void Softkey::setSoftkeyEnabled(bool value)
{
	softkeyEnabled = value;
}

const HobString &Softkey::getDefaultForegroundImage() const
{
	return defaultForegroundImage;
}
void Softkey::setDefaultForegroundImage(const HobString &image)
{
	defaultForegroundImage = image;
}

const HobString &Softkey::getScriptName() const
{
	return scriptName;
}

void Softkey::setScriptName(const HobString &name)
{
	scriptName = name;
}

int Softkey::getScriptInterval() const
{
	return scriptInterval;
}

void Softkey::setScriptInterval(HobRecordNum interval)
{
	scriptInterval = interval;
}

int Softkey::updateField(int fieldCode, const HobString &token, int format)
{
	if (Debug::isTracing("Softkey"))
	{
		HobString msgDebug("updateField() - fieldCode: ");
		msgDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("Softkey", msgDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetSKeyGroupNum:
			setGroupNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKeyDeptNumber:
			setDeptNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKeyHorizontalPos:
			setHorizontalPosition(Integer::valueOf(token));
			break;
		case fieldOffsetSKeyVerticalPos:
			setVerticalPosition(Integer::valueOf(token));
			break;
		case fieldOffsetSKeyHorizontalSize:
			setHorizontalSize(Integer::valueOf(token));
			break;
		case fieldOffsetSKeyVerticalSize:
			setVerticalSize(Integer::valueOf(token));
			break;

		case fieldOffsetSKeyBackgroundColor:
			setBackgroundColor(token);
			break;
		case fieldOffsetSKeyToggledBackgroundColor:
			setToggledBackgroundColor(token);
			break;
		case fieldOffsetSKeyBgColorUseDefault:
			setUseDefaultBackgroundColor(Bool::valueOf(token));
			break;

		case fieldOffsetSKeyBackgroundImage:
		{
			Filename gFilename(token);
			HobString gName = gFilename.getFileName();
			if (gName.compare(Configuration::getInstance()->toStringCfgI(CfgInternal::NOGRAPHICSELECTEDNAME)) == 0)
				setBackgroundImage("");
			else
				setBackgroundImage(gName);
			break;
		}
		case fieldOffsetSKeyToggledBackgroundImage:
		{
			Filename gFilename(token);
			HobString gName = gFilename.getFileName();
			if (gName.compare(Configuration::getInstance()->toStringCfgI(CfgInternal::NOGRAPHICSELECTEDNAME)) == 0)
				setToggledBackgroundImage("");
			else
				setToggledBackgroundImage(gName);
			break;
		}
		case fieldOffsetSKeyBgImageUseDefault:
			setUseDefaultBackgroundImage(Bool::valueOf(token));
			break;

		case fieldOffsetSKeyTextColor:
			setTextColor(token);
			break;
		case fieldOffsetSKeyTextColorUseDefault:
			setUseDefaultTextColor(Bool::valueOf(token));
			break;

		case fieldOffsetSKeyFlashkeyName:
			setName1(token);
			break;

		case fieldOffsetSKeyName1:
		{
			if ((token == " ") && (format == EvolutionTaggedFormat))
				forceNoText = true;
			setName1(token);
			break;
		}
		case fieldOffsetSKeyName1UseDefault:
			setUseDefaultName1(Bool::valueOf(token));
			break;

		case fieldOffsetSKeyName2:
		{
			if ((token == " ") && (format == EvolutionTaggedFormat))
				forceNoText = true;
			setName2(token);
			break;
		}
		case fieldOffsetSKeyName2UseDefault:
			setUseDefaultName2(Bool::valueOf(token));
			break;

		case fieldOffsetSKeyAction:
			setAction(Integer::valueOf(token));
			break;
		case fieldOffsetSKeyActionIntegerParam:
			setActionIntegerParam(Integer::valueOf(token));
			break;
		case fieldOffsetSKeyActionStringParam:
			setActionStringParam(token);
			break;
		case fieldOffsetSKeyActionParameter:
			setActionParameter(token);
			break;
		case fieldOffsetSKeyActionCategoryNum:
			setActionCategoryNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKeyActionOperatorNum:
			setActionOperatorNumber(token);
			break;
		case fieldOffsetSKeyActionProductNum:
			setActionProductNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKeyActionFeatureNum:
			if (token.size())
				setActionFeatureNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKeyFlashkeyNum:
			setFlashkeyNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKeyFeatureNum:
			if (token.size())
				setFeatureNumber((HobRecordNum)Long::valueOf(token));
			break;

		case fieldOffsetSKeyForceNoBgColor:
			setForceNoBackgroundColor(Bool::valueOf(token));
			break;
		case fieldOffsetSKeyForceNoBgImage:
			setForceNoBackgroundImage(Bool::valueOf(token));
			break;
		case fieldOffsetSKeyForceNoText:
			setForceNoText(Bool::valueOf(token));
			break;

		case fieldOffsetSKeyScriptName:
			setScriptName(token);
			break;
		case fieldOffsetSKeyScriptInterval:
			setScriptInterval((HobRecordNum)Long::valueOf(token));
			break;

		default:
			break;
	}

	return 0;
}



bool Softkey::update(const Softkey *newSoftkey)
{
	bool traceOn = Debug::isTracing("Softkey.update");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "update():  ";
	bool softkeyChanged = false;

	if (!newSoftkey)
		return softkeyChanged;

	if (traceOn)
		Debug::trace("Softkey", "Entering update ...");

	short loopCnt = 1;
	while (loopCnt < endSoftkeyFieldOffsets)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("loopCnt = ").append(Integer::toString(loopCnt));
			msgDebug2.append("  isReceived = ").append(Bool::toString(newSoftkey->isReceived(loopCnt)));
			Debug::trace("Softkey.update", msgDebug2);
		}
		if (newSoftkey->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetSKeyGroupNum:
					setGroupNumber(newSoftkey->getGroupNumber());
					break;
				case fieldOffsetSKeyDeptNumber:
					setDeptNum(newSoftkey->getDeptNum());
				case fieldOffsetSKeyHorizontalPos:
					if (getHorizontalPosition() != newSoftkey->getHorizontalPosition())
					{
						setHorizontalPosition(newSoftkey->getHorizontalPosition());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyVerticalPos:
					if (getVerticalPosition() != newSoftkey->getVerticalPosition())
					{
						setVerticalPosition(newSoftkey->getVerticalPosition());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyHorizontalSize:
					if (getHorizontalSize() != newSoftkey->getHorizontalSize())
					{
						setHorizontalSize(newSoftkey->getHorizontalSize());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyVerticalSize:
					if (getVerticalSize() != newSoftkey->getVerticalSize())
					{
						setVerticalSize(newSoftkey->getVerticalSize());
						softkeyChanged = true;
					}
					break;

				case fieldOffsetSKeyBackgroundColor:
					if (getBackgroundColor() != newSoftkey->getBackgroundColor())
					{
						setBackgroundColor(newSoftkey->getBackgroundColor());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyToggledBackgroundColor:
					if (getToggledBackgroundColor() != newSoftkey->getToggledBackgroundColor())
					{
						setToggledBackgroundColor(newSoftkey->getToggledBackgroundColor());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyBgColorUseDefault:
					if (getUseDefaultBackgroundColor() != newSoftkey->getUseDefaultBackgroundColor())
					{
						setUseDefaultBackgroundColor(newSoftkey->getUseDefaultBackgroundColor());
						softkeyChanged = true;
					}
					break;

				case fieldOffsetSKeyBackgroundImage:
					if (getBackgroundImage() != newSoftkey->getBackgroundImage())
					{
						setBackgroundImage(newSoftkey->getBackgroundImage());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyToggledBackgroundImage:
					if (getToggledBackgroundImage() != newSoftkey->getToggledBackgroundImage())
					{
						setToggledBackgroundImage(newSoftkey->getToggledBackgroundImage());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyBgImageUseDefault:
					if (getUseDefaultBackgroundImage() != newSoftkey->getUseDefaultBackgroundImage())
					{
						setUseDefaultBackgroundImage(newSoftkey->getUseDefaultBackgroundImage());
						softkeyChanged = true;
					}
					break;

				case fieldOffsetSKeyTextColor:
					if (getTextColor() != newSoftkey->getTextColor())
					{
						setTextColor(newSoftkey->getTextColor());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyTextColorUseDefault:
					if (getUseDefaultTextColor() != newSoftkey->getUseDefaultTextColor())
					{
						setUseDefaultTextColor(newSoftkey->getUseDefaultTextColor());
						softkeyChanged = true;
					}
					break;

				case fieldOffsetSKeyFlashkeyName:
				case fieldOffsetSKeyName1:
					if (getName1() != newSoftkey->getName1())
					{
						setName1(newSoftkey->getName1());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyName1UseDefault:
					if (getUseDefaultName1() != newSoftkey->getUseDefaultName1())
					{
						setUseDefaultName1(newSoftkey->getUseDefaultName1());
						softkeyChanged = true;
					}
					break;

				case fieldOffsetSKeyName2:
					if (getName2() != newSoftkey->getName2())
					{
						setName2(newSoftkey->getName2());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyName2UseDefault:
					if (getUseDefaultName2() != newSoftkey->getUseDefaultName2())
					{
						setUseDefaultName2(newSoftkey->getUseDefaultName2());
						softkeyChanged = true;
					}
					break;

				case fieldOffsetSKeyAction:
					if (getAction() != newSoftkey->getAction())
					{
						setAction(newSoftkey->getAction());
						softkeyChanged = true;
					}
					break;

				case fieldOffsetSKeyActionIntegerParam:		//	via Comm
					if (getActionIntegerParam() != newSoftkey->getActionIntegerParam())
					{
						setActionIntegerParam(newSoftkey->getActionIntegerParam());
						softkeyChanged = true;
					}
					break;

				case fieldOffsetSKeyActionStringParam:
				{
					if ((getAction() == Action_ChangeLabelPlacementAndPosition) && (getActionIntegerParam() != newSoftkey->getActionIntegerParam()))
					{
						//	Since this is a compound action returned via the action string, we need to set
						//	the actionIntegerParam here. parseActionParameter() should have already been called
						//	to process the string to an integer value.
						setActionIntegerParam(newSoftkey->getActionIntegerParam());
						softkeyChanged = true;
					}
					
					if (getActionStringParam() != newSoftkey->getActionStringParam())
					{
						setActionStringParam(newSoftkey->getActionStringParam());
						softkeyChanged = true;
					}
					break;
				}

				case fieldOffsetSKeyActionParameter:		//	via the Browser
				{
					switch (newSoftkey->getAction())
					{
						case Action_ApplyDiscount_PercentOff:
						case Action_ApplyDiscount_CentsOff:
						case Action_ChangeLanguage:			//	Number or string???
						case Action_ChangeScreen:			//	Screen number
						case Action_ChangeDepartment:
						case Action_LoadSoftkeyGroup:		//	Group number
						case Action_Swipe:
							if (getActionIntegerParam() != newSoftkey->getActionIntegerParam())
							{
								setActionIntegerParam(newSoftkey->getActionIntegerParam());
								softkeyChanged = true;
								if (traceOn)
								{
									HobString msgDebug2 = msgDebug;
									msgDebug2.append("New actionIntParam = ").append(Integer::toString(actionIntParam));
									Debug::trace("Softkey.update", msgDebug2);
								}
							}
							break;
						case Action_LoadCategory_First:
						case Action_LoadCategory_Next:
						case Action_LoadCategory_Previous:
							if (getActionCategoryNumber() != newSoftkey->getActionCategoryNumber())
							{
								setActionCategoryNumber(newSoftkey->getActionCategoryNumber());
								softkeyChanged = true;
								if (traceOn)
								{
									HobString msgDebug2 = msgDebug;
									msgDebug2.append("New actionCategoryNumber = ").append(Integer::toString(actionCategoryNumber));
									Debug::trace("Softkey.update", msgDebug2);
								}
							}
							break;
						case Action_LoginOperator:
							if (getActionOperatorNumber() != newSoftkey->getActionOperatorNumber())
							{
								setActionOperatorNumber(newSoftkey->getActionOperatorNumber());
								softkeyChanged = true;
								if (traceOn)
								{
									HobString msgDebug2 = msgDebug;
									msgDebug2.append("New actionOperatorNumber = ").append(actionOperatorNumber);
									Debug::trace("Softkey.update", msgDebug2);
								}
							}
							break;
						case Action_SelectProduct:
							if (getActionProductNumber() != newSoftkey->getActionProductNumber())
							{
								setActionProductNumber(newSoftkey->getActionProductNumber());
								softkeyChanged = true;
								if (traceOn)
								{
									HobString msgDebug2 = msgDebug;
									msgDebug2.append("New actionProductNumber = ").append(Integer::toString(actionProductNumber));
									Debug::trace("Softkey.update", msgDebug2);
								}
							}
							break;
						case Action_BrowseWeb:
						case Action_FPP_URL:
						case Action_Application_URL:
						case Action_PlayVideo:
						case Action_LaunchExternalApp:
						case Action_ForcedCameraOverride:
							if (getActionStringParam() != newSoftkey->getActionStringParam())
							{
								setActionStringParam(newSoftkey->getActionStringParam());
								softkeyChanged = true;
								if (traceOn)
								{
									HobString msgDebug2 = msgDebug;
									msgDebug2.append("New actionStringParam = ").append(actionStringParam);
									Debug::trace("Softkey.update", msgDebug2);
								}
							}
							break;
						case Action_LoadAlphabeticSearch_First:
						case Action_LoadAlphabeticSearch_Next:
						case Action_LoadAlphabeticSearch_Previous:
							if (getActionStringParam() != newSoftkey->getActionStringParam())
							{
								setActionStringParam(newSoftkey->getActionStringParam());
								softkeyChanged = true;
								if (traceOn)
								{
									HobString msgDebug2 = msgDebug;
									msgDebug2.append("New actionStringParam = ").append(actionStringParam);
									Debug::trace("Softkey.update", msgDebug2);
								}
							}
							break;
						
						case Action_SwitchTotalType:
							if (getActionIntegerParam() != newSoftkey->getActionIntegerParam())
							{
								setActionIntegerParam(newSoftkey->getActionIntegerParam());
								softkeyChanged = true;
							}
							break;

						case Action_LoadTareSoftkey:
							if (getActionIntegerParam() != newSoftkey->getActionIntegerParam())
							{
								setActionIntegerParam(newSoftkey->getActionIntegerParam());
								softkeyChanged = true;
							}
							break;

						default:
							break;
					}
					break;
				}

				case fieldOffsetSKeyActionFeatureNum:
					if (getActionFeatureNumber() != newSoftkey->getActionFeatureNumber())
					{
						setActionFeatureNumber(newSoftkey->getActionFeatureNumber());
						softkeyChanged = true;
					}
					break;

				case fieldOffsetSKeyFeatureNum:
					if (getFeatureNumber() != newSoftkey->getFeatureNumber())
					{
						setFeatureNumber(newSoftkey->getFeatureNumber());
						softkeyChanged = true;
					}
					break;

//				case fieldOffsetSKeyFlashkeyNum:
//					setFlashkeyNumber(newSoftkey->getFlashkeyNumber());
//					break;

				case fieldOffsetSKeyForceNoBgColor:
					if (getForceNoBackgroundColor() != newSoftkey->getForceNoBackgroundColor())
					{
						setForceNoBackgroundColor(newSoftkey->getForceNoBackgroundColor());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyForceNoBgImage:
					if (getForceNoBackgroundImage() != newSoftkey->getForceNoBackgroundImage())
					{
						setForceNoBackgroundImage(newSoftkey->getForceNoBackgroundImage());
						softkeyChanged = true;
					}
					break;
				case fieldOffsetSKeyForceNoText:
					if (getForceNoText() != newSoftkey->getForceNoText())
					{
						setForceNoText(newSoftkey->getForceNoText());
						softkeyChanged = true;
					}
					break;

				case fieldOffsetSKeyActionCategoryNum:
					if (getActionCategoryNumber() != newSoftkey->getActionCategoryNumber())
					{
						setActionCategoryNumber(newSoftkey->getActionCategoryNumber());
						softkeyChanged = true;
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("New actionCategoryNumber = ").append(Integer::toString(actionCategoryNumber));
							Debug::trace("Softkey.update", msgDebug2);
						}
					}
					break;

				case fieldOffsetSKeyActionOperatorNum:
					if (getActionOperatorNumber() != newSoftkey->getActionOperatorNumber())
					{
						setActionOperatorNumber(newSoftkey->getActionOperatorNumber());
						softkeyChanged = true;
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("New actionOperatorNumber = ").append(actionOperatorNumber);
							Debug::trace("Softkey.update", msgDebug2);
						}
					}
					break;

				case fieldOffsetSKeyActionProductNum:
					if (getActionProductNumber() != newSoftkey->getActionProductNumber())
					{
						setActionProductNumber(newSoftkey->getActionProductNumber());
						softkeyChanged = true;
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("New actionProductNumber = ").append(Integer::toString(actionProductNumber));
							Debug::trace("Softkey.update", msgDebug2);
						}
					}
					break;

				case fieldOffsetSKeyScriptName:
					if (getScriptName() != newSoftkey->getScriptName())
					{
						setScriptName(newSoftkey->getScriptName());
						softkeyChanged = true;
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("New scriptName = ").append(scriptName);
							Debug::trace("Softkey.update", msgDebug2);
						}
					}
					break;

				case fieldOffsetSKeyScriptInterval:
					if (getScriptInterval() != newSoftkey->getScriptInterval())
					{
						setScriptInterval(newSoftkey->getScriptInterval());
						softkeyChanged = true;
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug;
							msgDebug2.append("New scriptInterval = ").append(Integer::toString(scriptInterval));
							Debug::trace("Softkey.update", msgDebug2);
						}
					}
					break;

				default:
					break;
			}
		}
		loopCnt++;
	}
	Debug::trace("Softkey", "Leaving update ...");
	return softkeyChanged;
}


void Softkey::getFieldAsString(int fieldCode, HobString &result, int format) const
{
	result = "";

	bool traceOn = Debug::isTracing("Softkey");

	hfc::MultiLanguageText internationalize;

	switch (fieldCode)
	{
		case fieldOffsetSKeyGroupNum:
			if (groupNumber > 0)
				result = Long::toString(groupNumber);
			break;
		case fieldOffsetSKeyDeptNumber:
			result = Long::toString(deptNum);
			break;
		case fieldOffsetSKeyHorizontalPos:
			result = Integer::toString(horizontalPosition);
			break;
		case fieldOffsetSKeyVerticalPos:
			result = Integer::toString(verticalPosition);
			break;
		case fieldOffsetSKeyHorizontalSize:
			result = Integer::toString(horizontalSize);
			break;
		case fieldOffsetSKeyVerticalSize:
			result = Integer::toString(verticalSize);
			break;

		case fieldOffsetSKeyBackgroundColor:
			result = backgroundColor;
			break;
		case fieldOffsetSKeyToggledBackgroundColor:
			result = toggledBackgroundColor;
			break;
		case fieldOffsetSKeyBgColorDefault:
			result = defaultBackgroundColor;
			break;
		case fieldOffsetSKeyBgColorUseDefault:
			result = Bool::toString(useDefaultBgColor);
			break;

		case fieldOffsetSKeyBackgroundImage:
		{
			if ((format == BrowserFormat) && !bgImageExists)
			{
				//	To avoid browser errors, we don't want to try to display a graphic that does not exist.
				result = "";
			}
			else
			{
				result = backgroundImage;
			}
			break;
		}
		case fieldOffsetSKeyToggledBackgroundImage:
		{
			if ((format == BrowserFormat) && !bgImageExists)
			{
				//	To avoid browser errors, we don't want to try to display a graphic that does not exist.
				result = "";
			}
			else
			{
				result = toggledBackgroundImage;
			}
			break;
		}
		case fieldOffsetSKeyBgImageDefault:
			result = defaultBackgroundImage;
			break;
		case fieldOffsetSKeyBgImageUseDefault:
			result = Bool::toString(useDefaultBgImage);
			break;

		case fieldOffsetSKeyTextColor:
			result = textColor;
			break;
		case fieldOffsetSKeyTextColorDefault:
			result = defaultTextColor;
			break;
		case fieldOffsetSKeyTextColorUseDefault:
			result = Bool::toString(useDefaultTextColor);
			break;

		case fieldOffsetSKeyFlashkeyName:
			result = name1;
			break;

		case fieldOffsetSKeyName1:
		{
			if (forceNoText && (format == EvolutionTaggedFormat))
				result = " ";
			else if ((ReservedSK_SelfservAZCategoryButtons == groupNumber) && (Action_LoadMostCommon_First == action))
				result = internationalize("Softkey.BestSellers", PrimaryLanguage);
			else
				result = name1;
			break;
		}
		case fieldOffsetSKeyName1Default:
			result = defaultName1;
			break;
		case fieldOffsetSKeyName1UseDefault:
			result = Bool::toString(useDefaultName1);
			break;

		case fieldOffsetSKeyName2:
		{
			if (forceNoText && (format == EvolutionTaggedFormat))
				result = " ";
			else if ((ReservedSK_SelfservAZCategoryButtons == groupNumber) && (Action_LoadMostCommon_First == action))
				result = internationalize("Softkey.BestSellers", SecondaryLanguage);
			else
				result = name2;
			break;
		}
		case fieldOffsetSKeyName2Default:
			result = defaultName2;
			break;
		case fieldOffsetSKeyName2UseDefault:
			result = Bool::toString(useDefaultName2);
			break;

		case fieldOffsetSKeyAction:
			result = Integer::toString(action);
			break;
		case fieldOffsetSKeyActionIntegerParam:
			result = Integer::toString(actionIntParam);
			break;
		case fieldOffsetSKeyActionStringParam:
			result = actionStringParam;
			break;
		case fieldOffsetSKeyActionParameter:
			{	
				if (traceOn)
					Debug::trace("Softkey","getFieldAsString = "+ result);
				determineActionParameter(result, format);
				if (traceOn)
					Debug::trace("Softkey","getFieldAsString After =" + result);
				break;
			}
			break;
		case fieldOffsetSKeyActionCategoryNum:
			if (actionCategoryNumber > 0)
				result = Long::toString(actionCategoryNumber);
			break;
		case fieldOffsetSKeyActionOperatorNum:
			if (actionOperatorNumber.size() > 0)
				result = actionOperatorNumber;
			break;
		case fieldOffsetSKeyActionProductNum:
			if (actionProductNumber > 0)
				result = Long::toString(actionProductNumber);
			break;
		case fieldOffsetSKeyActionFeatureNum:
			if (actionFeatureNumber >= 0)
				result = Long::toString(actionFeatureNumber);
			break;
		case fieldOffsetSKeyFlashkeyNum:
			result = Long::toString(flashkeyNumber);
			break;

		case fieldOffsetSKeyForegroundImage:
			if ((format == BrowserFormat) && !fgImageExists)
			{
				//	To avoid browser errors, we don't want to try to display a graphic that does not exist.
				result = "";
			}
			else
			{
				result = defaultForegroundImage;
			}
			break;

		case fieldOffsetSKeyFeatureNum:
			if (featureNumber >= 0)
				result = Long::toString(featureNumber);
			break;

		case fieldOffsetSKeyForceNoBgColor:
			result = Bool::toString(forceNoBgColor);
			break;
		case fieldOffsetSKeyForceNoBgImage:
			result = Bool::toString(forceNoBgImage);
			break;
		case fieldOffsetSKeyForceNoText:
			result = Bool::toString(forceNoText);
			break;

		case fieldOffsetSKeyScriptName:
			if (traceOn) Debug::trace("Softkey", "scriptName stringify: " + scriptName);
			result = scriptName;
			break;

		case fieldOffsetSKeyScriptInterval:
			if (traceOn) Debug::trace("Softkey", "scriptInterval stringify: " + scriptInterval);
			result = Integer::toString(scriptInterval);
			break;

		default:
			break;
	}

	if (traceOn)
	{
		HobString msgDebug("getFieldAsString():   fieldCode = ");
		msgDebug.append(Integer::toString(fieldCode));
		msgDebug.append("   format = ").append(Integer::toString(format));
		msgDebug.append("   result = [").append(result).append("]");
		Debug::trace("Softkey", msgDebug);
	}
}

Softkey *Softkey::find(HobRecordNum groupNum, HobRecordNum deptNum, int horizPos, int verticalPos, DBAccess *dbAccess, short languageSelection)
{
	Softkey *readSoftkey = 0;
	bool traceOn = Debug::isTracing("Softkey");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug = "find(";
		msgDebug.append(Long::toString(groupNum)).append(", ").append(Long::toString(deptNum)).append(", ").append(Integer::toString(horizPos)).append(", ").append(Integer::toString(verticalPos)).append(")  ");
	}

	if (!dbAccess)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbAccess is NULL! Exiting!!!");
			Debug::trace("Softkey", msgDebug2);
		}
		return readSoftkey;
	}

	DBSoftKeysRM *dbSoftkeysRM = dbAccess->getDBSoftKeysRM();
	if (!dbSoftkeysRM)
	{
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbSoftkeysRM is NULL! Exiting!!!");
			Debug::trace("Softkey", msgDebug2);
		}
		return readSoftkey;
	}

	HobString num(Long::toString(groupNum));
	HobString dNum(Long::toString(deptNum));
	HobString hPos(Integer::toString(horizPos));
	HobString vPos(Integer::toString(verticalPos));
	try
	{
		DBSoftKey *dbSoftkeyRec = dbSoftkeysRM->find(num, dNum, hPos, vPos);
		if (traceOn)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("dbSoftkeyRec = dbSoftkeysRM->find(").append(num).append(", ").append(dNum).append(", ").append(hPos).append(", ");
			msgDebug2.append(vPos).append(") = ").append(ptrtostring(dbSoftkeyRec));
			Debug::trace("Softkey", msgDebug2);
		}
		if (!dbSoftkeyRec)
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Record not found in the db.");
				Debug::trace("Softkey", msgDebug2);
			}
		}
		else
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Record was found in the db.");
				Debug::trace("Softkey", msgDebug2);
			}
			readSoftkey = new Softkey(dbSoftkeyRec, dbAccess, languageSelection);
			if (!readSoftkey)
			{
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("But I can't create a new Softkey object!");
					Debug::trace("Softkey", msgDebug2);
				}
				delete dbSoftkeyRec;
			}
			else
			{
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("Created the hfc Softkey object: ").append(ptrtostring(readSoftkey));
					Debug::trace("Softkey", msgDebug2);
				}
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "Softkey::find() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString msgDebug("     DBException: type: ");
			msgDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", msgDebug);
		}
	}

	return readSoftkey;
}

int Softkey::deleteRecord()
{
	bool traceOn = Debug::isTracing("Softkey");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "deleteRecord()";

	int recordsDeleted = 0;

	if (myDbRecord && dbAccess)
	{
		DBSoftKeysRM *dbSoftkeysRM = dbAccess->getDBSoftKeysRM();
		if (dbSoftkeysRM)
		{
			recordsDeleted = dbSoftkeysRM->deleteRecord(myDbRecord);
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Record was deleted.");
				Debug::trace("Softkey", msgDebug2);
			}
		}
		else
		{
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Cannot Delete this softkey!");
				Debug::trace("Softkey", msgDebug2);
			}
		}
	}
	return recordsDeleted;
}

void Softkey::saveRecord()
{
	DBProduct *dbProductRec = 0;
	bool traceOn = Debug::isTracing("Softkey");
	bool traceOnSave = Debug::isTracing("Softkey.save");
	HobString msgDebug = "";
	
	if (traceOn || traceOnSave)
	{
		msgDebug = "saveRecord()";
		Debug::trace("Softkey", "saveRecord()");
		msgDebug.append(":  ");
	}
	
	if (dbAccess)
	{
		DBSoftKeysRM *dbSoftkeysRM = dbAccess->getDBSoftKeysRM();
		if (!dbSoftkeysRM)
			return;

		if (!myDbRecord)
		{
			if (traceOn)
				Debug::trace("Softkey", msgDebug + "Don't have a db record, creating it...");
			myDbRecord = dbSoftkeysRM->createRecord(groupNumber, deptNum, horizontalPosition, verticalPosition);
		}
		else
		{
			//	Set all of these to null if it is not a new record.
			//	We'll set the proper value below.
			myDbRecord->setActionIntegerParameterNull();
			myDbRecord->setActionStringParameterNull();
			myDbRecord->setActionCategoryNumberNull();
			myDbRecord->setActionOperatorNumberNull();
			myDbRecord->setActionFeatureNumberNull();
		}

		if (myDbRecord)
		{
			if (traceOn)
			{
				Debug::trace("Softkey", "Starting to save fields to db record ...");
				const char *z = serialize(BrowserFormat);
				int zl = getSerializedLength();
				HobString s(z, zl);
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Before saving fields to db record ... ").append(s);
				Debug::trace("Softkey", msgDebug2);
			}

			//	Plug the changes back into the dbSoftkey Record.
			myDbRecord->setGroupNumber(Long::toString(groupNumber));
			myDbRecord->setDeptNumber(Long::toString(deptNum));

			myDbRecord->setHorizontalPosition(Integer::toString(horizontalPosition));
			myDbRecord->setVerticalPosition(Integer::toString(verticalPosition));
			myDbRecord->setHorizontalSize(horizontalSize);
			myDbRecord->setVerticalSize(verticalSize);

			if (useDefaultBgColor || backgroundColor.empty())
				myDbRecord->setBackgroundColorNull();
			else
				myDbRecord->setBackgroundColor(Hex::longValueOf(backgroundColor));

			if (toggledBackgroundColor.empty())
				myDbRecord->setToggledBackgroundColorNull();
			else
				myDbRecord->setToggledBackgroundColor(Hex::longValueOf(toggledBackgroundColor));

			if (useDefaultBgImage || backgroundImage.empty())
				myDbRecord->setBackgroundImageNull();
			else
			{
				Filename gFilename(backgroundImage);
				HobString imageName = gFilename.getFileName();
				myDbRecord->setBackgroundImage(imageName);
				if (traceOnSave)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("Saving backgroundImage as [").append(imageName).append("]");
					Debug::trace("Softkey.save", msgDebug2);
				}
			}

			if (toggledBackgroundImage.empty())
				myDbRecord->setToggledBackgroundImageNull();
			else
			{
				Filename gFilename(toggledBackgroundImage);
				HobString imageName = gFilename.getFileName();
				myDbRecord->setToggledBackgroundImage(imageName);
				if (traceOnSave)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("Saving toggledBackgroundImage as [").append(imageName).append("]");
					Debug::trace("Softkey.save", msgDebug2);
				}
			}

			if (useDefaultTextColor || textColor.empty())
				myDbRecord->setTextColorNull();
			else
				myDbRecord->setTextColor(Hex::longValueOf(textColor));

			if (useDefaultName1 || name1.empty())
				myDbRecord->setName1Null();
			else
				myDbRecord->setName1(name1);

			if (useDefaultName2 || name2.empty())
				myDbRecord->setName2Null();
			else
				myDbRecord->setName2(name2);

			if (action == 0)
				myDbRecord->setActionNull();
			else
				myDbRecord->setAction(action);

			if (actionSubtype == 0)
				myDbRecord->setActionSubtypeNull();
			else
				myDbRecord->setActionSubtype(actionSubtype);

			if (actionIntParam == 0)
				myDbRecord->setActionIntegerParameterNull();
			else
				myDbRecord->setActionIntegerParameter(actionIntParam);

			if (actionStringParam.empty())
				myDbRecord->setActionStringParameterNull();
			else
				myDbRecord->setActionStringParameter(actionStringParam);

			if (actionCategoryNumber > 0)
				myDbRecord->setActionCategoryNumber(Long::toString(actionCategoryNumber));
			else
				myDbRecord->setActionCategoryNumberNull();

			if (actionOperatorNumber.size() > 0)
				myDbRecord->setActionOperatorNumber(actionOperatorNumber);
			else
				myDbRecord->setActionOperatorNumberNull();

			if (actionFeatureNumber > 0)
				myDbRecord->setActionFeatureNumber(Long::toString(actionFeatureNumber));
			else
				myDbRecord->setActionFeatureNumberNull();

			if (actionProductNumber > 0)
				myDbRecord->setActionProductNumber(Long::toString(actionProductNumber));
			else
				myDbRecord->setActionProductNumberNull();

			if (scriptName.size() > 0)
				myDbRecord->setScriptName(scriptName);
			else
				myDbRecord->setScriptNameNull();

			if (scriptInterval > 0)
				myDbRecord->setScriptInterval(scriptInterval);
			else
				myDbRecord->setScriptIntervalNull();

			myDbRecord->setForceNoBackgroundColor(forceNoBgColor);
			myDbRecord->setForceNoBackgroundImage(forceNoBgImage);
			myDbRecord->setForceNoText(forceNoText);

			//	Now save the Softkey record to the database.
			if (traceOnSave)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("groupNum = ").append(Long::toString(groupNumber));
				msgDebug2.append("   horizontalPosition = ").append(Long::toString(horizontalPosition));
				msgDebug2.append("   verticalPosition = ").append(Long::toString(verticalPosition));
				Debug::trace("Softkey.save", msgDebug2);
			}

			try
			{
				dbSoftkeysRM->saveRecord(myDbRecord);
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "Softkey::saveRecord() ********** database exception **********");
				DBException::Type t = dbe.getType();
				if (Debug::isTracing("DBAccess.exception"))
				{
					msgDebug = "   DBException: type: ";
					msgDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
					Debug::trace("DBAccess.exception", msgDebug);
				}
			}
		}
	}
	if (dbProductRec)
		delete dbProductRec;
}

const char *Softkey::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<SoftkeyFieldOffsets>::iterator fieldIter;
	switch (format)
	{
		case BrowserFormat:
		{
			HobString fieldData = "";
			
			if (action == Action_ChangeLabelPlacementAndPosition)
			{
				//	We need to set up the actionStringParam as the browser does not know how to interpret
				//	the actionIntParam. The browser can easily parse the actionStringParam
				//	to make the proper selections in the Select boxes.
				const_cast<Softkey *> (this)->getPlacementAndPositionActionString(const_cast<Softkey *> (this)->actionStringParam);
				//const_cast<Softkey *> (this)->getSecondPlacementAndPositionActionString(const_cast<Softkey *> (this)->secondActionStringParam);
			}
			
			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				if (*fieldIter > 1)
					serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData, BrowserFormat);

				if (isFieldQuoted(*fieldIter))
					fieldData.quoteForCSV();

				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			SoftkeyTags softkeyTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<SoftkeyFieldOffsets>::iterator fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = softkeyTags.getTag(*fieldIter);
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
			serializedData = "Softkey::serialize - unknown format: ";
			serializedData.append(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
		}
	}
	return serializedData.data();
}

void Softkey::setFlashkeyNumber(HobRecordNum number)
{
	flashkeyNumber = number;
}

HobRecordNum Softkey::setSoftkeyDataFromFlashkeyData()
{
	bool traceOn = Debug::isTracing("Softkey.FromQuantum");
	bool turnOff = false;
	if (traceOn && !Debug::isTracing("Softkey"))
	{
		turnOff = true;
		Debug::addTraceCategory("Softkey");
	}
	Debug::trace("Softkey.FromQuantum", "Flashkey from Quantum");
	show();

	HobRecordNum number = flashkeyNumber - 1;
	if (number < 0)
		return groupNumber;

	/*	From the Flashkey number, we can determine the Softkey Group as well as the Softkey position in the group.
		From there, we need to calculate actual position values based on the default height and width of a softkey.
	*/
	int position = number % FlashkeysPerGroup;
	groupNumber = (number / FlashkeysPerGroup) + 1;

	horizontalPosition = position / MaxFlashkeysWide;
	verticalPosition = position % MaxFlashkeysWide;

	//	Convert the action from Quantum to eVantage.
	switch (action)
	{
		case QuantumActionLoadProduct:
			action = Action_SelectProduct;
			actionProductNumber = actionIntParam;
			break;
		case QuantumActionLoadOperator:
			action = Action_LoginOperator;
			actionOperatorNumber = Integer::toString(actionIntParam);
			break;
		case QuantumActionLoadGroup:
			action = Action_LoadSoftkeyGroup;
			//actionIntParam = actionIntParam;
			break;
		default:
			action = Action_NoAction;
			break;
	}
	actionIntParam = 0;

	Debug::trace("Softkey.FromQuantum", "Softkey converted from Quantum flashkey:");
	show();
	if (turnOff)
		Debug::removeTraceCategory("Softkey");

	return groupNumber;
}



void Softkey::show()
{
	if (Debug::isTracing("Softkey"))
	{
		HobString msgDebug("show():");
		msgDebug.append("\n*******************************************************");
		msgDebug.append("\n   featureNumber:   ").append(Long::toString(featureNumber));
		msgDebug.append("\n   groupNumber:     ").append(Long::toString(groupNumber));
		msgDebug.append("\n   hPosition:       ").append(Integer::toString(horizontalPosition));
		msgDebug.append("\n   vPosition:       ").append(Integer::toString(verticalPosition));
		msgDebug.append("\n   hSize:           ").append(Integer::toString(horizontalSize));
		msgDebug.append("\n   vSize:           ").append(Integer::toString(verticalSize));
		msgDebug.append("\n   bgColor:         ").append(backgroundColor);
		msgDebug.append("\n   toggledBgColor:  ").append(toggledBackgroundColor);
		msgDebug.append("\n   bgImage:         ").append(backgroundImage);
		msgDebug.append("\n   toggledBgImage:  ").append(toggledBackgroundImage);
		msgDebug.append("\n   textColor:       ").append(textColor);
		msgDebug.append("\n   name1:           ").append(name1);
		msgDebug.append("\n   name2:           ").append(name2);
		msgDebug.append("\n   action:          ").append(Integer::toString(action));
		msgDebug.append("\n   aSubtype:        ").append(Integer::toString(actionSubtype));
		msgDebug.append("\n   aIntParam:       ").append(Integer::toString(actionIntParam));
		msgDebug.append("\n   aStringParam:    ").append(actionStringParam);
		msgDebug.append("\n   aCategoryNum:    ").append(Long::toString(actionCategoryNumber));
		msgDebug.append("\n   aOperatorNum:    ").append(actionOperatorNumber);
		msgDebug.append("\n   aProductNum:     ").append(Long::toString(actionProductNumber));
		msgDebug.append("\n   aFeatureNum:     ").append(Long::toString(actionFeatureNumber));
		msgDebug.append("\n   actionParameter: ").append(actionParameter);
		msgDebug.append("\n   scriptName:      ").append(scriptName);
		msgDebug.append("\n   scriptInterval:  ").append(Integer::toString(scriptInterval));


		msgDebug.append("\n*******************************************************");
		Debug::trace("Softkey", msgDebug);
	}
	else if (Debug::isTracing("Softkey.Clare"))
	{
		HobString msgDebug("show():");
		msgDebug.append("\n*******************************************************");
		msgDebug.append("\n   featureNumber:   ").append(Long::toString(featureNumber));
		msgDebug.append("\n   groupNumber:     ").append(Long::toString(groupNumber));
		msgDebug.append("\n   hPosition:       ").append(Integer::toString(horizontalPosition));
		msgDebug.append("\n   vPosition:       ").append(Integer::toString(verticalPosition));
		msgDebug.append("\n   name1:           ").append(name1);
		msgDebug.append("\n   action:          ").append(Integer::toString(action));
		msgDebug.append("\n   aSubtype:        ").append(Integer::toString(actionSubtype));
		msgDebug.append("\n   aIntParam:       ").append(Integer::toString(actionIntParam));
		msgDebug.append("\n   aStringParam:    [").append(actionStringParam).append("]");
		msgDebug.append("\n   actionParameter: ").append(actionParameter);

		msgDebug.append("\n*******************************************************");
		Debug::trace("Softkey.Clare", msgDebug);
	}
}


// static
void Softkey::verifyReservedRecords(DBAccess *dbAccess, int groupNumber)
{
	bool traceOn = Debug::isTracing("Softkey.reservedRecords");
	HobString msgDebug, msgText;
	if (traceOn)
	{
		msgDebug = "verifyReservedRecords():  ";
		msgText = "Creating reserved softkey ";
	}
	
	if (!dbAccess)
	{
		if (traceOn)
			Debug::trace("Softkey.reservedRecords", msgDebug + "dbAccess is NULL!");
		return;
	}
	
	if (traceOn)
		Debug::trace("Softkey.reservedRecords", msgDebug + "verifying all reserved Softkeys ...");

	Softkey *reservedSoftkey = 0;
	DBSoftKey *theSoftkey = 0;
	
	HobRecordNum groupNum;
	int hPos, vPos, oldvPos;

	// information for setting up records for all departments in order to loop thru them
	AllDepts allDepts;
	allDepts.findAll(dbAccess);
	HobList deptNumList = allDepts.getDeptNumList();
	HobString deptNumStr = "0";
	long deptNum = 0;

	while (!deptNumStr.empty())
	{
		deptNum = Long::valueOf(deptNumStr);

		if ( groupNumber == Reserved_StartScreenButtons || groupNumber == 0 )
		{

	// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1 | SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
	// 999002 |      0 |      0 |       1 |       1 |   #NULL | SEARCH  | BUSCAR  |        2 | #NULL | #NULL | #NULL | #NULL |       4 |         | blank129x40.gif
			groupNum = Reserved_StartScreenButtons;
			hPos = 0;
			vPos = 0;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999002, 0, 0: Go to SEARCH page");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("SEARCH");
					reservedSoftkey->setName2("BUSCAR");
					reservedSoftkey->setAction(Action_ChangeScreen);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(4);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("182x60_blue.png");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "Go to SEARCH page");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}
		}

		if ( groupNumber == Reserved_RunScreenButtons || groupNumber == 0 )
		{

	// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1 | SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
	// 999003 |      0 |      0 |       1 |       1 |   #NULL | SEARCH  | BUSCAR  |        2 | #NULL | #NULL | #NULL | #NULL |       4 |         | blank129x40.gif
			groupNum = Reserved_RunScreenButtons;
			hPos = 0;
			vPos = 0;
			oldvPos = 0;

			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				//Check if we have an "old UI" search button in the default position, if not then create the print button
				reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, oldvPos, dbAccess);
				if ( (!reservedSoftkey) || (strcmp(reservedSoftkey->getName1().c_str(), "SEARCH")) )
				{
					Debug::trace("Softkey", "creating reserved softkey 999003, 0, 0: Go to SEARCH page");
					reservedSoftkey = new Softkey(dbAccess);
					if (reservedSoftkey)
					{
						reservedSoftkey->setDeptNum(deptNum);
						reservedSoftkey->setGroupNumber(groupNum);
						reservedSoftkey->setHorizontalPosition(hPos);
						reservedSoftkey->setVerticalPosition(vPos);
						reservedSoftkey->setHorizontalSize(1);
						reservedSoftkey->setVerticalSize(1);
						reservedSoftkey->setBackgroundColor("");
						reservedSoftkey->setName1("Search");
						reservedSoftkey->setName2("Buscar");
						reservedSoftkey->setAction(Action_ChangeScreen);
						//	PRID, CANUM, OPNUM & SFNUM not applicable here.
						reservedSoftkey->setActionIntegerParam(4);
						reservedSoftkey->setActionStringParam("");
						reservedSoftkey->setBackgroundImage("Dk_Green.png");
					
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug + msgText;
							msgDebug2.append(Long::toString(groupNum)).append(", Position (");
							msgDebug2.append(Integer::toString(hPos)).append(", ");
							msgDebug2.append(Integer::toString(vPos)).append("), Size (");
							msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
							msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
							Debug::trace("Softkey.reservedRecords", msgDebug2 + "Go to Search page");
						}

						reservedSoftkey->saveRecord();
					}
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

	// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1       | SKNAME2        | SKACTION | PRID   | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
	// 999003 |      1 |      0 |       1 |       1 | #NULL   | Misc By Count | Misc Por Pieza |        8 | 999990 | #NULL | #NULL | #NULL |         |         | blank129x40.gif
			groupNum = Reserved_RunScreenButtons;
			hPos = 1;
			vPos = 0;
			oldvPos = 1;

			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				//Check if we have an "old UI" label data button in the default position, if not then create the print button
				reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, oldvPos, dbAccess);
			
				if ( (!reservedSoftkey) || (strcmp(reservedSoftkey->getName1().c_str(), "Misc By Count")) )
				{
					Debug::trace("Softkey", "creating reserved softkey 999003, 1, 0: ");
					reservedSoftkey = new Softkey(dbAccess);
					if (reservedSoftkey)
					{
						reservedSoftkey->setDeptNum(deptNum);
						reservedSoftkey->setGroupNumber(groupNum);
						reservedSoftkey->setHorizontalPosition(hPos);
						reservedSoftkey->setVerticalPosition(vPos);
						reservedSoftkey->setHorizontalSize(1);
						reservedSoftkey->setVerticalSize(1);
						reservedSoftkey->setBackgroundColor("");
						reservedSoftkey->setName1("Misc By Count");
						reservedSoftkey->setName2("Misc\nPor Pieza");
						reservedSoftkey->setAction(Action_SelectProduct);
						reservedSoftkey->setActionProductNumber(999990);
						reservedSoftkey->setActionStringParam("");
						reservedSoftkey->setBackgroundImage("DK_Blue.png");
					
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug + msgText;
							msgDebug2.append(Long::toString(groupNum)).append(", Position (");
							msgDebug2.append(Integer::toString(hPos)).append(", ");
							msgDebug2.append(Integer::toString(vPos)).append("), Size (");
							msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
							msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
							Debug::trace("Softkey.reservedRecords", msgDebug2 + "Misc By Count PLU");
						}

						reservedSoftkey->saveRecord();
					}
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

	// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1        | SKNAME2            | SKACTION | PRID   | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
	// 999003 |      2 |      0 |       1 |       1 | #NULL   | Misc By Weight | Misc Peso Variable |        8 | 999991 | #NULL | #NULL | #NULL |         |         | blank129x40.gif
			groupNum = Reserved_RunScreenButtons;
			hPos = 2;
			vPos = 0;
			oldvPos = 1;

			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				//Check if we have an "old UI" label data button in the default position, if not then create the print button
				reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, oldvPos, dbAccess);
			
				if ( (!reservedSoftkey) || (strcmp(reservedSoftkey->getName1().c_str(), "Misc By Weight")) )
				{
					Debug::trace("Softkey", "creating reserved softkey 999003, 2, 0: ");
					reservedSoftkey = new Softkey(dbAccess);
					if (reservedSoftkey)
					{
						reservedSoftkey->setDeptNum(deptNum);
						reservedSoftkey->setGroupNumber(groupNum);
						reservedSoftkey->setHorizontalPosition(hPos);
						reservedSoftkey->setVerticalPosition(vPos);
						reservedSoftkey->setHorizontalSize(1);
						reservedSoftkey->setVerticalSize(1);
						reservedSoftkey->setBackgroundColor("");
						reservedSoftkey->setName1("Misc By Weight");
						reservedSoftkey->setName2("Misc\nPeso Variable");
						reservedSoftkey->setAction(Action_SelectProduct);
						reservedSoftkey->setActionProductNumber(999991);
						reservedSoftkey->setActionStringParam("");
						reservedSoftkey->setBackgroundImage("DK_Blue.png");
					
						if (traceOn)
						{
							HobString msgDebug2 = msgDebug + msgText;
							msgDebug2.append(Long::toString(groupNum)).append(", Position (");
							msgDebug2.append(Integer::toString(hPos)).append(", ");
							msgDebug2.append(Integer::toString(vPos)).append("), Size (");
							msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
							msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
							Debug::trace("Softkey.reservedRecords", msgDebug2 + "Misc By Weight PLU");
						}

						reservedSoftkey->saveRecord();
					}
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}
		}

		if ( groupNumber == Reserved_CategoryButtons || groupNumber == 0 )
		{
	// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1 | SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
	// 999006 |      0 |      0 |       1 |       1 | #NULL   | Search  | Buscar  |        2 | #NULL | #NULL | #NULL | #NULL |       4 | #NULL   | medBlankYEL.jpg
			groupNum = Reserved_CategoryButtons;
			hPos = 0;
			vPos = 0;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999006, 0, 0: ESS Programmable Buttons Group - Search");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("Search");
					reservedSoftkey->setName2("Buscar");
					reservedSoftkey->setAction(Action_ChangeScreen);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(4);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("medBlankYEL.jpg");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "ESS Programmable Buttons Group - Search");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

	// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1		   | SKNAME2	| SKACTION | PRID  | CANUM | OPNUM |  SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
	// 999006 |      0 |      1 |       1 |       1 | #NULL   | Store Specials | Especiales |       16 | #NULL | #NULL | #NULL | 999007 |   #NULL | #NULL   | medBlankYEL.jpg
			groupNum = Reserved_CategoryButtons;
			hPos = 0;
			vPos = 1;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999006, 0, 1: ESS Programmable Buttons Group - Store Specials");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("Store Specials");
					reservedSoftkey->setName2("Especiales");
					reservedSoftkey->setAction(Action_LoadSpecials_First);
					//	PRID, CANUM & OPNUM not applicable here.
					reservedSoftkey->setActionFeatureNumber(999007);
					//	SKACTPI not applicable here.
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("medBlankYEL.jpg");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "ESS Programmable Buttons Group - Store Specials");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

	// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1	 	| SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
	// 999006 |      0 |      2 |       1 |       1 | #NULL   | Start over | Empezar |        2 | #NULL | #NULL | #NULL | #NULL |       0 | #NULL   | medBlankYEL.jpg
			groupNum = Reserved_CategoryButtons;
			hPos = 0;
			vPos = 2;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999006, 0, 2: ESS Programmable Buttons Group - Start Again");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("Start over");
					reservedSoftkey->setName2("Empezar");
					reservedSoftkey->setAction(Action_ChangeScreen);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(0);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("medBlankYEL.jpg");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "ESS Programmable Buttons Group - Start Again");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

	// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1	| SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
	// 999006 |      0 |      3 |       1 |       1 | #NULL   | #NULL	| #NULL	  |        0 | #NULL | #NULL | #NULL | #NULL |   #NULL | #NULL   | #NULL
			groupNum = Reserved_CategoryButtons;
			hPos = 0;
			vPos = 3;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999006, 0, 3: ESS Programmable Buttons Group - A spacer button that does nothing but take up space.");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("");
					reservedSoftkey->setName2("");
					reservedSoftkey->setAction(Action_NoAction);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					//	SKACTPI not applicable here.
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "ESS Programmable Buttons Group - A spacer button that does nothing but take up space.");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}
		}

		if ( groupNumber == Reserved_StartScreenCutTestOff || groupNumber == 0 )
		{
			//	Check to see that we have a softkey with Action_BeginCutTest in softkeyGroup Reserved_StartScreenCutTestOff (999012).
			groupNum = Reserved_StartScreenCutTestOff;
			if (traceOn)
				Debug::trace("Softkey.reservedRecords", msgDebug + "Cutting test Group " + longtostring(groupNum) + " for softkey action " + inttostring(Action_BeginCutTest));
			theSoftkey = findActionInSoftkeyGroup(groupNum, 0, Action_BeginCutTest, dbAccess);
			if (theSoftkey)
			{
				//	This softkey already exists. We don't have to do anything here.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "1 - This Cutting test softkey already exists.");
				delete theSoftkey;
				theSoftkey = 0;
			}
			else
			{
				//	This softkey does not exist. We need to create it, but we need to find an open slot for it.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "1 - Need to create this Cutting test softkey.");
				SoftkeyGroup *tempSoftkeyGroup = 0;
				int hgrid = 0, vgrid = 0;
				hPos = 0;
				vPos = 0;

				tempSoftkeyGroup = SoftkeyGroup::find(groupNum, deptNum, dbAccess);
				if (tempSoftkeyGroup)
				{
					hgrid = tempSoftkeyGroup->getHorizontalGridSize();
					vgrid = tempSoftkeyGroup->getVerticalGridSize();

					delete tempSoftkeyGroup;
					tempSoftkeyGroup = 0;
				}
				//	Search for the first available slot for this softkey.
				reservedSoftkey = 0;
				while (!reservedSoftkey && (hPos < hgrid))
				{
					while (!reservedSoftkey && (vPos < vgrid))
					{
						reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
						if (!reservedSoftkey)
							reservedSoftkey = new Softkey(dbAccess);
						else
							++vPos;
					}
					if (!reservedSoftkey)
						++hPos;
				}
				// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1       | SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
				// 999012 |      ? |      ? |       1 |       1 |   #NULL | Cut test OFF  |         |       23 | #NULL | #NULL | #NULL | #NULL |       0 |         | blank129x40.gif
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setTextColor("FFFFFF");					//	White
					reservedSoftkey->setName1("Cut test OFF");
					reservedSoftkey->setName2("Cut test OFF");
					reservedSoftkey->setAction(Action_BeginCutTest);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(0);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("182x60_blue.png");

					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "Turn Cut test ON.");
					}
					reservedSoftkey->saveRecord();

					delete reservedSoftkey;
					reservedSoftkey = 0;
				}
			}
		}

		if ( groupNumber == Reserved_RunScreenCutTestOff || groupNumber == 0 )
		{
			//	Check to see that we have a softkey with Action_BeginCutTest in softkeyGroup Reserved_RunScreenCutTestOff (999013).
			groupNum = Reserved_RunScreenCutTestOff;
			if (traceOn)
				Debug::trace("Softkey.reservedRecords", msgDebug + "Cutting test Group " + longtostring(groupNum) + " for softkey action " + inttostring(Action_BeginCutTest));
			theSoftkey = findActionInSoftkeyGroup(groupNum, 0, Action_BeginCutTest, dbAccess);
			if (theSoftkey)
			{
				//	This softkey already exists. We don't have to do anything here.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "2 - This Cutting test softkey already exists.");
				delete theSoftkey;
				theSoftkey = 0;
			}
			else
			{
				//	This softkey does not exist. We need to create it, but we need to find an open slot for it.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "2 - Need to create this Cutting test softkey.");
				SoftkeyGroup *tempSoftkeyGroup = 0;
				int hgrid = 0, vgrid = 0;
				hPos = 0;
				vPos = 0;

				tempSoftkeyGroup = SoftkeyGroup::find(groupNum, deptNum, dbAccess);
				if (tempSoftkeyGroup)
				{
					hgrid = tempSoftkeyGroup->getHorizontalGridSize();
					vgrid = tempSoftkeyGroup->getVerticalGridSize();

					delete tempSoftkeyGroup;
					tempSoftkeyGroup = 0;
				}
				//	Search for the first available slot for this softkey.
				reservedSoftkey = 0;
				while (!reservedSoftkey && (hPos < hgrid))
				{
					while (!reservedSoftkey && (vPos < vgrid))
					{
						reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
						if (!reservedSoftkey)
							reservedSoftkey = new Softkey(dbAccess);
						else
							++vPos;
					}
					if (!reservedSoftkey)
						++hPos;
				}

			// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1       | SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
			// 999013 |      ? |      ? |       1 |       1 |   #NULL | Cut test OFF  |         |       23 | #NULL | #NULL | #NULL | #NULL |       4 |         | blank129x40.gif
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setTextColor("FFFFFF");					//	White
					reservedSoftkey->setName1("Cut test OFF");
					reservedSoftkey->setName2("Cut test OFF");
					reservedSoftkey->setAction(Action_BeginCutTest);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(0);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("182x60_blue.png");

					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "Turn Cut test ON.");
					}
					reservedSoftkey->saveRecord();

					delete reservedSoftkey;
					reservedSoftkey = 0;
				}
			}
		}

		if ( groupNumber == Reserved_StartScreenCutTestOn || groupNumber == 0 )
		{
			//	Check to see that we have a softkey with Action_EndCutTest in softkeyGroup Reserved_StartScreenCutTestOn (999014).
			groupNum = Reserved_StartScreenCutTestOn;
			if (traceOn)
				Debug::trace("Softkey.reservedRecords", msgDebug + "Cutting test Group " + longtostring(groupNum) + " for softkey action " + inttostring(Action_EndCutTest));
			theSoftkey = findActionInSoftkeyGroup(groupNum, 0, Action_EndCutTest, dbAccess);
			if (theSoftkey)
			{
				//	This softkey already exists. We don't have to do anything here.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "3 - This Cutting test softkey already exists.");
				delete theSoftkey;
				theSoftkey = 0;
			}
			else
			{
				//	This softkey does not exist. We need to create it, but we need to find an open slot for it.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "3 - Need to create this Cutting test softkey.");
				SoftkeyGroup *tempSoftkeyGroup = 0;
				int hgrid = 0, vgrid = 0;
				hPos = 0;
				vPos = 0;

				tempSoftkeyGroup = SoftkeyGroup::find(groupNum, deptNum, dbAccess);
				if (tempSoftkeyGroup)
				{
					hgrid = tempSoftkeyGroup->getHorizontalGridSize();
					vgrid = tempSoftkeyGroup->getVerticalGridSize();

					delete tempSoftkeyGroup;
					tempSoftkeyGroup = 0;
				}
				//	Search for the first available slot for this softkey.
				reservedSoftkey = 0;
				while (!reservedSoftkey && (hPos < hgrid))
				{
					while (!reservedSoftkey && (vPos < vgrid))
					{
						reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
						if (!reservedSoftkey)
							reservedSoftkey = new Softkey(dbAccess);
						else
							++vPos;
					}
					if (!reservedSoftkey)
						++hPos;
				}

			// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1          | SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
			// 999014 |      ? |      ? |       1 |       1 |   #NULL | Cutting test ON  |         |       24 | #NULL | #NULL | #NULL | #NULL |       4 |         | blank129x40.gif
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setTextColor("FFFFFF");					//	White
					reservedSoftkey->setName1("Cut test ON");
					reservedSoftkey->setName2("Cut test ON");
					reservedSoftkey->setAction(Action_EndCutTest);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(0);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("182x60_blue.png");

					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "Turn Cut test OFF.");
					}
					reservedSoftkey->saveRecord();

					delete reservedSoftkey;
					reservedSoftkey = 0;
				}
			}
		}

		if ( groupNumber == Reserved_RunScreenCutTestOn || groupNumber == 0 )
		{
			//	Check to see that we have a softkey with Action_SelectAnyProduct in softkeyGroup Reserved_RunScreenCutTestOn (999015).
			groupNum = Reserved_RunScreenCutTestOn;
			if (traceOn)
				Debug::trace("Softkey.reservedRecords", msgDebug + "Cutting test Group " + longtostring(groupNum) + " for softkey action " + inttostring(Action_SelectAnyProduct));
			theSoftkey = findActionInSoftkeyGroup(groupNum, 0, Action_SelectAnyProduct, dbAccess);
			if (theSoftkey)
			{
				//	This softkey already exists. We don't have to do anything here.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "4 - This Cutting test softkey already exists.");
				delete theSoftkey;
				theSoftkey = 0;
			}
			else
			{
				//	This softkey does not exist. We need to create it, but we need to find an open slot for it.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "4 - Need to create this Cutting test softkey.");
				SoftkeyGroup *tempSoftkeyGroup = 0;
				int hgrid = 0, vgrid = 0;
				hPos = 0;
				vPos = 0;

				tempSoftkeyGroup = SoftkeyGroup::find(groupNum, deptNum, dbAccess);
				if (tempSoftkeyGroup)
				{
					hgrid = tempSoftkeyGroup->getHorizontalGridSize();
					vgrid = tempSoftkeyGroup->getVerticalGridSize();

					delete tempSoftkeyGroup;
					tempSoftkeyGroup = 0;
				}
				//	Search for the first available slot for this softkey.
				reservedSoftkey = 0;
				while (!reservedSoftkey && (hPos < hgrid))
				{
					while (!reservedSoftkey && (vPos < vgrid))
					{
						reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
						if (!reservedSoftkey)
							reservedSoftkey = new Softkey(dbAccess);
						else
							++vPos;
					}
					if (!reservedSoftkey)
						++hPos;
				}

			// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1  | SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
			// 999015 |      ? |      ? |       1 |       1 |   #NULL | End run  |         |       25 | #NULL | #NULL | #NULL | #NULL |         |         | blank129x40.gif
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setTextColor("FFFFFF");					//	White
					reservedSoftkey->setName1("End run");
					reservedSoftkey->setName2("End run");
					reservedSoftkey->setAction(Action_SelectAnyProduct);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(0);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("182x60_blue.png");

					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "End Cut test run - next PLU.");
					}
					reservedSoftkey->saveRecord();

					delete reservedSoftkey;
					reservedSoftkey = 0;
				}
			}

		//	Check to see that we have a softkey with Action_EndCutTest in softkeyGroup Reserved_RunScreenCutTestOn (999015).
			groupNum = Reserved_RunScreenCutTestOn;
			if (traceOn)
				Debug::trace("Softkey.reservedRecords", msgDebug + "Cutting test Group " + longtostring(groupNum) + " for softkey action " + inttostring(Action_EndCutTest));
			theSoftkey = findActionInSoftkeyGroup(groupNum, 0, Action_EndCutTest, dbAccess);
			if (theSoftkey)
			{
				//	This softkey already exists. We don't have to do anything here.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "5 - This Cutting test softkey already exists.");
				delete theSoftkey;
				theSoftkey = 0;
			}
			else
			{
				//	This softkey does not exist. We need to create it, but we need to find an open slot for it.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "5 - Need to create this Cutting test softkey.");
				SoftkeyGroup *tempSoftkeyGroup = 0;
				int hgrid = 0, vgrid = 0;
				hPos = 0;
				vPos = 0;

				tempSoftkeyGroup = SoftkeyGroup::find(groupNum, deptNum, dbAccess);
				if (tempSoftkeyGroup)
				{
					hgrid = tempSoftkeyGroup->getHorizontalGridSize();
					vgrid = tempSoftkeyGroup->getVerticalGridSize();

					delete tempSoftkeyGroup;
					tempSoftkeyGroup = 0;
				}
				//	Search for the first available slot for this softkey.
				reservedSoftkey = 0;
				while (!reservedSoftkey && (hPos < hgrid))
				{
					while (!reservedSoftkey && (vPos < vgrid))
					{
						reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
						if (!reservedSoftkey)
							reservedSoftkey = new Softkey(dbAccess);
						else
							++vPos;
					}
					if (!reservedSoftkey)
						++hPos;
				}
		// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1 | SKNAME2  | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
		// 999015 |      ? |      ? |       1 |       1 | #NULL   |         |          |       24 | #NULL | #NULL | #NULL | #NULL |         |         | blank129x40.gif
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setTextColor("FFFFFF");					//	White
					reservedSoftkey->setName1("Cut test ON");
					reservedSoftkey->setName2("Cut test ON");
					reservedSoftkey->setAction(Action_EndCutTest);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(0);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("182x60_blue.png");

					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "Turn Cut test OFF.");
					}
					reservedSoftkey->saveRecord();

					delete reservedSoftkey;
					reservedSoftkey = 0;
				}
			}
		}

		if ( groupNumber == Reserved_RunScreenCutTestOnOptionOne || groupNumber == 0 )
		{
			//	Check to see that we have a softkey with Action_SelectAnyProduct in softkeyGroup Reserved_RunScreenCutTestOnOptionOne (999035).
			groupNum = Reserved_RunScreenCutTestOnOptionOne;
			if (traceOn)
				Debug::trace("Softkey.reservedRecords", msgDebug + "Cutting test Group " + longtostring(groupNum) + " for softkey action " + inttostring(Action_SelectAnyProduct));
			theSoftkey = findActionInSoftkeyGroup(groupNum, 0, Action_SelectAnyProduct, dbAccess);
			if (theSoftkey)
			{
				//	This softkey already exists. We don't have to do anything here.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "4 - This Cutting test softkey already exists.");
				delete theSoftkey;
				theSoftkey = 0;
			}
			else
			{
				//	This softkey does not exist. We need to create it, but we need to find an open slot for it.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "4 - Need to create this Cutting test softkey.");
				SoftkeyGroup *tempSoftkeyGroup = 0;
				int hgrid = 0, vgrid = 0;
				hPos = 1;
				vPos = 0;

				tempSoftkeyGroup = SoftkeyGroup::find(groupNum, deptNum, dbAccess);
				if (tempSoftkeyGroup)
				{
					hgrid = tempSoftkeyGroup->getHorizontalGridSize();
					vgrid = tempSoftkeyGroup->getVerticalGridSize();

					delete tempSoftkeyGroup;
					tempSoftkeyGroup = 0;
				}
				//	Search for the first available slot for this softkey.
				reservedSoftkey = 0;
				while (!reservedSoftkey && (hPos < hgrid))
				{
					while (!reservedSoftkey && (vPos < vgrid))
					{
						reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
						if (!reservedSoftkey)
							reservedSoftkey = new Softkey(dbAccess);
						else
							++vPos;
					}
					if (!reservedSoftkey)
						++hPos;
				}

			// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1  | SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
			// 999035 |      1 |      0 |       1 |       1 |   #NULL | End run  |         |       27 | #NULL | #NULL | #NULL | #NULL |         |         | Dk_Blue.png
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setTextColor("FFFFFF");					//	White
					reservedSoftkey->setName1("Finish Run");
					reservedSoftkey->setName2("Finish Run");
					reservedSoftkey->setAction(Action_SelectAnyProduct);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(0);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("Dk_blue.png");

					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "End Cut test run - next PLU.");
					}
					reservedSoftkey->saveRecord();

					delete reservedSoftkey;
					reservedSoftkey = 0;
				}
			}

			//	Check to see that we have a softkey with Action_EndCutTest in softkeyGroup Reserved_RunScreenCutTestOnOptionOne (999035).
			groupNum = Reserved_RunScreenCutTestOnOptionOne;
			if (traceOn)
				Debug::trace("Softkey.reservedRecords", msgDebug + "Cutting test Group " + longtostring(groupNum) + " for softkey action " + inttostring(Action_EndCutTest));
			theSoftkey = findActionInSoftkeyGroup(groupNum, 0, Action_EndCutTest, dbAccess);
			if (theSoftkey)
			{
				//	This softkey already exists. We don't have to do anything here.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "5 - This Cutting test softkey already exists.");
				delete theSoftkey;
				theSoftkey = 0;
			}
			else
			{
				//	This softkey does not exist. We need to create it, but we need to find an open slot for it.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "5 - Need to create this Cutting test softkey.");
				SoftkeyGroup *tempSoftkeyGroup = 0;
				int hgrid = 0, vgrid = 0;
				hPos = 1;
				vPos = 1;

				tempSoftkeyGroup = SoftkeyGroup::find(groupNum, deptNum, dbAccess);
				if (tempSoftkeyGroup)
				{
					hgrid = tempSoftkeyGroup->getHorizontalGridSize();
					vgrid = tempSoftkeyGroup->getVerticalGridSize();

					delete tempSoftkeyGroup;
					tempSoftkeyGroup = 0;
				}
				//	Search for the first available slot for this softkey.
				reservedSoftkey = 0;
				while (!reservedSoftkey && (hPos < hgrid))
				{
					while (!reservedSoftkey && (vPos < vgrid))
					{
						reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
						if (!reservedSoftkey)
							reservedSoftkey = new Softkey(dbAccess);
						else
							++vPos;
					}
					if (!reservedSoftkey)
						++hPos;
				}
		// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1 | SKNAME2  | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
		// 999035 |      1 |      1 |       1 |       1 | #NULL   |         |          |       26 | #NULL | #NULL | #NULL | #NULL |         |         | Red1.png
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setTextColor("FFFFFF");					//	White
					reservedSoftkey->setName1("Finish Cutting Test");
					reservedSoftkey->setName2("Finish Cutting Test");
					reservedSoftkey->setAction(Action_EndCutTest);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(0);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("Red1.png");

					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "Turn Cut test OFF.");
					}
					reservedSoftkey->saveRecord();

					delete reservedSoftkey;
					reservedSoftkey = 0;
				}
			}

			groupNum = Reserved_RunScreenCutTestOnOptionOne;
			if (traceOn)
				Debug::trace("Softkey.reservedRecords", msgDebug + "Cutting test Group " + longtostring(groupNum) + " for softkey action " + inttostring(Action_CutTestSuspend));
			theSoftkey = findActionInSoftkeyGroup(groupNum, 0, Action_CutTestSuspend, dbAccess);
			if (theSoftkey)
			{
				//	This softkey already exists. We don't have to do anything here.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "5 - This Cutting test softkey already exists.");
				delete theSoftkey;
				theSoftkey = 0;
			}
			else
			{
				//	This softkey does not exist. We need to create it, but we need to find an open slot for it.
				if (traceOn)
					Debug::trace("Softkey.reservedRecords", msgDebug + "5 - Need to create this Cutting test softkey.");
				SoftkeyGroup *tempSoftkeyGroup = 0;
				int hgrid = 0, vgrid = 0;
				hPos = 1;
				vPos = 2;

				tempSoftkeyGroup = SoftkeyGroup::find(groupNum, deptNum, dbAccess);
				if (tempSoftkeyGroup)
				{
					hgrid = tempSoftkeyGroup->getHorizontalGridSize();
					vgrid = tempSoftkeyGroup->getVerticalGridSize();

					delete tempSoftkeyGroup;
					tempSoftkeyGroup = 0;
				}
				//	Search for the first available slot for this softkey.
				reservedSoftkey = 0;
				while (!reservedSoftkey && (hPos < hgrid))
				{
					while (!reservedSoftkey && (vPos < vgrid))
					{
						reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
						if (!reservedSoftkey)
							reservedSoftkey = new Softkey(dbAccess);
						else
							++vPos;
					}
					if (!reservedSoftkey)
						++hPos;
				}
				// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1 | SKNAME2  | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
				// 999035 |      1 |      1 |       1 |       1 | #NULL   |         |          |       58 | #NULL | #NULL | #NULL | #NULL |         |         | Yellow1.png
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setTextColor("FFFFFF");					//	White - but I might make this #000000
					reservedSoftkey->setName1("Put Cutting\nTest On Hold");
					reservedSoftkey->setName2("Put Cutting\nTest On Hold");
					reservedSoftkey->setAction(Action_CutTestSuspend);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(0);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("Orange1.png");

					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "Suspend the Cut Test.");
					}
					reservedSoftkey->saveRecord();

					delete reservedSoftkey;
					reservedSoftkey = 0;
				}
			}
		}

		if ( groupNumber == ReservedSK_SelfservAZCategoryButtons || groupNumber == 0 )
		{

	// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1 | SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
	// 999002 |      0 |      0 |       1 |       1 |   #NULL | SEARCH  | BUSCAR  |        2 | #NULL | #NULL | #NULL | #NULL |       4 |         | blank129x40.gif
			groupNum = ReservedSK_SelfservAZCategoryButtons;

			hPos = 0;
			vPos = 0;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999016, 0, 0: A");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("A");
					reservedSoftkey->setName2("A");
					reservedSoftkey->setAction(Action_LoadAlphabeticSearch_First);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionFeatureNumber(ReservedSK_SelfservCategoryResultsButtons);
					reservedSoftkey->setActionStringParam("A");
					reservedSoftkey->setBackgroundImage("");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "A");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

			hPos = 1;
			vPos = 0;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999016, 0, 0: B-C");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("B-C");
					reservedSoftkey->setName2("B-C");
					reservedSoftkey->setAction(Action_LoadAlphabeticSearch_First);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionFeatureNumber(ReservedSK_SelfservCategoryResultsButtons);
					reservedSoftkey->setActionStringParam("B,C");
					reservedSoftkey->setBackgroundImage("");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "B-C");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

			hPos = 0;
			vPos = 1;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999016, 0, 0: D-J");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("D-J");
					reservedSoftkey->setName2("D-J");
					reservedSoftkey->setAction(Action_LoadAlphabeticSearch_First);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionFeatureNumber(ReservedSK_SelfservCategoryResultsButtons);
					reservedSoftkey->setActionStringParam("D,E,F,G,H,I,J");
					reservedSoftkey->setBackgroundImage("");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "D-J");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

			hPos = 1;
			vPos = 1;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999016, 0, 0: K-M");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("K-M");
					reservedSoftkey->setName2("K-M");
					reservedSoftkey->setAction(Action_LoadAlphabeticSearch_First);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionFeatureNumber(ReservedSK_SelfservCategoryResultsButtons);
					reservedSoftkey->setActionStringParam("K,L,M");
					reservedSoftkey->setBackgroundImage("");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "K-M");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

			hPos = 0;
			vPos = 2;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999016, 0, 0: N-O");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("N-O");
					reservedSoftkey->setName2("N-O");
					reservedSoftkey->setAction(Action_LoadAlphabeticSearch_First);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionFeatureNumber(ReservedSK_SelfservCategoryResultsButtons);
					reservedSoftkey->setActionStringParam("N,O");
					reservedSoftkey->setBackgroundImage("");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "N-O");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

			hPos = 1;
			vPos = 2;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999016, 0, 0: P");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("P");
					reservedSoftkey->setName2("P");
					reservedSoftkey->setAction(Action_LoadAlphabeticSearch_First);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionFeatureNumber(ReservedSK_SelfservCategoryResultsButtons);
					reservedSoftkey->setActionStringParam("P");
					reservedSoftkey->setBackgroundImage("");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "P");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

			hPos = 0;
			vPos = 3;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999016, 0, 0: Q-S");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("Q-S");
					reservedSoftkey->setName2("Q-S");
					reservedSoftkey->setAction(Action_LoadAlphabeticSearch_First);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionFeatureNumber(ReservedSK_SelfservCategoryResultsButtons);
					reservedSoftkey->setActionStringParam("Q,R,S");
					reservedSoftkey->setBackgroundImage("");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "Q-S");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

			hPos = 1;
			vPos = 3;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999016, 0, 0: T-Z");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("T-Z");
					reservedSoftkey->setName2("T-Z");
					reservedSoftkey->setAction(Action_LoadAlphabeticSearch_First);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionFeatureNumber(ReservedSK_SelfservCategoryResultsButtons);
					reservedSoftkey->setActionStringParam("T,U,V,W,X,Y,Z");
					reservedSoftkey->setBackgroundImage("");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "T-Z");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}

			hPos = 0;
			vPos = 4;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999016, 0, 0: Best Sellers");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(2);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("Best Sellers");
					reservedSoftkey->setName2("Best Sellers");
					reservedSoftkey->setAction(Action_LoadMostCommon_First);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionFeatureNumber(ReservedSK_SelfservCategoryResultsButtons);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("");
				
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "A");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}
		}
	
		
		if (groupNumber == Reserved_FreshServeButtons || groupNumber == 0)
		{

			// SKGNUM | SKHPOS | SKVPOS | SKHSIZE | SKVSIZE | SKBGCLR | SKNAME1 | SKNAME2 | SKACTION | PRID  | CANUM | OPNUM | SFNUM | SKACTPI | SKACTPS | SKBGIMAGE
			// 888050 |      0 |      0 |       1 |       1 |   #NULL | SEARCH  | BUSCAR  |        2 | #NULL | #NULL | #NULL | #NULL |       4 |         | blank129x40.gif
			groupNum = Reserved_StartScreenButtons;
			hPos = 0;
			vPos = 0;
			reservedSoftkey = Softkey::find(groupNum, deptNum, hPos, vPos, dbAccess);
			if (!reservedSoftkey)
			{
				Debug::trace("Softkey", "creating reserved softkey 999002, 0, 0: Go to SEARCH page");
				reservedSoftkey = new Softkey(dbAccess);
				if (reservedSoftkey)
				{
					reservedSoftkey->setDeptNum(deptNum);
					reservedSoftkey->setGroupNumber(groupNum);
					reservedSoftkey->setHorizontalPosition(hPos);
					reservedSoftkey->setVerticalPosition(vPos);
					reservedSoftkey->setHorizontalSize(1);
					reservedSoftkey->setVerticalSize(1);
					reservedSoftkey->setBackgroundColor("");
					reservedSoftkey->setName1("SEARCH");
					reservedSoftkey->setName2("BUSCAR");
					reservedSoftkey->setAction(Action_ChangeScreen);
					//	PRID, CANUM, OPNUM & SFNUM not applicable here.
					reservedSoftkey->setActionIntegerParam(4);
					reservedSoftkey->setActionStringParam("");
					reservedSoftkey->setBackgroundImage("182x60_blue.png");

					if (traceOn)
					{
						HobString msgDebug2 = msgDebug + msgText;
						msgDebug2.append(Long::toString(groupNum)).append(", Position (");
						msgDebug2.append(Integer::toString(hPos)).append(", ");
						msgDebug2.append(Integer::toString(vPos)).append("), Size (");
						msgDebug2.append(Integer::toString(reservedSoftkey->getHorizontalSize())).append(", ");
						msgDebug2.append(Integer::toString(reservedSoftkey->getVerticalSize())).append("),  ");
						Debug::trace("Softkey.reservedRecords", msgDebug2 + "Go to SEARCH page");
					}

					reservedSoftkey->saveRecord();
				}
			}
			if (reservedSoftkey)
			{
				delete reservedSoftkey;
				reservedSoftkey = 0;
			}
		}

		// Advance to the next department in the department list
		deptNumStr = "";
		if (deptNumList.next(deptNumStr) != -1 && deptNumStr == "0")
		{
			deptNumStr = "";
			deptNumList.next(deptNumStr);
		}

	}
}

DBSoftKey *Softkey::findActionInSoftkeyGroup(HobRecordNum groupNum, HobRecordNum deptNum, int theAction, DBAccess *dbAccess)
{
	bool traceOn = Debug::isTracing("Softkey.reservedRecords");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "findActionInSoftkeyGroup(";
		msgDebug.append(Long::toString(groupNum)).append(", ");
		msgDebug.append(Integer::toString(theAction)).append(")");
		Debug::trace("Softkey.reservedRecords", msgDebug + "  - Enter.");
		msgDebug.append(":   ");
	}

	DBSoftKey *skey = 0;
	if (!dbAccess)
	{
		Debug::trace("Softkey.reservedRecords", msgDebug + "dbAccess is NULL!");
		return skey;
	}
	DBSoftKeysRM *dbSoftKeysRM = dbAccess->getDBSoftKeysRM();
	if (!dbSoftKeysRM)
	{
		Debug::trace("Softkey.reservedRecords", msgDebug + "dbSoftKeysRM is NULL!");
		return skey;
	}

	DBQuery<DBSoftKey>* skGroupQuery = dbSoftKeysRM->retrieveGroup(longtostring(groupNum), longtostring(deptNum) );
	if (skGroupQuery)
	{
		DBQuery<DBSoftKey>::const_iterator tempIter = skGroupQuery->begin();
		while (tempIter != skGroupQuery->end())
		{
			if (tempIter->getAction() != theAction)
				++tempIter;
			else
			{
				Debug::trace("Softkey.reservedRecords", msgDebug + "Found the action!");
				skey = new DBSoftKey(*tempIter);
				break;
			}
		}
	}
	delete skGroupQuery;
	return skey;
}

void Softkey::fitDescription(HobString &desc, unsigned int maxChars)
{
	bool traceOn = Debug::isTracing("Softkey.fitDesc");
	HobString msgDebug = "";
	if (traceOn)
	{
		msgDebug.append("fitDescription():  ");
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("desc = [").append(desc);
		msgDebug2.append("]  maxChars = ").append(Integer::toString(maxChars));
		Debug::trace("Softkey.fitDesc", msgDebug2);
	}

	desc.stripFontSequences();

	if (traceOn)
		Debug::trace("Softkey.fitDesc", msgDebug + "After stripFontSequences(): desc = [" + desc + "]");

	//	Replace all newline chars with spaces.
	std::string::size_type lfPos = desc.find_first_of(0x0A);
	while (lfPos != std::string::npos)
	{
		desc.replace(lfPos, 1, " ");
		lfPos = desc.find_first_of(0x0A);
	}

	if (traceOn)
		Debug::trace("Softkey.fitDesc", msgDebug + "After replacing newline chars with spaces: desc = [" + desc + "]");

	if (maxChars > 0)
	{
		HobTokenizerCSV tok(&desc, " ");
		HobString token, newDesc;

		while ((tok.next(&token) != -1) && (newDesc.length() <= maxChars))
		{
			newDesc.append(token).append(" ");
			if (traceOn)
				Debug::trace("Softkey.fitDesc", msgDebug + "Word search: newDesc = [" + newDesc + "]  newDesc.length() = " + Integer::toString((int)newDesc.length()));
		}
		//	Strip off the space char at the end.
		newDesc = newDesc.substr(0, newDesc.length() - 1);
		desc = newDesc;
		if (traceOn)
			Debug::trace("Softkey.fitDesc", msgDebug + "Done: desc = [" + desc + "]");
	}
}


void Softkey::setupSoftkeyDefaultValues(short languageSelection, short defaultSetting, HobRecordNum featureNum, DBProduct *dbProductRec, DBCategory *dbCategoryRec)
{
	bool traceOn = Debug::isTracing("Softkey");
	bool traceOnShowDefaults = Debug::isTracing("Softkey.showDefaults");
	HobString msgDebug = "";
	HobString fromWhere = "";
	if (traceOn)
	{
		msgDebug.append("setupSoftkeyDefaultValues():  ");

		HobString msgDebug2 = msgDebug;
		msgDebug2.append("languageSelection = ").append(Integer::toString(languageSelection));
		msgDebug2.append(" defaultSetting = ").append(Integer::toString(defaultSetting));
		msgDebug2.append(" featureNum = ").append(Long::toString(featureNum));
		msgDebug2.append("\n dbProductRec is ");
		if (dbProductRec)
			msgDebug2.append("not ");
		msgDebug2.append("Null");
		msgDebug2.append(" dbCategoryRec is ");
		if (dbCategoryRec)
			msgDebug2.append("not ");
		msgDebug2.append("Null");
		msgDebug2.append(" dbAccess = ").append(ptrtostring(dbAccess));
		Debug::trace("Softkey", msgDebug2);
	}

	bool deleteDbProductRec = true;
	if (dbProductRec)
	{
		deleteDbProductRec = false;
		action = Action_SelectProduct;
	}

	bool deleteDbCategoryRec = true;
	if (dbCategoryRec)
	{
		deleteDbCategoryRec = false;
		action = Action_LoadCategory_First;
	}

	DBProductsRM *dbProductsRM		= 0;
	DBCategoriesRM *dbCategoriesRM	= 0;
	DBOperatorsRM *dbOperatorsRM	= 0;
	DBOperator *dbOperatorRec		= 0;

	SoftkeyFeature *feature			= 0;
	SoftkeyStyle *style				= 0;

	HobString tempStr;


	if (dbAccess)
	{
		HobRecordNum lDept = Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum);
		if (deptNum != lDept)
		{
			Debug::trace("Softkey", "ALERT ALERT - The department and configDept did not match in setupSoftkeyDefaultValues()" + longtostring(actionProductNumber));
			deptNum = lDept;			// havent seen this be an issue yet, but I just want to make sure.
		}

		feature = SoftkeyFeature::find(featureNum, deptNum, dbAccess);
		if (feature)
			style = SoftkeyStyle::find(feature->getStyleNumber(), dbAccess);
	}

	switch(action)
	{
		case Action_SelectProduct:
		{
			if (traceOn)
			#if defined(LINUX_FIX)
				Debug::trace("Softkey", "Action: SelectProduct, product number: " + longtostring(actionProductNumber));
            #else
				Debug::trace("Softkey", "Action: SelectProduct, product number: " + inttostring(actionProductNumber));
            #endif

			if (!dbProductRec && dbAccess && actionProductNumber != RECORD_NOT_FOUND)
			{
				dbProductsRM = dbAccess->getDBProductsRM();
				if (dbProductsRM)
				{
					dbProductRec = dbProductsRM->findPLU(HobString(Long::toString(actionProductNumber)), Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				}
				
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("Product Number: ").append(Long::toString(actionProductNumber)).append(".  Product ");
					if (dbProductRec)
						msgDebug2.append("was");
					else
						msgDebug2.append("not");
					msgDebug2.append(" found.");
					Debug::trace("Softkey", msgDebug2);
				}
			}

			if (!dbProductRec)
				softkeyEnabled = false;
			else
			{
				actionProductNumber = (HobRecordNum) Long::valueOf(dbProductRec->getPLU());
				if (dbAccess)
					dbCategoriesRM = dbAccess->getDBCategoriesRM();
				if (dbCategoriesRM)
					dbCategoryRec = dbCategoriesRM->findNumber(dbProductRec->getCategory(), dbProductRec->getDeptNumber());
			}
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Product softkeyEnabled = ").append(Bool::toString(softkeyEnabled));
				Debug::trace("Softkey", msgDebug2);
			}
		}
		break;

		case Action_LoadCategory_First:
		case Action_LoadCategory_Next:
		case Action_LoadCategory_Previous:
		{
			if (!dbCategoryRec && dbAccess)
			{
				dbCategoriesRM = dbAccess->getDBCategoriesRM();
				if (dbCategoriesRM)
					dbCategoryRec = dbCategoriesRM->findNumber(longtostring(actionCategoryNumber), Configuration::getInstance()->toStringCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("Category Number: ").append(Long::toString(actionCategoryNumber)).append(".  Category ");
					if (dbCategoryRec)
						msgDebug2.append("was");
					else
						msgDebug2.append("not");
					msgDebug2.append(" found.");
					Debug::trace("Softkey", msgDebug2);
				}
			}
			if (!dbCategoryRec)
				softkeyEnabled = false;
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Category softkeyEnabled = ").append(Bool::toString(softkeyEnabled));
				Debug::trace("Softkey", msgDebug2);
			}
		}
		break;

		case Action_LoginOperator:
		{
			if (dbAccess)
				dbOperatorsRM = dbAccess->getDBOperatorsRM();
			if (dbOperatorsRM)
				dbOperatorRec = dbOperatorsRM->findNumber(actionOperatorNumber);
			if (!dbOperatorRec)
				softkeyEnabled = false;
			if (traceOn)
			{
				HobString msgDebug2 = msgDebug;
				msgDebug2.append("Operator Number: ").append(actionOperatorNumber).append(".  Operator ");
				if (dbOperatorRec)
					msgDebug2.append("was");
				else
					msgDebug2.append("not");
				msgDebug2.append(" found.  softkeyEnabled = ").append(Bool::toString(softkeyEnabled));
				Debug::trace("Softkey", msgDebug2);
			}
			break;
		}
	}

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("backgroundColor = [").append(backgroundColor).append("]");
		Debug::trace("Softkey", msgDebug2);
	}
	if (defaultSetting != SoftkeyDataOnly)
	{
		if (forceNoBgColor)
		{
			if (traceOnShowDefaults)
				fromWhere.append("nowhere - forceNoBgColor");

			backgroundColor = "";
		}
		else
		{
			if (backgroundColor.empty())
			{
				if (traceOnShowDefaults)
					fromWhere = "nowhere";

				tempStr = "";
				if (tempStr.empty() && dbCategoryRec)
				{
					if (!dbCategoryRec->isBackgroundColorNull())
					{
						tempStr = Hex::toHTMLColorString((long) dbCategoryRec->getBackgroundColor());
						if (traceOnShowDefaults && !tempStr.empty())
							fromWhere = "dbCategoryRec";
					}
				}

				if (tempStr.empty() && style)
				{
					tempStr = style->getBackgroundColor();
					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "the style";
				}

				if (defaultSetting == SoftkeyDataIncludingDefaultValues)
				{
					defaultBackgroundColor = tempStr;
					if (!tempStr.empty())
						useDefaultBgColor = true;
				}
				else if (defaultSetting == SoftkeyDataUsingDefaults)
				{
					backgroundColor = tempStr;
				}
			}
			else
			{
				if (traceOnShowDefaults)
					fromWhere = "the softkey";
			}
		}
		if (traceOnShowDefaults)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("  backgroundColor = [").append(backgroundColor).append("]");
			msgDebug2.append("  defaultBackgroundColor = [").append(defaultBackgroundColor).append("]");
			if (!defaultBackgroundColor.empty() || forceNoBgColor)
				msgDebug2.append("  and comes from ").append(fromWhere);
			msgDebug2.append("  useDefaultBgColor = ").append(Bool::toString(useDefaultBgColor));
			Debug::trace("Softkey.showDefaults", msgDebug2);
		}
	}

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("backgroundImage = [").append(backgroundImage).append("]");
		Debug::trace("Softkey", msgDebug2);
	}
	if (defaultSetting != SoftkeyDataOnly)
	{
		if (forceNoBgImage)
		{
			if (traceOnShowDefaults)
				fromWhere.append("nowhere - forceNoBgImage");

			backgroundImage = "";
		}
		else
		{
			tempStr = "";
			if (backgroundImage.empty())
			{
				if (traceOnShowDefaults)
					fromWhere = "nowhere";

				if (tempStr.empty() && dbProductRec)
				{
					if (!dbProductRec->isImageNull())
						tempStr = dbProductRec->getImage();
					if (tempStr.empty())
						tempStr = dbProductRec->getPLU();
					if (tempStr.size())
						tempStr = validateProductImage(tempStr, languageSelection);

					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "dbProductRec";
				}

				if (tempStr.empty() && dbCategoryRec)
				{
					if (!dbCategoryRec->isImageNull())
						tempStr = dbCategoryRec->getImage();
					if (tempStr.size())
						tempStr = validateCategoryImage(tempStr, languageSelection);

					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "dbCategoryRec";
				}

				if (tempStr.empty() && style)
				{
					tempStr = style->getBackgroundImage();
					bgImageExists = true;
					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "the style";
				}

				if (defaultSetting == SoftkeyDataIncludingDefaultValues)
				{
					defaultBackgroundImage = tempStr;
					if (!tempStr.empty())
						useDefaultBgImage = true;
				}
				else if (defaultSetting == SoftkeyDataUsingDefaults)
				{
					backgroundImage = tempStr;
				}
			}
			else
			{
				if (validateBackgroundImageFile)
					tempStr = validateSoftkeyImage(backgroundImage, languageSelection);
				else
					tempStr = backgroundImage;

				if (traceOnShowDefaults)
					fromWhere = "the softkey";
			}
		}
		if (traceOnShowDefaults)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("  backgroundImage = [").append(backgroundImage).append("]");
			msgDebug2.append("  defaultBackgroundImage = [").append(defaultBackgroundImage).append("]");
			if (!defaultBackgroundImage.empty() || forceNoBgImage)
				msgDebug2.append("  and comes from ").append(fromWhere);
			msgDebug2.append("  useDefaultBgImage = ").append(Bool::toString(useDefaultBgImage));
			Debug::trace("Softkey.showDefaults", msgDebug2);
		}
	}
	
	if (defaultSetting != SoftkeyDataOnly)
	{
		tempStr = "";
		if (tempStr.empty() && dbProductRec)
		{
			tempStr = getSpecialsImageFilename(Long::valueOf(dbProductRec->getDeptNumber()), Long::valueOf(dbProductRec->getSpecialNumber()), languageSelection, dbAccess);
			
			if (tempStr.size())
				tempStr = validateSpecialsImage(tempStr, languageSelection);
		
		}

		defaultForegroundImage = tempStr;
	}
	
	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("textColor = [").append(textColor).append("]");
		Debug::trace("Softkey", msgDebug2);
	}
	if (defaultSetting != SoftkeyDataOnly)
	{
		if (textColor.empty())
		{
			if (traceOnShowDefaults)
				fromWhere = "nowhere";

			tempStr = "";
			if (tempStr.empty() && dbCategoryRec)
			{
				if (!dbCategoryRec->isTextColorNull())
				{
					tempStr = Hex::toHTMLColorString((long) dbCategoryRec->getTextColor());
					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "dbCategoryRec";
				}
			}
			if (tempStr.empty() && style)
			{
				tempStr = style->getTextColor();
				if (traceOnShowDefaults && !tempStr.empty())
					fromWhere = "the style";
			}
			if (defaultSetting == SoftkeyDataIncludingDefaultValues)
			{
				defaultTextColor = tempStr;
				if (!tempStr.empty())
					useDefaultTextColor = true;
			}
			else if (defaultSetting == SoftkeyDataUsingDefaults)
			{
				textColor = tempStr;
			}
		}
		else
		{
			if (traceOnShowDefaults)
				fromWhere = "the softkey";
		}
		if (traceOnShowDefaults)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("  textColor = [").append(textColor).append("]");
			msgDebug2.append("  defaultTextColor = [").append(defaultTextColor).append("]");
			if (!defaultTextColor.empty())
				msgDebug2.append("  and comes from ").append(fromWhere);
			msgDebug2.append("  useDefaultTextColor = ").append(Bool::toString(useDefaultTextColor));
			Debug::trace("Softkey.showDefaults", msgDebug2);
		}
	}

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("name1 = [").append(name1).append("]");
		Debug::trace("Softkey", msgDebug2);
	}
	if (defaultSetting != SoftkeyDataOnly)
	{
		if (forceNoText)
		{
			if (traceOnShowDefaults)
				fromWhere.append("nowhere - forceNoText");

			name1 = "";
		}
		else
		{
			if (name1.empty())
			{
				if (traceOnShowDefaults)
					fromWhere = "nowhere";

				tempStr = "";
				if (tempStr.empty() && dbProductRec)
				{
					tempStr = getProductSoftkeyName(dbProductRec, 1);
					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "dbProductRec";
				}

				if (tempStr.empty() && dbCategoryRec)
				{
					tempStr = getCategorySoftkeyName(dbCategoryRec, 1);
					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "dbCategoryRec";
				}

				if (tempStr.empty() && dbOperatorRec)
				{
					tempStr = dbOperatorRec->getName();
					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "dbOperatorRec";
				}

				if (defaultSetting == SoftkeyDataIncludingDefaultValues)
				{
					defaultName1 = tempStr;
					if (!tempStr.empty())
						useDefaultName1 = true;
				}
				else if (defaultSetting == SoftkeyDataUsingDefaults)
				{
					name1 = tempStr;
				}
			}
			else
			{
				if (traceOnShowDefaults)
					fromWhere = "the softkey";
			}
		}
		if (traceOnShowDefaults)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("  name1 = [").append(name1).append("]");
			msgDebug2.append("  defaultName1 = [").append(defaultName1).append("]");
			if (!defaultName1.empty() || forceNoText)
				msgDebug2.append("  and comes from ").append(fromWhere);
			msgDebug2.append("  useDefaultName1 = ").append(Bool::toString(useDefaultName1));
			Debug::trace("Softkey.showDefaults", msgDebug2);
		}
	}

	if (traceOn)
	{
		HobString msgDebug2 = msgDebug;
		msgDebug2.append("name2 = [").append(name2).append("]");
		Debug::trace("Softkey", msgDebug2);
	}
	if (defaultSetting != SoftkeyDataOnly)
	{
		if (forceNoText)
		{
			if (traceOnShowDefaults)
				fromWhere.append("nowhere - forceNoText");

			name2 = "";
		}
		else
		{
			if (name2.empty())
			{
				if (traceOnShowDefaults)
					fromWhere = "nowhere";

				tempStr = "";
				if (tempStr.empty() && dbProductRec)
				{
					tempStr = getProductSoftkeyName(dbProductRec, 2);
					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "dbProductRec";
				}

				if (tempStr.empty() && dbCategoryRec)
				{
					tempStr = getCategorySoftkeyName(dbCategoryRec, 2);
					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "dbCategoryRec";
				}

				if (tempStr.empty() && dbOperatorRec)
				{
					tempStr = dbOperatorRec->getName();
					if (traceOnShowDefaults && !tempStr.empty())
						fromWhere = "dbOperatorRec";
				}

				if (defaultSetting == SoftkeyDataIncludingDefaultValues)
				{
					defaultName2 = tempStr;
					if (!tempStr.empty())
						useDefaultName2 = true;
				}
				else if (defaultSetting == SoftkeyDataUsingDefaults)
				{
					name2 = tempStr;
				}
			}
			else
			{
				if (traceOnShowDefaults)
					fromWhere = "the softkey";
			}
		}
		if (traceOnShowDefaults)
		{
			HobString msgDebug2 = msgDebug;
			msgDebug2.append("  name2 = [").append(name2).append("]");
			msgDebug2.append("  defaultName2 = [").append(defaultName2).append("]");
			if (!defaultName2.empty() || forceNoText)
				msgDebug2.append("  and comes from ").append(fromWhere);
			msgDebug2.append("  useDefaultName2 = ").append(Bool::toString(useDefaultName2));
			Debug::trace("Softkey.showDefaults", msgDebug2);
		}
	}

	if (deleteDbProductRec && dbProductRec)
		delete dbProductRec;
	if (deleteDbCategoryRec && dbCategoryRec)
		delete dbCategoryRec;
	if (dbOperatorRec)
		delete dbOperatorRec;
	if (feature)
		delete feature;
	if (style)
		delete style;

	Debug::trace("Softkey",HobString("setupSoftkeyDefaultValues IntegerParam = ") + Integer::toString(actionIntParam));
}

HobString Softkey::getProductSoftkeyName(DBProduct *dbProduct, short language)
{
	HobString sName("");

	if (dbProduct)
	{
		Configuration *config = Configuration::getInstance();

		// SOF-4355
		Configuration *scaleConfig;
		scaleConfig = Configuration::getInstance();
		bool isSelfServeMode = scaleConfig->isSelfServiceScale();

		if (language == 1 && !dbProduct->isName1Null() && !dbProduct->getName1().empty() )
			sName = dbProduct->getName1();
		else if (language == 2 && !dbProduct->isName2Null() && !dbProduct->getName2().empty() )
			sName = dbProduct->getName2();

		if (sName.empty() && !dbProduct->isDescriptionNull())
		{
			sName = dbProduct->getDescription();
			sName.stripFontSequences();
			Softkey::fitDescription(sName, (unsigned int) config->toIntCfg(fieldOffsetCfgNumOfDescCharsToUse));
		}

		if ( ( config->toBoolCfg( fieldOffsetCfgAppendingPluNumberToName ) && !isSelfServeMode ) || sName.empty() ) 
		{
			bool useNewline = config->toBoolCfgI(fieldOffsetCfgISoftKeyNewlineBetweenPluNumberAndName);
			char cr = '\r';
			char lf = '\n';
			string pluNumStr;
			if (config->toBoolCfgI(fieldOffsetCfgISoftKeyPluNumberBeforeName))
			{
				pluNumStr = "(" + dbProduct->getPLU() + ")";
				if (useNewline)
				{
					pluNumStr += cr;
					pluNumStr += lf;
				}
				else
					pluNumStr.append(" ");
				sName.insert(0, pluNumStr);
			}
			else
			{
				if (useNewline)
				{
					pluNumStr += cr;
					pluNumStr += lf;
				}
				else
					pluNumStr = " ";
				pluNumStr.append("(");
				pluNumStr.append(dbProduct->getPLU()).append(")");
				sName.append(pluNumStr);
			}
		}
	}
	return sName;
}

HobString Softkey::getCategorySoftkeyName(DBCategory *dbCategory, short language)
{
	HobString sName("");

	if (dbCategory)
	{
		if (language == 1 && !dbCategory->isName1Null())
			sName = dbCategory->getName1();
		else if (language == 2 && !dbCategory->isName2Null())
			sName = dbCategory->getName2();
	}
	return sName;
}

HobString Softkey::getSpecialsImageFilename(HobRecordNum deptNum, HobRecordNum specialNum, short language, DBAccess *dbAccess)
{
	bool traceOn = Debug::isTracing("Softkey");
	HobString msgDebug("getSpecialsImageFilename(): ");
	Filename gFilename;
	HobString imageFilename = "";

	Special *special = Special::findRecord(specialNum, deptNum, dbAccess);
	if (special)
	{
		if (language == 2)
			imageFilename = special->getImageName2();
		else
			imageFilename = special->getImageName1();

		if (traceOn)
		{
			HobString msgDebug2(msgDebug);
			msgDebug2.append("Found specialNum = ").append(Integer::toString(specialNum)).append(", imageFilename = [").append(imageFilename).append("]");
			Debug::trace("Softkey", msgDebug2);
		}
		delete special;
	}
	else
	{
		if (traceOn)
		{
			HobString msgDebug2(msgDebug);
			msgDebug2.append("specialNum = ").append(Integer::toString(specialNum)).append(" not found!");
			Debug::trace("Softkey", msgDebug2);
		}
	}
	return imageFilename;
}

HobString Softkey::validateSpecialsImage(HobString &graphicFileName, short language)
{
	Configuration *config = Configuration::getInstance();
	bool traceOn = Debug::isTracing("Softkey");
	HobString msgDebug("validateSpecialsImage(): ");
	Filename gFilename;
	HobString imageFilename = "";

	if (graphicFileName.size())
	{
		gFilename.setPath(config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDIR));
		gFilename.setFileName(graphicFileName);
		if (traceOn)
		{
			HobString msgDebug2(msgDebug);
			msgDebug2.append("gFilename = ").append(gFilename.getFullPath());
			Debug::trace("Softkey", msgDebug2);
		}
		if (gFilename.getFileNameExtension().empty())
		{
			gFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDEFAULTEXTENSION));
			if (traceOn)
			{
				HobString msgDebug2(msgDebug);
				msgDebug2.append("gFilename = ").append(gFilename.getFullPath());
				Debug::trace("Softkey", msgDebug2);
			}
		}
		HobString tempFilename = gFilename.getFileName();
		fgImageExists = checkImageExists(tempFilename, config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR), config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDIR), language);

		if (!fgImageExists)
		{
			/*  We no longer want to specify a "noImage" image - 3/4/2014
			if (config->isSelfServiceScale())
			{
				Filename tempFilename;
				tempFilename.setFileName(config->getNoGraphicAvailableFileName1());
				if (tempFilename.getFileNameExtension().empty())
					tempFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDEFAULTEXTENSION));
				imageFilename = tempFilename.getFileName();
				bgImageExists = true;
			}
			else
			*/
				imageFilename = "";
		}
		else
			imageFilename = gFilename.getFullPath();
	}

	if (traceOn)
	{
		HobString msgDebug2(msgDebug);
		msgDebug2.append("gFilename = ").append(gFilename.getFullPath());
		Debug::trace("Softkey", HobString("SoftKey.imageFileName = ") + imageFilename);
	}

	return imageFilename;
}

HobString Softkey::validateProductImage(HobString &graphicFileName, short language)
{
	Configuration *config = Configuration::getInstance();
	bool traceOn = Debug::isTracing("Softkey");
	HobString msgDebug("validateProductImage(): ");
	Filename gFilename;
	HobString imageFilename = "";

	if (graphicFileName.size())
	{
		gFilename.setPath(config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR));
		gFilename.setFileName(graphicFileName);
		if (traceOn)
		{
			HobString msgDebug2(msgDebug);
			msgDebug2.append("gFilename = ").append(gFilename.getFullPath());
			Debug::trace("Softkey", msgDebug2);
		}
		if (gFilename.getFileNameExtension().empty())
		{
			gFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDEFAULTEXTENSION));
			if (traceOn)
			{
				HobString msgDebug2(msgDebug);
				msgDebug2.append("gFilename = ").append(gFilename.getFullPath());
				Debug::trace("Softkey", msgDebug2);
			}
		}
		HobString tempFilename = gFilename.getFileName();
		bgImageExists = checkImageExists(tempFilename, config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR), config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR), language);

		if (!bgImageExists)
		{
			gFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMPRODUCTGRAPHICSDIR));
			tempFilename = gFilename.getFileName();
			bgImageExists = checkImageExists(tempFilename, config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR), config->toStringCfgI(CfgInternal::SYSTEMPRODUCTGRAPHICSDIR), language);
		}

		if (!bgImageExists)
		{
			/*  We no longer want to specify a "noImage" image - 3/4/2014
			if (config->isSelfServiceScale())
			{
				Filename tempFilename;
				tempFilename.setFileName(config->getNoGraphicAvailableFileName1());
				if (tempFilename.getFileNameExtension().empty())
					tempFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDEFAULTEXTENSION));
				imageFilename = tempFilename.getFileName();
				bgImageExists = true;
			}
			else
			*/
				imageFilename = ""; //gFilename.getFullPath();
		}
		else
			imageFilename = gFilename.getFullPath();
	}
	return imageFilename;
}

HobString Softkey::validateCategoryImage(HobString &graphicFileName, short language)
{
	Configuration *config = Configuration::getInstance();
	bool traceOn = Debug::isTracing("Softkey");
	HobString msgDebug("validateCategoryImage(): ");
	Filename gFilename;
	HobString imageFilename = "";

	if (graphicFileName.size())
	{
		gFilename.setPath(config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDIR));
		gFilename.setFileName(graphicFileName);
		if (traceOn)
		{
			HobString msgDebug2(msgDebug);
			msgDebug2.append("gFilename = ").append(gFilename.getFullPath());
			Debug::trace("Softkey", msgDebug2);
		}
		if (gFilename.getFileNameExtension().empty())
		{
			gFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDEFAULTEXTENSION));
			if (traceOn)
			{
				HobString msgDebug2(msgDebug);
				msgDebug2.append("gFilename = ").append(gFilename.getFullPath());
				Debug::trace("Softkey", msgDebug2);
			}
		}
		HobString tempFilename = gFilename.getFileName();
		bgImageExists = checkImageExists(tempFilename, config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR), config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDIR), language);

		if (!bgImageExists)
		{
			gFilename.setPath(config->toStringCfgI(CfgInternal::SYSTEMCATEGORYGRAPHICSDIR));
			tempFilename = gFilename.getFileName();
			bgImageExists = checkImageExists(tempFilename, config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR), config->toStringCfgI(CfgInternal::SYSTEMCATEGORYGRAPHICSDIR), language);
		}

		if (!bgImageExists)
		{
			/*  We no longer want to specify a "noImage" image - 3/4/2014
			if (config->isSelfServiceScale())
			{
				Filename tempFilename;
				tempFilename.setFileName(config->getNoGraphicAvailableFileName1());
				if (tempFilename.getFileNameExtension().empty())
					tempFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDEFAULTEXTENSION));
				imageFilename = tempFilename.getFileName();
				bgImageExists = true;
			}
			else
			*/
				imageFilename = "";
		}
		else
			imageFilename = gFilename.getFullPath();
	}
	return imageFilename;
}

HobString Softkey::validateSoftkeyImage(HobString &graphicFileName, short language)
{
	Configuration *config = Configuration::getInstance();
	bool traceOn = Debug::isTracing("Softkey");
	HobString msgDebug("validateSoftkeyImage(): ");
	Filename gFilename;
	HobString imageFilename = "";

	if (graphicFileName.size())
	{
		gFilename.setPath(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR));
		gFilename.setFileName(graphicFileName);
		if (traceOn)
		{
			HobString msgDebug2(msgDebug);
			msgDebug2.append("gFilename = ").append(gFilename.getFullPath());
			Debug::trace("Softkey", msgDebug2);
		}
		if (gFilename.getFileNameExtension().empty())
		{
			gFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDEFAULTEXTENSION));
			if (traceOn)
			{
				HobString msgDebug2(msgDebug);
				msgDebug2.append("gFilename = ").append(gFilename.getFullPath());
				Debug::trace("Softkey", msgDebug2);
			}
		}
		HobString tempFilename = gFilename.getFileName();
		bgImageExists = checkImageExists(tempFilename, config->toStringCfgI(CfgInternal::SYSTEMUSERROOTDIR), config->toStringCfgI(CfgInternal::SOFTKEYGRAPHICSDIR), language);

		if (!bgImageExists)
		{
			/*  We no longer want to specify a "noImage" image - 3/4/2014
			if (config->isSelfServiceScale())
			{
				Filename tempFilename;
				tempFilename.setFileName(config->getNoGraphicAvailableFileName1());
				if (tempFilename.getFileNameExtension().empty())
					tempFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDEFAULTEXTENSION));
				imageFilename = tempFilename.getFileName();
				bgImageExists = true;
			}
			else
			*/
				imageFilename = "";
		}
		else
			imageFilename = gFilename.getFullPath();
	}
	return imageFilename;
}

bool Softkey::checkImageExists(HobString &graphicFileName, HobString rootPath, HobString imageDir, short UNUSED(language))
{
//	Configuration *config = Configuration::getInstance();
	bool traceOn = Debug::isTracing("Softkey");
	bool imageExists = false;

	Filename tempFilename;
	tempFilename.setPath(rootPath);
	tempFilename.appendPath(imageDir);
	tempFilename.setFileName(graphicFileName);

	HobString msgDebug = "";
	HobString myFile = tempFilename.getFullPath();
	if (traceOn)
		msgDebug.append("checkImageExists(): ").append(myFile);

	File gFile(tempFilename);
	if (gFile.exists())
		imageExists = true;

	if (traceOn)
	{
		if (imageExists)
			msgDebug.append(" exists!");
		else
			msgDebug.append(" does not exist.");
		Debug::trace("Softkey", msgDebug);
	}
	return imageExists;
}

HobRecordNum Softkey::getFeatureNumber() const
{
	return featureNumber;
}
void Softkey::setFeatureNumber(HobRecordNum number)
{
	featureNumber = number;
}


bool Softkey::isFieldQuoted(int fieldOffset) const
{
	switch (fieldOffset)
	{
		case fieldOffsetSKeyName1:
		case fieldOffsetSKeyName2:
		case fieldOffsetSKeyName1Default:
		case fieldOffsetSKeyName2Default:
		case fieldOffsetSKeyActionStringParam:	
		case fieldOffsetSKeyScriptName:
			return true;
			break;

		default:
			break;
	}
	return false;
}


bool Softkey::getForceNoBackgroundColor() const
{
	return forceNoBgColor;
}
void Softkey::setForceNoBackgroundColor(bool boolValue)
{
	forceNoBgColor = boolValue;
}

bool Softkey::getForceNoBackgroundImage() const
{
	return forceNoBgImage;
}
void Softkey::setForceNoBackgroundImage(bool boolValue)
{
	forceNoBgImage = boolValue;
}

bool Softkey::getForceNoText() const
{
	return forceNoText;
}
void Softkey::setForceNoText(bool boolValue)
{
	forceNoText = boolValue;
}

bool Softkey::isValidateBackgroundImageFile() const
{
	return validateBackgroundImageFile;
}
void Softkey::setValidateBackgroundImageFile(bool validate)
{
	validateBackgroundImageFile = validate;
	if (!validate)
		bgImageExists = true;
}

int Softkey::getPlacementAndPositionAction(HobString &parseStr)
{
	//Debug::trace("Softkey.Placement", "getPlacementAndPositionAction() #1:   incoming parseStr = " + parseStr );
	
    int codedValue = Integer::valueOf( parseStr );
	//Debug::trace("Softkey.Placement", "getPlacementAndPositionAction() #1:   parseStr to int in codedValue = " + Integer::toString(codedValue) );

	int theReturnAction = p1_noChange_noChange;

	if (codedValue < 0x1000000)
	{
		// legacy support
	    HobString temp = parseStr;
	    HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
	    HobString token;

	    short printer = 0;				//	Current support for Primary printer only
	    HobString sideStr = "NC";
	    HobString rotStr = "NC";
	    unsigned int offset = 0;

	    while ((tok.next(&token) != -1) && (offset < 3))
	    {
		    if (offset == 0)
			    printer = Integer::valueOf(token);
		    else if (offset == 1)
			    sideStr = token;
		    else if (offset == 2)
			    rotStr = token;
		    ++offset;
	    }
        
        //	Debug::trace("Softkey.Placement", "getPlacementAndPositionAction():   printer = " + Integer::toString(printer) + ",  sideStr = " + sideStr + ",  rotStr = " + rotStr);

	    int rotVal = 0;
	    if (rotStr != "NC")
		    rotVal = ((Integer::valueOf(rotStr) / 45) + 1) * 3;
        
        //	Debug::trace("Softkey.Placement", "getPlacementAndPositionAction():   rotVal = " + Integer::toString(rotVal));

	    int sideVal = 0;
	    if (sideStr == "R")
		    sideVal = 2;
	    else if (sideStr == "L")
		    sideVal = 1;
 
        //	Debug::trace("Softkey.Placement", "getPlacementAndPositionAction():   sideVal = " + Integer::toString(sideVal));

	    theReturnAction = ((printer - 1) * 27) + rotVal + sideVal;
		
		//	Debug::trace("Softkey.Placement", "getPlacementAndPositionAction():   theReturnAction = " + Integer::toString(theReturnAction));
	}
	else
	{
		// just return the number as is
		theReturnAction = codedValue;
		
        //Debug::trace("Softkey.Placement", "getPlacementAndPositionAction()#:  theReturnAction = " + Integer::toString(codedValue));
	}
        
	return theReturnAction;
}

void Softkey::getPlacementAndPositionActionString(HobString &result)
{
	//Debug::trace("Softkey.Placement", "getPlacementAndPositionAction() #2:   incoming parseStr = " + result);
	
    int codedValue = Integer::valueOf(result);
	//Debug::trace("Softkey.Placement", "getPlacementAndPositionAction() #2:   parseStr to int in codedValue = " + Integer::toString(codedValue));

	int theReturnAction = p1_noChange_noChange;

	if (codedValue < 0x1000000)
	{
        //	Debug::trace("Softkey.Placement", "getPlacementAndPositionActionString(" + result + ")");
        //	Debug::trace("Softkey.Placement", "getPlacementAndPositionActionString():   actionIntParam = " + Integer::toString(actionIntParam));

	    int printer = (actionIntParam / 27) + 1;		//	1 - Primary printer, 2 - Second printer, 3 - Third printer
        //	Debug::trace("Softkey.Placement", "getPlacementAndPositionActionString():   printer = " + Integer::toString(printer));
	    
	    result = Integer::toString(printer);

	    int offset = actionIntParam % 27;
        //	Debug::trace("Softkey.Placement", "getPlacementAndPositionActionString():   offset = " + Integer::toString(offset));

	    int rotation = ((offset / 3) - 1) * 45;
        //	Debug::trace("Softkey.Placement", "getPlacementAndPositionActionString():   rotation = " + Integer::toString(rotation));

	    int placement = offset % 3;
        //	Debug::trace("Softkey.Placement", "getPlacementAndPositionActionString():   placement = " + Integer::toString(placement));

	    result.append(",");
	    if (placement == 2)
		    result.append("R");
	    else if (placement == 1)
		    result.append("L");
	    else
		    result.append("NC");

	    result.append(",");
	    if (rotation >= 0)
		    result.append(Integer::toString(rotation));
	    else
		    result.append("NC");
		    
		//Debug::trace("Softkey.Placement", "getPlacementAndPositionActionString():   result = " + result);
	}
	else
	{
		// just return the number as is
		result = Integer::toString( codedValue );
		//Debug::trace("Softkey.Placement", "getPlacementAndPositionAction() #2:  codedValue = " + Integer::toString(codedValue) );
		//Debug::trace("Softkey.Placement", "getPlacementAndPositionAction() #2:  result = " + result );
	}
}
