/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "Statistics.h"
#include "StatisticsTags.h"
#include "ProductManager.h"
#include "ClassManager.h"
#include "NutrifactsManager.h"
#include "ProductTextManager.h"
#include "LabelTypeManager.h"
#include "LabelGraphicManager.h"
#include "OperatorManager.h"
#include "VersionManager.h"
#include "HobComplexVersion.h"
#include "DBException.h"
#include "SoftkeyManager.h"
#include "TransactionManager.h"
#include "CutTestManager.h"
#include "Configuration.h"

#include "DBSession.h"

#include "Integer.h"
#include "Long.h"
#include "ULong.h"
#include "Hex.h"
#include "Bool.h"
#include "MemShow.h"


#include "DBProductsRM.h"
#include "DBClassesRM.h"
#include "DBNutritionsRM.h"
#include "DBOperatorsRM.h"
#include "DBProductTextsRM.h"
#include "DBLabelsRM.h"
#include "DBProductStatisticsRM.h"
#include "DBROMLabelsRM.h"
#include "DBCategoriesRM.h"


using namespace hfc;


#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int Statistics::dataType = 0;
std::string Statistics::dataTypeName("Statistics"); 
#endif

std::list<StatisticsFieldOffsets> Statistics::fieldList;


Statistics::Statistics(DBSession *session)
	: mySession(session)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();
}

/* We don't receive any of these messages, we only send them out when requested;
 * therefore, we don't have the constructor that takes the buffer received from the outside world.
 *
 * Statistics::Statistics(const char *data, int len, int formatType)
 *
 */

