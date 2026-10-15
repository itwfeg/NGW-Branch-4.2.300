#include <string>
#include <iostream>

#if 0
#define DEBUG_FIRMWARE_UPGRADE
#endif

using namespace std;

#include "SoftwareSystem.h"
#include "properties/properties.h"
#include "properties/propertiestree.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "HobDateTime.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"
#include <iostream>
#include <fstream>

#include "Debug.h"
#include "HobString.h"
#include "PeripheralRequestCodeUpdate.h"
#include "PeripheralCodeUpdate.h"
#include "ShutdownSystemRequest.h"
#include "ShutdownSystemResponse.h"

#include "CfgInternal.h"
#include "Configuration.h"

#include "VmStatusMsg.h"
//#include "bootParameters.h"
#include "CMessageCenter.h"
#include "CZip.h"
#include "PAL_platformheader.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int SoftwareSystem::dataType = 0;
std::string SoftwareSystem::dataTypeName("SoftwareSystem"); 
#endif

#define APC_ZIP_SOURCE_FILE "source.bin"
#define EPP_ZIP_SOURCE_FILE "source.bin"
#define EPP2_ZIP_SOURCE_FILE "source.bin"
#define EZWEIGHER_ZIP_SOURCE_FILE "Weigher.hex"
#define ARMWEIGHER_ZIP_SOURCE_FILE "source.bin"
#define AWS_WRAPPER_ZIP_SOURCE_FILES "source.bin" //all wrapper source files
#define GCDMANAGER_ZIP_SOURCE_FILES "source.bin" //all wrapper source files
#define CLA_ZIP_SOURCE_FILE "source.bin"

SoftwareSystem::SoftwareSystem(bool /* convert */):softProps_(NULL)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}
#if 0
SoftwareSystem::SoftwareSystem(const File& fEvolutionProps, bool installed)
{
	string componentVerID, componentDepID;
	const string * componentVer,* componentDepList;
	string nullStr(""), msg("");
	HobComplexVersion* pCV;

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

#ifdef DEBUG_FIRMWARE_UPGRADE
	Debug::trace("firmware.upgrade", HobString("SoftwareSystem:  START"));
#endif

	softProps_ = new PropertiesTree(fEvolutionProps.getFullPath());

	if (softProps_ != NULL)
	{
		PropertiesTree::iterator iter = softProps_->getBeginIterator("component.");

		while ( iter != softProps_->getEndIterator() )
		{

			componentVerID = *iter + ".version";
			componentVer = softProps_->getProperty(componentVerID);
			componentDepID = *iter + ".dependency";
			componentDepList = softProps_->getProperty(componentDepID);


			if (componentDepList == NULL)
				componentDepList = &nullStr;

			if (componentVer!=NULL) 
			{
				if (HobVersion::isAValidFullVersionString(*componentVer))
				{
					pCV = new HobComplexVersion(*componentVer, *componentDepList, installed);

					componentList_.insert(make_pair(pCV->getVersionType(), pCV));

					if (componentVer != NULL)
						msg = *componentVer;
					if (componentDepList != NULL)
					{
						msg += ":";
						msg += *componentDepList;
					}
#ifdef DEBUG_FIRMWARE_UPGRADE
					Debug::trace("firmware.upgrade", 0, msg);
#endif
				}
			}
			++iter;
		}
	}
#ifdef DEBUG_FIRMWARE_UPGRADE
	Debug::trace("firmware.upgrade", HobString("SoftwareSystem:   END"));
#endif
}
#endif
SoftwareSystem::SoftwareSystem(const File& fEvolutionProps, bool installed)
{
	string componentVerID, componentDepID;
	const string * componentVer,* componentDepList;
	string nullStr(""), msg("");
	HobComplexVersion* pCV;

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

#ifdef DEBUG_FIRMWARE_UPGRADE
	Debug::trace("firmware.upgrade", HobString("SoftwareSystem(") + fEvolutionProps.getFullPath() + HobString(", ") + Bool::toString(installed) + HobString("):  Start"));
#endif

	verIniFile_ = fEvolutionProps;

	softProps_ = new util_env::PropertiesTree(fEvolutionProps.getFullPath());

	if (softProps_ != NULL)
	{
		for (int i = 2; i <= HobVersion::EndOfComponents; i++)
		{
			char buffer[32];

			sprintf(buffer, "component.%d.version", i);

			componentVerID = buffer;
			componentVer = softProps_->getProperty(componentVerID);

			sprintf(buffer, "component.%d.dependency", i);

			componentDepID = buffer;
			componentDepList = softProps_->getProperty(componentDepID);

			if (componentDepList == NULL)
				componentDepList = &nullStr;

			if (componentVer!=NULL) 
			{
				if (HobVersion::isAValidFullVersionString(*componentVer))
				{
					pCV = new HobComplexVersion(*componentVer, *componentDepList, installed);

					componentList_.insert(make_pair(pCV->getVersionType(), pCV));

					if (componentVer != NULL)
						msg = *componentVer;
					if (componentDepList != NULL)
					{
						msg += ":";
						msg += *componentDepList;
					}
#ifdef DEBUG_FIRMWARE_UPGRADE
					Debug::trace("firmware.upgrade", 0, msg);
#endif
				}
			}
		}
	}
#ifdef DEBUG_FIRMWARE_UPGRADE
	Debug::trace("firmware.upgrade", HobString("SoftwareSystem:end"));
#endif
}


SoftwareSystem::SoftwareSystem(const SoftwareSystem& newSys)
{
	HobComplexVersion *pComponent, *pCopy;
	map<HobVersion::VersionType, HobComplexVersion *>::const_iterator i;

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	verIniFile_ = newSys.verIniFile_;

	softProps_ = new util_env::PropertiesTree(*newSys.softProps_);

	for (i = newSys.componentList_.begin(); i != newSys.componentList_.end(); i++)
	{
		pComponent = i->second;
		pCopy = new HobComplexVersion(*pComponent);

		componentList_.insert(make_pair(pCopy->getVersionType(), pCopy));
	}
}


SoftwareSystem::~SoftwareSystem()
{
	HobComplexVersion *pDel;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	for (i = componentList_.begin(); i != componentList_.end(); i++)
	{
		pDel = i->second;
		delete pDel;
	}

	componentList_.clear();

	if (softProps_)
		delete softProps_;
}


const string* SoftwareSystem::toString()
{
	HobComplexVersion *pComponent;
	map<HobVersion::VersionType, HobComplexVersion *>::const_iterator i;

	serialized_ = "Software components::";

	for (i = componentList_.begin(); i != componentList_.end(); i++)
	{
		pComponent = i->second;

		serialized_ += " [";
		serialized_ += *pComponent->toString();
		serialized_ += "] ";
	}

	return &serialized_;
}

HobComplexVersion * SoftwareSystem::getSoftwareVersionOfComponentType(HobVersion::VersionType checkType)
{
	HobComplexVersion *pComponent;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;

	i = componentList_.find(checkType);

	if (i != componentList_.end())
	{
		pComponent = i->second;

		return pComponent;
	}
	return NULL;
}

