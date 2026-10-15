#include <iostream>

using namespace std;

#include "HobComplexVersion.h"

#include "Integer.h"
#include "Debug.h"
#include "HobString.h"
#include "CfgInternal.h"
#include "Configuration.h"
#include "properties/propertiestree.h"
#include "ULong.h"                      // Convert pointer to string

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"

	int HobComplexVersion::dataType = 0;
	std::string HobComplexVersion::dataTypeName("HobComplexVersion"); 
#endif


HobComplexVersion::HobComplexVersion(): HobVersion(HobVersion::UnknownComponent, 0, 0, 0), installed_(false), softLocDir_("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
//	Debug::trace("HobComplexVersion", HobString("HobComplexVersion 1"));
}

HobComplexVersion::HobComplexVersion(const char *buffer): HobVersion (buffer), softLocDir_("")
{
	File dummy("");

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

//	Debug::trace("HobComplexVersion", HobString("HobComplexVersion 2"));
	getTempDir(versionType_, softLocDir_, dummy);
}

HobComplexVersion::HobComplexVersion(const HobComplexVersion& cv): HobVersion(cv), installed_(cv.installed_),
	softLocDir_(cv.softLocDir_)
{
	HobDependency *pDep = NULL, *pCopy = NULL;
	map<HobVersion::VersionType, HobDependency *>::const_iterator i;

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

//	Debug::trace("HobComplexVersion", HobString("HobComplexVersion 3"));
	for (i = cv.dependencyList_.begin(); i != cv.dependencyList_.end(); i++)
	{
		pDep = i->second;
		pCopy = new HobDependency(*pDep);

		dependencyList_.insert(make_pair(pCopy->getVersionType(), pCopy));
	}
}

HobComplexVersion::HobComplexVersion(const std::string &version, const std::string& dependencyList, bool installed) : 
	HobVersion (version), installed_(installed), softLocDir_("")
{
	std::string::size_type pos1 = 0, pos2 = 0;
	HobDependency* pTemp = NULL;
	string dp;

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

//	Debug::trace("HobComplexVersion", HobString("HobComplexVersion 4"));
	if (dependencyList.size() > 0)
	{
		do
		{
			pos2 = dependencyList.find(DEPENDENCY_SEPERATOR, pos1);

			if (pos2 != std::string::npos)
			{
				dp = dependencyList.substr(pos1, (pos2-pos1));
				pos1 = pos2 + 1;
			}
			else
			{
				dp = dependencyList.substr(pos1);
			}

			if (HobDependency::isAValidDependencyString(dp))
			{
				pTemp = new HobDependency (dp);

				dependencyList_.insert(make_pair(pTemp->getVersionType(), pTemp));
			}
		}
		while (pos2 != std::string::npos);
	}
	util_filesystem::File dummy("");

	getTempDir(versionType_, softLocDir_, dummy);
}


HobComplexVersion::~HobComplexVersion()
{
	HobDependency *pDel = NULL;
	map<HobVersion::VersionType, HobDependency *>::iterator i;

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

//	Debug::trace("HobComplexVersion", HobString("~HobComplexVersion"));

	for (i = dependencyList_.begin(); i != dependencyList_.end(); i++)
	{
		pDel = i->second;
		delete pDel;
	}
	dependencyList_.clear();
}


const std::string* HobComplexVersion::toString()
{
	HobDependency *pDep = NULL;
	map<HobVersion::VersionType, HobDependency *>::const_iterator i;
	HobVersion::toString();

	serialized_ += " D::";

	i = dependencyList_.begin();

	while (i != dependencyList_.end())
	{
		pDep = i->second;
		serialized_+= *pDep->toString();

		i++;
		if (i != dependencyList_.end())
			serialized_+= ",";
	}
	
	serialized_ += " ";

	if (installed_)
		serialized_ += "(I)";
	else
		serialized_ += "(U)";

	return &serialized_;
}

bool HobComplexVersion::isUpgradeForcible()
{
	switch(versionType_)
	{
		case HobVersion::SystemControllerSW:
#ifdef APC_PERIPHERAL_UPDATE_REQUIRES_REBOOT
		case HobVersion::ApcPrinterSW:
#endif
//	We can't support this!!!		case HobVersion::XPEImageSW:
			return true;
			break;
		default:
			return false;
	}
}

