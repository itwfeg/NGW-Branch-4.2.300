/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "evolution.h"
#include "SoftKeyLists.h"
#include "Softkey.h"
#include "Configuration.h"
#include "Product.h"
#include "Category.h"
#include "Special.h"
#include "util/filesys/file.h"
#include "util/filesys/filename.h"

#include "Integer.h"
#include "Long.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

LOCAL int dataType = 0;
LOCAL string dataTypeName = "SoftKeyLists";
#endif

SoftKeyLists::SoftKeyLists()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = registerDataType(dataTypeName);

	addInstance(dataType);
#endif

	init();
}

// private
SoftKeyLists::~SoftKeyLists()
{
#ifdef USE_ACCOUNTANT
	removeInstance(dataType);
#endif

}


void SoftKeyLists::init()
{
	descriptionList.setQuotingElements(true);
	currentSerializeOffset = 0;
}



const HobList &SoftKeyLists::getDataTypeList() const
{
	return dataTypeList;
}
const HobList &SoftKeyLists::getRecordNumList() const
{
	return recordNumList;
}
const HobList &SoftKeyLists::getDescriptionList() const
{
	return descriptionList;
}
const HobList &SoftKeyLists::getgraphicFileNameList() const
{
	return graphicFileNameList;
}
const HobList &SoftKeyLists::getOverlayGraphicFileNameList() const
{
	return overlayGraphicFileNameList;
}


void SoftKeyLists::add(const Product &product, int languageSelection)
{
	HobString myDebug;
	bool isTracing = Debug::isTracing("SoftKeyLists");
	if (isTracing)
		myDebug = "add(): ";

	Debug::trace("SoftKeyLists", "adding a product: ");
	Configuration *config = Configuration::getInstance();

	dataTypeList.push_back(HobString(Integer::toString(Softkey::DataType_Product)));

	recordNumList.push_back(HobString(Long::toString(product.getProductNum())));

	HobString desc("");
	if (languageSelection == 1 && !product.getProductName1().empty())
		desc = product.getProductName1();
	else if (languageSelection == 2 && !product.getProductName2().empty())
		desc = product.getProductName2();
	else
	{
		desc = product.getDescription();
		desc.stripFontSequences();
		if (config->getNumOfDescCharsToUse() > 0)
			desc = desc.substr(0, config->getNumOfDescCharsToUse());
	}
	if (config->isAppendingPluNumberToName())
	{
		desc.append(" (");
		desc.append(Integer::toString(product.getProductNum()));
		desc.append(")");
	}
	descriptionList.push_back(desc);

	HobString graphicFileName(product.getProductGraphicName());
	if (!graphicFileName.size())
		graphicFileName = Integer::toString(product.getProductNum());

	if (isTracing)
	{
		HobString myDebug2(myDebug);
		myDebug2.append("Product graphicFileName = ").append(graphicFileName);
		Debug::trace("SoftKeyLists", myDebug2);
	}
	Filename gFilename;
	if (graphicFileName.size())
	{
		gFilename.setPath(->getWebPagesDir()));
		gFilename.appendPath(config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR));
		gFilename.setFileName(graphicFileName);
		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("Product gFilename = ").append(gFilename.getFullPath());
			Debug::trace("SoftKeyLists", myDebug2);
		}
		if (gFilename.getFileNameExtension().empty())
			gFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDEFAULTEXTENSION));
		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("Product gFilename = ").append(gFilename.getFullPath());
			Debug::trace("SoftKeyLists", myDebug2);
		}

		File gFile(gFilename);
		if (gFile.exists())
		{
			Debug::trace("SoftKeyLists", "add(): Product file exists!");
			gFilename.setPath(config->toStringCfgI(CfgInternal::PRODUCTGRAPHICSDIR));
		}
		else
		{
			Debug::trace("SoftKeyLists", "add(): Product file does not exist.");
			gFilename.setPath(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDIRECTORYNAME));
			gFilename.setFileName((languageSelection == 1) ? config->getNoGraphicAvailableFileName1() : config->getNoGraphicAvailableFileName2());
			if (gFilename.getFileNameExtension().empty())
				gFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDEFAULTEXTENSION));
		}
	}
	if (isTracing)
	{
		HobString myDebug2(myDebug);
		myDebug2.append("Product gFilename = ").append(gFilename.getFullPath());
		Debug::trace("SoftKeyLists", myDebug2);
	}
	graphicFileNameList.push_back(gFilename.getFullPath());

	Filename oFilename;
	if (product.getSpecialProgram() && product.getSpecialProgram()->getErrorCode() == RECORD_OK)
	{
		if (languageSelection == 1)
			oFilename.setFileName(product.getSpecialProgram()->getImageName1());
		else
			oFilename.setFileName(product.getSpecialProgram()->getImageName2());

		if (!oFilename.getFullPath().empty())
		{
			if (oFilename.getFileNameExtension().empty())
				oFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDEFAULTEXTENSION));
			oFilename.setPath(config->toStringCfgI(CfgInternal::SPECIALSGRAPHICSDIR));
		}
	}
	overlayGraphicFileNameList.push_back(oFilename.getFullPath());

	Debug::trace("SoftKeyLists", "leaving adding a product: ");
}