bool SoftwareSystem::doesAComponentNeedInstalling(HobVersion::VersionType checkType)
{
	HobComplexVersion *pComponent;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;

	i = componentList_.find(checkType);

	if (i != componentList_.end())
	{
		pComponent = i->second;

		if (pComponent->getIsInstalled())
		{
			Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesAComponentNeedInstalling():  Already installed. checkType = " + Integer::toString((int)checkType) + " - Returning FALSE.");
			return false;
		}
	}
	else
	{
		Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesAComponentNeedInstalling():  Not in list. checkType = " + Integer::toString((int)checkType) + " - Returning FALSE.");
		return false;
	}
	Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesAComponentNeedInstalling():  checkType = " + Integer::toString((int)checkType) + " - Returning TRUE.");
	return true;
}


bool SoftwareSystem::doesAnyComponentNeedInstalling()
{
	HobComplexVersion *pComponent;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;

	//checks to see if any component in this software system is yet uninstalled
	for (i = componentList_.begin(); i != componentList_.end(); i++)
	{
		pComponent = i->second;

		//Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesAnyComponentNeedInstalling():  versionType = " + Integer::toString((int)i->first));
		if (!pComponent)
		{
			Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesAnyComponentNeedInstalling():  pComponent is NULL!");
		}
		else if (!pComponent->getIsInstalled())
		{
			Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesAnyComponentNeedInstalling():  return true");
			return true;
		}
	}

	Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesAnyComponentNeedInstalling():  return false");
	return false;
}

HobDependency::DependencyResult SoftwareSystem::validateSoftwareSystem(VersionManager::ComponentList& hardwareVersions)
{
	HobComplexVersion *pComponentI, *pComponentJ;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i,j;
	VersionManager::ComponentListIterator k;
	HobDependency::DependencyResult check;
	HobVersion * pHardComponent;

	for (i = componentList_.begin(); i != componentList_.end(); i++)
	{
		pComponentI = i->second;

		for (k = hardwareVersions.begin(); k != hardwareVersions.end(); k++)
		{
			pHardComponent = k->second;
			check = pComponentI->dependencyCheck(const_cast<HobVersion *> (pHardComponent));
			if (check == HobDependency::DependencyFailed)
			{
				string msg ("validateSoftwareSystem:hardware dependency failure HW Ver=");

				msg += *pHardComponent->toString();

				string msgComplex ("ComplexVersion: ");

				msgComplex += *pComponentI->toString();

				Debug::trace("SoftwareSystem.upgrade", msg + msgComplex);
				return HobDependency::DependencyFailed;
			}
		}
	}

	for (i = componentList_.begin(); i != componentList_.end(); i++)
	{
		pComponentI = i->second;
		for (j = componentList_.begin(); j != componentList_.end(); j++)
		{
			pComponentJ = j->second;
//			if (pComponentI != pComponentJ)
//			{
				check = pComponentI->dependencyCheck(pComponentJ);
				if(check == HobDependency::DependencyFailed)
				{
					string msg ("validateSoftwareSystem:software dependency failure SW Ver=");

					msg += *pComponentJ->toString();
					msg += *pComponentI->toString();

					Debug::trace("SoftwareSystem.upgrade", msg);
					return HobDependency::DependencyFailed;
				}
//			}
		}
	}
	return HobDependency::DependencySucceeded;
}

HobDependency::DependencyResult SoftwareSystem::validateNewHardware(HobVersion *pNewHard)
{
	HobDependency::DependencyResult rv = HobDependency::DependencySucceeded;
	HobComplexVersion *pComponentI = NULL;

	map<HobVersion::VersionType, HobComplexVersion *>::iterator i = componentList_.begin();
	while (i != componentList_.end())
	{
		pComponentI = i->second;

		HobDependency::DependencyResult check = pComponentI->dependencyCheck(const_cast<HobVersion *> (pNewHard));
		if (check == HobDependency::DependencyFailed)
		{
			string msg ("validateSoftwareSystem:hardware dependency failure HW Ver=");
			msg += *pNewHard->toString();
			Debug::trace("SoftwareSystem.upgrade", msg);

			rv = HobDependency::DependencyFailed;
			return rv;
		}
		else
		{
			i++;
		}
	}
	return rv;
}

void SoftwareSystem::addAComponent(HobComplexVersion *pNewComponent)
{
	HobComplexVersion *pCurComponent;
	HobVersion::VersionType newVerType;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;

	newVerType = pNewComponent->getVersionType();
	i = componentList_.find(newVerType);

	if (i != componentList_.end())
	{
		pCurComponent = i->second;
		delete pCurComponent;
		componentList_.erase(newVerType);
	}

	componentList_.insert(make_pair(pNewComponent->getVersionType(), pNewComponent));
}

HobComplexVersion *SoftwareSystem::findMajorComponent()
{
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;

	if (componentList_.empty())
	{
		Debug::trace("VersionManager.WMSSH", "SoftwareSystem::findMajorComponent():  componentList_ is empty!");
		return 0;
	}

	i = componentList_.find(HobVersion::SystemControllerSW);

	if (i != componentList_.end())
	{
		Debug::trace("VersionManager.WMSSH", "SoftwareSystem::findMajorComponent():  Found " + Hex::toString(i->first) + ", " + Hex::toString((long long) i->second));
		return i->second;
	}
	else
	{
		i = componentList_.begin();

		Debug::trace("VersionManager.WMSSH", "SoftwareSystem::findMajorComponent():  Returning first in list:  " + Hex::toString(i->first) + ", " + Hex::toString((long long) i->second));
		return i->second;
	}
}

