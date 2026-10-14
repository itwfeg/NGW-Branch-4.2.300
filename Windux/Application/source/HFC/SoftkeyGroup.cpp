/* Evolution Scale Project
 * COPYRIGHT (c) 2005.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "SoftkeyBasics.h"
#include "SoftkeyGroup.h"
#include "SoftkeyTags.h"
#include "HobTokenizerCSV.h"
#include "HobTokenizerUS.h"
#include "HobTokenTagged.h"
#include "Configuration.h"
#include "DBAccess.h"
#include "DBException.h"
#include "DBQuery.h"
#include "DBSoftKeysRM.h"
#include "DBSoftKey.h"
#include "SoftkeyFeature.h"
#include "SoftkeyStyle.h"
#include "Softkey.h"
#include "DBProductsRM.h"
#include "DBProduct.h"
#include "Product.h"
#include "DBCategoriesRM.h"
#include "DBCategory.h"
#include "Category.h"
#include "DBOperatorsRM.h"
#include "DBOperator.h"
#include "DBSoftKeyGroupsRM.h"
#include "DBSoftKeyGroup.h"

#include "AllDepts.h"

#include "Integer.h"
#include "Long.h"
#include "Hex.h"
#include "Bool.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SoftkeyGroup::dataType = 0;
std::string SoftkeyGroup::dataTypeName("SoftkeyGroup"); 
#endif

std::list<SoftkeyGroupFieldOffsets> SoftkeyGroup::browserFieldList;
std::list<SoftkeyGroupFieldOffsets> SoftkeyGroup::loadGroupFieldList;
std::list<SoftkeyGroupFieldOffsets> SoftkeyGroup::evolFieldList;


SoftkeyGroup::SoftkeyGroup(DBAccess *dbAccess)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	myDbAccess = dbAccess;
}

SoftkeyGroup::SoftkeyGroup(DBSoftKeyGroup *dbGroupRec, DBAccess *access)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	myDbAccess = access;

	if (dbGroupRec)
	{
		myDbRecord = dbGroupRec;

		groupNumber = Long::valueOf(dbGroupRec->getNumber());

		setHorizontalGridSize(dbGroupRec->getHorizontalGridSize());
		setVerticalGridSize(dbGroupRec->getVerticalGridSize());
		horizontalSpacing = dbGroupRec->getHorizontalSpacing();
		verticalSpacing = dbGroupRec->getVerticalSpacing();
		deptNum = Long::valueOf(dbGroupRec->getDeptNumber());
	}
}

SoftkeyGroup::SoftkeyGroup(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	bool traceOn = Debug::isTracing("SoftkeyGroup");
	HobString msgDebug = "";
	if (traceOn)
		msgDebug = "Constructor:  ";

	init();
	switch (formatType)
	{
		case LoadGroupFormat:
		{
			HobString msgDebug2 = "LoadGroupFormat, token = ";
			HobString msgDebug3 = "  offset = ";

			HobString temp(data, len);
			if (traceOn)
			{
				HobString msgDebug4 = "LoadGroupFormat, data = ";
				Debug::trace("SoftkeyGroup", msgDebug + msgDebug4 + temp);
			}
			std::list<SoftkeyGroupFieldOffsets>::iterator fieldIter = loadGroupFieldList.begin();
			int offset;

			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			while ((tok.next(&token) != -1) && (fieldIter != loadGroupFieldList.end()))
			{
				offset = *fieldIter;
				if (traceOn)
				{
					HobString msgDebug4(Integer::toString(offset));
					Debug::trace("SoftkeyGroup", msgDebug + msgDebug2 + token + msgDebug3 + msgDebug4);
				}
				setReceived(offset, (token.length() == 0) ? false : true);
				updateField(offset, token);
				fieldIter++;
			}
			if (traceOn)
			{
				HobString msgDebug("Constructor:  LoadGroupFormat,  featureNumber = ");
				msgDebug.append(Long::toString(featureNumber));
				msgDebug.append("   groupNumber = ").append(Long::toString(groupNumber));
				Debug::trace("SoftkeyGroup", msgDebug);
			}
			break;
		}

		case BrowserFormat:
		{
			HobString temp(data, len);
			std::list<SoftkeyGroupFieldOffsets>::iterator fieldIter = browserFieldList.begin();
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
			if (!isReceived(fieldOffsetSKGDeptNum))
			{
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
				setReceived(fieldOffsetSKGDeptNum, true);
			}

			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			SoftkeyGroupTags softkeyGroupTags;

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
					offset = (temp.size()) ? softkeyGroupTags.getOffset(temp) : 0;
					Debug::trace("SoftkeyGroup", HobString("SoftkeyGroup(data,len,format) token: ") + temp + HobString("  offset:") + HobString(Integer::toString(offset)) + HobString("  data:") + token->getData());
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
			if (!isReceived(fieldOffsetSKGDeptNum) && Configuration::getInstance()->toBoolCfg(fieldOffsetCfgUseCurrentDeptnumWhenNotReceived))
			{
				setReceived(fieldOffsetSKGDeptNum, true);
				setDeptNum(Configuration::getInstance()->toIntCfg(fieldOffsetCfgMostRecentActiveDeptNum));
			}
			break;
		}

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

SoftkeyGroup::~SoftkeyGroup()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	if (feature)
		delete feature;
	if (style)
		delete style;
	if (myDbRecord)
		delete myDbRecord;
}

// private
void SoftkeyGroup::init()
{
	feature = 0;
	style = 0;

	featureNumber	= RECORD_NOT_FOUND;
	groupNumber		= RECORD_NOT_FOUND;

	lang1NameList.setQuotingElements(true);
	lang2NameList.setQuotingElements(true);

	categoryAction = Action_NoAction;
	languageSelection = 1;
	currentSerializeOffset = 0;
	gridCellCount = 0;

	horizontalGridSize	= 1;
	verticalGridSize	= 1;
	horizontalSpacing	= 0;
	verticalSpacing		= 0;

	deptNum = 0;

	myDbAccess = 0;
	myDbRecord = 0;

	initFieldLists();
}

void SoftkeyGroup::initFieldLists()
{
	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetSKGGroupNumber);
		browserFieldList.push_back(fieldOffsetSKGHorizontalGridSize);
		browserFieldList.push_back(fieldOffsetSKGVerticalGridSize);
		browserFieldList.push_back(fieldOffsetSKGHorizontalSpacing);
		browserFieldList.push_back(fieldOffsetSKGVerticalSpacing);
	}
	if (loadGroupFieldList.empty())
	{
		loadGroupFieldList.push_back(fieldOffsetSKGFeatureNumber);
		loadGroupFieldList.push_back(fieldOffsetSKGGroupNumber);
	}
	if (evolFieldList.empty())
	{
		evolFieldList.push_back(fieldOffsetSKGDeptNum);
		evolFieldList.push_back(fieldOffsetSKGGroupNumber);
		evolFieldList.push_back(fieldOffsetSKGHorizontalGridSize);
		evolFieldList.push_back(fieldOffsetSKGVerticalGridSize);
		evolFieldList.push_back(fieldOffsetSKGHorizontalSpacing);
		evolFieldList.push_back(fieldOffsetSKGVerticalSpacing);

		SoftkeyGroupTags softkeyGroupTags((FieldOffsetsList *)&evolFieldList);
	}
}

HobRecordNum SoftkeyGroup::getDeptNum() const
{
	return deptNum;
}
void SoftkeyGroup::setDeptNum(HobRecordNum num)
{
	deptNum = num;
}

void SoftkeyGroup::setDBAccess(DBAccess *access)
{
	myDbAccess = access;
}

DBSoftKeyGroup *SoftkeyGroup::getMyDbRecord()
{
	return myDbRecord;
}

bool SoftkeyGroup::isRecordValid()
{
	if (myDbRecord)
	 	return myDbRecord->isRecordValid();
	else
		return false;
}


HobRecordNum SoftkeyGroup::getGroupNumber() const
{
	return groupNumber;
}
void SoftkeyGroup::setGroupNumber(HobRecordNum number)
{
	groupNumber = number;
}


SoftkeyFeature *SoftkeyGroup::getSoftkeyFeature()
{
	return feature;
}
void SoftkeyGroup::setSoftkeyFeature(SoftkeyFeature *newFeature)
{
	feature = newFeature;
}


SoftkeyStyle *SoftkeyGroup::getSoftkeyStyle()
{
	return style;
}
void SoftkeyGroup::setSoftkeyStyle(SoftkeyStyle *newStyle)
{
	style = newStyle;
}

HobRecordNum SoftkeyGroup::getFeatureNumber() const
{
	return featureNumber;
}
void SoftkeyGroup::setFeatureNumber(HobRecordNum number)
{
	featureNumber = number;
}


short SoftkeyGroup::getCategoryAction()
{
	return categoryAction;
}
void SoftkeyGroup::setCategoryAction(short number)
{
	categoryAction = number;
}

int SoftkeyGroup::getLanguageSelection()
{
	return languageSelection;
}
void SoftkeyGroup::setLanguageSelection(int selection)
{
	languageSelection = selection;
}


bool SoftkeyGroup::doesSoftkeyFeatureExist()
{
	if (feature)
		return true;
	else
		return false;
}
bool SoftkeyGroup::doesSoftkeyStyleExist()
{
	if (style)
		return true;
	else
		return false;
}


const char *SoftkeyGroup::serialize(int format) const
{
	bool traceOn = Debug::isTracing("SoftkeyGroup");
	HobString myDebug = "serialize():  ";
	HobString featureData, styleData;

	serializedData = "";
	int loopCount = 0;

	if (getFormatType() != UnknownFormat)
		format = getFormatType();
	if (traceOn)
	{
		HobString myDebug2 = "format = ";
		myDebug2.append(Integer::toString(format));
		myDebug2.append("   feature = ").append(ptrtostring(feature));
		myDebug2.append("   style = ").append(ptrtostring(style));
		if ( feature )
			myDebug2.append("   featureNumber = ").append(Long::toString(feature->getFeatureNumber()));
		Debug::trace("SoftkeyGroup", myDebug + myDebug2);
	}

	std::list<SoftkeyGroupFieldOffsets>::iterator fieldIter;
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

				getFieldAsString(*fieldIter, fieldData);
				serializedData.append(fieldData);
				fieldIter++;
			}
			break;
		}

		case SoftkeyGroupFormat:
		{
			HobString fieldData = "";

			getFieldAsString(fieldOffsetSKGFeatureNumber, fieldData);
			serializedData.append(fieldData);

			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData);
				serializedData.append(fieldData);
				fieldIter++;
			}
			serializedData.append(BrowserFormatSeparator);

			styleData = style->serialize(format);
			serializedData.append(styleData);
			serializedData.append(BrowserFormatSeparator);

			fieldData = Integer::toString(currentSerializeOffset);
			serializedData.append(fieldData);
			serializedData.append(BrowserFormatSeparator);

			int totalNumberOfSoftkeys = (int)hPosList.size();
			fieldData = Integer::toString(totalNumberOfSoftkeys);
			serializedData.append(fieldData);
			serializedData.append(BrowserFormatSeparator);

			//	Change the format for the lists.
			int format2 = BrowserFormat;
	
			serializedData.append(hPosList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(vPosList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(hSizeList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(vSizeList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(bgColorList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(bgImageList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);

			if (languageSelection == 1)
				serializedData.append(lang1NameList.serialize(format2));
			else
				serializedData.append(lang2NameList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);

			serializedData.append(actionList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(actionParameterList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(actionFeatureNumList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(isEnabledList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(fgImageList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(textColorList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(scriptNameList.serialize(format2));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(scriptIntervalList.serialize(format2));
			break;
		}

		case SoftkeyGroupSelfServeFormat:
		{
			HobString fieldData = "";

			fieldData = Long::toString(feature->getFeatureNumber());
			serializedData.append(fieldData);

			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				serializedData.append(BrowserFormatSeparator);

				getFieldAsString(*fieldIter, fieldData);
				serializedData.append(fieldData);
				fieldIter++;
			}
			serializedData.append(BrowserFormatSeparator);

			styleData = style->serialize(SoftkeyGroupFormat);
			serializedData.append(styleData);
			serializedData.append(BrowserFormatSeparator);

			fieldData = Integer::toString(currentSerializeOffset);
			serializedData.append(fieldData);
			serializedData.append(BrowserFormatSeparator);

			int totalNumberOfSoftkeys = (int)hPosList.size();
			fieldData = Integer::toString(totalNumberOfSoftkeys);
			serializedData.append(fieldData);
			serializedData.append(BrowserFormatSeparator);

			int cellCount = horizontalGridSize * verticalGridSize;
			if (traceOn)
			{
				HobString myDebug = "serialize(SoftkeyGroupSelfServeFormat):  cellCount = ";
				myDebug.append(Integer::toString(cellCount)).append("   currentSerializeOffset = ").append(Integer::toString(currentSerializeOffset));
				Debug::trace("SoftkeyGroup", myDebug);
			}

			//	Change the format for the lists.
			int format2 = BrowserFormat;

			serializedData.append(hPosList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(vPosList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(hSizeList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(vSizeList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(bgColorList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(bgImageList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);

			if (languageSelection == 1)
				serializedData.append(lang1NameList.serialize(format2, currentSerializeOffset, cellCount));
			else
				serializedData.append(lang2NameList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);

			serializedData.append(actionList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(actionParameterList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(actionFeatureNumList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(isEnabledList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(fgImageList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(textColorList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(scriptNameList.serialize(format2, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(scriptIntervalList.serialize(format2, currentSerializeOffset, cellCount));
			break;
		}

		case SynchronizationTaggedFormat:
		case EvolutionTaggedFormat:
		case DBIndependentFormatFormat:
		{
			SoftkeyGroupTags softkeyGroupTags;
			HobString tag = "";
			HobString fieldData = "";

			std::list<SoftkeyGroupFieldOffsets>::iterator fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				tag = softkeyGroupTags.getTag(*fieldIter);
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
		{
			serializedData = "SoftkeyGroup::serialize - unknown format: ";
			serializedData.append(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
		}
	}
	return serializedData.c_str();
}


void SoftkeyGroup::gotoFirstPage()
{
	currentSerializeOffset = 0;
	if (Debug::isTracing("SoftkeyGroup.test"))
	{
		HobString myDebug("gotoFirstPage():  currentSerializeOffset = ");
		myDebug.append(Integer::toString(currentSerializeOffset));
		Debug::trace("SoftkeyGroup.test", myDebug);
	}
}

void SoftkeyGroup::gotoNextPage()
{
#if defined(LINUX_FIX)
	if ((currentSerializeOffset + gridCellCount) < (int)hPosList.size())
#else
	if ((currentSerializeOffset + gridCellCount) < hPosList.size())
#endif
    {
		currentSerializeOffset += gridCellCount;
    }
	if (Debug::isTracing("SoftkeyGroup.test"))
	{
		HobString myDebug("gotoNextPage():  currentSerializeOffset = ");
		myDebug.append(Integer::toString(currentSerializeOffset));
		Debug::trace("SoftkeyGroup.test", myDebug);
	}
}

void SoftkeyGroup::gotoPreviousPage()
{
	if (currentSerializeOffset > 0)
		currentSerializeOffset -= gridCellCount;
	if (Debug::isTracing("SoftkeyGroup.test"))
	{
		HobString myDebug("gotoPreviousPage():  currentSerializeOffset = ");
		myDebug.append(Integer::toString(currentSerializeOffset));
		Debug::trace("SoftkeyGroup.test", myDebug);
	}
}

void SoftkeyGroup::setGridCellCount(int count)
{
	gridCellCount = count;
	if (Debug::isTracing("SoftkeyGroup.test"))
	{
		HobString myDebug("setGridCellCount():  gridCellCount = ");
		myDebug.append(Integer::toString(gridCellCount));
		Debug::trace("SoftkeyGroup.test", myDebug);
	}
}

int SoftkeyGroup::getGridCellCount() const
{
	return gridCellCount;
}

void SoftkeyGroup::getEndOfPath(HobString& result, const HobString& fullPath)
{
	std::string::size_type pos1 = 0;

	pos1 = fullPath.find("/");
	if (pos1 > 0)
		result = fullPath.substr(pos1 + 1);
	else
		result = "";
}


void SoftkeyGroup::add(Softkey &softkey, long featureNum, int languageSelection, int hPos, int vPos, int hSize, int vSize, bool setDefaultValues)
{
	Debug::trace("SoftkeyGroup", "add(Softkey)");

	HobString tempStr = "";
	int format = BrowserFormat;

	if (setDefaultValues)
		softkey.setupSoftkeyDefaultValues(languageSelection, SoftkeyDataUsingDefaults, featureNum);

	//	Push the softkey info on the list
	if (hPos == -1)
		hPos = softkey.getHorizontalPosition();
//	Debug::trace("SoftkeyGroup", "add(Softkey): pushing hPos as " + inttostring(hPos));
	hPosList.push_back(Integer::toString(hPos));

	if (vPos == -1)
		vPos = softkey.getVerticalPosition();
//	Debug::trace("SoftkeyGroup", "add(Softkey): pushing vPos as " + inttostring(vPos));
	vPosList.push_back(Integer::toString(vPos));

	if (hSize == -1)
		hSize = softkey.getHorizontalSize();
	hSizeList.push_back(Integer::toString(hSize));

	if (vSize == -1)
		vSize = softkey.getVerticalSize();
	vSizeList.push_back(Integer::toString(vSize));

	softkey.getFieldAsString(fieldOffsetSKeyBackgroundColor, tempStr, format);
	bgColorList.push_back(tempStr);

	softkey.getFieldAsString(fieldOffsetSKeyBackgroundImage, tempStr, format);
	bgImageList.push_back(tempStr);

	softkey.getFieldAsString(fieldOffsetSKeyForegroundImage, tempStr, format);
	fgImageList.push_back(tempStr);

	softkey.getFieldAsString(fieldOffsetSKeyTextColor, tempStr, format);
	textColorList.push_back(tempStr);

	softkey.getFieldAsString(fieldOffsetSKeyName1, tempStr, format);
	lang1NameList.push_back(tempStr);

	softkey.getFieldAsString(fieldOffsetSKeyName2, tempStr, format);
	lang2NameList.push_back(tempStr);

	softkey.getFieldAsString(fieldOffsetSKeyAction, tempStr, format);
	actionList.push_back(tempStr);

	long actionFeatureNum = softkey.getActionFeatureNumber();
	if (actionFeatureNum == RECORD_NOT_FOUND)
		actionFeatureNum = getSoftkeyFeature()->getFeatureNumber();
	actionFeatureNumList.push_back(Long::toString(actionFeatureNum));

	isEnabledList.push_back(Bool::toString(softkey.isSoftkeyEnabled()));

	switch (softkey.getAction())
	{
		case Action_ApplyDiscount_PercentOff:
		case Action_ApplyDiscount_CentsOff:
		case Action_ChangeLanguage:
		case Action_ChangeScreen:
		case Action_ChangeDepartment:
		case Action_SwitchTotalType:
		case Action_LoadSoftkeyGroup:
		case Action_LoadTareSoftkey:
			softkey.getFieldAsString(fieldOffsetSKeyActionIntegerParam, tempStr, format);
			break;
			
		case Action_ChangeLabelPlacementAndPosition:
		case Action_Swipe:
			softkey.getFieldAsString(fieldOffsetSKeyActionIntegerParam, tempStr, format);
			break;
		case Action_BrowseWeb:
		case Action_FPP_URL:
		case Action_Application_URL:
		case Action_PlayVideo:
		case Action_LaunchExternalApp:
		case Action_ToggleLabelType:
		case Action_ForcedCameraOverride:
			softkey.getFieldAsString(fieldOffsetSKeyActionStringParam, tempStr, format);
			actionParameterList.setQuotingElements(true);
			break;
		case Action_LoadCategory_First:
		case Action_LoadCategory_Next:
		case Action_LoadCategory_Previous:
			softkey.getFieldAsString(fieldOffsetSKeyActionCategoryNum, tempStr, format);
			break;
		case Action_LoginOperator:
			softkey.getFieldAsString(fieldOffsetSKeyActionOperatorNum, tempStr, format);
			break;
		case Action_SelectProduct:
			softkey.getFieldAsString(fieldOffsetSKeyActionProductNum, tempStr, format);
			break;
		case Action_LoadAlphabeticSearch_First:
		case Action_LoadAlphabeticSearch_Next:
		case Action_LoadAlphabeticSearch_Previous:
			softkey.getFieldAsString(fieldOffsetSKeyActionStringParam, tempStr, format);
			actionParameterList.setQuotingElements(true);
			break;
		default:
			break;
	}
	actionParameterList.push_back(tempStr);
	if (softkey.getAction() == Action_ToggleLabelType)
		Debug::trace("SoftkeyGroup", "add(Softkey): Action_ToggleLabelType [" + tempStr + "]");

	softkey.getFieldAsString(fieldOffsetSKeyScriptName, tempStr, format);
	scriptNameList.push_back(tempStr);

	softkey.getFieldAsString(fieldOffsetSKeyScriptInterval, tempStr, format);
	scriptIntervalList.push_back(tempStr);
}


bool SoftkeyGroup::addProductSoftkey(DBProduct *dbProductRec, long featureNum, short languageSelection, int hPos, int vPos, int hSize, int vSize, DBCategory *dbCategoryRec, bool allowDuplicates)
{
	Debug::trace("SoftkeyGroup", "addProductSoftkey()");

	if (!dbProductRec || !myDbAccess)
		return false;

	if (!allowDuplicates)
	{
		// Duplicates not allowed, so check and see if this plu is already in this softkey group.
		// If it is, ignore it and just return; don't add it.
//		HobRecordNum pluNum = Long::valueOf(dbProductRec->getPLU());
		HobString pluNumS = dbProductRec->getPLU();
		if (actionParameterList.exists(pluNumS))
			return false;
	}
	Softkey softkey(myDbAccess);
	softkey.setupSoftkeyDefaultValues(languageSelection, SoftkeyDataUsingDefaults, featureNum, dbProductRec, dbCategoryRec);
	softkey.setAction(Action_SelectProduct);
	softkey.setActionProductNumber(Long::valueOf(dbProductRec->getPLU()));

	add(softkey, featureNum, languageSelection, hPos, vPos, hSize, vSize, false);
	return true;
}

void SoftkeyGroup::addCategorySoftkey(DBCategory *dbCategoryRec, long featureNum, short languageSelection, int hPos, int vPos, int hSize, int vSize)
{
	Debug::trace("SoftkeyGroup", "addCategorySoftkey()");

	if (!dbCategoryRec || !myDbAccess)
		return;

	Softkey softkey(myDbAccess);
	softkey.setupSoftkeyDefaultValues(languageSelection, SoftkeyDataUsingDefaults, featureNum, (DBProduct *) NULL, dbCategoryRec);
	softkey.setAction(Action_LoadCategory_First);
	softkey.setActionCategoryNumber(Long::valueOf(dbCategoryRec->getNumber()));

	add(softkey, featureNum, languageSelection, hPos, vPos, hSize, vSize, false);
}

int SoftkeyGroup::getHorizontalGridSize() const
{
	return horizontalGridSize;
}
void SoftkeyGroup::setHorizontalGridSize(int size)
{
	if (size > 0)
		horizontalGridSize = size;
}

int SoftkeyGroup::getVerticalGridSize() const
{
	return verticalGridSize;
}
void SoftkeyGroup::setVerticalGridSize(int size)
{
	if (size > 0)
		verticalGridSize = size;
}

int SoftkeyGroup::getHorizontalSpacing() const
{
	return horizontalSpacing;
}
void SoftkeyGroup::setHorizontalSpacing(int spacing)
{
	horizontalSpacing = spacing;
}

int SoftkeyGroup::getVerticalSpacing() const
{
	return verticalSpacing;
}
void SoftkeyGroup::setVerticalSpacing(int spacing)
{
	verticalSpacing = spacing;
}


SoftkeyGroup *SoftkeyGroup::find(HobRecordNum recordNum, HobRecordNum deptNum, DBAccess *dbAccess)
{
	SoftkeyGroup *readSoftkeyGroup = (SoftkeyGroup *)NULL;

	if (recordNum <= 0 || !dbAccess)
		return readSoftkeyGroup;

	DBSoftKeyGroupsRM *dbSoftkeyGroupsRM = dbAccess->getDBSoftKeyGroupsRM();
	if (!dbSoftkeyGroupsRM)
		return readSoftkeyGroup;

	HobString num(Long::toString(recordNum));
	HobString dnum(Long::toString(deptNum));

	try
	{
		DBSoftKeyGroup *dbSoftkeyGroupRec = dbSoftkeyGroupsRM->find(num, dnum);
		if (dbSoftkeyGroupRec)
		{
			Debug::trace("SoftkeyGroup", "find() - found the record in the db");
			readSoftkeyGroup = new SoftkeyGroup(dbSoftkeyGroupRec, dbAccess);
			if (!readSoftkeyGroup)
			{
				Debug::trace("SoftkeyGroup", "find() - But I can't create a new SoftkeyGroup object!");
				delete dbSoftkeyGroupRec;
			}
			else
			{
				if (Debug::isTracing("SoftkeyGroup"))
				{
					HobString myDebug("find() - created the hfc SoftkeyGroup object - SoftkeyGroup: ");
					myDebug.append(ptrtostring(readSoftkeyGroup)).append("   recordNum: ").append(num).append("   deptNum: ").append(Long::toString((long)deptNum));
					Debug::trace("SoftkeyGroup", myDebug);
				}
			}
		}
	}
	catch (DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", "SoftkeyGroup::find() ********** database exception **********");
			DBException::Type t = dbe.getType();
			HobString myDebug("     DBException: type: ");
			myDebug.append(Integer::toString(t)).append("  info: ").append(dbe.info());
			Debug::trace("DBAccess.exception", myDebug);
		}
	}

	return readSoftkeyGroup;
}


int SoftkeyGroup::updateField(int fieldCode, const HobString &token)
{
	if (Debug::isTracing("SoftkeyGroup"))
	{
		HobString myDebug("updateField() - fieldCode: ");
		myDebug.append(Integer::toString(fieldCode)).append("  data: ").append(token);
		Debug::trace("SoftkeyGroup", myDebug);
	}

	if (fieldCode < 0)
		return -1;

	switch (fieldCode)
	{
		case fieldOffsetSKGGroupNumber:
			setGroupNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKGFeatureNumber:
			setFeatureNumber((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetSKGHorizontalGridSize:
			setHorizontalGridSize(Integer::valueOf(token));
			break;
		case fieldOffsetSKGVerticalGridSize:
			setVerticalGridSize(Integer::valueOf(token));
			break;
		case fieldOffsetSKGHorizontalSpacing:
			setHorizontalSpacing(Integer::valueOf(token));
			break;
		case fieldOffsetSKGVerticalSpacing:
			setVerticalSpacing(Integer::valueOf(token));
			break;
		case fieldOffsetSKGDeptNum:
			setDeptNum((HobRecordNum)Long::valueOf(token));
			break;
		default:
			break;
	}

	return 0;
}


void SoftkeyGroup::update(const SoftkeyGroup *newGroup)
{
	if (!newGroup)
		return;

	Debug::trace("SoftkeyGroup", "Entering update ...");

	short loopCnt = 1;
	while (loopCnt < endSoftkeyGroupFieldOffsets)
	{
		if (newGroup->isReceived(loopCnt))
		{
			switch (loopCnt)
			{
				case fieldOffsetSKGGroupNumber:
					setGroupNumber(newGroup->getGroupNumber());
					break;
				case fieldOffsetSKGFeatureNumber:
					setFeatureNumber(newGroup->getFeatureNumber());
					break;
				case fieldOffsetSKGHorizontalGridSize:
					setHorizontalGridSize(newGroup->getHorizontalGridSize());
					break;
				case fieldOffsetSKGVerticalGridSize:
					setVerticalGridSize(newGroup->getVerticalGridSize());
					break;
				case fieldOffsetSKGHorizontalSpacing:
					setHorizontalSpacing(newGroup->getHorizontalSpacing());
					break;
				case fieldOffsetSKGVerticalSpacing:
					setVerticalSpacing(newGroup->getVerticalSpacing());
					break;
				case fieldOffsetSKGDeptNum:
					setDeptNum(newGroup->getDeptNum());
					break;
				default:
					break;
			}
		}
		loopCnt++;
	}
	Debug::trace("SoftkeyGroup", "Leaving update ...");
}


void SoftkeyGroup::getFieldAsString(int fieldCode, HobString &result) const
{
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetSKGGroupNumber:
			if (groupNumber > 0)
				result = Long::toString(groupNumber);
			break;
		case fieldOffsetSKGFeatureNumber:
			result = Long::toString(featureNumber);
			break;
		case fieldOffsetSKGDeptNum:
			result = Long::toString(deptNum);
			break;
		case fieldOffsetSKGHorizontalGridSize:
			result = Integer::toString(horizontalGridSize);
			break;
		case fieldOffsetSKGVerticalGridSize:
			result = Integer::toString(verticalGridSize);
			break;
		case fieldOffsetSKGHorizontalSpacing:
			result = Integer::toString(horizontalSpacing);
			break;
		case fieldOffsetSKGVerticalSpacing:
			result = Integer::toString(verticalSpacing);
			break;
		default:
			Debug::trace("SoftkeyGroup", "getFieldAsString():  Invalid field code!!!");
			break;
	}
	if (Debug::isTracing("SoftkeyGroup"))
	{
		HobString myDebug = "getFieldAsString():  fieldCode = ";
		myDebug.append(Integer::toString(fieldCode)).append("   result = [");
		myDebug.append(result).append("]");
		Debug::trace("SoftkeyGroup", myDebug);
	}
}

int SoftkeyGroup::deleteRecord()
{
	int recordsDeleted = 0;

	if (myDbRecord && myDbAccess)
	{
		DBSoftKeyGroupsRM *dbSoftkeyGroupsRM = myDbAccess->getDBSoftKeyGroupsRM();
		if (dbSoftkeyGroupsRM)
		{
			recordsDeleted = dbSoftkeyGroupsRM->deleteRecord(myDbRecord);
		}
		else
		{
			Debug::trace("SoftkeyGroup", "Cannot delete this record!");
		}
	}
	return recordsDeleted;
}

void SoftkeyGroup::saveRecord()
{
	HobString msgDebug, msgDebug2;
	bool saveDebug = Debug::isTracing("SoftkeyGroup.save");

	Debug::trace("SoftkeyGroup", "Entering saveRecord ...");
	if (myDbAccess)
	{
		DBSoftKeyGroupsRM *dbSoftkeyGroupsRM = myDbAccess->getDBSoftKeyGroupsRM();
		if (!dbSoftkeyGroupsRM)
			return;

		if (!myDbRecord)
		{
			Debug::trace("SoftkeyGroup", "saveRecord() - don't have a db record, creating it...");
			myDbRecord = dbSoftkeyGroupsRM->createRecord(Long::toString(groupNumber));
		}

		if (myDbRecord)
		{
			if (Debug::isTracing("SoftkeyGroup"))
			{
				Debug::trace("SoftkeyGroup", "Starting to save fields to db record ...");
				const char *z = serialize(BrowserFormat);
				msgDebug = "Before saving fields to db record ... ";
				msgDebug.append(z);
				Debug::trace("SoftkeyGroup", msgDebug);
			}

			//	Plug the changes back into the dbSoftkey Record.
			myDbRecord->setDeptNumber(Long::toString(deptNum));
			myDbRecord->setNumber(Integer::toString(groupNumber));
			myDbRecord->setHorizontalGridSize(horizontalGridSize);
			myDbRecord->setVerticalGridSize(verticalGridSize);
			myDbRecord->setHorizontalSpacing(horizontalSpacing);
			myDbRecord->setVerticalSpacing(verticalSpacing);

			//	Now save the SoftkeyGroup record to the database.
			if (saveDebug)
			{
				msgDebug = "Before saving to db ...  groupNumber = ";
				msgDebug2 = Long::toString(groupNumber);
				msgDebug.append(msgDebug2);
				Debug::trace("SoftkeyGroup.save", msgDebug);
			}

			try
			{
				dbSoftkeyGroupsRM->saveRecord(myDbRecord);
			}
			catch (DBException &dbe)
			{
				Debug::trace("DBAccess.exception", "SoftkeyGroup::saveRecord() ********** database exception **********");
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
				msgDebug = "After saving to db ...  groupNumber = ";
				msgDebug.append(msgDebug2);
				Debug::trace("SoftkeyGroup.save", msgDebug);
			}
		}
	}
}


// static
void SoftkeyGroup::verifyReservedRecords(DBAccess *dbAccess, bool createDefaultKeys)
{
	Debug::trace("SoftkeyGroup", "verifying all reserved Softkey Groups ...");

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

		//	NOTE: These SoftkeyGroups are based on the SoftkeyFeatures with the same record number.

		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_ProductGrid, 6, 4, 1, 1, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_StartScreenButtons, 1, 5, 0, 0, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_RunScreenButtons, 3, 3, 2, 2, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_PresetTareGrid, 6, 4, 1, 1, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_CategoryButtons, 1, 4, 0, 24, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_MostCommonlyUsedItems, 3, 4, 2, 2, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, ReservedSK_SelfservCategoryButtons, 5, 1, 2, 2, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_StartScreenCutTestOff, 1, 5, 0, 1, createDefaultKeys);				//	Start screen Cutting test OFF buttons
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_RunScreenCutTestOff, 1, 7, 0, 1, createDefaultKeys);				//	Run screen Cutting test OFF buttons
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_StartScreenCutTestOn, 1, 5, 0, 1, createDefaultKeys);				//	Start screen Cutting test ON buttons
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_RunScreenCutTestOn, 1, 7, 0, 1, createDefaultKeys);					//	Run screen Cutting test ON buttons
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, ReservedSK_SelfservCategoryResultsButtons, 6, 3, 2, 2, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, ReservedSK_SelfservAZCategoryResults, 7, 2, 0, 0, createDefaultKeys);	    // Category A-Z results feature for Self Serve
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_RunScreenNumericKeypadButtons, 2, 4, 1, 1, createDefaultKeys);		//	Run screen when the numeric keypad is hidden (Fastrack)
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_BuildAMealScreenButtons, 4, 4, 1, 1, createDefaultKeys);			//	ProgrammableButtons for the Build-A-Meal screen (Fastrack).
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, ReservedSK_SelfservAZCategoryButtons, 2, 5, 0, 1, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_RunScreenCutTestOnOptionOne, 3, 3, 2, 2, createDefaultKeys);		//	Run screen Cutting test Option One ON buttons
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_FixedRowColGrid, 6, 4, 1, 1, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_CameraSystemPredictions, 3, 2, 2, 2, createDefaultKeys);			// CAMERA -> Edgify and Tiliter Camera Predictions for Self Service 
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_ForcedCameraOverride, 4, 4, 1, 1, createDefaultKeys);
		SoftkeyGroup::createReservedGroupRecord(dbAccess, deptNum, Reserved_FreshServeButtons, 5, 3, 1, 1, createDefaultKeys);					// New 888050 feature for SS G2 FS UI

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
void SoftkeyGroup::createReservedGroupRecord(DBAccess *dbAccess, HobRecordNum deptNum, HobRecordNum groupNum, int hSize, int vSize, int hSpacing, int vSpacing, bool createDefaultKeys)
{
	if (!dbAccess)
		return;

	bool traceOn = Debug::isTracing("SoftkeyGroup");

	try
	{
		SoftkeyGroup *reservedSoftkeyGroup = SoftkeyGroup::find(groupNum, deptNum, dbAccess);
		if (!reservedSoftkeyGroup)
		{
			if (traceOn)
				Debug::trace("SoftkeyGroup", string("SoftkeyGroup::createReservedGroupRecord() - creating Group # ") + longtostring(groupNum) +
											"  hSpacing = " + inttostring(hSpacing) + "  vSpacing = " + inttostring(vSpacing) + "  hSize = " + inttostring(hSize) + "  vSize = " + inttostring(vSize));
			reservedSoftkeyGroup = new SoftkeyGroup();
			if (reservedSoftkeyGroup)
			{
				reservedSoftkeyGroup->setDeptNum(deptNum);
				reservedSoftkeyGroup->setGroupNumber(groupNum);
				reservedSoftkeyGroup->setHorizontalSpacing(hSpacing);
				reservedSoftkeyGroup->setVerticalSpacing(vSpacing);
				reservedSoftkeyGroup->setHorizontalGridSize(hSize);
				reservedSoftkeyGroup->setVerticalGridSize(vSize);
				reservedSoftkeyGroup->setDBAccess(dbAccess);
				reservedSoftkeyGroup->saveRecord();
			}

			if (createDefaultKeys)
				Softkey::verifyReservedRecords(dbAccess, groupNum);
		}

		if (reservedSoftkeyGroup)
		{
			delete reservedSoftkeyGroup;
			reservedSoftkeyGroup = 0;
		}
	}
	catch (DBException &dbe)
	{
		DBException::Type t = dbe.getType();
		Debug::trace("SoftkeyGroup", string("SoftkeyGroup::createReservedGroupRecord() - ********** EXCEPTION **********   DBException: type: ") + inttostring(t) + "  info: " + dbe.info());
	}

}


Softkey *SoftkeyGroup::getSoftkey(int whichButton)
//add(Softkey &softkey, long featureNum, int languageSelection, int hPos, int vPos, int hSize, int vSize, bool setDefaultValues)
{
	Debug::trace("SoftkeyGroup", "add(Softkey)");

	if ( whichButton > 0 && whichButton <= (int) actionList.size() )
	{
		--whichButton;  // Is really zero-based when accessing the data in the lists, so make it easier and subtract 1 up front, before accessing the lists.
		Softkey *softKey = new Softkey();
		if (softKey)
		{
			softKey->setGroupNumber(groupNumber);
			softKey->setHorizontalPosition(stringtoint(hPosList.getElementAtPosition(whichButton)));
			softKey->setVerticalPosition(stringtoint(vPosList.getElementAtPosition(whichButton)));
			softKey->setHorizontalSize(stringtoint(hSizeList.getElementAtPosition(whichButton)));
			softKey->setVerticalSize(stringtoint(vSizeList.getElementAtPosition(whichButton)));
			softKey->setBackgroundColor(bgColorList.getElementAtPosition(whichButton));
			softKey->setBackgroundImage(bgImageList.getElementAtPosition(whichButton));
			softKey->setDefaultForegroundImage(fgImageList.getElementAtPosition(whichButton));
			softKey->setTextColor(textColorList.getElementAtPosition(whichButton));
			softKey->setName1(lang1NameList.getElementAtPosition(whichButton));
			softKey->setName2(lang2NameList.getElementAtPosition(whichButton));
			softKey->setAction(stringtoint(actionList.getElementAtPosition(whichButton)));
			softKey->setActionFeatureNumber(stringtolong(actionFeatureNumList.getElementAtPosition(whichButton)));

			switch (softKey->getAction())
			{
				case Action_ApplyDiscount_PercentOff:
				case Action_ApplyDiscount_CentsOff:
				case Action_ChangeLanguage:
				case Action_ChangeScreen:
				case Action_ChangeDepartment:
				case Action_SwitchTotalType:
				case Action_LoadSoftkeyGroup:
				case Action_LoadTareSoftkey:
				case Action_ChangeLabelPlacementAndPosition:
				case Action_Swipe:
					softKey->setActionIntegerParam(stringtoint(actionParameterList.getElementAtPosition(whichButton)));
					break;
				case Action_BrowseWeb:
				case Action_FPP_URL:
				case Action_Application_URL:
				case Action_PlayVideo:
				case Action_LaunchExternalApp:
				case Action_ToggleLabelType:
					softKey->setActionStringParam(actionParameterList.getElementAtPosition(whichButton));
					break;
				case Action_LoadCategory_First:
				case Action_LoadCategory_Next:
				case Action_LoadCategory_Previous:
					softKey->setActionCategoryNumber(stringtolong(actionParameterList.getElementAtPosition(whichButton)));
					break;
				case Action_LoginOperator:
					softKey->setActionOperatorNumber(actionParameterList.getElementAtPosition(whichButton));
					break;
				case Action_SelectProduct:
					softKey->setActionProductNumber(stringtolong(actionParameterList.getElementAtPosition(whichButton)));
					break;
				case Action_LoadAlphabeticSearch_First:
				case Action_LoadAlphabeticSearch_Next:
				case Action_LoadAlphabeticSearch_Previous:
					softKey->setActionStringParam(actionParameterList.getElementAtPosition(whichButton));
					break;
				default:
					break;
			}
		}
		return softKey;
	}

	return 0;
}


void SoftkeyGroup::show(const HobString &title)
{
	if (Debug::isTracing("SoftkeyGroup.show"))
	{
		HobString myDebug = title;
		myDebug.append("\n*******************************************************");
		myDebug.append("\n   groupNumber:        ").append(Long::toString(groupNumber));
		myDebug.append("\n   horizontalGridSize: ").append(Integer::toString(horizontalGridSize));
		myDebug.append("\n   verticalGridSize:   ").append(Integer::toString(verticalGridSize));
		myDebug.append("\n   horizontalSpacing:  ").append(Integer::toString(horizontalSpacing));
		myDebug.append("\n   verticalSpacing:    ").append(Integer::toString(verticalSpacing));
		myDebug.append("\n*******************************************************");
		Debug::trace("SoftkeyGroup.show", myDebug);
	}
}



void SoftkeyGroup::concatenateSoftkeyGroup(SoftkeyGroup *fixedSoftkeyGroup, FixedRowColMethod fixedRowColMethod)
{
	if (!fixedSoftkeyGroup || fixedRowColMethod == FRCM_None)
		return;

	int softkeysPerPage = 0;  // Also means that all softkeys fit on one page
	int gridSize = getHorizontalGridSize() * getVerticalGridSize();
	int softkeyCount = (int) hPosList.size();
	if (softkeyCount > gridSize)
	{
		// So I need to paste in the fixed row/col flashkeys more than once
		softkeysPerPage = gridSize;
	}

	if (fixedRowColMethod == FRCM_TopRow || fixedRowColMethod == FRCM_BottomRow)
	{
		setVerticalGridSize( getVerticalGridSize() + 1 );
		if (fixedRowColMethod == FRCM_TopRow)
			updateForNewTopRow(fixedSoftkeyGroup, softkeysPerPage);
		else
			updateForNewBottomRow(fixedSoftkeyGroup, softkeysPerPage);
	}
	else if (fixedRowColMethod == FRCM_FirstColumn || fixedRowColMethod == FRCM_LastColumn)
	{
		setHorizontalGridSize( getHorizontalGridSize() + 1 );
		if (fixedRowColMethod == FRCM_FirstColumn)
			updateForNewFirstColumn(fixedSoftkeyGroup, softkeysPerPage);
		else
			updateForNewLastColumn(fixedSoftkeyGroup, softkeysPerPage);
	}

	setGridCellCount(fixedSoftkeyGroup->getGridCellCount() + getGridCellCount());
	// or   setGridCellCount(hPosList.size());

	/*
	SoftkeyGroup *concatenatedCurrentGridData = new SoftkeyGroup(dbAccess);
	if (concatenatedCurrentGridData)
	{
	concatenatedCurrentGridData->setDeptNum(fixedSoftkeyGroup->getDeptNum());
	concatenatedCurrentGridData->setFeatureNumber(fixedSoftkeyGroup->getFeatureNumber());
	concatenatedCurrentGridData->setGroupNumber(fixedSoftkeyGroup->getGroupNumber());
	concatenatedCurrentGridData->setGridCellCount(fixedSoftkeyGroup->getGridCellCount() + currentGridData->getGridCellCount());

	concatenatedCurrentGridData->setHorizontalGridSize(currentGridData->getHorizontalGridSize());
	concatenatedCurrentGridData->setVerticalGridSize(currentGridData->getVerticalGridSize());
	concatenatedCurrentGridData->setHorizontalSpacing(currentGridData->getHorizontalSpacing());
	concatenatedCurrentGridData->setVerticalSpacing(currentGridData->getVerticalSpacing());

	if (fixedRowColMethod == FRCM_TopRow || fixedRowColMethod == FRCM_BottomRow)
	{
	concatenatedCurrentGridData->setVerticalGridSize(concatenatedCurrentGridData->getVerticalGridSize() + 1);
	if (fixedRowColMethod == FRCM_TopRow)
	concatenatedCurrentGridData->updateForNewTopRow(fixedSoftkeyGroup);
	else
	concatenatedCurrentGridData->updateForNewBottomRow(fixedSoftkeyGroup);
	}
	else  // is FRCM_FirstColumn || FRCM_LastColumn
	{
	concatenatedCurrentGridData->setHorizontalGridSize(concatenatedCurrentGridData->getHorizontalGridSize() + 1);
	}
	}
	return concatenatedCurrentGridData;
	*/
}


