/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgSelfService.h"
#include "Configuration.h"
#include "ConfigurationTags.h"
#include "HobTokenizerCSV.h"
#include "DBConfigurationsRM.h"

#include "Debug.h"

#include "Integer.h"
#include "Long.h"
#include "Bool.h"
#include "Hex.h"


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int CfgSelfService::dataType = 0;
std::string CfgSelfService::dataTypeName("CfgSelfService"); 
#endif

std::list<CfgFieldOffsets> CfgSelfService::cfgSSFieldList;
std::list<CfgFieldOffsets> CfgSelfService::browserFieldList;
std::list<CfgFieldOffsets> CfgSelfService::evolFieldList;


CfgSelfService::CfgSelfService()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

#if defined(LINUX_FIX)
CfgSelfService::CfgSelfService(const CfgSelfService& cfgSelfService)
    : HfcObject()
#else
CfgSelfService::CfgSelfService(const CfgSelfService& cfgSelfService)
#endif
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
	
	gridXElements = cfgSelfService.getGridXElements();
	gridYElements = cfgSelfService.getGridYElements();

	numOfMostCommonCategories = cfgSelfService.getNumOfMostCommonCategories();
	imageTypeOnStartScreen = cfgSelfService.getImageTypeOnStartScreen();
	initialStreamingGraphicNum	 = cfgSelfService.getInitialStreamingGraphicNum();
	productStatsHoursMethod = cfgSelfService.getProductStatsHoursMethod();
	selfServeInactivityTimeout = cfgSelfService.getSelfServeInactivityTimeout();

	logoGraphicFileName = cfgSelfService.getLogoGraphicFileName();
	noGraphicAvailableFileName1 = cfgSelfService.getNoGraphicAvailableFileName1();
	noGraphicAvailableFileName2 = cfgSelfService.getNoGraphicAvailableFileName2();
	previousGraphicFileName1 = cfgSelfService.getPreviousGraphicFileName1();
	previousGraphicFileName2 = cfgSelfService.getPreviousGraphicFileName2();
	nextGraphicFileName1 = cfgSelfService.getNextGraphicFileName1();
	nextGraphicFileName2 = cfgSelfService.getNextGraphicFileName2();
}

CfgSelfService::CfgSelfService(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	HobString temp(data, len);
	std::list<CfgFieldOffsets>::iterator fieldIter;

	switch (formatType)
	{
		case BrowserFormat:
		{
			HobTokenizerCSV tok(&temp, BrowserFormatSeparator);
			HobString token;

			fieldIter = browserFieldList.begin();
			while ((tok.next(&token) != -1) && (fieldIter != browserFieldList.end()))
			{
				if (token.size())
					updateField(*fieldIter, token);					
				fieldIter++;
			}
			break;
		}

/*
		case EvolutionTaggedFormat:
		{
			ConfigurationTags	configTags;			

			HobString evolData(data, len);

			//use the UnitSeparator to parse the string
			HobTokenizerUS tok(&evolData, UnitSeparator);
			HobString temp;

			HobTokenTagged *token = (HobTokenTagged *) HobToken::create(temp,HobTaggedToken);
			if (token)
			{
				while (tok.next(token) != -1)
				{
					offset = configTags.getOffset(token->getTag());
					temp = token->getData();
					updateField(offset, temp);
				}	
				delete token;
			}
			break;
		}
*/

		default:
			// todo - throw exception("invalid format");
			break;
	}
}

CfgSelfService::~CfgSelfService()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}


//private
void CfgSelfService::init()
{
	gridXElements = 5;
	gridYElements = 4;

	numOfMostCommonCategories = 5;
	imageTypeOnStartScreen = 1;
	initialStreamingGraphicNum	= 1;
	productStatsHoursMethod = 0;
	selfServeInactivityTimeout = 20;  // in seconds

	logoGraphicFileName = "";
	noGraphicAvailableFileName1 = "noImage";
	noGraphicAvailableFileName2 = "noImageES";
	previousGraphicFileName1 = "less";
	previousGraphicFileName2 = "lessES";
	nextGraphicFileName1 = "more";
	nextGraphicFileName2 = "moreES";

	initFieldList();
}