SoftwareSystem::PeripheralVersionCheck SoftwareSystem::doesUSBComponentNeedToBeLoaded(PeripheralRequestCodeUpdate *pUSBMsg,
		VersionManager::ComponentList &UNUSED(hardwareVersions), HobVersion::VersionType &newVerType)
{
	UNREFERENCED_PARAMETER(hardwareVersions);
	
	const HobVersion *softVer = pUSBMsg->getSoftwareVersion();
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;
	HobVersion *hardVer = const_cast<HobVersion *> (pUSBMsg->getHardwareVersion());

	Debug::trace("VersionManager.updatePeripheral", HobString("doesUSBComponentNeedToBeLoaded:software version ") + softVer->getRevisionString());

	if ((softVer == NULL) || softVer->isNULL())
	{
		//we have a case in which a usb peripheral does not have any software. we need to update it
		Debug::trace("VersionManager.updatePeripheral", HobString("doesUSBComponentNeedToBeLoaded:peripheral has no soft version"));

		newVerType = HobComplexVersion::getCorrespondingSoftTypeFromHardType(hardVer->getVersionType());

		if (validateNewHardware(hardVer) == HobDependency::DependencySucceeded)
		{
			i = componentList_.find(newVerType);

			if (i != componentList_.end())
			{
				Debug::trace("VersionManager.updatePeripheral", HobString("doesUSBComponentNeedToBeLoaded:send msg to upgrade peripheral with current software on CF"));

				return AvailableUpgrade;
			}
			else
			{
				Debug::trace("VersionManager.updatePeripheral", HobString("doesUSBComponentNeedToBeLoaded:Unkown software type for given hardware type"));

				return NotAvailable;
			}
		}
		else
		{
			Debug::trace("VersionManager.updatePeripheral", HobString("doesUSBComponentNeedToBeLoaded:invalid software system"));
			return AvailableInvalidHWAndSW;
		}
	}
	else
	{
		// we try to keep all usb devices running the SW version that is on CF in the firmware dir.
		newVerType = softVer->getVersionType();

		if (validateNewHardware(hardVer) == HobDependency::DependencySucceeded)	
		{
			i = componentList_.find(newVerType);

			if (i != componentList_.end())
			{
				HobComplexVersion *pCurComponent = i->second;

				if (*pCurComponent == *softVer)
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("doesUSBComponentNeedToBeLoaded:  The version on the usb peripheral is current for: ") + Integer::toString(newVerType));

					return AvailableSameVersion;
				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("doesUSBComponentNeedToBeLoaded:  The usb peripheral needs updating and after the update the system would be valid for: ") + Integer::toString(newVerType));

					return AvailableUpgrade;
				}
			}
			else
			{
				Debug::trace("SoftwareSystem.upgrade", HobString("doesUSBComponentNeedToBeLoaded:  Firmware is not available for: ") + Integer::toString(newVerType));

				return NotAvailable;
			}
		}
		else
		{
			Debug::trace("VersionManager.updatePeripheral", HobString("doesUSBComponentNeedToBeLoaded:  Invalid software system"));
			return AvailableInvalidHWAndSW;
		}
	}
	//	return NotAvailable;
}