void HobComplexVersion::getSoftPath(HobVersion::VersionType vt, std::string& path)
{
	Directory vmBaseDir("");
	vmBaseDir.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREDIRECTORY));//treats path as directory.

	util_filesystem::File vmSoftwareFile("");
	string fileName("");

	switch (vt)
	{
		case HobVersion::SystemControllerSW:
		{
			vmBaseDir.setFullPath("");
			vmBaseDir.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::EVOLUTIONDIRECTORY));
		}
		break;
		case HobVersion::UsbWeigher15KgSW:
		{
			fileName = "weigher.zip";
		}
		break;
		case HobVersion::EssPrinterSW:
		{
			fileName = "ESSPrn.zip";
		}
		break;
		case HobVersion::EppPrinterSW:
		{
			fileName = "EPPPrn.zip";
		}
		break;
		/* Alex Zavala's code for XPE version */
		case HobVersion::Epp2PrinterSW:
		{
			fileName = "EPP2Prn.zip";
		}
		break;
		case HobVersion::EscPrinterSW:
		{
			fileName = "ESCPrn.zip";
		}
		break;
		case HobVersion::EssPrnBootCodeSW:
		{
			fileName = "ESSBoot.zip";
		}
		break;
		case HobVersion::EppPrnBootCodeSW:
		{
			fileName = "EPPBoot.zip";
		}
		break;
		/* Alex Zavala's code for XPE version */
		case HobVersion::Epp2PrnBootCodeSW:
		{
			fileName = "EPP2Boot.zip";
		}
		break;
		case HobVersion::EscPrnBootCodeSW:
		{
			fileName = "ESCBoot.zip";
		}
		break;
		case HobVersion::EssPrnFlashUpdateSW:
		{
			fileName = "ESSFlashUpdate.zip";
		}
		break;
		case HobVersion::EppPrnFlashUpdateSW:
		{
			fileName = "EPPFlashUpdate.zip";
		}
		break;
		/* Alex Zavala's code for XPE version */
		case HobVersion::Epp2PrnFlashUpdateSW:
		{
			fileName = "EPP2FlashUpdate.zip";
		}
		break;
		case HobVersion::EscPrnFlashUpdateSW:
		{
			fileName = "ESCFlashUpdate.zip";
		}
		break;
		case HobVersion::WRMasterNVDataSW:
		{
			fileName = "wrMasterData.zip";
		}
		break;
		case HobVersion::WREMasterNVDataSW:
		{
			fileName = "wreMasterData.zip";
		}
		break;
		case HobVersion::WRMasterBootSW:
		{
			fileName = "wrMasterBoot.zip";
		}
		break;
		case HobVersion::WREMasterBootSW:
		{
			fileName = "wreMasterBoot.zip";
		}
		break;
		case HobVersion::WRMasterAppSW:
		{
			fileName = "wrMasterApp.zip";
		}
		break;
		case HobVersion::WREMasterAppSW:
		{
			fileName = "wreMasterApp.zip";
		}
		break;
		case HobVersion::WRSlave1NVDataSW:
		{
			fileName = "wrSlave1Data.zip";
		}
		break;
		case HobVersion::WRESlave1NVDataSW:
		{
			fileName = "wreSlave1Data.zip";
		}
		break;
		case HobVersion::WRSlave1BootSW:
		{
			fileName = "wrSlave1Boot.zip";
		}
		break;
		case HobVersion::WRESlave1BootSW:
		{
			fileName = "wreSlave1Boot.zip";
		}
		break;
		case HobVersion::WRSlave1AppSW:
		{
			fileName = "wrSlave1App.zip";
		}
		break;
		case HobVersion::WRESlave1AppSW:
		{
			fileName = "wreSlave1App.zip";
		}
		break;

		case HobVersion::WRSlave3NVDataSW:
		{
			fileName = "wrSlave3Data.zip";
		}
		break;
		case HobVersion::WRESlave3NVDataSW:
		{
			fileName = "wreSlave3Data.zip";
		}
		break;
		case HobVersion::WRSlave3BootSW:
		{
			fileName = "wrSlave3Boot.zip";
		}
		break;
		case HobVersion::WRESlave3BootSW:
		{
			fileName = "wreSlave3Boot.zip";
		}
		break;
		case HobVersion::WRSlave3AppSW:
		{
			fileName = "wrSlave3App.zip";
		}
		break;
		case HobVersion::WRESlave3AppSW:
		{
			fileName = "wreSlave3App.zip";
		}
		break;
		case HobVersion::WRSlave4NVDataSW:
		{
			fileName = "wrSlave4Data.zip";
		}
		break;
		case HobVersion::WRESlave4NVDataSW:
		{
			fileName = "wreSlave4Data.zip";
		}
		break;
		case HobVersion::WRSlave4BootSW:
		{
			fileName = "wrSlave4Boot.zip";
		}
		break;
		case HobVersion::WRESlave4BootSW:
		{
			fileName = "wreSlave4Boot.zip";
		}
		break;
		case HobVersion::WRSlave4AppSW:
		{
			fileName = "wrSlave4App.zip";
		}
		break;
		case HobVersion::WRESlave4AppSW:
		{
			fileName = "wreSlave4App.zip";
		}
		break;
		case HobVersion::WRSlave5NVDataSW:
		{
			fileName = "wrSlave5Data.zip";
		}
		break;
		case HobVersion::WRESlave5NVDataSW:
		{
			fileName = "wreSlave5Data.zip";
		}
		break;
		case HobVersion::WRSlave5BootSW:
		{
			fileName = "wrSlave5Boot.zip";
		}
		break;
		case HobVersion::WRESlave5BootSW:
		{
			fileName = "wreSlave5Boot.zip";
		}
		break;
		case HobVersion::WRSlave5AppSW:
		{
			fileName = "wrSlave5App.zip";
		}
		break;
		case HobVersion::WRESlave5AppSW:
		{
			fileName = "wreSlave5App.zip";
		}
		break;
		case HobVersion::FpcPrinterSW:
		{
			fileName = "FPC.zip";
		}
		break;
		case HobVersion::ApcPrinterSW:
		{
			fileName = "APC.zip";
		}
		break;
		case HobVersion::KpcPrinterSW:
		{
			fileName = "KPC.zip";
		}
		break;
		case HobVersion::SSPrinterSW:
		{
			fileName = "SSP.zip";
		}
		break;
		/* Look into this case Alex Zavala XPE version */
		case HobVersion::Aws2WeigherSW:
		{
			fileName = "XPEWeigher.bin";
		}
		break;
		case HobVersion::GCDAppSW:
		{
			fileName = "GCD.zip";
		}
		break;
		case HobVersion::CLAAppSW://Alex Zavala 9/24
		{
			fileName = "CLAApp.zip";
		}
		break;
		case HobVersion::CLADataSW:
		{
			fileName = "CLAData.zip";//Alex Zavala 10/1 changed from CLADataSW
		}
		break;
		case HobVersion::CLABootSW:
		{
			fileName = "CLABoot.zip";
		}
		break;
		default:
		break;
	}

	vmSoftwareFile.setPath(vmBaseDir.getFullPath());
	vmSoftwareFile.setFileName(fileName);
	path = vmSoftwareFile.getFullPath();
}