// static
void CfgSelfService::initFieldList()
{
	if (cfgSSFieldList.empty())
	{
		cfgSSFieldList.push_back(fieldOffsetCfgGridXElements);
		cfgSSFieldList.push_back(fieldOffsetCfgGridYElements);
		cfgSSFieldList.push_back(fieldOffsetCfgNumOfMostCommonCategories);
		cfgSSFieldList.push_back(fieldOffsetCfgImageTypeOnStartScreen);
		cfgSSFieldList.push_back(fieldOffsetCfgInitialStreamingGraphicNum);
		cfgSSFieldList.push_back(fieldOffsetCfgProductStatsHoursMethod);
		cfgSSFieldList.push_back(fieldOffsetCfgSelfServeInactivityTimeout);

		cfgSSFieldList.push_back(fieldOffsetCfgLogoGraphicFileName);
		cfgSSFieldList.push_back(fieldOffsetCfgNoGraphicAvailableFileName1);
		cfgSSFieldList.push_back(fieldOffsetCfgNoGraphicAvailableFileName2);
		cfgSSFieldList.push_back(fieldOffsetCfgPreviousGraphicFileName1);
		cfgSSFieldList.push_back(fieldOffsetCfgPreviousGraphicFileName2);
		cfgSSFieldList.push_back(fieldOffsetCfgNextGraphicFileName1);
		cfgSSFieldList.push_back(fieldOffsetCfgNextGraphicFileName2);

		ConfigurationTags configurationTags((FieldOffsetsList *)&cfgSSFieldList);
	}

	if (browserFieldList.empty())
	{
		browserFieldList.push_back(fieldOffsetCfgGridXElements);
		browserFieldList.push_back(fieldOffsetCfgGridYElements);
		browserFieldList.push_back(fieldOffsetCfgNumOfMostCommonCategories);
		browserFieldList.push_back(fieldOffsetCfgImageTypeOnStartScreen);
		browserFieldList.push_back(fieldOffsetCfgInitialStreamingGraphicNum);
		browserFieldList.push_back(fieldOffsetCfgProductStatsHoursMethod);
		browserFieldList.push_back(fieldOffsetCfgSelfServeInactivityTimeout);

		browserFieldList.push_back(fieldOffsetCfgLogoGraphicFileName);
		browserFieldList.push_back(fieldOffsetCfgNoGraphicAvailableFileName1);
		browserFieldList.push_back(fieldOffsetCfgNoGraphicAvailableFileName2);
		browserFieldList.push_back(fieldOffsetCfgPreviousGraphicFileName1);
		browserFieldList.push_back(fieldOffsetCfgPreviousGraphicFileName2);
		browserFieldList.push_back(fieldOffsetCfgNextGraphicFileName1);
		browserFieldList.push_back(fieldOffsetCfgNextGraphicFileName2);
	}

	if (evolFieldList.empty())
	{
		//evolFieldList.push_back();
	}
}