Statistics::~Statistics()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void Statistics::init()
{
//	Debug::trace("Statistics", HobString(" - before getting memory statistics"));
//	Debug::trace("Statistics", HobString(" - after getting memory statistics"));

	//fstatfs("/ram0");
	usedEVMMemory_ = 0;
	freeEVMMemory_ = 0;

	initFieldList();

	HobString driveName = Configuration::getInstance()->getCompactFlashDriveName();
#if defined (USES_RAMDISK)
	HobString ramName = Configuration::getInstance()->getPermanentRamDriveName();
#endif
	try
	{
		util_platform::Platform& plat = util_platform::Platform::getInstance();

		// Amount of used and free flash memory
		// Browser wants this in KB, not bytes.  So divde the number of bytes by 1024.
		flashUsedBytes = plat.fileSystemUsedSpace(driveName);
		flashUsedKb = flashUsedBytes / 1024;
		flashFreeBytes = plat.fileSystemFreeSpace(driveName);
		flashFreeKb = flashFreeBytes / 1024;
#if defined (USES_RAMDISK)
		ramDiskUsedBytes = plat.fileSystemUsedSpace(ramName);
		ramDiskUsedKb = ramDiskUsedBytes / 1024;
		ramDiskFreeBytes = plat.fileSystemFreeSpace(ramName);
		ramDiskFreeKb = ramDiskFreeBytes / 1024;
#else
		ramDiskUsedBytes = 0;
		ramDiskUsedKb = 0;
		ramDiskFreeBytes = 0;
		ramDiskFreeKb = 0;
#endif
		// Amount of used and free RAM memory
		PAL_absclass::PAL_globalMemoryStatus(&memStats);

#if defined ( __linux__ )
//	Latest change has us getting the data in Kb already.
		ramUsedKb = memStats.dwTotalPhys - memStats.dwAvailPhys;
		ramFreeKb = memStats.dwAvailPhys;
#else
		ramUsedBytes = memStats.dwTotalPhys - memStats.dwAvailPhys;
		ramUsedKb = ramUsedBytes / 1024;
		ramFreeBytes = memStats.dwAvailPhys;
		ramFreeKb = ramFreeBytes / 1024;
#endif
		bool isTracing = Debug::isTracing("Statistics");
		if (isTracing)
		{
			Debug::trace("Statistics", HobString(" - memStats.dwTotalPhys: ") + HobString(ULong::toString((unsigned long)memStats.dwTotalPhys)));
			Debug::trace("Statistics", HobString(" - memStats.dwAvailPhys: ") + HobString(ULong::toString((unsigned long)memStats.dwAvailPhys)));

			Debug::trace("Statistics", HobString(" - flash used (bytes): ") + HobString(ULong::toString((unsigned long)flashUsedBytes)));
			Debug::trace("Statistics", HobString(" - flash used (Kbytes): ") + HobString(ULong::toString((unsigned long)flashUsedKb)));
			Debug::trace("Statistics", HobString(" - flash free (bytes): ") + HobString(ULong::toString((unsigned long)flashFreeBytes)));
			Debug::trace("Statistics", HobString(" - flash free (Kbytes): ") + HobString(ULong::toString((unsigned long)flashFreeKb)));

			Debug::trace("Statistics", HobString(" - RAM used (bytes): ") + HobString(ULong::toString((unsigned long)ramUsedBytes)));
			Debug::trace("Statistics", HobString(" - RAM used (Kbytes): ") + HobString(ULong::toString((unsigned long)ramUsedKb)));
			Debug::trace("Statistics", HobString(" - RAM free (bytes): ") + HobString(ULong::toString((unsigned long)ramFreeBytes)));
			Debug::trace("Statistics", HobString(" - RAM free (Kbytes): ") + HobString(ULong::toString((unsigned long)ramFreeKb)));
#if defined (USES_RAMDISK)
			Debug::trace("Statistics", HobString(" - RAM Disk used (bytes): ") + HobString(ULong::toString((unsigned long)ramDiskUsedBytes)));
			Debug::trace("Statistics", HobString(" - RAM Disk used (Kbytes): ") + HobString(ULong::toString((unsigned long)ramDiskUsedKb)));
			Debug::trace("Statistics", HobString(" - RAM Disk free (bytes): ") + HobString(ULong::toString((unsigned long)ramDiskFreeBytes)));
			Debug::trace("Statistics", HobString(" - RAM Disk free (Kbytes): ") + HobString(ULong::toString((unsigned long)ramDiskFreeKb)));
#endif
			PAL_absclass::PAL_sleep(1);
			memShow(0);
			PAL_absclass::PAL_sleep(1);
		}

		if (isTracing)
		{
//			Debug::trace("Statistics", HobString(" - sys phys mem top: ") + HobString(Hex::toString((unsigned long)physMemTop)) +
//												HobString(" : ") + HobString(ULong::toString((unsigned long)(physMemTop - (char *)0))));
//			Debug::trace("Statistics", HobString(" - sys phys mem top: ") + HobString(Hex::toString((unsigned int)physMemTop)) +
//												HobString(" : ") + HobString(ULong::toString((unsigned long)(physMemTop - (char *)0))));
//			Debug::trace("Statistics", HobString(" - sys mem top: ") + HobString(Hex::toString((unsigned long)memTop)) +
//												HobString(" : ") + HobString(ULong::toString((unsigned long)(memTop - (char *)0))));
//			Debug::trace("Statistics", HobString(" - sys mem top: ") + HobString(Hex::toString((unsigned int)memTop)) +
//												HobString(" : ") + HobString(ULong::toString((unsigned long)(memTop - (char *)0))));

			/*Debug::trace("Statistics", HobString(" - difference in physical to sys mem top: ") +
												HobString(ULong::toString((unsigned long)totMem)));*/
		}

//		ramUsedBytes = ((unsigned long) memTop) - ramFreeBytes;
//		ramUsedKb = ramUsedBytes / 1024;

//		if (isTracing)
//		{
//			Debug::trace("Statistics", HobString(" - total RAM used (memTop - free) (bytes): ") + HobString(ULong::toString((unsigned long)ramUsedBytes)));
//			Debug::trace("Statistics", HobString(" - total RAM used (memTop - free) (Kbytes): ") + HobString(ULong::toString((unsigned long)ramUsedKb)));
//		}

		// Number of products in the database
		ProductManager pm(mySession);
		productCount = pm.countAllProducts();
		PAL_absclass::PAL_sleep(1);

		// Number of classes in the database
		ClassManager cm(mySession);
		classCount = cm.countAllClasses();
		PAL_absclass::PAL_sleep(1);

		// Number of expanded texts in the database
		ProductTextManager etm(psttExpandedText, mySession);
		expTextCount = etm.countAllProductTexts();
		PAL_absclass::PAL_sleep(1);

		// Number of special messages in the database
		ProductTextManager smm(psttSpecialMessage, mySession);
		spMsgCount = smm.countAllProductTexts();
		PAL_absclass::PAL_sleep(1);

		// Number of product notes in the database
		ProductTextManager pnm(psttProductNote, mySession);
		productNotesCount = pnm.countAllProductTexts();
		PAL_absclass::PAL_sleep(1);

		// Number of nutrifacts in the database
		NutrifactsManager nfm(mySession);
		nutrifactsCount = nfm.countAllNutrifacts();
		PAL_absclass::PAL_sleep(1);

		// Number of label types in the database
		LabelTypeManager ltm(mySession);
		labelTypeCount = ltm.countAllLabelTypes();
		PAL_absclass::PAL_sleep(1);

		// Number of User (custom) label graphics in the database
		LabelGraphicManager lgm(mySession);
		graphicCount = lgm.countAllCustomGraphics();
		PAL_absclass::PAL_sleep(1);

		// Number of operator ID's in the database
		OperatorManager om(mySession);
		operCount = om.countAllOperators();
		PAL_absclass::PAL_sleep(1);

		// Number of Flashkeys/Softkeys in the database
		SoftkeyManager sm(mySession);
		softkeysCount = sm.countAllSoftkeys();
		PAL_absclass::PAL_sleep(1);

		// Now for the Transactions in the database
		TransactionManager tm(mySession);
		transactionsCount = tm.countAllRecords();
		PAL_absclass::PAL_sleep(1);

		// Now for the Cutting Tests in the database
		CutTestManager ctm(mySession);
		cuttingTestsCount = ctm.countAllCuttingTests();
		PAL_absclass::PAL_sleep(1);

		// The average number of products that can be added to the database
		avgSpace = pm.calculateAverageSpace();  // in bytes
		Debug::trace("Statistics", HobString(" - average space used per product (bytes): ") + HobString(Integer::toString(avgSpace)));
#if defined (USES_RAMDISK)
		Debug::trace("Statistics", HobString(" - space used determine number of additional products (bytes): ") + HobString(ULong::toString((unsigned long)ramDiskFreeBytes)));
#endif
		additionalItemCount = 0;
		if (avgSpace)
			 additionalItemCount = (int)flashFreeBytes / avgSpace;
		Debug::trace("Statistics", HobString(" - number of additional products: ") + HobString(Integer::toString(additionalItemCount)));
	}
	catch (edb::DBException &dbe)
	{
		if (Debug::isTracing("DBAccess.exception"))
		{
			Debug::trace("DBAccess.exception", HobString("Statistics::init() ********** database exception **********"));
			edb::DBException::Type t = dbe.getType();
			Debug::trace("DBAccess.exception", HobString("	 DBException: type:") + HobString(Integer::toString(t)) + HobString("  info:") + dbe.info());
		}
	}
	catch (...)
	{
		Debug::trace("Statistics", "*****************************************************************");
		Debug::trace("Statistics", "****************** Just got an unknown exception ****************");
		Debug::trace("Statistics", "*****************************************************************");
	}
}