void HobComplexVersion::getTempDir(HobVersion::VersionType vt, util_filesystem::Directory& tempDir, util_filesystem::File& tempZip)
{
	Directory vmTmpFiles("");
	vmTmpFiles.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));//treats path as directory.

	switch (vt)
	{
		case HobVersion::SystemControllerSW:
		{
			vmTmpFiles.appendPath("SC");
		}
		break;
		case HobVersion::UsbWeigher15KgSW:
		{
			vmTmpFiles.appendPath("weigher");
		}
		break;
		case HobVersion::EssPrinterSW:
		{
			vmTmpFiles.appendPath("ESSprn");
		}
		break;
		case HobVersion::EppPrinterSW:
		{
			vmTmpFiles.appendPath("EPPprn");
		}
		break;
		/* Alex Zavala's version for XPE version */
		case HobVersion::Epp2PrinterSW:
		{
			vmTmpFiles.appendPath("EPP2prn");
		}
		break;
		case HobVersion::EscPrinterSW:
		{
			vmTmpFiles.appendPath("ESCprn");
		}
		break;
		case HobVersion::EssPrnBootCodeSW:
		{
			vmTmpFiles.appendPath("ESSBootCode");
		}
		break;
		case HobVersion::EppPrnBootCodeSW:
		{
			vmTmpFiles.appendPath("EPPBootCode");
		}
		break;
		/* Alex Zavala's version for XPE version (value does not exist)*/
		case HobVersion::Epp2PrnBootCodeSW:
		{
			vmTmpFiles.appendPath("EPP2BootCode");
		}
		break;
		case HobVersion::EscPrnBootCodeSW:
		{
			vmTmpFiles.appendPath("ESCBootCode");
		}
		break;
		case HobVersion::EssPrnFlashUpdateSW:
		{
			vmTmpFiles.appendPath("ESSFLUD");
		}
		break;
		case HobVersion::EppPrnFlashUpdateSW:
		{
			vmTmpFiles.appendPath("EPPFLUD");
		}
		break;
		/* Alex Zavala's version for XPE version (value does not exist)*/
		case HobVersion::Epp2PrnFlashUpdateSW:
		{
			vmTmpFiles.appendPath("EPP2FLUD");
		}
		break;
		case HobVersion::EscPrnFlashUpdateSW:
		{
			vmTmpFiles.appendPath("ESCFLUD");
		}
		break;
		case HobVersion::WRMasterNVDataSW:
		{
			vmTmpFiles.appendPath("wrMasterData");
		}
		break;
		case HobVersion::WREMasterNVDataSW:
		{
			vmTmpFiles.appendPath("wreMasterData");
		}
		break;
		case HobVersion::WRMasterBootSW:
		{
			vmTmpFiles.appendPath("wrMasterBoot");
		}
		break;
		case HobVersion::WREMasterBootSW:
		{
			vmTmpFiles.appendPath("wreMasterBoot");
		}
		break;
		case HobVersion::WRMasterAppSW:
		{
			vmTmpFiles.appendPath("wrMasterApp");
		}
		break;
		case HobVersion::WREMasterAppSW:
		{
			vmTmpFiles.appendPath("wreMasterApp");
		}
		break;
		case HobVersion::WRSlave1NVDataSW:
		{
			vmTmpFiles.appendPath("wrSlave1Data");
		}
		break;
		case HobVersion::WRESlave1NVDataSW:
		{
			vmTmpFiles.appendPath("wreSlave1Data");
		}
		break;
		case HobVersion::WRSlave1BootSW:
		{
			vmTmpFiles.appendPath("wrSlave1Boot");
		}
		break;
		case HobVersion::WRESlave1BootSW:
		{
			vmTmpFiles.appendPath("wreSlave1Boot");
		}
		break;
		case HobVersion::WRSlave1AppSW:
		{
			vmTmpFiles.appendPath("wrSlave1App");
		}
		break;
		case HobVersion::WRESlave1AppSW:
		{
			vmTmpFiles.appendPath("wreSlave1App");
		}
		break;
		case HobVersion::WRSlave3NVDataSW:
		{
			vmTmpFiles.appendPath("wrSlave3Data");
		}
		break;
		case HobVersion::WRESlave3NVDataSW:
		{
			vmTmpFiles.appendPath("wreSlave3Data");
		}
		break;
		case HobVersion::WRSlave3BootSW:
		{
			vmTmpFiles.appendPath("wrSlave3Boot");
		}
		break;
		case HobVersion::WRESlave3BootSW:
		{
			vmTmpFiles.appendPath("wreSlave3Boot");
		}
		break;
		case HobVersion::WRSlave3AppSW:
		{
			vmTmpFiles.appendPath("wrSlave3App");
		}
		break;
		case HobVersion::WRESlave3AppSW:
		{
			vmTmpFiles.appendPath("wreSlave3App");
		}
		break;
		case HobVersion::WRSlave4NVDataSW:
		{
			vmTmpFiles.appendPath("wrSlave4Data");
		}
		break;
		case HobVersion::WRESlave4NVDataSW:
		{
			vmTmpFiles.appendPath("wreSlave4Data");
		}
		break;
		case HobVersion::WRSlave4BootSW:
		{
			vmTmpFiles.appendPath("wrSlave4Boot");
		}
		break;
		case HobVersion::WRESlave4BootSW:
		{
			vmTmpFiles.appendPath("wreSlave4Boot");
		}
		break;
		case HobVersion::WRSlave4AppSW:
		{
			vmTmpFiles.appendPath("wrSlave4App");
		}
		break;
		case HobVersion::WRESlave4AppSW:
		{
			vmTmpFiles.appendPath("wreSlave4App");
		}
		break;
		case HobVersion::WRSlave5NVDataSW:
		{
			vmTmpFiles.appendPath("wrSlave5Data");
		}
		break;
		case HobVersion::WRESlave5NVDataSW:
		{
			vmTmpFiles.appendPath("wreSlave5Data");
		}
		break;
		case HobVersion::WRSlave5BootSW:
		{
			vmTmpFiles.appendPath("wrSlave5Boot");
		}
		break;
		case HobVersion::WRESlave5BootSW:
		{
			vmTmpFiles.appendPath("wreSlave5Boot");
		}
		break;
		case HobVersion::WRSlave5AppSW:
		{
			vmTmpFiles.appendPath("wrSlave5App");
		}
		break;
		case HobVersion::WRESlave5AppSW:
		{
			vmTmpFiles.appendPath("wreSlave5App");
		}
		break;
		//this is the wrapper camera system and it's firmware image does not change for the EWS.
		case HobVersion::WRSlave6NVDataSW:
		{
			vmTmpFiles.appendPath("wrSlave6Data");
		}
		break;
		case HobVersion::WRSlave6BootSW:
		{
			vmTmpFiles.appendPath("wrSlave6Boot");
		}
		break;
		case HobVersion::WRSlave6AppSW:
		{
			vmTmpFiles.appendPath("wrSlave6App");
		}
		break;
		case HobVersion::FpcPrinterSW:
		{
			vmTmpFiles.appendPath("FPC");
			break;
		}
		case HobVersion::ApcPrinterSW:
		{
			vmTmpFiles.appendPath("APC");
		}
		break;
		case HobVersion::KpcPrinterSW:
		{
			vmTmpFiles.appendPath("KPC");
		}
		break;
		case HobVersion::SSPrinterSW:
		{
			vmTmpFiles.appendPath("SSP");
		}
		break;
		/* Alex Zavala look into the XPE version case below */
		case HobVersion::Aws2WeigherSW:
		{
			vmTmpFiles.appendPath("XPEWeigher");
		}
		break;
		case HobVersion::CLAAppSW://Added 9/24 Alex Zavala
		{
			vmTmpFiles.appendPath("CLAApp");
		}
		break;
		case HobVersion::CLADataSW:
		{
			vmTmpFiles.appendPath("CLAData");
		}
		break;
		case HobVersion::CLABootSW:
		{
			vmTmpFiles.appendPath("CLABoot");
		}
		break;
		case HobVersion::GCDAppSW:
		{
			vmTmpFiles.appendPath("GCDManager");
		}
		break;
	    default:
	    {
	        // Nothing to do
	        break;
	    }
	}

	tempDir.appendPath(vmTmpFiles.getFullPath());
	tempZip.setPath(vmTmpFiles.getFullPath());
	tempZip.setFileName("SW.zip");
}