void SoftKeyLists::add(const Category &category, int languageSelection)
{
	HobString myDebug;
	bool isTracing = Debug::isTracing("SoftKeyLists");
	if (isTracing)
		myDebug = "add(): Category ";

	// todo - Is this where I determine whether to push the category on the list,
	// or if there is only one plu for the category, then push the product??
	// Probably not, since this can be used to send the 5 most common categories.
	// Thus, if not, then where?
	Debug::trace("SoftKeyLists", "adding a category: ");

	Configuration *config = Configuration::getInstance();

	dataTypeList.push_back(HobString(Integer::toString(Softkey::DataType_Category)));

	recordNumList.push_back(HobString(Long::toString(category.getCategoryNum())));

	HobString desc("");
	if (languageSelection == 1)
		desc = category.getCategoryName1();
	else if (languageSelection == 2)
		desc = category.getCategoryName2();
	descriptionList.push_back(desc);

	HobString graphicFileName(category.getGraphicName());
	if (isTracing)
	{
		HobString myDebug2(myDebug);
		myDebug2.append("graphicFileName = ").append(graphicFileName);
		Debug::trace("SoftKeyLists", myDebug2);
	}

	Filename gFilename;
	if (graphicFileName.size())
	{
		gFilename.setPath(config->getWebPagesDir()));
		gFilename.appendPath(config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDIR));
		gFilename.setFileName(graphicFileName);
		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("gFilename = ").append(gFilename.getFullPath());
			Debug::trace("SoftKeyLists", myDebug2);
		}
		if (gFilename.getFileNameExtension().empty())
			gFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDEFAULTEXTENSION));
		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("gFilename = ").append(gFilename.getFullPath());
			Debug::trace("SoftKeyLists", myDebug2);
		}

		File gFile(gFilename);
		if (gFile.exists())
		{
			Debug::trace("SoftKeyLists", myDebug + "file exists!");
			gFilename.setPath(config->toStringCfgI(CfgInternal::CATEGORYGRAPHICSDIR));
		}
		else
		{
			Debug::trace("SoftKeyLists", myDebug + "file does not exist.");
			gFilename.setPath(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDIRECTORYNAME));
			gFilename.setFileName((languageSelection == 1) ? config->getNoGraphicAvailableFileName1() : config->getNoGraphicAvailableFileName2());
			if (gFilename.getFileNameExtension().empty())
				gFilename.setFileNameExtension(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDEFAULTEXTENSION));
		}
	}
	if (isTracing)
	{
		HobString myDebug2(myDebug);
		myDebug2.append("gFilename = ").append(gFilename.getFullPath());
		Debug::trace("SoftKeyLists", myDebug2);
	}
	graphicFileNameList.push_back(gFilename.getFullPath());

	overlayGraphicFileNameList.push_back("");