void CfgSelfService::load(DBAccess *dbAccess)
{
	if (!dbAccess)
		return;

	HobString dbVal;
	
	if (loadField(dbAccess, "gridXElements", dbVal))
		gridXElements = Integer::valueOf(dbVal);
	if (loadField(dbAccess, "gridYElements", dbVal))
		gridYElements = Integer::valueOf(dbVal);
	if (loadField(dbAccess, "numOfMostCommonCategories", dbVal))
		numOfMostCommonCategories = Integer::valueOf(dbVal);
	if (loadField(dbAccess, "imageTypeOnStartScreen", dbVal))
		imageTypeOnStartScreen = Integer::valueOf(dbVal);
	if (loadField(dbAccess, "initialStreamingGraphicNum", dbVal))
		initialStreamingGraphicNum = (HobRecordNum)Long::valueOf(dbVal);
	if (loadField(dbAccess, "productStatsHoursMethod", dbVal))
		productStatsHoursMethod = Integer::valueOf(dbVal);
	if (loadField(dbAccess, "selfServeInactivityTimeout", dbVal))
		selfServeInactivityTimeout = Integer::valueOf(dbVal);

	if (loadField(dbAccess, "logoGraphicFileName", dbVal))
		logoGraphicFileName = dbVal;
	if (loadField(dbAccess, "noGraphicAvailableFileName1", dbVal))
		noGraphicAvailableFileName1 = dbVal;
	if (loadField(dbAccess, "noGraphicAvailableFileName2", dbVal))
		noGraphicAvailableFileName2 = dbVal;
	if (loadField(dbAccess, "previousGraphicFileName1", dbVal))
		previousGraphicFileName1 = dbVal;
	if (loadField(dbAccess, "previousGraphicFileName2", dbVal))
		previousGraphicFileName2 = dbVal;
	if (loadField(dbAccess, "nextGraphicFileName1", dbVal))
		nextGraphicFileName1 = dbVal;
	if (loadField(dbAccess, "nextGraphicFileName2", dbVal))
		nextGraphicFileName2 = dbVal;

	saveNeeded = false;
}


void CfgSelfService::updateField(int fieldCode, const HobString &token)
{
	if (fieldCode <= 0)
		return;

	bool foundField = true;
	switch (fieldCode)
	{
		case fieldOffsetCfgGridXElements:
			setGridXElements(Integer::valueOf(token));
			break;
		case fieldOffsetCfgGridYElements:
			setGridYElements(Integer::valueOf(token));
			break;
		case fieldOffsetCfgNumOfMostCommonCategories:
			setNumOfMostCommonCategories(Integer::valueOf(token));
			break;
		case fieldOffsetCfgImageTypeOnStartScreen:
			setImageTypeOnStartScreen(Integer::valueOf(token));
			break;
		case fieldOffsetCfgInitialStreamingGraphicNum:
			setInitialStreamingGraphicNum((HobRecordNum)Long::valueOf(token));
			break;
		case fieldOffsetCfgProductStatsHoursMethod:
			setProductStatsHoursMethod(Integer::valueOf(token));
			break;
		case fieldOffsetCfgSelfServeInactivityTimeout:
			setSelfServeInactivityTimeout(Integer::valueOf(token));
			break;

		case fieldOffsetCfgLogoGraphicFileName:
			setLogoGraphicFileName(token);
			break;
		case fieldOffsetCfgNoGraphicAvailableFileName1:
			setNoGraphicAvailableFileName1(token);
			break;
		case fieldOffsetCfgNoGraphicAvailableFileName2:
			setNoGraphicAvailableFileName2(token);
			break;
		case fieldOffsetCfgPreviousGraphicFileName1:
			setPreviousGraphicFileName1(token);
			break;
		case fieldOffsetCfgPreviousGraphicFileName2:
			setPreviousGraphicFileName2(token);
			break;
		case fieldOffsetCfgNextGraphicFileName1:
			setNextGraphicFileName1(token);
			break;
		case fieldOffsetCfgNextGraphicFileName2:
			setNextGraphicFileName2(token);
			break;

		default:
			foundField = false;
			break;
	}

	if (foundField)
		setReceived(fieldCode, true);
}