void HobComplexVersion::getExtractionDir(HobVersion::VersionType vt, Directory& tempDir, USBD_NODE_ID UNUSED(nodeId))
{
	UNREFERENCED_PARAMETER(nodeId);

	Directory vmTmpFiles("");
	vmTmpFiles.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));//treats path as directory.

	Debug::trace("VersionManager.upgrade", HobString("HobComplexVersion::getExtractionDir(") + Integer::toString(vt) + "):  vmTmpFiles = " + vmTmpFiles.getFullPath());
	
	/*change firmware folder names from nodeId which is meaningless to the peripheral name -- ats*/		
	
	switch (vt)
	{
		case HobVersion::SystemControllerSW:
		{
			vmTmpFiles.appendPath("SC");
		}
		break;
		case HobVersion::UsbWeigher15KgSW:
		{		
			vmTmpFiles.appendPath("EzUSBWeigher");	
		}
		break;
		case HobVersion::Aws2WeigherSW:
		{
			vmTmpFiles.appendPath("Aws2Weigher");
		}
		break;
		case HobVersion::FpcPrinterSW:
		{
			vmTmpFiles.appendPath("FpcPrinter");
		}
		break;
		case HobVersion::ApcPrinterSW:
		{
			vmTmpFiles.appendPath("ApcPrinter");
		}
		break;
		case HobVersion::KpcPrinterSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("KpcPrinter\\app");
			#else
			vmTmpFiles.appendPath("KpcPrinter/app");
			#endif	
		}
		break;
		case HobVersion::SSPrinterSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("SSPrinter\\app");
			#else
			vmTmpFiles.appendPath("SSPrinter/app");
			#endif	
		}
		break;
		/* Alex Zavala XPE version code*/
		/************************************** BEGINNING OF XPE PRINTER CODE ********************************************/
		case HobVersion::Epp2PrinterSW:
		{
			vmTmpFiles.appendPath("Epp2Printer");
		}
		break;
		case HobVersion::Epp2PrnBootCodeSW:	
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("Epp2Printer\\boot");
			#else
			vmTmpFiles.appendPath("Epp2Printer/boot");
			#endif		
		}
		break;
		case HobVersion::Epp2PrnFlashUpdateSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("Epp2Printer\\flud");
			#else
			vmTmpFiles.appendPath("Epp2Printer/flud");
			#endif		
		}
		break;
		/************************************** END OF XPE PRINTER CODE ********************************************/
		//NOTE: Ess and Esc printers never went into production. support is not needed -- ats / tf		
		case HobVersion::EssPrinterSW:
		case HobVersion::EppPrinterSW:
		case HobVersion::EscPrinterSW:
		{		
			vmTmpFiles.appendPath("EppPrinter");
		}
		break;
		case HobVersion::EssPrnBootCodeSW:
		case HobVersion::EscPrnBootCodeSW:
		case HobVersion::EppPrnBootCodeSW:	
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EppPrinter\\boot");
			#else
			vmTmpFiles.appendPath("EppPrinter/boot");
			#endif		
		}
		break;
		case HobVersion::EssPrnFlashUpdateSW:
		case HobVersion::EppPrnFlashUpdateSW:
		case HobVersion::EscPrnFlashUpdateSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EppPrinter\\flud");
			#else
			vmTmpFiles.appendPath("EppPrinter/flud");
			#endif		
		}
		break;
		case HobVersion::WRMasterNVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\master\\data");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/master/data");
			#endif
		}
		break;
		case HobVersion::WREMasterNVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\master\\data");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/master/data");
			#endif
		}
		break;

		case HobVersion::WRMasterBootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\master\\boot");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/master/boot");
			#endif
		}
		break;
		case HobVersion::WREMasterBootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\master\\boot");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/master/boot");
			#endif
		}
		break;

		case HobVersion::WRMasterAppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\master\\app");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/master/app");
			#endif		
		}
		break;
		case HobVersion::WREMasterAppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\master\\app");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/master/app");
			#endif		
		}
		break;

		case HobVersion::WRSlave1NVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave1\\data");
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave1/data");
			#endif
		}
		break;
		case HobVersion::WRESlave1NVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave1\\data");
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave1/data");
			#endif
		}
		break;

		case HobVersion::WRSlave1BootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave1\\boot");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave1/boot");
			#endif		
		}
		break;
		case HobVersion::WRESlave1BootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave1\\boot");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave1/boot");
			#endif		
		}
		break;

		case HobVersion::WRSlave1AppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave1\\app");
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave1/app");
			#endif		
		}
		break;
		case HobVersion::WRESlave1AppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave1\\app");
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave1/app");
			#endif		
		}
		break;

		case HobVersion::WRSlave3NVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave3\\data");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave3/data");
			#endif		
		}
		break;
		case HobVersion::WRESlave3NVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave3\\data");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave3/data");
			#endif		
		}
		break;

		case HobVersion::WRSlave3BootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave3\\boot");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave3/boot");
			#endif		
		}
		break;
		case HobVersion::WRESlave3BootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave3\\boot");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave3/boot");
			#endif		
		}
		break;

		case HobVersion::WRSlave3AppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave3\\app");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave3/app");
			#endif
		}
		break;
		case HobVersion::WRESlave3AppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave3\\app");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave3/app");
			#endif
		}
		break;

		case HobVersion::WRSlave4NVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave4\\data");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave4/data");
			#endif		
		}
		break;
		case HobVersion::WRESlave4NVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave4\\data");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave4/data");
			#endif		
		}
		break;

		case HobVersion::WRSlave4BootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave4\\boot");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave4/boot");
			#endif
		}
		break;
		case HobVersion::WRESlave4BootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave4\\boot");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave4/boot");
			#endif
		}
		break;

		case HobVersion::WRSlave4AppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave4\\app");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave4/app");
			#endif		
		}
		break;
		case HobVersion::WRESlave4AppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave4\\app");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave4/app");
			#endif		
		}
		break;

		case HobVersion::WRSlave5NVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave5\\data");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave5/data");
			#endif		
		}
		break;
		case HobVersion::WRESlave5NVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave5\\data");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave5/data");
			#endif		
		}
		break;

		case HobVersion::WRSlave5BootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave5\\boot");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave5/boot");
			#endif
			
		}
		break;
		case HobVersion::WRESlave5BootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave5\\boot");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave5/boot");
			#endif
			
		}
		break;

		case HobVersion::WRSlave5AppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave5\\app");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave5/app");
			#endif		
		}
		break;
		case HobVersion::WRESlave5AppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("EWSWrapper\\slave5\\app");		
			#else
			vmTmpFiles.appendPath("EWSWrapper/slave5/app");
			#endif		
		}
		break;

		case HobVersion::WRSlave6NVDataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave6\\data");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave6/data");
			#endif		
		}
		break;
		case HobVersion::WRSlave6BootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave6\\boot");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave6/boot");
			#endif		
		}
		break;
		case HobVersion::WRSlave6AppSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("AWSWrapper\\slave6\\app");		
			#else
			vmTmpFiles.appendPath("AWSWrapper/slave6/app");
			#endif		
		}
		break;
		case HobVersion::CLAAppSW://Alex Zavala 9/24
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("CLA\\app");		
			#else
			vmTmpFiles.appendPath("CLA/app");
			#endif
		}
		break;
		case HobVersion::CLADataSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("CLA\\data");		
			#else
			vmTmpFiles.appendPath("CLA/data");
			#endif
		}
		break;
		case HobVersion::CLABootSW:
		{
			#if defined(_WIN32)
			vmTmpFiles.appendPath("CLA\\boot");		
			#else
			vmTmpFiles.appendPath("CLA/boot");
			#endif
		}
		break;
		case HobVersion::GCDAppSW:
		{
			vmTmpFiles.appendPath("GCDManager");
		}
		break;
		default:
		break;
	}
	tempDir.appendPath(vmTmpFiles.getFullPath());
	Debug::trace("VersionManager.upgrade", HobString("HobComplexVersion::getExtractionDir(") + Integer::toString(vt) + "):  Returning tempDir = " + tempDir.getFullPath());
}