bool SoftwareSystem::requestPeripheralUpgrade(PeripheralRequestCodeUpdate *pUSBMsg,
		VersionManager::ComponentList &hardwareVersions, PeripheralCodeUpdate **pReplyMsg)
{
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;
	HobVersion::VersionType newVerType;
	int unZipSingleFile_RetVal = 0;

	const std::string pPath = Configuration::getInstance()->toStringCfgI(CfgInternal::FIRMWAREDIRECTORY);				
	Debug::trace("SoftwareSystem.upgrade", "FIRMWAREDIRECTORY: " + pPath);


	PeripheralVersionCheck res = doesUSBComponentNeedToBeLoaded(pUSBMsg, hardwareVersions, newVerType);

	HobComplexVersion *pCurComponent = 0;

	Debug::trace("SoftwareSystem.upgrade", "handle request for component: " +  Integer::toString(newVerType));
	if (res != NotAvailable)
	{
		i = componentList_.find(newVerType);

		if (i != componentList_.end())
		{
			pCurComponent = i->second;
		}
	}
	else
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade: Not Available. Create response message with null component."));
		*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
	}

	if (!pPath.empty() && (res != NotAvailable))
	{
		switch (newVerType)
		{
			case HobVersion::ApcPrinterSW:
			{
				Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade: APC peripheral request."));

				if (pCurComponent)
				{
					util_filesystem::File apcSrcZip(pPath, "APC.zip");

					if (!apcSrcZip.exists())
						break;
					
					util_filesystem::Directory vmTmpDir("");
					HobComplexVersion::getExtractionDir(newVerType, vmTmpDir, 0);					
					if (!vmTmpDir.exists())
						vmTmpDir.create();

					Hobart::CZip::unZipSingleFile(apcSrcZip.getFullPath().c_str(), APC_ZIP_SOURCE_FILE, vmTmpDir.getFullPath().c_str());

					util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), APC_ZIP_SOURCE_FILE);

					if (firmSourceFile.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:found software component create response message."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, *pCurComponent);
					}
				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
					*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
				}
			}
			break;
			case HobVersion::FpcPrinterSW:
			{
				Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade: FPC peripheral request."));

				if (pCurComponent)
				{
					util_filesystem::File fpcSrcZip(pPath, "FPC.zip");

					if (!fpcSrcZip.exists())
						break;

					util_filesystem::Directory vmTmpDir("");
					HobComplexVersion::getExtractionDir(newVerType, vmTmpDir, 0);
					if (!vmTmpDir.exists())
						vmTmpDir.create();

					Hobart::CZip::unZipSingleFile(fpcSrcZip.getFullPath().c_str(), APC_ZIP_SOURCE_FILE, vmTmpDir.getFullPath().c_str());

					util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), APC_ZIP_SOURCE_FILE);

					if (firmSourceFile.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:found software component create response message."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, *pCurComponent);
					}
				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
					*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
				}
			}
			break;
			case HobVersion::EppPrnBootCodeSW:
			case HobVersion::EppPrnFlashUpdateSW:
			case HobVersion::EppPrinterSW: 
			{
				Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade: EppPrinterSW peripheral request."));
	 
				if (pCurComponent)
				{
					util_filesystem::File eppSrcZip(pPath, "");

					if(newVerType == HobVersion::EppPrnBootCodeSW)					
					eppSrcZip.setFileName("EPPBoot.zip");

					if(newVerType == HobVersion::EppPrnFlashUpdateSW)					
					eppSrcZip.setFileName("EPPFlashUpdate.zip");

					if(newVerType == HobVersion::EppPrinterSW)
					eppSrcZip.setFileName("EPPPrn.zip");
					

					if (!eppSrcZip.exists())
					{
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());					
						break;
					}
					
					util_filesystem::Directory vmTmpDir("");
					
					HobComplexVersion::getExtractionDir(newVerType, vmTmpDir, 0);
					
					if (!vmTmpDir.exists())
						vmTmpDir.create();

					unZipSingleFile_RetVal = Hobart::CZip::unZipSingleFile(eppSrcZip.getFullPath().c_str(), EPP_ZIP_SOURCE_FILE, vmTmpDir.getFullPath().c_str());
		Debug::trace("SoftwareSystem.upgrade", HobString("unZipSingleFile(") + eppSrcZip.getFullPath().c_str() + HobString(", ") + EPP_ZIP_SOURCE_FILE + HobString(", ") + vmTmpDir.getFullPath().c_str() + HobString("):  Returns ") + Integer::toString(unZipSingleFile_RetVal));

					util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), EPP_ZIP_SOURCE_FILE);

					if (firmSourceFile.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:found software component create response message."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, *pCurComponent);
					}
					else
					{
						//if the source does not exist, we still need to responed to the updaters request
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
					}
				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
					*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
				}
			}
			break;
			/* Alex Zavala's code for XPE version */

			/*case HobVersion::Epp2PrnBootCodeSW:
			case HobVersion::Epp2PrnFlashUpdateSW:
			case HobVersion::Epp2PrinterSW: 
			{
				Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade: Epp2PrinterSW peripheral request."));
	 
				if (pCurComponent)
				{
					util_filesystem::File epp2SrcZip(*pPath, "");

					if(newVerType == HobVersion::Epp2PrnBootCodeSW)					
					epp2SrcZip.setFileName("EPP2Boot.zip");

					if(newVerType == HobVersion::Epp2PrnFlashUpdateSW)					
					epp2SrcZip.setFileName("EPP2FlashUpdate.zip");

					if(newVerType == HobVersion::Epp2PrinterSW)
					epp2SrcZip.setFileName("EPP2Prn.zip");
					

					if (!epp2SrcZip.exists())
					{
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());					
						break;
					}

					util_filesystem::Directory vmTmpDir("");
					
					HobComplexVersion::getExtractionDir(newVerType, vmTmpDir, 0);
					
					if (!vmTmpDir.exists())
						vmTmpDir.create();

					Hobart::CZip::unZipSingleFile(epp2SrcZip.getFullPath().c_str(), EPP2_ZIP_SOURCE_FILE, vmTmpDir.getFullPath().c_str());

					util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), EPP2_ZIP_SOURCE_FILE);

					if (firmSourceFile.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:found software component create response message."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, *pCurComponent);
					}
					else
					{
						//if the source does not exist, we still need to responed to the updaters request
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
					}
				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
					*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
				}
			}
			break;*/
			case HobVersion::CLAAppSW:
			case HobVersion::CLADataSW:
			case HobVersion::CLABootSW:
			{				
				if (pCurComponent)
				{
					util_filesystem::File claSrcZip(pPath, "");
										
					if(newVerType == HobVersion::CLAAppSW)
						claSrcZip.setFileName("CLAApp.zip");
					if(newVerType == HobVersion::CLADataSW)
						claSrcZip.setFileName("CLAData.zip");
					if(newVerType == HobVersion::CLABootSW)
						claSrcZip.setFileName("CLABoot.zip");

					if (!claSrcZip.exists())
					{
						Debug::trace("SoftwareSystem.NoFile", HobString("requestPeripheralUpgrade: No Source Zip: create response message with null component."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res,  HobComplexVersion());

						const HobVersion *softVer = pUSBMsg->getSoftwareVersion();
						HobVersion myVersion;
						myVersion.setVersionType(softVer->getVersionType());
						myVersion.setMajorRevision(softVer->getMajorRevision());
						myVersion.setMinorRevision(softVer->getMinorRevision());
						Debug::trace("SoftwareSystem.NoFile", "No Zip Source File: " + Integer::toString(myVersion.getVersionType()));
						(*pReplyMsg)->setCurrentSoftwareVersion(myVersion);
						(*pReplyMsg)->setResponseType(NotAvailable);
						Debug::trace("SoftwareSystem.NoFile", "No Zip Source File");
						break;
					}

					util_filesystem::Directory vmTmpDir("");
					
					HobComplexVersion::getExtractionDir(newVerType, vmTmpDir, 0);
					
					if (!vmTmpDir.exists())
						vmTmpDir.create();


					Hobart::CZip::unZipSingleFile(claSrcZip.getFullPath().c_str(), CLA_ZIP_SOURCE_FILE, vmTmpDir.getFullPath().c_str());

					util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), CLA_ZIP_SOURCE_FILE);

					if (firmSourceFile.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:found software component create response message for: ") + Integer::toString(newVerType));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, *pCurComponent);
					}
					else
					{
						//if the source does not exist, we still need to respond to the updaters request
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component. ")  + Integer::toString(newVerType));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
					}

				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
					*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
				}					

				if (*pReplyMsg)
				{
					const HobVersion *softVer = pUSBMsg->getSoftwareVersion();
					HobVersion myVersion;
					myVersion.setVersionType(softVer->getVersionType());
					myVersion.setMajorRevision(softVer->getMajorRevision());
					myVersion.setMinorRevision(softVer->getMinorRevision());
					(*pReplyMsg)->setCurrentSoftwareVersion(myVersion);
				}
			}
			break;
			case HobVersion::UsbWeigher15KgSW:
			{
				Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade: UsbWeigher15KgSW peripheral request."));

				if (pCurComponent)
				{
					util_filesystem::File ezWeigherSrcZip(pPath, "weigher.zip");

					if (!ezWeigherSrcZip.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade: " + ezWeigherSrcZip.getFullPath() + " does NOT exist."));
						break;
					}
					
					util_filesystem::Directory vmTmpDir("");
					HobComplexVersion::getExtractionDir(newVerType, vmTmpDir, 0);
					
					if (!vmTmpDir.exists())
						vmTmpDir.create();

					unZipSingleFile_RetVal = Hobart::CZip::unZipSingleFile(ezWeigherSrcZip.getFullPath().c_str(), EZWEIGHER_ZIP_SOURCE_FILE, vmTmpDir.getFullPath().c_str());
		Debug::trace("SoftwareSystem.upgrade", HobString("unZipSingleFile(") + ezWeigherSrcZip.getFullPath().c_str() + HobString(", ") + EZWEIGHER_ZIP_SOURCE_FILE + HobString(", ") + vmTmpDir.getFullPath().c_str() + HobString("):  Returns ") + Integer::toString(unZipSingleFile_RetVal));
					if (unZipSingleFile_RetVal == -2)
						Debug::trace("SoftwareSystem.upgrade", HobString("unZipSingleFile_RetVal = -2"));
					else if (unZipSingleFile_RetVal == 1)
						Debug::trace("SoftwareSystem.upgrade", HobString("unZipSingleFile_RetVal = 1"));
					else if (unZipSingleFile_RetVal == -1)
						Debug::trace("SoftwareSystem.upgrade", HobString("unZipSingleFile_RetVal = -1"));
					else if (unZipSingleFile_RetVal == 0)
						Debug::trace("SoftwareSystem.upgrade", HobString("unZipSingleFile_RetVal = 0"));

					util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), EZWEIGHER_ZIP_SOURCE_FILE);

					if (firmSourceFile.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:found software component create response message."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, *pCurComponent);
					}
					else
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:source file NOT found."));
						//TO DO -- if this occurs, new error dialog box needed here!
					}
				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
					*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
				}
			}
			break;
			case HobVersion::Epp2PrinterSW:
			{
				Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade: Epp2PrinterSW peripheral request."));

				if (pCurComponent)
				{
					util_filesystem::File armEppSrcZip(pPath, "EPP2Prn.zip");

					if (!armEppSrcZip.exists())
						break;

					util_filesystem::Directory vmTmpDir("");
					
					HobComplexVersion::getExtractionDir(newVerType, vmTmpDir, 0);
					
					if (!vmTmpDir.exists())
						vmTmpDir.create();

					Hobart::CZip::unZipSingleFile(armEppSrcZip.getFullPath().c_str(), EPP2_ZIP_SOURCE_FILE, vmTmpDir.getFullPath().c_str());

					util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), EPP2_ZIP_SOURCE_FILE);

					if (firmSourceFile.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:found software component create response message."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, *pCurComponent);
					}
					else
					{
						//if the source does not exist, we still need to responed to the updaters request
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
					}
				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
					*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
				}		
			}
			break;
			case HobVersion::Aws2WeigherSW:
			{
				Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade: Aws2WeigherSW peripheral request."));

				if (pCurComponent)
				{
					util_filesystem::File armWeigherSrcZip(pPath, "XPEWeigher.bin");

					if (!armWeigherSrcZip.exists())
						break;

					util_filesystem::Directory vmTmpDir("");
					
					HobComplexVersion::getExtractionDir(newVerType, vmTmpDir, 0);
					
					if (!vmTmpDir.exists())
						vmTmpDir.create();

					Hobart::CZip::unZipSingleFile(armWeigherSrcZip.getFullPath().c_str(), ARMWEIGHER_ZIP_SOURCE_FILE, vmTmpDir.getFullPath().c_str());

					util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), ARMWEIGHER_ZIP_SOURCE_FILE);

					if (firmSourceFile.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:found software component create response message."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, *pCurComponent);
					}
					else
					{
						//if the source does not exist, we still need to responed to the updaters request
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
					}

				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
					*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
				}					
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
			{				
				if (pCurComponent)
				{
					util_filesystem::File wrapperSrcZip(pPath, "");
					if(newVerType == HobVersion::WRMasterNVDataSW)
						wrapperSrcZip.setFileName("wrMasterData.zip");
					if(newVerType == HobVersion::WREMasterNVDataSW)
						wrapperSrcZip.setFileName("wreMasterData.zip");
					
					if(newVerType == HobVersion::WRMasterBootSW)
						wrapperSrcZip.setFileName("wrMasterBoot.zip");
					if(newVerType == HobVersion::WREMasterBootSW)
						wrapperSrcZip.setFileName("wreMasterBoot.zip");
						
					if(newVerType == HobVersion::WRMasterAppSW)
						wrapperSrcZip.setFileName("wrMasterApp.zip");
					if(newVerType == HobVersion::WREMasterAppSW)
						wrapperSrcZip.setFileName("wreMasterApp.zip");

					if(newVerType == HobVersion::WRSlave1NVDataSW)
						wrapperSrcZip.setFileName("wrSlave1Data.zip");
					if(newVerType == HobVersion::WRESlave1NVDataSW)
						wrapperSrcZip.setFileName("wreSlave1Data.zip");
		
					if(newVerType == HobVersion::WRSlave1BootSW)
						wrapperSrcZip.setFileName("wrSlave1Boot.zip");
					if(newVerType == HobVersion::WRESlave1BootSW)
						wrapperSrcZip.setFileName("wreSlave1Boot.zip");

					if(newVerType == HobVersion::WRSlave1AppSW)
						wrapperSrcZip.setFileName("wrSlave1App.zip");				
					if(newVerType == HobVersion::WRESlave1AppSW)
						wrapperSrcZip.setFileName("wreSlave1App.zip");				

					if(newVerType == HobVersion::WRSlave3NVDataSW)
						wrapperSrcZip.setFileName("wrSlave3Data.zip");				
					if(newVerType == HobVersion::WRESlave3NVDataSW)
						wrapperSrcZip.setFileName("wreSlave3Data.zip");				

					if(newVerType == HobVersion::WRSlave3BootSW)
						wrapperSrcZip.setFileName("wrSlave3Boot.zip");				
					if(newVerType == HobVersion::WRESlave3BootSW)
						wrapperSrcZip.setFileName("wreSlave3Boot.zip");				
					
					if(newVerType == HobVersion::WRSlave3AppSW)
						wrapperSrcZip.setFileName("wrSlave3App.zip");				
					if(newVerType == HobVersion::WRESlave3AppSW)
						wrapperSrcZip.setFileName("wreSlave3App.zip");				

					if(newVerType == HobVersion::WRSlave4NVDataSW)
						wrapperSrcZip.setFileName("wrSlave4Data.zip");				
					if(newVerType == HobVersion::WRESlave4NVDataSW)
						wrapperSrcZip.setFileName("wreSlave4Data.zip");				

					if(newVerType == HobVersion::WRSlave4BootSW)
						wrapperSrcZip.setFileName("wrSlave4Boot.zip");				
					if(newVerType == HobVersion::WRESlave4BootSW)
						wrapperSrcZip.setFileName("wreSlave4Boot.zip");				

					if(newVerType == HobVersion::WRSlave4AppSW)
						wrapperSrcZip.setFileName("wrSlave4App.zip");									
					if(newVerType == HobVersion::WRESlave4AppSW)
						wrapperSrcZip.setFileName("wreSlave4App.zip");									

					if(newVerType == HobVersion::WRSlave5NVDataSW)
						wrapperSrcZip.setFileName("wrSlave5Data.zip");									
					if(newVerType == HobVersion::WRESlave5NVDataSW)
						wrapperSrcZip.setFileName("wreSlave5Data.zip");									

					if(newVerType == HobVersion::WRSlave5BootSW)
						wrapperSrcZip.setFileName("wrSlave5Boot.zip");								
					if(newVerType == HobVersion::WRESlave5BootSW)
						wrapperSrcZip.setFileName("wreSlave5Boot.zip");								

					if(newVerType == HobVersion::WRSlave5AppSW)
						wrapperSrcZip.setFileName("wrSlave5App.zip");									
					if(newVerType == HobVersion::WRESlave5AppSW)
						wrapperSrcZip.setFileName("wreSlave5App.zip");									

					if (!wrapperSrcZip.exists())
					{
						Debug::trace("SoftwareSystem.NoFile", HobString("requestPeripheralUpgrade: No Source Zip: create response message with null component."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res,  HobComplexVersion());

						const HobVersion *softVer = pUSBMsg->getSoftwareVersion();
						HobVersion myVersion;
						myVersion.setVersionType(softVer->getVersionType());
						myVersion.setMajorRevision(softVer->getMajorRevision());
						myVersion.setMinorRevision(softVer->getMinorRevision());
						Debug::trace("SoftwareSystem.NoFile", "No Zip Source File: " + Integer::toString(myVersion.getVersionType()));
						(*pReplyMsg)->setCurrentSoftwareVersion(myVersion);
						(*pReplyMsg)->setResponseType(NotAvailable);
						Debug::trace("SoftwareSystem.NoFile", "No Zip Source File");
						break;
					}

					util_filesystem::Directory vmTmpDir("");
					
					HobComplexVersion::getExtractionDir(newVerType, vmTmpDir, 0);
					
					if (!vmTmpDir.exists())
						vmTmpDir.create();


					Hobart::CZip::unZipSingleFile(wrapperSrcZip.getFullPath().c_str(), AWS_WRAPPER_ZIP_SOURCE_FILES, vmTmpDir.getFullPath().c_str());

					util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), AWS_WRAPPER_ZIP_SOURCE_FILES);

					if (firmSourceFile.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:found software component create response message for: ") + Integer::toString(newVerType));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, *pCurComponent);
					}
					else
					{
						//if the source does not exist, we still need to responed to the updaters request
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component. ")  + Integer::toString(newVerType));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
					}

				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
					*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
				}					

				if (*pReplyMsg)
				{
					const HobVersion *softVer = pUSBMsg->getSoftwareVersion();
					HobVersion myVersion;
					myVersion.setVersionType(softVer->getVersionType());
					myVersion.setMajorRevision(softVer->getMajorRevision());
					myVersion.setMinorRevision(softVer->getMinorRevision());
					(*pReplyMsg)->setCurrentSoftwareVersion(myVersion);
				}
			}
			break;
			case HobVersion::GCDAppSW:
			{
				Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade: GCDManagerSW peripheral request."));

				if (pCurComponent)
				{
					util_filesystem::File srcZip(pPath, "GCD.zip");

					if (!srcZip.exists())
						break;

					util_filesystem::Directory vmTmpDir("");

					HobComplexVersion::getExtractionDir(newVerType, vmTmpDir, 0);

					if (!vmTmpDir.exists())
						vmTmpDir.create();

					Hobart::CZip::unZipSingleFile(srcZip.getFullPath().c_str(), GCDMANAGER_ZIP_SOURCE_FILES, vmTmpDir.getFullPath().c_str());

					util_filesystem::File firmSourceFile (vmTmpDir.getFullPath(), GCDMANAGER_ZIP_SOURCE_FILES);

					if (firmSourceFile.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:found software component create response message."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, *pCurComponent);
					}
					else
					{
						//if the source does not exist, we still need to responed to the updaters request
						Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
						*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
					}

				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:create response message with null component."));
					*pReplyMsg = new PeripheralCodeUpdate(pUSBMsg->getNodeId(), res, HobComplexVersion());
				}					
			}
			break;
			default:
				break;
		}
	}
	else
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("requestPeripheralUpgrade:pPath is NULL."));
	}

	return false;
}