void CfgSelfService::update(const CfgBase *newValues, bool scaleOnlyFields)
{
	Debug::trace("Configuration.CfgSelfService", "update() newValues = " + ptrtostring(newValues));	

	if (!newValues)
		return;

	const CfgSelfService *myNewValues = dynamic_cast<const CfgSelfService *> (newValues);
	if (!myNewValues)
		return;

	std::list<CfgFieldOffsets>::iterator fieldIter = cfgSSFieldList.begin();
	while (fieldIter != cfgSSFieldList.end())
	{
		if (myNewValues->isReceived(*fieldIter) && (!scaleOnlyFields || isFieldScaleOnly(*fieldIter)))
		{
			switch (*fieldIter)
			{
				case fieldOffsetCfgGridXElements:
					setGridXElements(myNewValues->getGridXElements());
					break;
				case fieldOffsetCfgGridYElements:
					setGridYElements(myNewValues->getGridYElements());
					break;
				case fieldOffsetCfgNumOfMostCommonCategories:
					setNumOfMostCommonCategories(myNewValues->getNumOfMostCommonCategories());
					break;
				case fieldOffsetCfgImageTypeOnStartScreen:
					setImageTypeOnStartScreen(myNewValues->getImageTypeOnStartScreen());
					break;
				case fieldOffsetCfgInitialStreamingGraphicNum:
					setInitialStreamingGraphicNum(myNewValues->getInitialStreamingGraphicNum());
					break;
				case fieldOffsetCfgProductStatsHoursMethod:
					setProductStatsHoursMethod(myNewValues->getProductStatsHoursMethod());
					break;
				case fieldOffsetCfgSelfServeInactivityTimeout:
					setSelfServeInactivityTimeout(myNewValues->getSelfServeInactivityTimeout());
					break;

				case fieldOffsetCfgLogoGraphicFileName:
					setLogoGraphicFileName(myNewValues->getLogoGraphicFileName());
					break;
				case fieldOffsetCfgNoGraphicAvailableFileName1:
					setNoGraphicAvailableFileName1(myNewValues->getNoGraphicAvailableFileName1());
					break;
				case fieldOffsetCfgNoGraphicAvailableFileName2:
					setNoGraphicAvailableFileName2(myNewValues->getNoGraphicAvailableFileName2());
					break;
				case fieldOffsetCfgPreviousGraphicFileName1:
					setPreviousGraphicFileName1(myNewValues->getPreviousGraphicFileName1());
					break;
				case fieldOffsetCfgPreviousGraphicFileName2:
					setPreviousGraphicFileName2(myNewValues->getPreviousGraphicFileName2());
					break;
				case fieldOffsetCfgNextGraphicFileName1:
					setNextGraphicFileName1(myNewValues->getNextGraphicFileName1());
					break;
				case fieldOffsetCfgNextGraphicFileName2:
					setNextGraphicFileName2(myNewValues->getNextGraphicFileName2());
					break;

				default:
					break;
			}
		}
		fieldIter++;
	}
}


void CfgSelfService::save(DBAccess *dbAccess, const CfgBase *newValues, bool scaleOnlyFields)
{
	if (newValues)
		update(newValues, scaleOnlyFields);

	Debug::trace("Configuration.CfgSelfService", "save():   saveNeeded = " + HobString(Bool::toString(saveNeeded)));
	if (saveNeeded && dbAccess)
	{
		Debug::trace("Configuration.CfgSelfService", "saving data ...");

		saveField(dbAccess, "gridXElements",					Integer::toString(gridXElements));
		saveField(dbAccess, "gridYElements",					Integer::toString(gridYElements));
		saveField(dbAccess, "numOfMostCommonCategories",		Integer::toString(numOfMostCommonCategories));
		saveField(dbAccess, "imageTypeOnStartScreen",			Integer::toString(imageTypeOnStartScreen));
		saveField(dbAccess, "initialStreamingGraphicNum",		Integer::toString(initialStreamingGraphicNum));
		saveField(dbAccess, "productStatsHoursMethod",		Integer::toString(productStatsHoursMethod));
		saveField(dbAccess, "selfServeInactivityTimeout",		Integer::toString(selfServeInactivityTimeout));

		saveField(dbAccess, "logoGraphicFileName",			logoGraphicFileName);
		saveField(dbAccess, "noGraphicAvailableFileName1",	noGraphicAvailableFileName1);
		saveField(dbAccess, "noGraphicAvailableFileName2",	noGraphicAvailableFileName2);
		saveField(dbAccess, "previousGraphicFileName1",		previousGraphicFileName1);
		saveField(dbAccess, "previousGraphicFileName2",		previousGraphicFileName2);
		saveField(dbAccess, "nextGraphicFileName1",			nextGraphicFileName1);
		saveField(dbAccess, "nextGraphicFileName2",			nextGraphicFileName2);

		saveNeeded = false;
	}
	else
		Debug::trace("Configuration.CfgSelfService", "save method called, but no fields modified");	
}