HobVersion::VersionType HobComplexVersion::getHardwareTypeForSoftwareType(HobVersion::VersionType swvt)
{
	switch (swvt)
	{
		case HobVersion::SystemControllerSW:
		{
			return HobVersion::ASSSysControllerHW;
		}
		break;
		case HobVersion::UsbWeigher15KgSW:
		{
			return HobVersion::UsbWeigher15KgHW;
		}
		break;
		case HobVersion::Aws2WeigherSW: // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		{
			return HobVersion::Aws2WeigherHW;
		}
		break;
		case HobVersion::EssPrinterSW:
		{
			return HobVersion::EssPrinterHW;
		}
		break;
		case HobVersion::EppPrinterSW:
		{
			return HobVersion::EppPrinterHW;
		}
		break;
		case HobVersion::Epp2PrinterSW:	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
		{
			return HobVersion::Epp2PrinterHW;
		}
		break;
		case HobVersion::EscPrinterSW:
		{
			return HobVersion::EscPrinterHW;
		}
		break;
		case HobVersion::EssPrnBootCodeSW:
		{
			return HobVersion::EssPrinterHW;
		}
		break;
		case HobVersion::EppPrnBootCodeSW:
		{
			return HobVersion::EppPrinterHW;
		}
		break;
		/* Alex Zavala's code for XPE version (values do not exist) */
		case HobVersion::Epp2PrnBootCodeSW:
		{
			return HobVersion::Epp2PrinterHW;
		}
		break;
		case HobVersion::EscPrnBootCodeSW:
		{
			return HobVersion::EscPrinterHW;
		}
		break;
		case HobVersion::EssPrnFlashUpdateSW:
		{
			return HobVersion::EssPrinterHW;
		}
		break;
		case HobVersion::EppPrnFlashUpdateSW:
		{
			return HobVersion::EppPrinterHW;
		}	 
		break;
		/* Alex Zavala's code for XPE version (values do not exist) */
		case HobVersion::Epp2PrnFlashUpdateSW:
		{
			return HobVersion::Epp2PrinterHW;
		}	 
		break;
		case HobVersion::EscPrnFlashUpdateSW:
		{
			return HobVersion::EscPrinterHW;
		}
		break;
		case HobVersion::WRMasterNVDataSW:
		case HobVersion::WREMasterNVDataSW:
		case HobVersion::WRMasterBootSW:
		case HobVersion::WREMasterBootSW:	
		case HobVersion::WRMasterAppSW:
		case HobVersion::WREMasterAppSW:
		case HobVersion::WRSlave1NVDataSW:
		case HobVersion::WRESlave1NVDataSW:
		case HobVersion::WRSlave1BootSW:
		case HobVersion::WRESlave1BootSW:
		case HobVersion::WRSlave1AppSW:
		case HobVersion::WRESlave1AppSW:
		case HobVersion::WRSlave3NVDataSW:
		case HobVersion::WRESlave3NVDataSW:
		case HobVersion::WRSlave3BootSW:
		case HobVersion::WRESlave3BootSW:
		case HobVersion::WRSlave3AppSW:
		case HobVersion::WRESlave3AppSW:
		case HobVersion::WRSlave4NVDataSW:
		case HobVersion::WRESlave4NVDataSW:
		case HobVersion::WRSlave4BootSW:
		case HobVersion::WRESlave4BootSW:
		case HobVersion::WRSlave4AppSW:
		case HobVersion::WRESlave4AppSW:
		case HobVersion::WRSlave5NVDataSW:
		case HobVersion::WRESlave5NVDataSW:
		case HobVersion::WRSlave5BootSW:
		case HobVersion::WRESlave5BootSW:
		case HobVersion::WRSlave5AppSW:
		case HobVersion::WRESlave5AppSW:
		case HobVersion::WRSlave6NVDataSW:
		case HobVersion::WRSlave6BootSW:
		case HobVersion::WRSlave6AppSW:
		{
			return HobVersion::WrapperControllerHW;
		}
		break;
		case HobVersion::FpcPrinterSW:
		{
			return HobVersion::FpcPrinterHW;
		}
		break;
		case HobVersion::ApcPrinterSW:
		{
			return HobVersion::ApcPrinterHW;
		}
		break;
		case HobVersion::KpcPrinterSW:
		{
			return HobVersion::KpcPrinterHW;
		}
		break;
		case HobVersion::SSPrinterSW:
		{
			return HobVersion::SSPrinterHW;
		}
		break;
		case HobVersion::CLAAppSW://Alex Zavala 9/24
		case HobVersion::CLADataSW:
		case HobVersion::CLABootSW:
		{
			return HobVersion::CLAControllerHW;
		}
		case HobVersion::GCDAppSW:
		default:
		{
			return UnknownComponent;
			break;
		}
	}
}