STATUS SoftwareSystem::extractPeripheralTar(USBD_NODE_ID nodeId, HobVersion::VersionType verType)
{
    Configuration* pCFG = Configuration::getInstance();
	string srcPath("");
	util_filesystem::Directory tmpPeripheralDir("");
	HobComplexVersion::getExtractionDir(verType, tmpPeripheralDir, nodeId);
	util_filesystem::File tmpPeripheralTar(tmpPeripheralDir.getFullPath(), "SW.TAR");
	util_filesystem::File tmpPeripheralIni(tmpPeripheralDir.getFullPath(), pCFG->toStringCfgI(CfgInternal::VERSIONINFOFILENAME));

	HobComplexVersion::getSoftPath(verType, srcPath);
	util_filesystem::File srcPeripheralTar(srcPath);

	if (tmpPeripheralDir.exists())
		tmpPeripheralDir.remove(true);

	tmpPeripheralDir.create();
	Debug::trace("VersionManager.upgrade", HobString("SoftwareSystem::extractPeripheralTar():  Created ") + tmpPeripheralDir.getFullPath());

	string msg("extractPeripheralTar:copying usb peripheral software to path = ");
	msg += tmpPeripheralTar.getFullPath();

	Debug::trace("SoftwareSystem.upgrade", msg);

	tmpPeripheralTar << srcPeripheralTar;

	if (!tmpPeripheralTar.exists())
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("extractPeripheralTar:failed to copy peripheral tar"));

		return ERROR_CODE;
	}

	util_filesystem::File peripheral_tar(  tmpPeripheralTar.getFullPath().c_str() );
	if (!peripheral_tar.exists())
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("extractPeripheralTar:peripheral tar archive is BAD"));

		return ERROR_CODE;
	}
	else
		Debug::trace("SoftwareSystem.upgrade", HobString("extractPeripheralTar:peripheral tar archive is OK"));

	Debug::trace("SoftwareSystem.upgrade", HobString("extractPeripheralTar:extracting peripheral.tar"));

	Hobart::CZip::unZip(tmpPeripheralTar.getFullPath().c_str(), tmpPeripheralDir.getFullPath().c_str());

	if (!tmpPeripheralIni.exists())
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("extractPeripheralTar:version.ini failed to extract"));

		return ERROR_CODE;
	}
	else
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("extractPeripheralTar:removing extra files"));

		tmpPeripheralIni.remove();
		tmpPeripheralTar.remove();

	}

	return OK;
}