void CfgSelfService::operator=(const CfgSelfService &cfgSelfService)
{
	gridXElements = cfgSelfService.getGridXElements();
	gridYElements = cfgSelfService.getGridYElements();
	numOfMostCommonCategories = cfgSelfService.getNumOfMostCommonCategories();
	imageTypeOnStartScreen = cfgSelfService.getImageTypeOnStartScreen();
	initialStreamingGraphicNum = cfgSelfService.getInitialStreamingGraphicNum();
	productStatsHoursMethod = cfgSelfService.getProductStatsHoursMethod();
	selfServeInactivityTimeout = cfgSelfService.getSelfServeInactivityTimeout();

	logoGraphicFileName = cfgSelfService.getLogoGraphicFileName();
	noGraphicAvailableFileName1 = cfgSelfService.getNoGraphicAvailableFileName1();
	noGraphicAvailableFileName2 = cfgSelfService.getNoGraphicAvailableFileName2();
	previousGraphicFileName1 = cfgSelfService.getPreviousGraphicFileName1();
	previousGraphicFileName2 = cfgSelfService.getPreviousGraphicFileName2();
	nextGraphicFileName1 = cfgSelfService.getNextGraphicFileName1();
	nextGraphicFileName2 = cfgSelfService.getNextGraphicFileName2();
}


void CfgSelfService::getFieldAsString(int fieldCode, HobString &result, int /* format */) const
{
//	Right now we don't do anything with format but keep it in case we need it.
	result = "";

	switch (fieldCode)
	{
		case fieldOffsetCfgGridXElements:
			result = Integer::toString(gridXElements);
			break;
		case fieldOffsetCfgGridYElements:
			result = Integer::toString(gridYElements);
			break;
		case fieldOffsetCfgNumOfMostCommonCategories:
			result = Integer::toString(numOfMostCommonCategories);
			break;
		case fieldOffsetCfgImageTypeOnStartScreen:
			result = Integer::toString(imageTypeOnStartScreen);
			break;
		case fieldOffsetCfgInitialStreamingGraphicNum:
			result = Long::toString(initialStreamingGraphicNum);
			break;
		case fieldOffsetCfgProductStatsHoursMethod:
			result = Integer::toString(productStatsHoursMethod);
			break;
		case fieldOffsetCfgSelfServeInactivityTimeout:
			result = Integer::toString(selfServeInactivityTimeout);
			break;

		case fieldOffsetCfgLogoGraphicFileName:
			result = logoGraphicFileName;
			break;
		case fieldOffsetCfgNoGraphicAvailableFileName1:
			result = noGraphicAvailableFileName1;
			break;
		case fieldOffsetCfgNoGraphicAvailableFileName2:
			result = noGraphicAvailableFileName2;
			break;
		case fieldOffsetCfgPreviousGraphicFileName1:
			result = previousGraphicFileName1;
			break;
		case fieldOffsetCfgPreviousGraphicFileName2:
			result = previousGraphicFileName2;
			break;
		case fieldOffsetCfgNextGraphicFileName1:
			result = nextGraphicFileName1;
			break;
		case fieldOffsetCfgNextGraphicFileName2:
			result = nextGraphicFileName2;
			break;

		default:
			break;
	}
}


const HobString& CfgSelfService::getField(int fieldCode, int format) const
{
	getFieldAsString(fieldCode, tempValue_, format);

	return tempValue_;
}