void SoftkeyGroup::updateForNewTopRow(SoftkeyGroup *fixedSoftkeyGroup, int softkeysPerPage)
{
	HobString before = vPosList.serialize(BrowserFormat);

	int ret = 0;
	HobString val;

	if (vPosList.size())
	{
		HobList	newvPosList;

		ret = vPosList.first(val);
		while (ret == 0)
		{
			val = inttostring(stringtoint(val) + 1);
			newvPosList.push_back(val);
			ret = vPosList.next(val);
		}

		vPosList = newvPosList;
	}

	HobString after = vPosList.serialize(BrowserFormat);
	Debug::trace("SoftkeyGroup.show", string("vPosList - before: ") + before + "  after: " + after);

	concatenateLists(fixedSoftkeyGroup, softkeysPerPage);
}

void SoftkeyGroup::updateForNewBottomRow(SoftkeyGroup *fixedSoftkeyGroup, int softkeysPerPage)
{
	// Need to put all the cells in the list to indicate they are in the bottom row (vertical position set to 1 greater than what this grid has)

	concatenateLists(fixedSoftkeyGroup, softkeysPerPage);
}

void SoftkeyGroup::updateForNewFirstColumn(SoftkeyGroup *fixedSoftkeyGroup, int softkeysPerPage)
{
	HobString before = hPosList.serialize(BrowserFormat);

	int ret = 0;
	HobString val;

	if (hPosList.size())
	{
		HobList	newhPosList;

		ret = hPosList.first(val);
		while (ret == 0)
		{
			val = inttostring(stringtoint(val) + 1);
			newhPosList.push_back(val);
			ret = hPosList.next(val);
		}

		hPosList = newhPosList;
	}

	HobString after = hPosList.serialize(BrowserFormat);
	Debug::trace("SoftkeyGroup.show", string("hPosList - before: ") + before + "  after: " + after);

	concatenateLists(fixedSoftkeyGroup, softkeysPerPage);

}