void SoftwareSystem::addSystem(SoftwareSystem *pNewSystem)
{
	HobComplexVersion *pNewComponent;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i, j;
	HobVersion::VersionType newVerType;

	//takes and creates copies of all HobComplex versions contained in pNewSystem. And then adds them to the current system.

	//does not add in componenets if they already exist in this system.
	for (i = pNewSystem->componentList_.begin(); i != pNewSystem->componentList_.end(); i++)
	{

		pNewComponent = i->second;
		newVerType = pNewComponent->getVersionType();

		j = componentList_.find(newVerType);
		if (j == componentList_.end())
		{
			componentList_.insert(make_pair(newVerType, new HobComplexVersion(*pNewComponent)));
		}
	}
}


void SoftwareSystem::mergeSystems(SoftwareSystem *pNewSystem, bool onlyNewerVersions, bool bForce)
{
	HobComplexVersion *pCurComponent, *pNewComponent;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i, j;
	HobVersion::VersionType newVerType;

	//This takes a list of uninstalled HobComplexVersions in pNewSystem and checks to see if they need to be installed.
	//if the HobComplex version does not exist in this system, then a copy of it is added to this system.
	//if the HobComplexVersion exists then a check to see if the versions differ. If they differ, then
	//a copy is made. This copy is added to the current system. The old (installed version) HobComplexVersion in the 
	//current system is removed. This leaves us with only the uninstalled HobComplexVersion in the current system.

	Debug::trace("VersionManager.WMSSH", HobString("SoftwareSystem::mergeSystems():  Begin."));
	for (i = pNewSystem->componentList_.begin(); i != pNewSystem->componentList_.end(); i++)
	{
		pNewComponent = i->second;
		newVerType = pNewComponent->getVersionType();

		j = componentList_.find(newVerType);
		if (j == componentList_.end())
		{
			componentList_.insert(make_pair(newVerType, new HobComplexVersion(*pNewComponent)));
		}
		else
		{
			pCurComponent = j->second;

			//depending upon the selection. an uninstalled component will be added to the resultant system if one of the two
			//conditions is met..
			// if onlyNewerVersions is set and the uninstalled component is newer (a larger version number)
			// if onlyNewerVersions is not set and the uininstalled component is different (either newer or older)
			if ( (bForce && pNewComponent->isUpgradeForcible())
					|| ((onlyNewerVersions) && (*pNewComponent > *pCurComponent))
					|| ((!onlyNewerVersions) && (*pNewComponent != *pCurComponent)) )
			{
				delete pCurComponent;
				componentList_.erase(newVerType);

				componentList_.insert(make_pair(newVerType, new HobComplexVersion(*pNewComponent)));
			}
		}
	}
	Debug::trace("VersionManager.WMSSH", HobString("SoftwareSystem::mergeSystems():  Done."));
}


bool SoftwareSystem::doesDBNeedBackingUp()
{
	//if the db exists uninstalled in this software system, then we need to backup the db.
	//this allows the db to be backed up whether or not we install new SC software.
	if (doesAComponentNeedInstalling(HobVersion::DB))
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("doesDBNeedBackingUp:DB needs to be backed up"));
		return true;
	}
	else
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("doesDBNeedBackingUp:DB does not need to be backed up"));
		return false;
	}
}