void Statistics::initFieldList()
{
	if (fieldList.empty())
	{
		fieldList.push_back(fieldOffsetUsedFlashMemory);
		fieldList.push_back(fieldOffsetFreeFlashMemory);
		fieldList.push_back(fieldOffsetUsedRamMemory);
		fieldList.push_back(fieldOffsetFreeRamMemory);
		fieldList.push_back(fieldOffsetProductsCount);
		fieldList.push_back(fieldOffsetClassesCount);
		fieldList.push_back(fieldOffsetExpandedTextCount);
		fieldList.push_back(fieldOffsetSpecialMessagesCount);
		fieldList.push_back(fieldOffsetProductNotesCount);
		fieldList.push_back(fieldOffsetLabelTypesCount);
		fieldList.push_back(fieldOffsetGraphicsCount);
		fieldList.push_back(fieldOffsetOperatorsCount);
		fieldList.push_back(fieldOffsetScaleName);
		fieldList.push_back(fieldOffsetFirmwareVersionStat);
		fieldList.push_back(fieldOffsetFirmwareRevisionStat);

		StatisticsTags statisticsTags((FieldOffsetsList *)&fieldList);
	}
}


const char *Statistics::serialize(int format) const
{
	serializedData = "";
	if (mySession)
	{

		switch (format)
		{
			case BrowserFormat:
				// Amount of used flash memory
				serializedData.append(ULong::toString((unsigned long)flashUsedKb));
				serializedData.append(BrowserFormatSeparator);

				// Amount of free flash memory
				serializedData.append(ULong::toString((unsigned long)flashFreeKb));
				serializedData.append(BrowserFormatSeparator);

				// Amount of used RAM memory
				serializedData.append(ULong::toString((unsigned long)ramUsedKb));
				serializedData.append(BrowserFormatSeparator);

				// Amount of free RAM memory
				serializedData.append(ULong::toString((unsigned long)ramFreeKb));
				serializedData.append(BrowserFormatSeparator);

				// Amount of used ramDisk memory
				serializedData.append(ULong::toString((unsigned long)ramDiskUsedKb));
				serializedData.append(BrowserFormatSeparator);

				// Amount of free ramDisk memory
				serializedData.append(ULong::toString((unsigned long)ramDiskFreeKb));
				serializedData.append(BrowserFormatSeparator);

				// Amount of used EVM memory
				serializedData.append(ULong::toString((unsigned long)usedEVMMemory_));
				serializedData.append(BrowserFormatSeparator);

				// Amount of free EVM memory
				serializedData.append(ULong::toString((unsigned long)freeEVMMemory_));
				serializedData.append(BrowserFormatSeparator);

				// Number of products in the database
				serializedData.append(Integer::toString(productCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of classes in the database
				serializedData.append(Integer::toString(classCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of expanded text in the database
				serializedData.append(Integer::toString(expTextCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of special messages in the database
				serializedData.append(Integer::toString(spMsgCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of product notes in the database
				serializedData.append(Integer::toString(productNotesCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of label types in the database
				serializedData.append(Integer::toString(labelTypeCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of graphics in the database
				serializedData.append(Integer::toString(graphicCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of operator ID's in the database
				serializedData.append(Integer::toString(operCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of avg number of products that can be added to the database
				serializedData.append(Integer::toString(additionalItemCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of nutrifacts in the database
				serializedData.append(Integer::toString(nutrifactsCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of Flashkeys/Softkeys in the database
				serializedData.append(Integer::toString(softkeysCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of Transactions in the database
				serializedData.append(Integer::toString(transactionsCount));
				serializedData.append(BrowserFormatSeparator);

				// Number of Cutting Tests in the database
				serializedData.append(Integer::toString(cuttingTestsCount));
				break;

			case EvolutionTaggedFormat:
			{
				StatisticsTags statisticsTags((FieldOffsetsList *)&fieldList);
				serializedData = "";
				HobString tag;
				HobString fieldData = "";
										
				for (int loopCnt = 1; loopCnt < endStatisticsFieldOffsets; loopCnt++)
				{
					tag = "";
					switch (loopCnt)
					{
						// Total Flash written, even though the 'used' Flash tag is used
						case fieldOffsetUsedFlashMemory:
//							tag = statisticsTags.getTag(loopCnt);
//							fieldData = ULong::toString((unsigned long)((flashFreeBytes + flashUsedBytes) / 1024));
							break;

						case fieldOffsetFreeFlashMemory:
//							tag = statisticsTags.getTag(loopCnt);
//							fieldData = ULong::toString((unsigned long)flashFreeKb);
							break;

						// Total RAM written out, even though the 'used' RAM tag is used
						case fieldOffsetUsedRamMemory:
							tag = statisticsTags.getTag(loopCnt);
//							fieldData = ULong::toString((unsigned long)(((unsigned long) memTop) / 1024));
							fieldData = ULong::toString((unsigned long)flashUsedKb);
							break;

						case fieldOffsetFreeRamMemory:
							tag = statisticsTags.getTag(loopCnt);
//							fieldData = ULong::toString((unsigned long)ramFreeKb);
							fieldData = ULong::toString((unsigned long)flashFreeKb);
							break;

						case fieldOffsetProductsCount:
							tag = statisticsTags.getTag(loopCnt);
							fieldData = Integer::toString(productCount);
							break;

						case fieldOffsetClassesCount:
							tag = statisticsTags.getTag(loopCnt);
							fieldData = Integer::toString(classCount);
							break;

						case fieldOffsetExpandedTextCount:
							tag = statisticsTags.getTag(loopCnt);
							fieldData = Integer::toString(expTextCount);
							break;

						case fieldOffsetSpecialMessagesCount:
							tag = statisticsTags.getTag(loopCnt);
							fieldData = Integer::toString(spMsgCount);
							break;

						case fieldOffsetProductNotesCount:
//							tag = statisticsTags.getTag(loopCnt);
//							fieldData = Integer::toString(productNotesCount);
							break;

						case fieldOffsetNutrifactsCount:
							tag = statisticsTags.getTag(loopCnt);
							fieldData = Integer::toString(nutrifactsCount);
							break;

						case fieldOffsetLabelTypesCount:
//							tag = statisticsTags.getTag(loopCnt);
//							fieldData = Integer::toString(labelTypeCount);
							break;

						case fieldOffsetGraphicsCount:
							tag = statisticsTags.getTag(loopCnt);
							fieldData = Integer::toString(graphicCount);
							break;

						case fieldOffsetFontsCount:
//							tag = statisticsTags.getTag(loopCnt);
//							fieldData = Integer::toString(fontCount);
							break;

						case fieldOffsetOperatorsCount:
							tag = statisticsTags.getTag(loopCnt);
							fieldData = Integer::toString(operCount);
							break;

						case fieldOffsetTotalsCount:
							tag = statisticsTags.getTag(loopCnt);
							fieldData = Integer::toString(0);			//todo - get the actual count
							break;

						case fieldOffsetScaleName:
							tag = statisticsTags.getTag(loopCnt);
							if (Configuration::getInstance()->isServiceCounterScale())
								fieldData = "HT Service Counter";
							else if (Configuration::getInstance()->isPrepackScale())
								fieldData = "HT Prepack";
							else
								fieldData = "HT Self-service";
							break;

						case fieldOffsetFirmwareVersionStat:
						{
							tag = statisticsTags.getTag(loopCnt);
							HobComplexVersion *myVersion = VersionManager::getInstance()->getSystemControllerSoftwareVersion();
							if (myVersion)
								fieldData = Integer::toString(myVersion->getMajorRevision());
							break;
						}

						case fieldOffsetFirmwareRevisionStat:
						{
							tag = statisticsTags.getTag(loopCnt);
							HobComplexVersion *myVersion = VersionManager::getInstance()->getSystemControllerSoftwareVersion();
							if (myVersion)
							{
								fieldData = Integer::toString(myVersion->getMinorRevision());
								fieldData.append(".");
								fieldData.append(Integer::toString(myVersion->getBuildNumber()));
							}
							break;
						}

						default:
							break;
					}

					if (tag.size())
					{
						serializedData.append(tag);
						serializedData.append(fieldData);
						serializedData.append(UnitSeparator);
					}
				}
				break;
			}

			default:
				// todo - throw exception("invalid format");
				break;
		}
	}
	return serializedData.data();
}