const char *CfgSelfService::serialize(int format) const
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	std::list<CfgFieldOffsets>::iterator fieldIter;
	bool appendSeparator = false;

	switch (format)
	{
		case BrowserFormat:
		{
			HobString fieldData = "";
			fieldIter = browserFieldList.begin();
			while (fieldIter != browserFieldList.end())
			{
				if (appendSeparator)
					serializedData.append(BrowserFormatSeparator);
				else
					appendSeparator = true;

				getFieldAsString(*fieldIter, fieldData, BrowserFormat);
				serializedData.append(fieldData);

				fieldIter++;
			}
			break;
		}

/*
		case EvolutionTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag;
			HobString fieldData = "";

			fieldIter = evolFieldList.begin();
			while (fieldIter != evolFieldList.end())
			{
				offset = *fieldIter;
				tag = configTags.getTag(offset);
				if (tag.size())
				{
					serializedData.append(tag);

					getFieldAsString(offset, fieldData, format);
					serializedData.append(fieldData);

					serializedData.append(UnitSeparator);
				}
				fieldIter++;
			}
			break;
		}
*/

		case SelfServiceReportFormat:
		{
			ConfigurationTags configNames;

//			serializedData.assign("Self Service Settings\n");
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgSelfServiceScaleFlag)).append(1, '\t').
//					append(getField(fieldOffsetCfgSelfServiceScaleFlag, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgImageTypeOnStartScreen)).append(1, '\t').
//					append(getField(fieldOffsetCfgImageTypeOnStartScreen, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgInitialStreamingGraphicNum)).append(1, '\t').
//					append(getField(fieldOffsetCfgInitialStreamingGraphicNum, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgGridXElements)).append(1, '\t').
//					append(getField(fieldOffsetCfgGridXElements, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgGridYElements)).append(1, '\t').
//					append(getField(fieldOffsetCfgGridYElements, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgNumOfMostCommonCategories)).append(1, '\t').
//					append(getField(fieldOffsetCfgNumOfMostCommonCategories, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgProductStatsHoursMethod)).append(2, '\t').
//					append(getField(fieldOffsetCfgProductStatsHoursMethod, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgSelfServeInactivityTimeout)).append(4, '\t').
//					append(getField(fieldOffsetCfgSelfServeInactivityTimeout, EvolutionTaggedFormat)).append(" sec.").append(2, '\n');

//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgProductGraphicsDirectoryName)).append(1, '\t').
//					append(getField(fieldOffsetCfgProductGraphicsDirectoryName, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgProductGraphicsDefaultExtension)).append(1, '\t').
//					append(getField(fieldOffsetCfgProductGraphicsDefaultExtension, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgCategoryGraphicsDirectoryName)).append(1, '\t').
//					append(getField(fieldOffsetCfgCategoryGraphicsDirectoryName, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgCategoryGraphicsDefaultExtension)).append(1, '\t').
//					append(getField(fieldOffsetCfgCategoryGraphicsDefaultExtension, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgSpecialsGraphicsDirectoryName)).append(1, '\t').
//					append(getField(fieldOffsetCfgSpecialsGraphicsDirectoryName, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgSpecialsGraphicsDefaultExtension)).append(1, '\t').
//					append(getField(fieldOffsetCfgSpecialsGraphicsDefaultExtension, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgSlideShowGraphicsDirectoryName)).append(1, '\t').
//					append(getField(fieldOffsetCfgSlideShowGraphicsDirectoryName, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgSlideShowGraphicsDefaultExtension)).append(1, '\t').
//					append(getField(fieldOffsetCfgSlideShowGraphicsDefaultExtension, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgSelfServeGraphicsDirectoryName)).append(1, '\t').
//					append(getField(fieldOffsetCfgSelfServeGraphicsDirectoryName, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgSelfServeGraphicsDefaultExtension)).append(1, '\t').
//					append(getField(fieldOffsetCfgSelfServeGraphicsDefaultExtension, EvolutionTaggedFormat)).append(2, '\n');