HobVersion::VersionType HobComplexVersion::getCorrespondingSoftTypeFromHardType(HobVersion::VersionType hardwareType)
{
	switch(hardwareType)
	{
		case ASSSysControllerHW:
		{
			return SystemControllerSW;
			break;
		}
		case UsbWeigher15KgHW:
		{
			return UsbWeigher15KgSW;
			break;
		}
		case Aws2WeigherHW: // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
		{
			return Aws2WeigherSW;
			break;
		}
		case EssPrinterHW:
		{
			return EssPrinterSW;
			break;
		}
		case EppPrinterHW:
		{
			return EppPrinterSW;
			break;
		}
		case Epp2PrinterHW:	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
		{
			return Epp2PrinterSW;
			break;
		}
		case EscPrinterHW:
		{
			return EscPrinterSW;
			break;
		}
		case HobVersion::FpcPrinterHW:
		{
			return FpcPrinterSW;
			break;
		}
		case HobVersion::ApcPrinterHW:
		{
			return ApcPrinterSW;
			break;
		}
		case HobVersion::KpcPrinterHW:
		{
			return KpcPrinterSW;
			break;
		}
		case HobVersion::SSPrinterHW:
		{
			return SSPrinterSW;
			break;
		}
		case HobVersion::GCDHW:
		{
			return GCDAppSW;
			break;
		}
		case HobVersion::CLAControllerHW://Alex Zavala 9/24
		{
			return CLAAppSW;
			break;
		}
		default:
		{
			return UnknownComponent;
			break;
		}
	}
}