#ifdef NEXT_FOR_CLARE
	//lcg rev 0.1!!! displayOrderList.push_back(HobString(Integer::toString(category.getDisplayOrder())));
#endif

	Debug::trace("SoftKeyLists", "leaving adding a category: ");
}


void SoftKeyLists::insertNextandPreviousCells(int languageSelection)
{
	HobString myDebug;
	bool isTracing = Debug::isTracing("SoftKeyLists");
	if (isTracing)
		myDebug = "insertNextandPreviousCells():  ";

	int cellCount = Configuration::getInstance()->getGridXElements() * Configuration::getInstance()->getGridYElements();
	if (recordNumList.size() > cellCount)
	{
		// There are too many products/categories than will fit on the screen.  Add the 'next' and 'previous' cells
		// at the appropriate spots in the lists so the user can get to the remaining items.

		if (isTracing)
		{
			HobString myDebug2(myDebug);
			myDebug2.append("cellCount: ").append(Integer::toString(cellCount));
			Debug::trace("SoftKeyLists", myDebug2);
		}
		HobString temp;

		Configuration *config = Configuration::getInstance();

		Filename nextFileName;
		nextFileName.setFileName((languageSelection == 1) ? config->getNextGraphicFileName1() : config->getNextGraphicFileName2());
		if (nextFileName.getFullPath().empty())
			nextFileName.setFileName((languageSelection == 1) ? config->getNoGraphicAvailableFileName1() : config->getNoGraphicAvailableFileName2());
		if (!nextFileName.getFullPath().empty())
		{
			nextFileName.setPath(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDIRECTORYNAME));
			if (nextFileName.getFileNameExtension().empty())
				nextFileName.setFileNameExtension(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDEFAULTEXTENSION));
		}

		Filename previousFileName;
		previousFileName.setFileName((languageSelection == 1) ? config->getPreviousGraphicFileName1() : config->getPreviousGraphicFileName2());
		if (previousFileName.getFullPath().empty())
			previousFileName.setFileName((languageSelection == 1) ? config->getNoGraphicAvailableFileName1() : config->getNoGraphicAvailableFileName2());
		if (!previousFileName.getFullPath().empty())
		{
			previousFileName.setPath(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDIRECTORYNAME));
			if (previousFileName.getFileNameExtension().empty())
				previousFileName.setFileNameExtension(config->toStringCfgI(CfgInternal::SELFSERVEGRAPHICSDEFAULTEXTENSION));
		}

		recordNumList.first(temp);
		dataTypeList.first(temp);
		descriptionList.first(temp);
		graphicFileNameList.first(temp);
		overlayGraphicFileNameList.first(temp);
#ifdef NEXT_FOR_CLARE
		displayOrderList.first(temp);
#endif

		// The first page contains a Next cell, plus cellCount-1 products/categories.
		int cellCountOnPage = cellCount - 1;
		int cellsRemaining = recordNumList.size();
		while (!recordNumList.end())
		{
			if (cellCountOnPage == 0)
			{
				// All pages after page 1 have a Previous and Next cell, so there are only cellCount-2 cells left for products/categories;
				// except for the last page, which could fit up to cellCount-1 products/categories.
				cellCountOnPage = (cellsRemaining < cellCount) ? cellsRemaining : cellCount - 2;

				if (isTracing)
				{
					HobString myDebug2(myDebug);
					myDebug2.append("end of current page, going to next page - cellCount on next page: ").append(Integer::toString(cellCountOnPage));
					Debug::trace("SoftKeyLists", myDebug2);
				}

				// add a Next cell, for the current page.
				Debug::trace("SoftKeyLists", "insertNextandPreviousCells() - inserting 'next' cell");
				graphicFileNameList.insert((nextFileName.getFullPath().empty()) ? "" : nextFileName.getFullPath());
				dataTypeList.insert(HobString(Integer::toString(Softkey::DataType_Next)));
				recordNumList.insert("0");
				descriptionList.insert("");
				overlayGraphicFileNameList.insert("");

				// add a Previous cell, for the next page.
				Debug::trace("SoftKeyLists", "insertNextandPreviousCells() - inserting 'previous' cell");
				graphicFileNameList.insert((previousFileName.getFullPath().empty()) ? "" : previousFileName.getFullPath());
				dataTypeList.insert(HobString(Integer::toString(Softkey::DataType_Previous)));
				recordNumList.insert("0");
				descriptionList.insert("");
				overlayGraphicFileNameList.insert("");
			}
			else
			{
				cellCountOnPage--;
				cellsRemaining--;

				recordNumList.next(temp);
				dataTypeList.next(temp);
				descriptionList.next(temp);
				graphicFileNameList.next(temp);
				overlayGraphicFileNameList.next(temp);
#ifdef NEXT_FOR_CLARE
				displayOrderList.next(temp);
#endif
			}
		}
	}
}