bool SoftwareSystem::doesSystemNeedRebooting()
{
	//if the system controller needs installing then a reboot is necessary
	if (doesAComponentNeedInstalling(HobVersion::SystemControllerSW))
	{
		Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesSystemNeedRebooting():  Returning TRUE.");
		return true;
	}

	//if the db needs backing up then we need to reboot.
	//this is for the case where we have a new db schema but not a new system controller sw.
	if (doesAComponentNeedInstalling(HobVersion::DB))
	{
		Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesSystemNeedRebooting():  Returning TRUE.");
		return true;
	}

	//if new bios software needs installing then we must reboot system.
	if (doesAComponentNeedInstalling(HobVersion::BIOS))
	{
		Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesSystemNeedRebooting():  Returning TRUE.");
		return true;
	}

#ifdef APC_PERIPHERAL_UPDATE_REQUIRES_REBOOT
	if (doesAComponentNeedInstalling(HobVersion::ApcPrinterSW))
	{
		Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesSystemNeedRebooting():  Returning TRUE.");
		return true;
	}
#endif

	Debug::trace("VersionManager.WMSSH", "SoftwareSystem::doesSystemNeedRebooting():  Returning FALSE.");
	return false;
}

STATUS SoftwareSystem::flashSystem(VersionManager::ComponentList * /* pHardwareMap */)
{
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;
	HobComplexVersion *pComponent = NULL;
	HobVersion::VersionType verType = HobVersion::UnknownComponent;
	bool genNewPropsFile = false;

	for (i = componentList_.begin(); i != componentList_.end(); i++)
	{
		pComponent = i->second;
		verType = pComponent->getVersionType();

#ifdef DEBUG_FIRMWARE_UPGRADE
		string msg("does component (");
		msg += *pComponent->toString();
		msg += ") need to be installed (";
		msg += Bool::toString(!pComponent->getIsInstalled());
		msg += ")";

		Debug::trace("SoftwareSystem.upgrade", msg);
#endif
		switch (verType)
		{
			case HobVersion::SystemControllerSW:
			{
				util_filesystem::Directory tmpDir("");
				util_filesystem::File tmpZip ("");
				HobComplexVersion::getTempDir(verType, tmpDir, tmpZip);

				if (!pComponent->getIsInstalled())
				{ //system must be rebooted before software is actually installed.

                	Debug::trace("SoftwareSystem.upgrade", HobString("flashSystem:removing sc zip file path:") + tmpZip.getFullPath());

                    tmpZip.remove();

                    PAL_absclass::PAL_sleep( 20000 );

					logEntryInAppInstallerLog(pComponent);
                        logEntryInInstallationLog(pComponent);

					genNewPropsFile = true;
				}
				else
				{
					Debug::trace("SoftwareSystem.upgrade", HobString("flashSystem:SC already installed remove tmp dir if exists:") + tmpDir.getFullPath());
					if (tmpDir.exists())
						tmpDir.remove(true);
				}
			}
			break;
			case HobVersion::BIOS:
				if (!pComponent->getIsInstalled())
				{
					logEntryInInstallationLog(pComponent);

					genNewPropsFile = true;
				}
				break;
			case HobVersion::DB:
				if (!pComponent->getIsInstalled())
				{
					logEntryInInstallationLog(pComponent);

					genNewPropsFile = true;
				}
				break;

			case HobVersion::UsbWeigher15KgSW:
			case HobVersion::EppPrinterSW:
			case HobVersion::Epp2PrinterSW:			/* Alex Zavala's change for XPE version */
			case HobVersion::EssPrinterSW:
			case HobVersion::EscPrinterSW:
			case HobVersion::EssPrnBootCodeSW:
			case HobVersion::EssPrnFlashUpdateSW:
			case HobVersion::EppPrnBootCodeSW:
			case HobVersion::EppPrnFlashUpdateSW:
			case HobVersion::Epp2PrnBootCodeSW:		/* Alex Zavala's change for XPE version */
			case HobVersion::Epp2PrnFlashUpdateSW:	/* Alex Zavala's change for XPE version */
			case HobVersion::EscPrnBootCodeSW:
			case HobVersion::EscPrnFlashUpdateSW:
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
			case HobVersion::ApcPrinterSW:
			case HobVersion::FpcPrinterSW:
			case HobVersion::GCDAppSW:
			case HobVersion::CLAAppSW:
			case HobVersion::CLADataSW:
			case HobVersion::CLABootSW:
			{
				util_filesystem::Directory srcDir("");
				util_filesystem::File fSrcZip ("");
				HobComplexVersion::getTempDir(verType, srcDir, fSrcZip);

				if (!pComponent->getIsInstalled())
				{
					//install a usb peripheral
					Debug::trace("SoftwareSystem.upgrade", HobString("flashSystem:installing new peripheral software onto CF"));
					string tmp("");
					HobComplexVersion::getSoftPath(verType, tmp);
					util_filesystem::File fDestZip(tmp);
					util_filesystem::Directory fDestDir(fDestZip.getPath());

					if (!fDestDir.exists())
					{
						fDestDir.create();
						Debug::trace("SoftwareSystem.upgrade", HobString("SoftwareSystem::flashSystem():  Created ") + fDestDir.getFullPath());
					}
					else
					{
						if (fDestZip.exists())
							fDestZip.remove();
					}

					if(!fSrcZip.exists())
					{
						Debug::trace("SoftwareSystem.upgrade", HobString("srcZip: ") + " DOES NOT EXIST");
					}

					Debug::trace("SoftwareSystem.upgrade", HobString("Moving srcZip: ") +  fSrcZip.getFullPath() + " to destZip: " + fDestZip.getFullPath() );
					fDestZip << fSrcZip; //copy zip into firmware library for usb peripherals.

					if(!fDestZip.exists())
					{
						Debug::trace("SoftwareSystem.upgrade",HobString("fDestZip: ") + fDestZip.getFullPath() + " DOES NOT EXIST AFTER MOVE");
					}

					logEntryInInstallationLog(pComponent);

					genNewPropsFile = true;

#ifdef APC_PERIPHERAL_UPDATE_REQUIRES_REBOOT
					if (verType == HobVersion::ApcPrinterSW)
						logEntryInAppInstallerLog(pComponent);
#endif
				}
				srcDir.remove(true);
			}
			break;

			default:
			{
                    // Nothing to do
				break;
			}
		}
	}

	Debug::trace("VersionManager.WMSSH", HobString("SoftwareSystem::flashSystem():  genNewPropsFile = ") + Bool::toString(genNewPropsFile));
	if (genNewPropsFile)
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("flashSystem:update the evolution.ini on CF to reflect new software system"));
		savePropertiesToFile();
	}

	return OK;
}