HobDependency::DependencyResult HobComplexVersion::dependencyCheck(HobVersion* pVer)
{
	HobVersion::VersionType theirVT = pVer->getVersionType();
	map<HobVersion::VersionType, HobDependency *>::iterator i;
	HobDependency *pDep = NULL;
	HobDependency::DependencyResult firstCheck = HobDependency::NoDependency;

	i = dependencyList_.find(theirVT);
	if (i != dependencyList_.end())
	{
		pDep = i->second;

		firstCheck = pDep->dependencyCheck(pVer);
	}
	return firstCheck;
}

void HobComplexVersion::saveToPropertiesFile(util_env::PropertiesTree *pINI)
{
	std::string depList("");
	map<HobVersion::VersionType, HobDependency *>::iterator i;
	HobDependency *pDep = NULL;
	char tmp[10];

	Debug::trace("firmware.upgrade", HobString("saveToPropertiesFile:start"));

	if (pINI != NULL)
	{
		HobVersion::saveToPropertiesFile(pINI);

		sprintf(tmp, "%d", versionType_);

		std::string keyDep("component.");
		keyDep += tmp;
		keyDep += ".dependency";

		i = dependencyList_.begin();

		while (i != dependencyList_.end())
		{
			pDep = i->second;
			depList += pDep->getDependencyString();

			i++;
			if (i != dependencyList_.end())
				depList+= ",";
		}

		pINI->setProperty(keyDep, depList);
	}
	Debug::trace("firmware.upgrade", HobString("saveToPropertiesFile:end"));
}

const char *HobComplexVersion::serialize() const
{
	return HobVersion::serialize();
}