void SoftKeyLists::gotoFirstPage()
{
	currentSerializeOffset = 0;
}

void SoftKeyLists::gotoNextPage()
{
	currentSerializeOffset += (Configuration::getInstance()->getGridXElements() * Configuration::getInstance()->getGridYElements());
}

void SoftKeyLists::gotoPreviousPage()
{
	if (currentSerializeOffset > 0)
		currentSerializeOffset -= (Configuration::getInstance()->getGridXElements() * Configuration::getInstance()->getGridYElements());
}


void SoftKeyLists::blankFillLists(unsigned int totalListEntries)
{
	if (dataTypeList.size() < totalListEntries)
	{
		int neededEntries = totalListEntries - dataTypeList.size();
		HobString temp(Integer::toString(Softkey::DataType_Unknown));
		while (neededEntries--)
			dataTypeList.push_back(temp);
	}

	if (recordNumList.size() < totalListEntries)
	{
		int neededEntries = totalListEntries - recordNumList.size();
		HobString temp("0");
		while (neededEntries--)
			recordNumList.push_back(temp);
	}

	if (descriptionList.size() < totalListEntries)
	{
		int neededEntries = totalListEntries - descriptionList.size();
		HobString temp("");
		while (neededEntries--)
			descriptionList.push_back(temp);
	}

	if (graphicFileNameList.size() < totalListEntries)
	{
		int neededEntries = totalListEntries - graphicFileNameList.size();
		HobString temp("");
		while (neededEntries--)
			graphicFileNameList.push_back(temp);
	}

	if (overlayGraphicFileNameList.size() < totalListEntries)
	{
		int neededEntries = totalListEntries - overlayGraphicFileNameList.size();
		HobString temp("");
		while (neededEntries--)
			overlayGraphicFileNameList.push_back(temp);
	}
}


const char *SoftKeyLists::serialize(int format)
{
	serializedData = "";

	if (getFormatType() != UnknownFormat)
		format = getFormatType();

	int cellCount = Configuration::getInstance()->getGridXElements() * Configuration::getInstance()->getGridYElements();

	switch (format)
	{
		case BrowserFormat:
			Debug::trace("SoftKeyLists", "serializing for format BrowserFormat: ");
			serializedData.append(dataTypeList.serialize(format, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(recordNumList.serialize(format, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(descriptionList.serialize(format, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(graphicFileNameList.serialize(format, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(overlayGraphicFileNameList.serialize(format, currentSerializeOffset, cellCount));
			break;


		case SoftKeyListNoGraphics:
			Debug::trace("SoftKeyLists", "serializing for format SoftKeyListNoGraphics: ");
			serializedData.append(dataTypeList.serialize(BrowserFormat, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(recordNumList.serialize(BrowserFormat, currentSerializeOffset, cellCount));
			serializedData.append(BrowserFormatSeparator);
			descriptionList.setQuotingElements(false);
			serializedData.append(descriptionList.serialize(BrowserFormat, currentSerializeOffset, cellCount));
#ifdef NEXT_FOR_CLARE
			serializedData.append(BrowserFormatSeparator);
			serializedData.append(displayOrderList.serialize(BrowserFormat, currentSerializeOffset, cellCount));
#endif
			break;

		default:
			// todo - throw exception("invalid format");
			break;
	}
	return serializedData.c_str();
}