void SoftwareSystem::logEntryInAppInstallerLog(HobComplexVersion *pComponent)
{
	Directory tmpDir ("");
    tmpDir.appendPath(Configuration::getInstance()->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR));

	if (!tmpDir.exists())
	{
		tmpDir.create();
		Debug::trace("VersionManager.upgrade", HobString("SoftwareSystem::v():  Created ") + tmpDir.getFullPath());
	}

    File vmAppInstLog(Configuration::getInstance()->toStringCfgI(CfgInternal::VERSIONMANAGERTEMPDIR), "appInstaller.log");

	ofstream out(vmAppInstLog.getFullPath().c_str(), ios::app);

	if (!out)
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("logEntryInAppInstallerLog:failed to open appInstaller log file"));
	}
	else
	{
		out << pComponent->getRevisionString() << "\r\n";
	}

	out.close();

	Debug::trace("SoftwareSystem.upgrade", HobString("logEntryInAppInstallerLog:end"));
}


void SoftwareSystem::logEntryInInstallationLog(HobComplexVersion *pComponent)
{
    ofstream out(Configuration::getInstance()->toStringCfgI(CfgInternal::INSTALLOGFILE).c_str(), ios::app);

	if (!out)
	{
		Debug::trace("SoftwareSystem.upgrade", HobString("logEntryInInstallationLog:failed to open installation log file"));
	}
	else
	{
		HobDateTime hdt;
		HobString installDate = hdt.formatDate(FormatDate_MMDDYYYY);
		HobString installTime = hdt.formatTime(FormatTime_HHMMSS, HourFormat_AMPM);
		string typeDescriptor;

		pComponent->getTypeDescriptor(typeDescriptor);

		out << installDate << ";" << installTime << ";" << pComponent->getRevisionString() << ";" << typeDescriptor << "\r\n";
	}

	out.close();

	Debug::trace("SoftwareSystem.upgrade", HobString("logEntryInInstallationLog:end"));
}

void SoftwareSystem::savePropertiesToFile(bool clearProperties)
{
	HobComplexVersion *pComponent;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;
	const string* strtmp;

	Debug::trace("SoftwareSystem.upgrade", HobString("savePropertiesToFile:start"));

	if (softProps_ != NULL)
	{
		if (clearProperties)
		{
			Debug::trace("SoftwareSystem", HobString("removing properties file first. path = ") += verIniFile_.getFullPath());
			verIniFile_.remove();

			delete softProps_;
			softProps_ = new util_env::PropertiesTree(verIniFile_.getFullPath());
		}

		i = componentList_.begin();

		while (i != componentList_.end())
		{
			pComponent = i->second;
			strtmp = pComponent->toString();

			Debug::trace("SoftwareSystem.upgrade", HobString(*strtmp));

            pComponent->saveToPropertiesFile(softProps_);
			i++;
		}

		softProps_->save();
	}
	Debug::trace("SoftwareSystem.upgrade", HobString("savePropertiesToFile:end"));
}

bool SoftwareSystem::removeAllWrapperSWComponents()
{
	HobComplexVersion *pComponent;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;
	bool somethingWasRemoved = false;
	bool traceOn = Debug::isTracing("SoftwareSystem");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "removeAllWrapperSWComponents()";
		Debug::trace("SoftwareSystem", msgDebug);
		msgDebug.append(":  ");
	}

	//checks to see if any component in this software system is yet uninstalled
	for (i = componentList_.begin(); i != componentList_.end();)
	{
		HobVersion::VersionType searchVT;

		pComponent = i->second;
		searchVT = pComponent->getVersionType();

		switch (searchVT)
		{
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
			case HobVersion::GCDAppSW:
			{
				string softwarePath;

				delete pComponent;

				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("Erasing # ").append(Integer::toString(searchVT));
					msgDebug2.append("  Count before = ").append(Integer::toString((int)componentList_.size()));
					Debug::trace("SoftwareSystem", msgDebug2);
				}
				componentList_.erase(i++);
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("Count after = ").append(Integer::toString((int)componentList_.size()));
					Debug::trace("SoftwareSystem", msgDebug2);
				}

				HobComplexVersion::getSoftPath(searchVT, softwarePath);
				File fSoftpath(softwarePath);
				if (fSoftpath.exists())
				{
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("Removing wrapper tar file: ").append(fSoftpath.getFullPath());
						Debug::trace("SoftwareSystem", msgDebug2);
					}
					fSoftpath.remove();
					somethingWasRemoved = true;
					if (traceOn)
						Debug::trace("SoftwareSystem", msgDebug + "File was removed.");
				}
				else if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("File: ").append(fSoftpath.getFullPath()).append(" does not exist on the CF.");
					Debug::trace("SoftwareSystem", msgDebug2);
				}
				break;
			}
			default:
			{
				++i;
				break;
			}
		}
	}
	return somethingWasRemoved;
}
//Added 9/24 Alex Zavala
bool SoftwareSystem::removeAllCLASWComponents()
{
	HobComplexVersion *pComponent;
	map<HobVersion::VersionType, HobComplexVersion *>::iterator i;
	bool somethingWasRemoved = false;
	bool traceOn = Debug::isTracing("SoftwareSystem");
	HobString msgDebug;
	if (traceOn)
	{
		msgDebug = "removeAllCLASWComponents()";
		Debug::trace("SoftwareSystem", msgDebug);
		msgDebug.append(":  ");
	}

	//checks to see if any component in this software system is yet uninstalled
	for (i = componentList_.begin(); i != componentList_.end();)
	{
		HobVersion::VersionType searchVT;

		pComponent = i->second;
		searchVT = pComponent->getVersionType();

		switch (searchVT)
		{
			case HobVersion::CLAAppSW:
			case HobVersion::CLADataSW:
			case HobVersion::CLABootSW:
			{
				string softwarePath;

				delete pComponent;

				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("Erasing # ").append(Integer::toString(searchVT));
					msgDebug2.append("  Count before = ").append(Integer::toString((int)componentList_.size()));
					Debug::trace("SoftwareSystem", msgDebug2);
				}
				componentList_.erase(i++);
				if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("Count after = ").append(Integer::toString((int)componentList_.size()));
					Debug::trace("SoftwareSystem", msgDebug2);
				}

				HobComplexVersion::getSoftPath(searchVT, softwarePath);
				File fSoftpath(softwarePath);
				if (fSoftpath.exists())
				{
					if (traceOn)
					{
						HobString msgDebug2 = msgDebug;
						msgDebug2.append("Removing CLA tar file: ").append(fSoftpath.getFullPath());
						Debug::trace("SoftwareSystem", msgDebug2);
					}
					fSoftpath.remove();
					somethingWasRemoved = true;
					if (traceOn)
						Debug::trace("SoftwareSystem", msgDebug + "File was removed.");
				}
				else if (traceOn)
				{
					HobString msgDebug2 = msgDebug;
					msgDebug2.append("File: ").append(fSoftpath.getFullPath()).append(" does not exist on the CF.");
					Debug::trace("SoftwareSystem", msgDebug2);
				}
				break;
			}
			default:
			{
				++i;
				break;
			}
		}
	}
	return somethingWasRemoved;
}