//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgLogoGraphicFileName)).append(1, '\t').
//					append(getField(fieldOffsetCfgLogoGraphicFileName, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgNoGraphicAvailableFileName1)).append(1, '\t').
//					append(getField(fieldOffsetCfgNoGraphicAvailableFileName1, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgNoGraphicAvailableFileName2)).append(1, '\t').
//					append(getField(fieldOffsetCfgNoGraphicAvailableFileName2, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgPreviousGraphicFileName1)).append(1, '\t').
//					append(getField(fieldOffsetCfgPreviousGraphicFileName1, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgPreviousGraphicFileName2)).append(1, '\t').
//					append(getField(fieldOffsetCfgPreviousGraphicFileName2, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgNextGraphicFileName1)).append(1, '\t').
//					append(getField(fieldOffsetCfgNextGraphicFileName1, EvolutionTaggedFormat)).append(1, '\n');
//			serializedData.append(2, ' ').append(configNames.getName(fieldOffsetCfgNextGraphicFileName2)).append(1, '\t').
//					append(getField(fieldOffsetCfgNextGraphicFileName2, EvolutionTaggedFormat)).append(1, '\n');

			break;
		}

		case SynchronizationTaggedFormat:
		{
			ConfigurationTags configTags;
			HobString tag = "";
			HobString fieldData = "";

			fieldIter = cfgSSFieldList.begin();
			while (fieldIter != cfgSSFieldList.end())
			{
				tag = configTags.getTag(*fieldIter);
				if (tag.size())
				{
					fieldData = "";
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
			serializedData = HobString("CfgSelfService::serialize - unknown format:") + HobString(Integer::toString(format));
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}


HobString CfgSelfService::quoteForSerialize(HobString& string)
{
	HobString tempString = string;
	if (tempString.empty())
		tempString = " ";		//	Temporary, until Joe can handle Null strings.
	tempString.quote();
	return tempString;
}


bool CfgSelfService::isChanged()
{
	Debug::trace("Configuration.CfgSelfService", HobString("Has changed is ") + HobString(Bool::toString(saveNeeded)));
	return saveNeeded;
}


void CfgSelfService::show()
{
	Debug::trace("Configuration", "*** CfgSelfService parameters ***");
	Debug::trace("Configuration", HobString("gridXElements:") + HobString(Integer::toString(gridXElements)));
	Debug::trace("Configuration", HobString("gridYElements:") + HobString(Integer::toString(gridYElements)));
	Debug::trace("Configuration", HobString("numOfMostCommonCategories:") + HobString(Integer::toString(numOfMostCommonCategories)));
	Debug::trace("Configuration", HobString("imageTypeOnStartScreen:") + HobString(Integer::toString(imageTypeOnStartScreen)));
	Debug::trace("Configuration", HobString("initialStreamingGraphicNum:") + HobString(Integer::toString(initialStreamingGraphicNum)));
	Debug::trace("Configuration", HobString("productStatsHoursMethod:") + HobString(Integer::toString(productStatsHoursMethod)));
	Debug::trace("Configuration", HobString("selfServeInactivityTimeout:") + HobString(Integer::toString(selfServeInactivityTimeout)));

	Debug::trace("Configuration", HobString("logoGraphicFileName:") + logoGraphicFileName);
	Debug::trace("Configuration", HobString("noGraphicAvailableFileName1:") + noGraphicAvailableFileName1);
	Debug::trace("Configuration", HobString("noGraphicAvailableFileName2:") + noGraphicAvailableFileName2);
	Debug::trace("Configuration", HobString("previousGraphicFileName1:") + previousGraphicFileName1);
	Debug::trace("Configuration", HobString("previousGraphicFileName2:") + previousGraphicFileName2);
	Debug::trace("Configuration", HobString("nextGraphicFileName1:") + nextGraphicFileName1);
	Debug::trace("Configuration", HobString("nextGraphicFileName2:") + nextGraphicFileName2);
}