void SoftkeyGroup::updateForNewLastColumn(SoftkeyGroup *fixedSoftkeyGroup, int softkeysPerPage)
{
	// Need to put all the cells in the list to indicate they are in the bottom row (vertical position set to 1 greater than what this grid has)

	concatenateLists(fixedSoftkeyGroup, softkeysPerPage);
}

void SoftkeyGroup::concatenateLists(SoftkeyGroup *fixedSoftkeyGroup, int softkeysPerPage)
{
	concatenateList(hPosList, fixedSoftkeyGroup->gethPosList(), softkeysPerPage);
	concatenateList(vPosList, fixedSoftkeyGroup->getvPosList(), softkeysPerPage);
	concatenateList(hSizeList, fixedSoftkeyGroup->gethSizeList(), softkeysPerPage);
	concatenateList(vSizeList, fixedSoftkeyGroup->getvSizeList(), softkeysPerPage);
	concatenateList(bgColorList, fixedSoftkeyGroup->getbgColorList(), softkeysPerPage);
	concatenateList(bgImageList, fixedSoftkeyGroup->getbgImageList(), softkeysPerPage);
	concatenateList(fgImageList, fixedSoftkeyGroup->getfgImageList(), softkeysPerPage);
	concatenateList(lang1NameList, fixedSoftkeyGroup->getlang1NameList(), softkeysPerPage);
	concatenateList(lang2NameList, fixedSoftkeyGroup->getlang2NameList(), softkeysPerPage);
	concatenateList(actionList, fixedSoftkeyGroup->getactionList(), softkeysPerPage);
	concatenateList(actionParameterList, fixedSoftkeyGroup->getactionParameterList(), softkeysPerPage);
	concatenateList(actionFeatureNumList, fixedSoftkeyGroup->getactionFeatureNumList(), softkeysPerPage);
	concatenateList(isEnabledList, fixedSoftkeyGroup->getisEnabledList(), softkeysPerPage);
	concatenateList(textColorList, fixedSoftkeyGroup->gettextColorList(), softkeysPerPage);
	concatenateList(scriptNameList, fixedSoftkeyGroup->getscriptNameList(), softkeysPerPage);
	concatenateList(scriptIntervalList, fixedSoftkeyGroup->getscriptIntervalList(), softkeysPerPage);
}

void SoftkeyGroup::concatenateList(HobList &myList, HobList l, int softkeysPerPage)
{
	HobString before = myList.serialize(BrowserFormat);
	HobString beforeF = l.serialize(BrowserFormat);

	if (softkeysPerPage == 0)
	{
		// This is the quicker version of the else condition below.
		// Do this if the grid doesn't have a second page of data (which can happen when loading Categories and Specials)
		HobString val;
		int ret = l.first(val);
		while (ret == 0)
		{
			myList.push_back(val);
			ret = l.next(val);
		}
	}
	else
	{
		int countPerPage = softkeysPerPage;
		bool moreKeys = true;
		int myret, lret = 0;
		HobString myval, lval;

		myret = myList.first(myval);
		while (moreKeys)
		{
			countPerPage = softkeysPerPage;
			// Position the list's iterator to where we want to insert new elements
			while (myret == 0 && countPerPage--)
				myret = myList.next(myval);

			// Insert the fixed row/col grid values
			lret = l.first(lval);
			while (lret == 0)
			{
				if (myret == 0)
					myList.insert(lval);
				else
					myList.push_back(lval);

				lret = l.next(lval);
			}

			//if (myRet != 0)
			if (myList.end())
				moreKeys = false;
		}
	}

	HobString after = myList.serialize(BrowserFormat);
	Debug::trace("SoftkeyGroup.show", string("concatenateList - before: ") + before + "  beforeF: " + beforeF + "  after: " + after);
}
