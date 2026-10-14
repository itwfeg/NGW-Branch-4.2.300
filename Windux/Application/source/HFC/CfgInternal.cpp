/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "CfgInternal.h"
#include "Configuration.h"
#include "properties/properties.h"
#include "HobTokenizerCSV.h"
#include "filesys/filename.h"
#include "filesys/file.h"
#include "PrinterStockMetrics.h"
#include "SessionManagerHelper.h"

#include "Debug.h"

#include "Integer.h"
#include "Bool.h"

#include "util/e_exception/exception.h"
#include "util/e_string/utilstring.h"

#include "ServiceSpeaker.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
	int CfgInternal::_dataType_ = 0;
	std::string CfgInternal::_dataTypeName_("CfgInternal");
#endif


using namespace std;
using namespace util_string;



CfgInternal::CfgInternal()
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}


CfgInternal::CfgInternal(const char *data, int len, int formatType)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	init();
}


CfgInternal::~CfgInternal()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif

	if (cfgIProperties_)
		delete cfgIProperties_;

	if (cfgIDefaults_.size())
		cfgIDefaults_.clear();
}



// static
// defined in PAL
//#define DEFAULT_INTERNAL_CONFIG_FILE_NAME "C:\\Program Files\\Hobart\\Access Scale\\config.dat"
//#define DEFAULT_CUSTOM_INTERNAL_CONFIG_FILE_NAME "C:\\Program Files\\Hobart\\Access Scale\\configc.dat"

std::string CfgInternal::internalConfigFileName_(DEFAULT_INTERNAL_CONFIG_FILE_NAME);
std::string CfgInternal::customInternalConfigFileName_(DEFAULT_CUSTOM_INTERNAL_CONFIG_FILE_NAME);

const char *CfgInternal::getInternalConfigFileName()
{
	return internalConfigFileName_.c_str();
}
std::string CfgInternal::getInternalConfigFileNameStr()
{
	return internalConfigFileName_;
}
void CfgInternal::setInternalConfigDirectory(const char *directory)
{
	if (directory)
	{
		util_filesystem::Filename cfgName(internalConfigFileName_);
		cfgName.setPath(directory);
		internalConfigFileName_ = cfgName.getFullPath();
	}
}


// define in PAL
//#define DEFAULT_NETWORK_CONFIG_FILE_NAME "C:\\Program Files\\Hobart\\Access Scale\\bootParams.cfg"

std::string CfgInternal::networkConfigFileName_(DEFAULT_NETWORK_CONFIG_FILE_NAME);

const char *CfgInternal::getNetworkConfigFileName()
{
	return networkConfigFileName_.c_str();
}
std::string CfgInternal::getNetworkConfigFileNameStr()
{
	return networkConfigFileName_;
}
void CfgInternal::setNetworkConfigDirectory(const char *directory)
{
	if (directory)
	{
		util_filesystem::Filename cfgName(networkConfigFileName_);
		cfgName.setPath(directory);
		networkConfigFileName_ = cfgName.getFullPath();
	}
}


std::string CfgInternal::getName(int fieldCode) const
{
	int offset = fieldCode - INTERNAL_CONFIGURATION_FIRST_FIELDCODE;

	if (( offset >= 0 ) && ( fieldCode < endCfgInternalFieldOffsets ) && ( offset < (int) configNames_.size() ))
		return configNames_[offset];
	return "";
}


string CfgInternal::toString(const std::string& name) const
{
	Util::CLockCriticalSection lock( cfgICS_ );
	
	const string *temp = cfgIProperties_->getProperty(name);
	if (!temp)
		temp = lookupDefault(name);
	if (temp)
	{
		if (traceCfgInternal_)
			Debug::trace("CfgInternal", "CfgInternal reading " + name + ": " + *temp);
		return *temp;
	}
	else
	{
		string exc = "Internal Config Item not found: " + name;
		if (traceCfgInternalExc_)
			Debug::trace("CfgInternal.exception", exc);
		
		if (cfgIParameterNotFoundThrowsException_)
			throw util_exception::Exception(exc);
		else	
			return "";
	}
}


int CfgInternal::toInt(const std::string& name) const
{
	Util::CLockCriticalSection lock( cfgICS_ );
	
	const string *temp = cfgIProperties_->getProperty(name);
	if (!temp)
		temp = lookupDefault(name);
	if (temp)
	{
		if (traceCfgInternal_)
			Debug::trace("CfgInternal", "CfgInternal reading " + name + ": " + inttostring(stringtoint(*temp)));
		return stringtoint(*temp);
	}
	else
	{
		string exc = "Internal Config Item not found: " + name;
		if (traceCfgInternalExc_)
			Debug::trace("CfgInternal.exception", exc);
		
		if (cfgIParameterNotFoundThrowsException_)
			throw util_exception::Exception(exc);
		else	
			return 0;
	}
}


bool CfgInternal::toBool(const std::string& name) const
{
	Util::CLockCriticalSection lock( cfgICS_ );
	
	const string *temp = cfgIProperties_->getProperty(name);
	if (!temp)
		temp = lookupDefault(name);
	if (temp)
	{
		if (traceCfgInternal_)
			Debug::trace("CfgInternal", "CfgInternal reading " + name + ": " + booltostring(stringtobool(*temp)));
		return stringtobool(*temp);
	}
	else
	{
		string exc = "Internal Config Item not found: " + name;
		if (traceCfgInternalExc_)
			Debug::trace("CfgInternal.exception", exc);
		
		if (cfgIParameterNotFoundThrowsException_)
			throw util_exception::Exception(exc);
		else	
			return false;
	}
}


string CfgInternal::toString(int fieldCode) const
{
	Util::CLockCriticalSection lock( cfgICS_ );
	
	string exc;
	int offset = fieldCode - INTERNAL_CONFIGURATION_FIRST_FIELDCODE;

	if (offset >= 0 && fieldCode < endCfgInternalFieldOffsets && configNames_[offset].size())
	{
		const string *temp = cfgIProperties_->getProperty(configNames_[offset]);
		if (!temp)
			temp = lookupDefault(configNames_[offset]);
		if (temp)
		{
			if (traceCfgInternal_)
			{
				exc = "CfgInternal reading " + inttostring(fieldCode) + " " + configNames_[offset] + ": " + *temp;
				Debug::trace("CfgInternal", exc);
			}
			return *temp;
		}
		else
			exc = "Internal Config Item not found: " + configNames_[offset];
	}
	else
		exc = "Internal Config Item not found: Invalid field offset: " + inttostring(fieldCode);

	if (traceCfgInternalExc_)
		Debug::trace("CfgInternal.exception", exc);

	if (cfgIParameterNotFoundThrowsException_)
		throw util_exception::Exception(exc);
	else	
		return "";
}


int CfgInternal::toInt(int fieldCode) const
{
	Util::CLockCriticalSection lock( cfgICS_ );
	
	string exc;
	int offset = fieldCode - INTERNAL_CONFIGURATION_FIRST_FIELDCODE;

	if (offset >= 0 && fieldCode < endCfgInternalFieldOffsets && configNames_[offset].size())
	{
		const string *temp = cfgIProperties_->getProperty(configNames_[offset]);
		if (!temp)
			temp = lookupDefault(configNames_[offset]);
		if (temp)
		{
			if (traceCfgInternal_)
			{
				exc = "CfgInternal reading " + inttostring(fieldCode) + " " + configNames_[offset] + ": " + inttostring(stringtoint(*temp));
				Debug::trace("CfgInternal", exc);
			}
			return stringtoint(*temp);
		}
		else
			exc = "Internal Config Item not found: " + configNames_[offset];
	}
	else
		exc = "Internal Config Item not found: Invalid field offset: " + inttostring(fieldCode);
		
	if (traceCfgInternalExc_)
		Debug::trace("CfgInternal.exception", exc);
		
	if (cfgIParameterNotFoundThrowsException_)
		throw util_exception::Exception(exc);
	else	
		return 0;
}


bool CfgInternal::toBool(int fieldCode) const
{
	Util::CLockCriticalSection lock( cfgICS_ );

	string exc;
	int offset = fieldCode - INTERNAL_CONFIGURATION_FIRST_FIELDCODE;

	if (offset >= 0 && fieldCode < endCfgInternalFieldOffsets && configNames_[offset].size())
	{
		const string *temp = cfgIProperties_->getProperty(configNames_[offset]);
		if (!temp)
			temp = lookupDefault(configNames_[offset]);
		if (temp)
		{
			if (traceCfgInternal_)
			{
				exc = "CfgInternal reading " + inttostring(fieldCode) + " " + configNames_[offset] + ": " + booltostring(stringtobool(*temp));
				Debug::trace("CfgInternal", exc);
			}
			return stringtobool(*temp);
		}
		else
			exc = "Internal Config Item not found: " + configNames_[offset];
	}
	else
		exc = "Internal Config Item not found: Invalid field offset: " + inttostring(fieldCode);
		
	if (traceCfgInternalExc_)
		Debug::trace("CfgInternal.exception", exc);
	
	if (cfgIParameterNotFoundThrowsException_)
		throw util_exception::Exception(exc);
	else	
		return false;
}


const string *CfgInternal::lookupDefault(const std::string &name) const
{
	std::map<std::string, std::string>::const_iterator iter = cfgIDefaults_.find(name);

	if ( iter == cfgIDefaults_.end() )
		return 0;
	return &(iter->second);
}


void CfgInternal::load()
{
	Util::CLockCriticalSection lock( cfgICS_ );

	if (cfgIProperties_)
		delete cfgIProperties_;

	util_filesystem::Filename cfgName(internalConfigFileName_);
	util_filesystem::File cfgFile(cfgName);
	internalConfigChecksum_ = (cfgFile.exists()) ? cfgFile.checksum() : 0;
	Debug::trace("CfgInternal.load", "load - internalConfigChecksum_=" + inttostring(internalConfigChecksum_));

	cfgName = customInternalConfigFileName_;
	util_filesystem::File cfgcustomFile(cfgName);
	customInternalConfigChecksum_ = (cfgcustomFile.exists()) ? cfgcustomFile.checksum() : 0;
	Debug::trace("CfgInternal.load", "load - customInternalConfigChecksum_=" + inttostring(customInternalConfigChecksum_));

	cfgIProperties_ = new util_env::Properties(customInternalConfigFileName_, internalConfigFileName_);
	if (!cfgIProperties_)
		return;


	// Have to initialize to false before looking it up in the properties!
	cfgIParameterNotFoundThrowsException_ = false;
	cfgIParameterNotFoundThrowsException_ = toBool(CFGIPARAMETERNOTFOUNDTHROWSEXCEPTION);
	
	try
	{
		PrinterStockMetrics::initStatics();

		loadScaleModelCfg();
		loadIndividualCfgItems();
	}
	catch (std::exception e)
	{
		string es = e.what();
	}
}


void CfgInternal::checkReloadOfCfgI()
{
	Util::CLockCriticalSection lock( cfgICS_ );

	bool reloaded = false;

	util_filesystem::Filename cfgName(internalConfigFileName_);
	util_filesystem::File cfgFile(cfgName);

	if (cfgFile.exists())
	{
		unsigned int newChecksum = cfgFile.checksum();
		if (newChecksum != internalConfigChecksum_)
		{
			load();
			reloaded = true;
		}
		else
		{
			cfgName = customInternalConfigFileName_;
			util_filesystem::File cfgcustomFile(cfgName);

			if (cfgcustomFile.exists())
			{
				newChecksum = cfgcustomFile.checksum();
				if (newChecksum != customInternalConfigChecksum_)
				{
					load();
					reloaded = true;
				}
			}
			else if (customInternalConfigChecksum_)
			{
				// If we are here, then custom internal config file used to exist, but has been deleted, so need to reload.
				load();
				reloaded = true;
			}
		}
	}

	try
	{
		if (reloaded && toBool("notifyWhenCfgIReloaded"))
			SessionManagerHelper::getInstance()->addDialog(0, 1, "Internal Configuration reloaded.");
	}
	catch (std::exception e)
	{
        Debug::trace("SessionManager", "notifyWhenCfgIReloaded exception");
		string es = e.what();
	}
}


void CfgInternal::checkTracingCfgI()
{
	traceCfgInternal_ = Debug::isTracing("CfgInternal");
	traceCfgInternalExc_ = Debug::isTracing("CfgInternal.exception");
}


const char *CfgInternal::serialize(int format) const
{
	serializedData = "CfgInternal::serialize - unknown format:" + inttostring(format);
	return serializedData.c_str();
}


void CfgInternal::show()
{
	Debug::trace("Configuration", "*** CfgInternal parameters ***");
}


void CfgInternal::init()
{
	cfgIProperties_ = (util_env::Properties *)NULL;

	configNames_.resize(endCfgInternalFieldOffsets - INTERNAL_CONFIGURATION_FIRST_FIELDCODE);

	internalConfigChecksum_ = 0;
	customInternalConfigChecksum_ = 0;

	cfgIParameterNotFoundThrowsException_ = false;

	traceCfgInternal_ = false;
	traceCfgInternalExc_ = false;

	configNames_[fieldOffsetCfgIScaleModel                        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SCALEMODEL;
	configNames_[fieldOffsetCfgIScaleName                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SCALENAME;
	configNames_[fieldOffsetCfgIScaleServiceName                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SCALESERVICENAME;
	configNames_[fieldOffsetCfgIWebServiceName                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = WEBSERVICENAME;
	configNames_[fieldOffsetCfgIAppInstallerExe                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = APPINSTALLEREXE;

	configNames_[fieldOffsetCfgISystemGraphicsLocation            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SYSTEMGRAPHICSLOCATION;
	configNames_[fieldOffsetCfgIUserGraphicsLocation              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USERGRAPHICSLOCATION;
	configNames_[fieldOffsetCfgIImagesLocation                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = IMAGESLOCATION;
	configNames_[fieldOffsetCfgISystemUserRootDir                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SYSTEMUSERROOTDIR;
	configNames_[fieldOffsetCfgIGraphicsExtensions                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GRAPHICSEXTENSIONS;
	configNames_[fieldOffsetCfgIGraphicFileSpecifiers             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GRAPHICFILESPECIFIERS;
	configNames_[fieldOffsetCfgINoGraphicSelectedName             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = NOGRAPHICSELECTEDNAME;
	configNames_[fieldOffsetCfgISystemGraphicsTar                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SYSTEMGRAPHICSTAR;
	configNames_[fieldOffsetCfgIUserGraphicsTar                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USERGRAPHICSTAR;
	configNames_[fieldOffsetCfgIProductGraphicsTar                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTGRAPHICSTAR;
	configNames_[fieldOffsetCfgIProductGraphicsDir                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTGRAPHICSDIR;
	configNames_[fieldOffsetCfgISystemProductGraphicsDir          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SYSTEMPRODUCTGRAPHICSDIR;
	configNames_[fieldOffsetCfgIProductGraphicsDefaultExtension   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTGRAPHICSDEFAULTEXTENSION;
	configNames_[fieldOffsetCfgICategoryGraphicsTar               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CATEGORYGRAPHICSTAR;
	configNames_[fieldOffsetCfgICategoryGraphicsDir               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CATEGORYGRAPHICSDIR;
	configNames_[fieldOffsetCfgISystemCategoryGraphicsDir         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SYSTEMCATEGORYGRAPHICSDIR;
	configNames_[fieldOffsetCfgICategoryGraphicsDefaultExtension  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CATEGORYGRAPHICSDEFAULTEXTENSION;
	configNames_[fieldOffsetCfgISpecialsGraphicsTar               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SPECIALSGRAPHICSTAR;
	configNames_[fieldOffsetCfgISpecialsGraphicsDir               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SPECIALSGRAPHICSDIR;
	configNames_[fieldOffsetCfgISpecialsGraphicsDefaultExtension  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SPECIALSGRAPHICSDEFAULTEXTENSION;
	configNames_[fieldOffsetCfgIStreamingGraphicsTar              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = STREAMINGGRAPHICSTAR;
	configNames_[fieldOffsetCfgIStreamingGraphicsDir              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = STREAMINGGRAPHICSDIR;
	configNames_[fieldOffsetCfgIStreamingGraphicsDefaultExtension - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = STREAMINGGRAPHICSDEFAULTEXTENSION;
	configNames_[fieldOffsetCfgISoftkeyGraphicsTar                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SOFTKEYGRAPHICSTAR;
	configNames_[fieldOffsetCfgISoftkeyGraphicsDir                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SOFTKEYGRAPHICSDIR;
	configNames_[fieldOffsetCfgISoftkeyGraphicsDefaultExtension   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SOFTKEYGRAPHICSDEFAULTEXTENSION;
	configNames_[fieldOffsetCfgIReservedSoftkeyGraphicsDir        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = RESERVEDSOFTKEYGRAPHICSDIR;
	configNames_[fieldOffsetCfgISelfServeGraphicsDir              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVEGRAPHICSDIR;
	configNames_[fieldOffsetCfgISystemSelfServeGraphicsDir        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SYSTEMSELFSERVEGRAPHICSDIR;
	configNames_[fieldOffsetCfgISelfServeGraphicsDefaultExtension - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVEGRAPHICSDEFAULTEXTENSION;
	configNames_[fieldOffsetCfgIPlanogramGraphicsDir              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PLANOGRAMGRAPHICSDIR;
	configNames_[fieldOffsetCfgIPlanogramGraphicsDefaultExtension - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PLANOGRAMGRAPHICSDEFAULTEXTENSION;
	configNames_[fieldOffsetCfgIVideosDir                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = VIDEOSDIR;
	configNames_[fieldOffsetCfgIVideosDefaultExtension            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = VIDEOSDEFAULTEXTENSION;

	configNames_[fieldOffsetCfgILangFileName                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LANGFILENAME;

	configNames_[fieldOffsetCfgIKeyMountDir                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = KEYMOUNTDIR;
	configNames_[fieldOffsetCfgIWebPagesRootDir                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = WEBPAGESROOTDIR;
	configNames_[fieldOffsetCfgIGeneralWebPageDirName             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GENERALWEBPAGEDIRNAME;
	configNames_[fieldOffsetCfgILinuxWebPageDirName               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LINUXWEBPAGEDIRNAME;
	configNames_[fieldOffsetCfgILanguageFilesDir                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LANGUAGEFILESDIR;
	configNames_[fieldOffsetCfgIPythonScriptsDir                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PYTHONSCRIPTSDIR;

	configNames_[fieldOffsetCfgIServiceLoginId                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SERVICELOGINID;
	configNames_[fieldOffsetCfgIServiceKeyFile                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SERVICEKEYFILE;
	configNames_[fieldOffsetCfgIServiceKeyRemoveTimeout           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SERVICEKEYREMOVETIMEOUT;
	configNames_[fieldOffsetCfgISuperKeyFile                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SUPERKEYFILE;
	configNames_[fieldOffsetCfgISelfServeKeyFile                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVEKEYFILE;

	configNames_[fieldOffsetCfgIDirectorySeparator                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIRECTORYSEPARATOR;
	configNames_[fieldOffsetCfgITempDBDir                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = TEMPORARYDBDIRECTORY;
	configNames_[fieldOffsetCfgIDbLocation                        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DBLOCATION;
	configNames_[fieldOffsetCfgIEmptyDbLocation                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EMPTYDBLOCATION;
	configNames_[fieldOffsetCfgIDbDefaultsFile                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DBDEFAULTSFILE;
	configNames_[fieldOffsetCfgIDbBackupEnabled                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DBBACKUPENABLED;
	configNames_[fieldOffsetCfgIDbBackupLocation                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DBBACKUPLOCATION;
	configNames_[fieldOffsetCfgIDbMirrorLocation                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DBMIRRORLOCATION;
	configNames_[fieldOffsetCfgIDbCheckBeforeShutdown             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DBCHECKBEFORESHUTDOWN;
	configNames_[fieldOffsetCfgICheckIndicesBeforeOpenDatabase    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DBCHECKINDICES;
	configNames_[fieldOffsetCfgIDbCheckIntegrity                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DBCHECKINTEGRITY;
	configNames_[fieldOffsetCfgIDbCheckIntegrityFrequency         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DBCHECKINTEGRITYFREQUENCY;
	configNames_[fieldOffsetCfgIRebuildIndicesEnabled             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DBREBUILDINDICESENABLED;
	configNames_[fieldOffsetCfgIVersionIndependentBackupFileName  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = VERSIONINDEPENDENTBACKUPFILE;
	configNames_[fieldOffsetCfgIAutoLoadVersionIndependentData    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = AUTOLOADVERSIONINDEPENDENTDATA;
	configNames_[fieldOffsetCfgIBackupVersionIndependentDataNow   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BACKUPVERSIONINDEPENDENTDATANOW;
	configNames_[fieldOffsetCfgIMakeCopyOfVersionIndependentData  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAKECOPYOFVERSIONINDEPENDENTDATA;

	configNames_[fieldOffsetCfgIAutoBackupDatabaseLocation        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = AUTOBACKUPDATABASELOCATION;
	configNames_[fieldOffsetCfgIAutoBackupPLUDataFile             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = AUTOBACKUPPLUDATAFILE;
	configNames_[fieldOffsetCfgIAutoBackupFlashkeyDataFile        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = AUTOBACKUPFLASHKEYDATAFILE;
	configNames_[fieldOffsetCfgIAutoBackupLabelDataFile           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = AUTOBACKUPLABELDATAFILE;
	configNames_[fieldOffsetCfgIAutoBackupOperatorDataFile        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = AUTOBACKUPOPERATORDATAFILE;
	configNames_[fieldOffsetCfgIAutoBackupStoreNameNumberFile     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = AUTOBACKUPSTORENAMENUMBERFILE;
	configNames_[fieldOffsetCfgIAutoBackupConfigFile              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = AUTOBACKUPCONFIGFILE;
	configNames_[fieldOffsetCfgIAutoBackupSynchConfigFile         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = AUTOBACKUPSYNCHCONFIGFILE;

	configNames_[fieldOffsetCfgIPrimaryConsoleSessionPriority     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRIMARYCONSOLESESSIONPRIORITY;
	configNames_[fieldOffsetCfgIRemoteConsoleSessionPriority      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = REMOTECONSOLESESSIONPRIORITY;
	configNames_[fieldOffsetCfgIMaxRemoteConsoleSessions          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXREMOTECONSOLESESSIONS;
	configNames_[fieldOffsetCfgIMaxConsecutiveRTCMessages         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXCONSECUTIVERTCMESSAGES;
	configNames_[fieldOffsetCfgIEventManagerEnabled               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EVENTMANAGERENABLED;
	configNames_[fieldOffsetCfgISessionManagerEnabled             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SESSIONMANAGERENABLED;
	configNames_[fieldOffsetCfgISessionManagerStartupDelay        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SESSIONMANAGERSTARTUPDELAY;
	configNames_[fieldOffsetCfgISessionManagerPeriodicTimer       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SESSIONMANAGERPERIODICTIMER;
	configNames_[fieldOffsetCfgIRealTimeControllerEnabled         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = REALTIMECONTROLLERENABLED;
	configNames_[fieldOffsetCfgITimerManagerEnabled               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = TIMERMANAGERENABLED;
	configNames_[fieldOffsetCfgIVersionManagerEnabled             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = VERSIONMANAGERENABLED;
	configNames_[fieldOffsetCfgIUSBEnabledAtBootup                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USBENABLEDATBOOTUP;
	configNames_[fieldOffsetCfgIInterruptXferTimeout              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = INTERRUPTXFERTIMEOUT;
	configNames_[fieldOffsetCfgIBulkXferTimeout                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BULKXFERTIMEOUT;
	configNames_[fieldOffsetCfgIExpiredRecordsCheckFrequency      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EXPIREDRECORDSCHECKFREQUENCY;

	configNames_[fieldOffsetCfgIFakeWeigherManagerEnabled         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FAKEWEIGHERMANAGERENABLED;
	configNames_[fieldOffsetCfgIFakeWeigherFrequency              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FAKEWEIGHERFREQUENCY;
	configNames_[fieldOffsetCfgIFakePrinterManagerEnabled         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FAKEPRINTERMANAGERENABLED;
	configNames_[fieldOffsetCfgIFakeCLAManagerEnabled             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FAKECLAMANAGERENABLED;
	configNames_[fieldOffsetCfgIFakeWrapperManagerEnabled         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FAKEWRAPPERMANAGERENABLED;
	configNames_[fieldOffsetCfgITestBrowserEnabled                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = TESTBROWSERENABLED;
	configNames_[fieldOffsetCfgITestBrowserMsgDelay               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = TESTBROWSERMSGDELAY;
	configNames_[fieldOffsetCfgITestMessagesEnabled               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = TESTMESSAGESENABLED;

	configNames_[fieldOffsetCfgIRFIDEnabled                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = RFIDENABLED;
	configNames_[fieldOffsetCfgIRFIDDebounceTime                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = RFIDDEBOUNCETIME;
	configNames_[fieldOffsetCfgIRFIDComPort                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = RFIDCOMPORT;
	configNames_[fieldOffsetCfgIRFIDHWPresent                     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = "";

	configNames_[fieldOffsetCfgIDiagnosticsManagerEnabled         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIAGNOSTICSMANAGERENABLED;
	configNames_[fieldOffsetCfgIDiagnosticsDBName                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIAGNOSTICSDBNAME;
	configNames_[fieldOffsetCfgIDiagnosticsDBBackupName           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIAGNOSTICSDBBACKUPNAME;
	configNames_[fieldOffsetCfgIDiagnosticsDBOldName              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIAGNOSTICSDBOLDNAME;

	configNames_[fieldOffsetCfgIScannerEnabled                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BARCODESCANNERENABLED;

	configNames_[fieldOffsetCfgIPrinterProbeTime                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRINTERPROBETIME;
	configNames_[fieldOffsetCfgIWeigherProbeTime                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = WEIGHERPROBETIME;
	configNames_[fieldOffsetCfgICustDispProbeTime                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CUSTDISPPROBETIME;
	configNames_[fieldOffsetCfgILabelApplierProbeTime             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LABELAPPLIERPROBETIME;
	configNames_[fieldOffsetCfgIWrapperProbeTime                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = WRAPPERPROBETIME;

	configNames_[fieldOffsetCfgIUSBBulkDriveOnlyMode              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USBBULKDRIVEONLYMODE;
	configNames_[fieldOffsetCfgIUSBBulkDriveName                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USBBULKDRIVENAME;
	configNames_[fieldOffsetCfgICompactFlashDriveName             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = COMPACTFLASHDRIVENAME;
	configNames_[fieldOffsetCfgIPermanentRamDriveName             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PERMANENTRAMDRIVENAME;
	configNames_[fieldOffsetCfgITemporaryRamDriveName             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = TEMPORARYRAMDRIVENAME;
	configNames_[fieldOffsetCfgIEvolutionDirectory                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EVOLUTIONDIRECTORY;
	configNames_[fieldOffsetCfgIBootParametersFileName            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BOOTPARAMETERSFILENAME;
	configNames_[fieldOffsetCfgIDefaultNetCfgDir                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEFAULTNETWORKCONFIGDIR;
	configNames_[fieldOffsetCfgITmpNetCfgDir                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = TMPNETWORKCONFIGDIR;
	configNames_[fieldOffsetCfgIFirmwareDirectory                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FIRMWAREDIRECTORY;
	configNames_[fieldOffsetCfgIFirmwareGenericFilename           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FIRMWAREGENERICFILENAME;
	configNames_[fieldOffsetCfgIFirmwareFilenames                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FIRMWAREFILENAMES;

	configNames_[fieldOffsetCfgIVmTempDir                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = VERSIONMANAGERTEMPDIR;
	configNames_[fieldOffsetCfgIVmForceUpgrade                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = VERSIONMANAGERFORCEUPGRADE;
	configNames_[fieldOffsetCfgIDefaultUpgradeFileName            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEFAULTUPGRADEFILENAME;
	configNames_[fieldOffsetCfgIInstalLogFile                     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = INSTALLOGFILE;  
	configNames_[fieldOffsetCfgIVersionInfoFileName               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = VERSIONINFOFILENAME;
	configNames_[fieldOffsetCfgIDefaultUpgradeDirectory           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEFAULTUPGRADEDIRECTORY;
	configNames_[fieldOffsetCfgIUpgradeZipFile                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UPGRADEZIPFILE;
	configNames_[fieldOffsetCfgIUpgradeIniFile                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UPGRADEINIFILE;

	configNames_[fieldOffsetCfgIMaxGraphicsPerProduct             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXGRAPHICSPERPRODUCT;
	configNames_[fieldOffsetCfgIProductNameMaxLength              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTNAMEMAXLENGTH;
	configNames_[fieldOffsetCfgIUpdatePrinterConfig               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UPDATEPRINTERCONFIG;
	configNames_[fieldOffsetCfgIUseOnlyPrimaryLanguageForProductStatistics - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = ONLYPRILANGFORPRODSTATS;
	configNames_[fieldOffsetCfgIPercentagePrecision               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PERCENTAGEPRECISION;
	configNames_[fieldOffsetCfgIMaxLifeDays                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXLIFEDAYS;
	configNames_[fieldOffsetCfgIMaxLifeHours                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXLIFEHOURS;
	configNames_[fieldOffsetCfgIMaxLifeMinutes                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXLIFEMINUTES;
	
	configNames_[fieldOffsetCfgIMaxBonusPoints                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXBONUSPOINTS;
	configNames_[fieldOffsetCfgIQuarterPoundFor                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = QUARTERPOUNDFOR;
	configNames_[fieldOffsetCfgIHalfPoundFor                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HALFPOUNDFOR;
	configNames_[fieldOffsetCfgIHundredGramsFor                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HUNDREDGRAMSFOR;
	configNames_[fieldOffsetCfgIContinuousStatisticsDelay         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CONTINUOUSSTATISTICSDELAY;
	configNames_[fieldOffsetCfgILostPeripheralsCheckTime          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LOSTPERIPHERALSCHECKTIME;

	configNames_[fieldOffsetCfgIKeyboardNetWeightPrecision        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = KEYBOARDNETWEIGHTPRECISION;

	configNames_[fieldOffsetCfgISubtractDescent                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SUBTRACTDESCENT;
	configNames_[fieldOffsetCfgIMaxLabelNumber                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXLABELNUMBER;
	configNames_[fieldOffsetCfgILabelFeedHeelFirst                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LABELFEEDHEELFIRST;
	configNames_[fieldOffsetCfgITestLabelType                     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = TESTLABELTYPE;
	configNames_[fieldOffsetCfgIIgnoreLabelFit                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = IGNORELABELFIT;
	configNames_[fieldOffsetCfgILabelHorizontalOffset             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LABELHORIZONTALOFFSET;
	configNames_[fieldOffsetCfgIDefaultNutriLabelTemplate         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEFAULTNUTRILABELTEMPLATE;
	configNames_[fieldOffsetCfgIShelfLifeExpLabelFamily           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SHELFLIFEEXPLABELFAMILY;
	configNames_[fieldOffsetCfgIExpirationLabelExpiredLimit       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EXPIRATIONLABELEXPIREDLIMIT;
	configNames_[fieldOffsetCfgIRunTotalsLabelFamily              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = RUNTOTALSLABELFAMILY;

	configNames_[fieldOffsetCfgIFBWFActivated                     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FBWFACTIVATED;
	configNames_[fieldOffsetCfgIUsingRestrictedLogins             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USINGRESTRICTEDLOGINS;
	configNames_[fieldOffsetCfgIAppInstallerIsService             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = APPINSTALLERISSERVICE;
	configNames_[fieldOffsetCfgIStartupDelay                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = STARTUPDELAY;
	configNames_[fieldOffsetCfgISimulatePrinting                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SIMULATEPRINTING;
	configNames_[fieldOffsetCfgIMonitorMemoryInterval             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MONITORMEMORYINTERVAL;

	configNames_[fieldOffsetCfgIDualRangeLimitAvoir               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DUALRANGELIMITAVOIR;
	configNames_[fieldOffsetCfgIDualRangeLimitMetric              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DUALRANGELIMITMETRIC;
	configNames_[fieldOffsetCfgIRezeroWeightLimitAvoir            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = REZEROWEIGHTLIMITAVOIR;
	configNames_[fieldOffsetCfgIRezeroWeightLimitMetric           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = REZEROWEIGHTLIMITMETRIC;

	configNames_[fieldOffsetCfgIConnectionIdentificationTimer     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CONNECTIONIDENTIFICATIONTIMER;
	configNames_[fieldOffsetCfgIConnectionIdentificationDelay     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CONNECTIONIDENTIFICATIONDELAY;
	configNames_[fieldOffsetCfgICommSessionReadMessageTimer       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = COMMSESSIONREADMESSAGETIMER;
	configNames_[fieldOffsetCfgICommSessionReadMessageDelay       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = COMMSESSIONREADMESSAGEDELAY;
	configNames_[fieldOffsetCfgIWaitBeforeConnectingTimer         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = WAITBEFORECONNECTINGTIMER;
	configNames_[fieldOffsetCfgIWaitBeforeConnectingDelay         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = WAITBEFORECONNECTINGDELAY;

	configNames_[fieldOffsetCfgIPingConnectivityFrequency         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PINGCONNECTIVITYFREQUENCY;
	configNames_[fieldOffsetCfgIPingConnectivityAttempts          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PINGCONNECTIVITYATTEMPTS;
	configNames_[fieldOffsetCfgIOfflinePingConnectivityFrequency  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = OFFLINEPINGCONNECTIVITYFREQUENCY;
	configNames_[fieldOffsetCfgIOfflinePingConnectivityAttempts   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = OFFLINEPINGCONNECTIVITYATTEMPTS;

	configNames_[fieldOffsetCfgISynchronizationEnabled            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SYNCHRONIZATIONENABLED;
	configNames_[fieldOffsetCfgISynchronizeAllSecondariesTimer    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SYNCHRONIZEALLSECONDARIESTIMER;
	configNames_[fieldOffsetCfgINotifyLocalBatchesTimer           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = NOTIFYLOCALBATCHESTIMER;
	configNames_[fieldOffsetCfgIRecordsPerSyncIteration           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = RECORDSPERSYNCITERATION;
	configNames_[fieldOffsetCfgIDelayPerSyncIteration             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DELAYPERSYNCITERATION;
	configNames_[fieldOffsetCfgIMaximumNumberRetries              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXIMUMNUMBERRETRIES;
	configNames_[fieldOffsetCfgIRetryCommandTimer                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = RETRYCOMMANDTIMER;
	configNames_[fieldOffsetCfgIHoursBeforeDeregistering          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HOURSBEFOREDEREGISTERING;
	configNames_[fieldOffsetCfgIDelayPurgingBatches               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DELAYPURGINGBATCHES;
	configNames_[fieldOffsetCfgIHoursBeforeRemovingDeletedBatches - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HOURSBEFOREREMOVINGDELETEDBATCHES;
	configNames_[fieldOffsetCfgIRecordsPerApplyIteration          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = RECORDSPERAPPLYITERATION;
	configNames_[fieldOffsetCfgIDelayPerApplyIteration            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DELAYPERAPPLYITERATION;
	configNames_[fieldOffsetCfgIDelayPerDeleteIteration           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DELAYPERDELETEITERATION;
	configNames_[fieldOffsetCfgISynchronizationStatusMessagesEnabled - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SYNCHRONIZATIONSTATUSMESSAGESENABLED;

	configNames_[fieldOffsetCfgIUseProductionPlanningDialog       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USEPRODUCTIONPLANNINGDIALOG;
	configNames_[fieldOffsetCfgIHTTPRequestMethod                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPREQUESTMETHOD;
	configNames_[fieldOffsetCfgIHTTPRequestContentType            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPREQUESTCONTENTTYPE;
	configNames_[fieldOffsetCfgIHTTPWaitingDelay                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPWAITINGDELAY;
	configNames_[fieldOffsetCfgIHTTPReadResponseTimer             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPREADRESPONSETIMER;
	configNames_[fieldOffsetCfgIHTTPReadSomeResponseDelay         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPREADSOMERESPONSEDELAY;
	configNames_[fieldOffsetCfgIHTTPReadNoResponseDelay           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPREADNORESPONSEDELAY;
	configNames_[fieldOffsetCfgIHTTPMaxResponseLineLength         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPMAXRESPONSELINELENGTH;
	configNames_[fieldOffsetCfgIHTTPReportErrors                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPREPORTERRORS;
	configNames_[fieldOffsetCfgIHTTPQueryStore                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPQUERYSTORE;
	configNames_[fieldOffsetCfgIHTTPQueryDept                     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPQUERYDEPT;
	configNames_[fieldOffsetCfgIHTTPQueryModel                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPQUERYMODEL;
	configNames_[fieldOffsetCfgIHTTPQueryPLU                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPQUERYPLU;
	configNames_[fieldOffsetCfgIHTTPQueryWeight                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPQUERYWEIGHT;
	configNames_[fieldOffsetCfgIHTTPQueryQuantity                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPQUERYQUANTITY;
	configNames_[fieldOffsetCfgIHTTPQueryLanguage                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPQUERYLANGUAGE;
	configNames_[fieldOffsetCfgIHTTPUseOfflineSpool               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPUSEOFFLINESPOOL;
	configNames_[fieldOffsetCfgIHTTPOfflineSpoolRetry             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPOFFLINESPOOLRETRY;
	configNames_[fieldOffsetCfgIHTTPOfflineSpoolMaxRequestAge     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HTTPOFFLINESPOOLMAXREQUESTAGE;

	configNames_[fieldOffsetCfgIExpandedTextWidth                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EXPANDEDTEXTWIDTH;
	configNames_[fieldOffsetCfgIExpandedTextHeight                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EXPANDEDTEXTHEIGHT;
	configNames_[fieldOffsetCfgIExpandedTextFontName              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EXPANDEDTEXTFONTNAME;
	configNames_[fieldOffsetCfgIExpandedTextPointSize             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EXPANDEDTEXTPOINTSIZE;
	configNames_[fieldOffsetCfgISpecialMessageWidth               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SPECIALMESSAGEWIDTH;
	configNames_[fieldOffsetCfgISpecialMessageHeight              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SPECIALMESSAGEHEIGHT;
	configNames_[fieldOffsetCfgISpecialMessageFontName            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SPECIALMESSAGEFONTNAME;
	configNames_[fieldOffsetCfgISpecialMessagePointSize           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SPECIALMESSAGEPOINTSIZE;
	configNames_[fieldOffsetCfgIProductNoteWidth                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTNOTEWIDTH;
	configNames_[fieldOffsetCfgIProductNoteHeight                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTNOTEHEIGHT;
	configNames_[fieldOffsetCfgIProductNoteFontName               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTNOTEFONTNAME;
	configNames_[fieldOffsetCfgIProductNotePointSize              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTNOTEPOINTSIZE;
	configNames_[fieldOffsetCfgIMarqueeTextWidth                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MARQUEETEXTWIDTH;
	configNames_[fieldOffsetCfgIMarqueeTextHeight                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MARQUEETEXTHEIGHT;
	configNames_[fieldOffsetCfgIMarqueeTextFontName               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MARQUEETEXTFONTNAME;
	configNames_[fieldOffsetCfgIMarqueeTextPointSize              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MARQUEETEXTPOINTSIZE;
	configNames_[fieldOffsetCfgIProductText5Width                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT5WIDTH;
	configNames_[fieldOffsetCfgIProductText5Height                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT5HEIGHT;
	configNames_[fieldOffsetCfgIProductText5FontName              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT5FONTNAME;
	configNames_[fieldOffsetCfgIProductText5PointSize             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT5POINTSIZE;
	configNames_[fieldOffsetCfgIProductText6Width                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT6WIDTH;
	configNames_[fieldOffsetCfgIProductText6Height                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT6HEIGHT;
	configNames_[fieldOffsetCfgIProductText6FontName              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT6FONTNAME;
	configNames_[fieldOffsetCfgIProductText6PointSize             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT6POINTSIZE;
	configNames_[fieldOffsetCfgIProductText7Width                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT7WIDTH;
	configNames_[fieldOffsetCfgIProductText7Height                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT7HEIGHT;
	configNames_[fieldOffsetCfgIProductText7FontName              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT7FONTNAME;
	configNames_[fieldOffsetCfgIProductText7PointSize             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT7POINTSIZE;
	configNames_[fieldOffsetCfgIProductText8Width                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT8WIDTH;
	configNames_[fieldOffsetCfgIProductText8Height                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT8HEIGHT;
	configNames_[fieldOffsetCfgIProductText8FontName              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT8FONTNAME;
	configNames_[fieldOffsetCfgIProductText8PointSize             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT8POINTSIZE;
	configNames_[fieldOffsetCfgIProductText9Width                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT9WIDTH;
	configNames_[fieldOffsetCfgIProductText9Height                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT9HEIGHT;
	configNames_[fieldOffsetCfgIProductText9FontName              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT9FONTNAME;
	configNames_[fieldOffsetCfgIProductText9PointSize             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTTEXT9POINTSIZE;

	configNames_[fieldOffsetCfgIProdDescriptionWidth              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODDESCRIPTIONWIDTH;
	configNames_[fieldOffsetCfgIProdDescriptionHeight             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODDESCRIPTIONHEIGHT;
	configNames_[fieldOffsetCfgIProdDescriptionFontName           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODDESCRIPTIONFONTNAME;
	configNames_[fieldOffsetCfgIProdDescriptionPointSize          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODDESCRIPTIONPOINTSIZE;
	configNames_[fieldOffsetCfgICOOLTrackingTextWidth             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = COOLTRACKINGTEXTWIDTH;
	configNames_[fieldOffsetCfgICOOLTrackingTextHeight            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = COOLTRACKINGTEXTHEIGHT;
	configNames_[fieldOffsetCfgICOOLTrackingTextFontName          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = COOLTRACKINGTEXTFONTNAME;
	configNames_[fieldOffsetCfgICOOLTrackingTextPointSize         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = COOLTRACKINGTEXTPOINTSIZE;

	configNames_[fieldOffsetCfgIMaxLabelLengthPct                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXLABELLENGTHPCT;
	
	configNames_[fieldOffsetCfgIMaxLabelLengthPct                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXLABELLENGTHPCT;

	configNames_[fieldOffsetCfgIDimBLength1_000                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH1_000;
	configNames_[fieldOffsetCfgIDimBLength1_500                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH1_500;
	configNames_[fieldOffsetCfgIDimBLength1_750                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH1_750;
	configNames_[fieldOffsetCfgIDimBLength2_000                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH2_000;
	configNames_[fieldOffsetCfgIDimBLength2_375                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH2_375;
	configNames_[fieldOffsetCfgIDimBLength2_500                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH2_500;
	configNames_[fieldOffsetCfgIDimBLength3_000                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH3_000;
	configNames_[fieldOffsetCfgIDimBLength3_500                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH3_500;
	configNames_[fieldOffsetCfgIDimBLength4_000                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH4_000;
	configNames_[fieldOffsetCfgIDimBLength4_500                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH4_500;
	configNames_[fieldOffsetCfgIDimBLength5_000                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH5_000;
	configNames_[fieldOffsetCfgIDimBLength5_500                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH5_500;
	configNames_[fieldOffsetCfgIDimBLength6_000                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH6_000;
	configNames_[fieldOffsetCfgIDimBLength6_500                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH6_500;
	configNames_[fieldOffsetCfgIDimBLength7_000                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH7_000;
	configNames_[fieldOffsetCfgIDimBLength7_500                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH7_500;
	configNames_[fieldOffsetCfgIDimBLength8_000                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH8_000;
	configNames_[fieldOffsetCfgIDimBLength8_500                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH8_500;
	configNames_[fieldOffsetCfgIDimBLength9_000                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH9_000;
	configNames_[fieldOffsetCfgIDimBLength9_500                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH9_500;
	configNames_[fieldOffsetCfgIDimBLength10_00                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTH10_00;
	configNames_[fieldOffsetCfgIDimBLengthContinuous              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DIMBLENGTHCONTINUOUS;

	configNames_[fieldOffsetCfgIGCDEnabled                             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GCDENABLED;
	configNames_[fieldOffsetCfgIGCDEnabledStandalone                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GCDENABLEDSTANDALONE;
	configNames_[fieldOffsetCfgIDesktopStoreLogoPath                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DESKTOPSTORELOGOPATH;
	configNames_[fieldOffsetCfgIDesktopStoreLogoFileName               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DESKTOPSTORELOGOFILENAME;
	configNames_[fieldOffsetCfgIDeviceStoreLogoPath                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEVICESTORELOGOPATH;
	configNames_[fieldOffsetCfgIDeviceStoreLogoFileName                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEVICESTORELOGOFILENAME;
	configNames_[fieldOffsetCfgIDeviceApplicationUpdatesDir            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEVICEAPPLICATIONUPDATESDIR;
	configNames_[fieldOffsetCfgIDeviceApplicationUpdatesSourceFileName - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEVICEAPPLICATIONUPDATESSOURCEFILENAME;
	configNames_[fieldOffsetCfgIDevicePlaylistUpdatesDir               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEVICEPLAYLISTUPDATESDIR;
	configNames_[fieldOffsetCfgILocalTempDir                           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LOCALTEMPDIR;
	configNames_[fieldOffsetCfgIAdapterDescSearchRNDIS                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = ADAPTERDESCSEARCHRNDIS;
	configNames_[fieldOffsetCfgIIpSubnetMask                           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = IPSUBNETMASK;
	configNames_[fieldOffsetCfgIIpAddressHLX                           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = IPADDRESSHLX;
	configNames_[fieldOffsetCfgIIpAddressGCD                           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = IPADDRESSGCD;
	configNames_[fieldOffsetCfgIGcdServerPort                          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GCDSERVERPORT;
	configNames_[fieldOffsetCfgIGcdDisplayObjectXmlPathFormat          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GCDDISPLAYOBJECTXMLPATHFORMAT;
	configNames_[fieldOffsetCfgIGcdDiscoveryMessage                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GCDDISCOVERYMESSAGE;
	configNames_[fieldOffsetCfgIGcdDiscoveryPort                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GCDDISCOVERYPORT;

	configNames_[fieldOffsetCfgIGoodBeepEnabled                        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GOODBEEPENABLED;
	configNames_[fieldOffsetCfgIGoodBeepFrequency                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GOODBEEPFREQUENCY;
	configNames_[fieldOffsetCfgIGoodBeepPeriod                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GOODBEEPPERIOD;
	configNames_[fieldOffsetCfgIGoodBeepDelay                          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GOODBEEPDELAY;
	configNames_[fieldOffsetCfgIBadBeepEnabled                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BADBEEPENABLED;
	configNames_[fieldOffsetCfgIBadBeepFrequency                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BADBEEPFREQUENCY;
	configNames_[fieldOffsetCfgIBadBeepPeriod                          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BADBEEPPERIOD;
	configNames_[fieldOffsetCfgIBadBeepDelay                           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BADBEEPDELAY;
	
	configNames_[fieldOffsetCfgIUpKeyboardXCoordinate                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UPKEYBOARDXCOORDINATE;
	configNames_[fieldOffsetCfgIUpKeyboardYCoordinate                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UPKEYBOARDYCOORDINATE;
	configNames_[fieldOffsetCfgIUpKeyboardWidth                        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UPKEYBOARDWIDTH;
	configNames_[fieldOffsetCfgIUpKeyboardHeight                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UPKEYBOARDHEIGHT;
	configNames_[fieldOffsetCfgIDownKeyboardXCoordinate                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DOWNKEYBOARDXCOORDINATE;
	configNames_[fieldOffsetCfgIDownKeyboardYCoordinate                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DOWNKEYBOARDYCOORDINATE;
	configNames_[fieldOffsetCfgIDownKeyboardWidth                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DOWNKEYBOARDWIDTH;
	configNames_[fieldOffsetCfgIDownKeyboardHeight                     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DOWNKEYBOARDHEIGHT;
	configNames_[fieldOffsetCfgIMiddleKeyboardXCoordinate              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MIDDLEKEYBOARDXCOORDINATE;
	configNames_[fieldOffsetCfgIMiddleKeyboardYCoordinate              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MIDDLEKEYBOARDYCOORDINATE;
	configNames_[fieldOffsetCfgIMiddleKeyboardWidth                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MIDDLEKEYBOARDWIDTH;
	configNames_[fieldOffsetCfgIMiddleKeyboardHeight                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MIDDLEKEYBOARDHEIGHT;

	configNames_[fieldOffsetCfgIProductDiscountLevel1Label             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTDISCOUNTLEVEL1LABEL;
	configNames_[fieldOffsetCfgIProductDiscountLevel2Label             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTDISCOUNTLEVEL2LABEL;
	configNames_[fieldOffsetCfgIProductDiscountLevel3Label             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTDISCOUNTLEVEL3LABEL;

	configNames_[fieldOffsetCfgIDefaultFontName                        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEFAULTFONTNAME;
	configNames_[fieldOffsetCfgINoFontSelectedName                     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = NOFONTSELECTEDNAME;
	configNames_[fieldOffsetCfgIFontExtensions                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FONTEXTENSIONS;
	configNames_[fieldOffsetCfgISystemFontsDir                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SYSTEMFONTSDIR;
	configNames_[fieldOffsetCfgIUserFontsDir                           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USERFONTSDIR;

	configNames_[fieldOffsetCfgINotifyUIOfUpdateStatus                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = NOTIFYUIOFUPDATESTATUS;

	configNames_[fieldOffsetCfgIDelayBetweenManagerThreadMessages      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DELAYBETWEENMANAGERTHREADMESSAGES;
	configNames_[fieldOffsetCfgIDelayBetweenSessionMessages            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DELAYBETWEENSESSIONMESSAGES;

	configNames_[fieldOffsetCfgIParameterNotFoundThrowsException       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CFGIPARAMETERNOTFOUNDTHROWSEXCEPTION;

	configNames_[fieldOffsetCfgINumberOfProductTexts                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = NUMBEROFPRODUCTTEXTS;

	configNames_[fieldOffsetCfgIAccumItemsLabelFamilyStart             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = ACCUMITEMSLABELFAMILYSTART;
	configNames_[fieldOffsetCfgIAccumItemsItemLabelType                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = ACCUMITEMSITEMLABELTYPE;
	configNames_[fieldOffsetCfgIAccumItemsTotalsOnlyLabelFamilyStart   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = ACCUMITEMSTOTALSONLYLABELFAMILYSTART;
	configNames_[fieldOffsetCfgIAccumItemsItemCountRenderNotify        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = ACCUMITEMSITEMCOUNTRENDERNOTIFY;
	configNames_[fieldOffsetCfgIAccumItemsSimulatePrinting             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = ACCUMITEMSSIMULATEPRINTING;

	configNames_[fieldOffsetCfgIPrinterStatusNonRepeatingMsgsToUI      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRINTERSTATUSNONREPEATINGMSGSTOUI;

	configNames_[fieldOffsetCfgIContinuousNetworkStatusDelay                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CONTINUOUSNETWORKSTATUSDELAY;
	configNames_[fieldOffsetCfgINetworkConnectivityOnlineWaitPeriodSecs            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = NETWORKCONNECTIVITYONLINEWAITPERIODSECS;
	configNames_[fieldOffsetCfgINetworkConnectivityOfflineWaitPeriodSecs           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = NETWORKCONNECTIVITYOFFLINEWAITPERIODSECS;
	configNames_[fieldOffsetCfgINetworkConnectivityOfflineRetransmissionPeriodSecs - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = NETWORKCONNECTIVITYOFFLINERETRANSMISSIONPERIODSECS;
	configNames_[fieldOffsetCfgINetworkConnectivityInterfaceDetectionTime          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = NETWORKCONNECTIVITYINTERFACEDETECTIONTIME;

	configNames_[fieldOffsetCfgIAutoTesterNeedsLabelTaken              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = AUTOTESTERNEEDSLABELTAKEN;

	configNames_[fieldOffsetCfgIPlanogramsEnabled                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PLANOGRAMSENABLED;

	configNames_[fieldOffsetCfgIMaxBestSellerSoftkeys                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXBESTSELLERSOFTKEYS;
	configNames_[fieldOffsetCfgIMaxBestSellerSoftkeysFromFeature       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXBESTSELLERSOFTKEYSFROMFEATURE;

	configNames_[fieldOffsetCfgISoftKeyPluNumberBeforeName             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SOFTKEYPLUNUMBERBEFORENAME;
	configNames_[fieldOffsetCfgISoftKeyNewlineBetweenPluNumberAndName  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SOFTKEYNEWLINEBETWEENPLUNUMBERANDNAME;

	configNames_[fieldOffsetCfgIMessageLoggingDirectory                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MESSAGELOGGINGDIRECTORY;
	configNames_[fieldOffsetCfgIMessageLoggingInfoFilename             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MESSAGELOGGINGINFOFILENAME;
	configNames_[fieldOffsetCfgIMessageLoggingWarningsFilename         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MESSAGELOGGINGWARNINGSFILENAME;
	configNames_[fieldOffsetCfgIMessageLoggingErrorsFilename           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MESSAGELOGGINGERRORSFILENAME;
	configNames_[fieldOffsetCfgIMessageLoggingStartupShutdownFilename  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MESSAGELOGGINGSTARTUPSHUTDOWNFILENAME;
	configNames_[fieldOffsetCfgIMessageLoggingScaleEventsFilename      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MESSAGELOGGINGSCALEEVENTSFILENAME;
	configNames_[fieldOffsetCfgIMessageLoggingDataMaintenanceFilename  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MESSAGELOGGINGDATAMAINTENANCEFILENAME;
	configNames_[fieldOffsetCfgIMessageLoggingScheduledEventsFilename  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MESSAGELOGGINGSCHEDULEDEVENTSFILENAME;
	configNames_[fieldOffsetCfgIMessageLoggingMaxFileSize              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MESSAGELOGGINGMAXFILESIZE;

	configNames_[fieldOffsetCfgIRemoteScaleMaintenanceEnabled          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = REMOTESCALEMAINTENANCEENABLED;
	configNames_[fieldOffsetCfgIRemoteScaleMaintClrOutboxDays          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = REMOTESCALEMAINTCLROUTBOXDAYS;

	configNames_[fieldOffsetCfgIOperAccessLevelsForProductsEnabled     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = OPERACCESSLEVELSFORPRODUCTSENABLED;

	configNames_[fieldOffsetCfgIRecallsEnabled                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = RECALLSENABLED;

	configNames_[fieldOffsetCfgIProductSupportTextLabelNum             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTSUPPORTTEXTLABELNUM;
	configNames_[fieldOffsetCfgIProductUINutriLabelNum                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRODUCTUINUTRILABELNUM;

	configNames_[fieldOffsetCfgIUnDimScreenOnZeroWeight                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UNDIMSCREENONZEROWEIGHT;
	configNames_[fieldOffsetCfgIUnDimScreenOnNonZeroWeight             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UNDIMSCREENONNONZEROWEIGHT;
	configNames_[fieldOffsetCfgIUnDimScreenOnTapPlatter                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UNDIMSCREENONTAPPLATTER;

	configNames_[fieldOffsetCfgICommRequiresDataTypeTag                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = COMMREQUIRESDATATYPETAG;

	configNames_[fieldOffsetCfgIDownloadDir                            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DOWNLOADDIR;
	configNames_[fieldOffsetCfgIDownloadFileSpecifiers                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DOWNLOADFILESPECIFIERS;
	configNames_[fieldOffsetCfgIDownloadCheckingIntervalTimer          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DOWNLOADCHECKINGINTERVALTIMER;
	configNames_[fieldOffsetCfgIDownloadCheckingUpdateTimer            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DOWNLOADCHECKINGUPDATETIMER;
	configNames_[fieldOffsetCfgIDownloadProcessingDir                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DOWNLOADPROCESSINGDIR;
	configNames_[fieldOffsetCfgIDownloadOutputDir                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DOWNLOADOUTPUTDIR;

	configNames_[fieldOffsetCfgIMergeDir					           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MERGEDIR;
	configNames_[fieldOffsetCfgIMergeFileSpecifiers                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MERGEFILESPECIFIERS;

	configNames_[fieldOffsetCfgIKeepCopyOfProcessedData                - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = KEEPCOPYOFPROCESSEDDATA;
	configNames_[fieldOffsetCfgIMakeProcessedFilenameUnique            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAKEPROCESSEDFILENAMEUNIQUE;
	configNames_[fieldOffsetCfgIProcessedDir                           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PROCESSEDDIR;

	configNames_[fieldOffsetCfgIPythonEnabled                          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PYTHONENABLED;
	configNames_[fieldOffsetCfgISmBackendPort                          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SMBACKENDPORT;
	configNames_[fieldOffsetCfgIPythonScriptPort                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PYTHONSCRIPTPORT;
	configNames_[fieldOffsetCfgIPythonSftpUpdatePort                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PYTHONSFTPUPDATEPORT;
	configNames_[fieldOffsetCfgIPythonDBBackupResponsePort             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PYTHONDBBACKUPRESPONSEPORT;
	configNames_[fieldOffsetCfgIPythonDeviceEventMessageScriptName     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PYTHONDEVICEEVENTMESSAGESCRIPTNAME;
	configNames_[fieldOffsetCfgISendSmBackendMessageScriptName          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PYTHONSENDSMBACKENDMESSAGESCRIPTNAME;
	configNames_[fieldOffsetCfgIScaleManagementHandlesUpgrade          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SCALEMANAGEMENTHANDLESUPGRADE;

	configNames_[fieldOffsetCfgICoreDumpEnabled                        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = COREDUMPENABLED;

	configNames_[fieldOffsetCfgIDefaultPingInterval                    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEFAULTPINGINTERVAL;

	configNames_[fieldOffsetCfgISwitchToRunScreenOnStableWeight        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SWITCHTORUNSCREENONSTABLEWEIGHT;

	configNames_[fieldOffsetCfgISelfServeAlphabeticSearchQuery         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVEALPHABETICSEARCHQUERY;

	configNames_[fieldOffsetCfgILicensePackageStrings                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LICENSEPACKAGESTRINGS;
	configNames_[fieldOffsetCfgILicensedPackageStrings                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LICENSEDPACKAGESTRINGS;

	configNames_[fieldOffsetCfgIUSBPrinterOutputFilename               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USBPRINTEROUTPUTFILENAME;
	configNames_[fieldOffsetCfgIUSBPrinterXCoordinate                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USBPRINTERXCOORDINATE;
	configNames_[fieldOffsetCfgIUSBPrinterYCoordinate                  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = USBPRINTERYCOORDINATE;

	configNames_[fieldOffsetCfgINetworkPrinterDriversDirectory         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = NETWORKPRINTERDRIVERSDIRECTORY;
	configNames_[fieldOffsetCfgIKnownUsbPrinters                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = KNOWNUSBPRINTERS;
	configNames_[fieldOffsetCfgIKnownUsbPrinterDrivers                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = KNOWNUSBPRINTERDRIVERS;
	configNames_[fieldOffsetCfgIKnownLanPrinters                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = KNOWNLANPRINTERS;
	configNames_[fieldOffsetCfgIKnownLanPrinterDrivers                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = KNOWNLANPRINTERDRIVERS;

	configNames_[fieldOffsetCfgIExtPrintersPDFExtensions               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EXTPRINTERSPDFEXTENSIONS;
	configNames_[fieldOffsetCfgIExtPrintersDOCExtensions               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = EXTPRINTERSDOCEXTENSIONS;
	configNames_[fieldOffsetCfgIDefaultGhostScriptDevice               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = DEFAULTGHOSTSCRIPTDEVICE;
	configNames_[fieldOffsetCfgIGhostScriptDevices                     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GHOSTSCRIPTDEVICES;

	configNames_[fieldOffsetCfgIMfgTestServer                          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MFGTESTSERVER;

	configNames_[fieldOffsetCfgIStagingDir                             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = STAGINGDIR;

	configNames_[fieldOffsetCfgIStaticProductDataTarballName           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = STATICPRODUCTDATATARBALLNAME;

	configNames_[fieldOffsetCfgIMaxCutTestNumber                       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = MAXCUTTESTNUMBER;
	configNames_[fieldOffsetCfgIRequireMeatSpecSheet                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = REQUIREMEATSPECSHEET;

	configNames_[fieldOffsetCfgICat3AuditTrailDefaultFileName          - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CAT3AUDITTRAILDEFAULTFILENAME;
	configNames_[fieldOffsetCfgICat3WeigherConfigFileName              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CAT3WEIGHERCONFIGFILENAME;

	configNames_[fieldOffsetCfgISelfServePrintToScreenFilename         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVEPRINTTOSCREENFILENAME;
	configNames_[fieldOffsetCfgISelfServePrintToScreenLabelType        - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVEPRINTTOSCREENLABELTYPE;
	configNames_[fieldOffsetCfgISelfServePrintToScreenLabelTypeFS      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVEPRINTTOSCREENLABELTYPEFS;

	configNames_[fieldOffsetCfgIConserveContinuousStockByNotSizing     - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CONSERVECONTINUOUSSTOCKBYNOTSIZING;

	configNames_[fieldOffsetCfgIUnitPricePrintOptionsEnabled           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UNITPRICEPRINTOPTIONSENABLED;
	configNames_[fieldOffsetCfgIunitPricePrintExactCalcBackupMethod    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UNITPRICEPRINTEXACTCALCBACKUPMETHOD;

	configNames_[fieldOffsetCfgISaveLabel                              		- INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SAVELABEL;
	configNames_[fieldOffsetCfgIIgnoreVendorAndProductNumTagsWhenUpcNumTag  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = IGNOREVNPNTAGSWHENUPTAG;
	
	configNames_[fieldOffsetCfgIForceNonZeroByCountForFixedWeightItems      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FORCENONZEROBYCOUNTFORFIXEDWEIGHTITEMS;

	configNames_[fieldOffsetCfgIAllowProductionEntryModeOnWrapper      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = ALLOWPRODUCTIONENTRYMODEONWRAPPER;

	configNames_[fieldOffsetCfgIScaleSettingsFile                      - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SCALESETTINGSFILE;

	configNames_[fieldOffsetCfgIfpmManagerNotATeamPlayer               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = FPMMANAGERNOTATEAMPLAYER;
	
	configNames_[fieldOffsetCfgITemplateLabelBarcodeOffset             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = TEMPLATELABELBARCODEOFFSET;
	configNames_[fieldOffsetCfgITemplateLabelShowOperatorDialogs       - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = TEMPLATELABELSHOWOPERATORDIALOGS;

	configNames_[fieldOffsetCfgIUtilizeCustomerHookForOperateModeOnly  - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = UTILIZECUSTOMERHOOKFOROPERATEMODEONLY;

	configNames_[fieldOffsetCfgIHFPDDIpAddress                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HFPDDIPADDRESS;
	configNames_[fieldOffsetCfgIHFPDDPort                              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = HFPDDPORT;
	configNames_[fieldOffsetCfgIConsecutiveTimesClosestToBeLoggedIn    - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CONSECUTIVETIMESCLOSESTTOBELOGGEDIN;
	configNames_[fieldOffsetCfgIGetDevicesInLoginRangeInterval         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GETDEVICESINLOGINRANGEINTERVAL;
	configNames_[fieldOffsetCfgIGetDevicesInLoginRangeAge              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GETDEVICESINLOGINRANGEAGE;
	configNames_[fieldOffsetCfgIGetDevicesInRangeInterval              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GETDEVICESINRANGEINTERVAL;
	configNames_[fieldOffsetCfgIGetDevicesInRangeAge                   - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = GETDEVICESINRANGEAGE;
	
	configNames_[fieldOffsetCfgIBAMNumOfSidesRepresentingUnlimitedSides - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BAMNUMOFSIDESREPRESENTINGUNLIMITEDSIDES;

	configNames_[fieldOffsetCfgIBackupRomLabels                         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BACKUPROMLABELS;

	configNames_[fieldOffsetCfgIPrintToScreenBarcodeLabelType           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRINTTOSCREENBARCODELABELTYPE;
	configNames_[fieldOffsetCfgIPrintToScreenBarcodeDirname             - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRINTTOSCREENBARCODEDIRNAME;
	configNames_[fieldOffsetCfgIPrintToScreenBarcodeFilename            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRINTTOSCREENBARCODEFILENAME;
	configNames_[fieldOffsetCfgIPrintToScreenBarcodeExtension           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = PRINTTOSCREENBARCODEEXTENSION;
	configNames_[fieldOffsetCfgISelfServeWeightFieldWidth               - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVEWEIGHTFIELDWIDTH;
	configNames_[fieldOffsetCfgISelfServeWeightFieldHeight              - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVEWEIGHTFIELDHEIGHT;
	configNames_[fieldOffsetCfgISelfServeWeightFieldFontSize            - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVEWEIGHTFIELDFONTSIZE;

	configNames_[fieldOffsetCfgICameraSystemPredictWaitTime				- INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CAMERASYSTEMPREDICTWAITTIME;
	configNames_[fieldOffsetCfgICameraSystemPredictLoops				- INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CAMERASYSTEMPREDICTLOOPS;
	configNames_[fieldOffsetCfgICameraSystemAllowedInOperate			- INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CAMERASYSTEMALLOWEDOPERATE;
	configNames_[fieldOffsetCfgICameraSystemAllowedInItemScreen         - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = CAMERASYSTEMALLOWEDITEMSCREEN;

	// SOF-4216
	configNames_[fieldOffsetCfgILimitRunInMax                           - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = LIMITRUNINMAX;
	configNames_[fieldOffsetSelfServeTimeout							- INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = SELFSERVETIMEOUT;
	configNames_[fieldOffsetCfgBarcodeFormatToUseOffline				- INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BARCODEFORMATTOUSEOFFLINE;
	configNames_[fieldOffsetCfgBarcodeRequestTimeout					- INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = BARCODEREQUESTTIMEOUT;
	
	//CK-RFID
	configNames_[fieldOffsetCfgIRFIDVoidLabelTypeNumber                 - INTERNAL_CONFIGURATION_FIRST_FIELDCODE] = RFIDVOIDLABELTYPENUMBER;



	// Some internal config items aren't in config.dat (either intentionally or not).
	// The methods toString(name/fieldOffsetCfg), toInt(...), and toBool(...) first look
	// in the config.dat Properties file, but won't find them there.
	// So I created a cfgIDefaults_ map to hold their values.
	// If we ever have any of these, populate that map now.

	initIndividualCfgItems();
}

const string CfgInternal::SCALEMODEL                        ("scaleModel");
const string CfgInternal::SCALENAME                         ("scaleName");
const string CfgInternal::SCALESERVICENAME                  ("scaleServiceName");
const string CfgInternal::WEBSERVICENAME                    ("webServiceName");
const string CfgInternal::APPINSTALLEREXE                   ("appInstallerExe");

const string CfgInternal::SYSTEMGRAPHICSLOCATION            ("systemGraphicsLocation");
const string CfgInternal::USERGRAPHICSLOCATION              ("userGraphicsLocation");
const string CfgInternal::IMAGESLOCATION                    ("imagesLocation");
const string CfgInternal::SYSTEMUSERROOTDIR                 ("systemUserRootDir");
const string CfgInternal::GRAPHICSEXTENSIONS                ("graphicsExtensions");
const string CfgInternal::GRAPHICFILESPECIFIERS             ("graphicFileSpecifiers");
const string CfgInternal::NOGRAPHICSELECTEDNAME             ("noGraphicSelectedName");
const string CfgInternal::SYSTEMGRAPHICSTAR                 ("systemGraphicsTar");
const string CfgInternal::USERGRAPHICSTAR                   ("userGraphicsTar");
const string CfgInternal::PRODUCTGRAPHICSTAR                ("productGraphicsTar");
const string CfgInternal::PRODUCTGRAPHICSDIR                ("productGraphicsDir");
const string CfgInternal::SYSTEMPRODUCTGRAPHICSDIR          ("systemProductGraphicsDir");
const string CfgInternal::PRODUCTGRAPHICSDEFAULTEXTENSION   ("productGraphicsDefaultExtension");
const string CfgInternal::CATEGORYGRAPHICSTAR               ("categoryGraphicsTar");
const string CfgInternal::CATEGORYGRAPHICSDIR               ("categoryGraphicsDir");
const string CfgInternal::SYSTEMCATEGORYGRAPHICSDIR         ("systemCategoryGraphicsDir");
const string CfgInternal::CATEGORYGRAPHICSDEFAULTEXTENSION  ("categoryGraphicsDefaultExtension");
const string CfgInternal::SPECIALSGRAPHICSTAR               ("specialsGraphicsTar");
const string CfgInternal::SPECIALSGRAPHICSDIR               ("specialsGraphicsDir");
const string CfgInternal::SPECIALSGRAPHICSDEFAULTEXTENSION  ("specialsGraphicsDefaultExtension");
const string CfgInternal::STREAMINGGRAPHICSTAR              ("streamingGraphicsTar");
const string CfgInternal::STREAMINGGRAPHICSDIR              ("streamingGraphicsDir");
const string CfgInternal::STREAMINGGRAPHICSDEFAULTEXTENSION ("streamingGraphicsDefaultExtension");
const string CfgInternal::SOFTKEYGRAPHICSTAR                ("softkeyGraphicsTar");
const string CfgInternal::SOFTKEYGRAPHICSDIR                ("softkeyGraphicsDir");
const string CfgInternal::SOFTKEYGRAPHICSDEFAULTEXTENSION   ("softkeyGraphicsDefaultExtension");
const string CfgInternal::RESERVEDSOFTKEYGRAPHICSDIR        ("reservedSoftkeyGraphicsDir");
const string CfgInternal::SELFSERVEGRAPHICSDIR              ("selfServeGraphicsDir");
const string CfgInternal::SYSTEMSELFSERVEGRAPHICSDIR        ("systemSelfServeGraphicsDir");
const string CfgInternal::SELFSERVEGRAPHICSDEFAULTEXTENSION ("selfServeGraphicsDefaultExtension");
const string CfgInternal::PLANOGRAMGRAPHICSDIR              ("planogramGraphicsDir");
const string CfgInternal::PLANOGRAMGRAPHICSDEFAULTEXTENSION ("planogramGraphicsDefaultExtension");
const string CfgInternal::VIDEOSDIR                         ("videosDir");
const string CfgInternal::VIDEOSDEFAULTEXTENSION            ("videosDefaultExtension");

const string CfgInternal::LANGFILENAME                      ("langFileName");

const string CfgInternal::WEBPAGESROOTDIR                   ("webPagesRootDir");
const string CfgInternal::GENERALWEBPAGEDIRNAME             ("generalWebPageDirName");
const string CfgInternal::LINUXWEBPAGEDIRNAME               ("linuxWebPageDirName");
const string CfgInternal::LANGUAGEFILESDIR                  ("languageFilesDir");
const string CfgInternal::PYTHONSCRIPTSDIR                  ("pythonScriptsDir");

const string CfgInternal::KEYMOUNTDIR                       ("keyMountDir");

const string CfgInternal::SERVICELOGINID                    ("serviceLoginId");
const string CfgInternal::SERVICEKEYREMOVETIMEOUT           ("serviceKeyRemoveTimeout");
const string CfgInternal::SERVICEKEYFILE                    ("serviceKeyFile");
const string CfgInternal::SUPERKEYFILE                      ("superKeyFile");
const string CfgInternal::SELFSERVEKEYFILE                  ("selfServeKeyFile");

const string CfgInternal::DIRECTORYSEPARATOR                ("directorySeparator");
const string CfgInternal::TEMPORARYDBDIRECTORY              ("TemporaryDBDirectory");
const string CfgInternal::DBLOCATION                        ("dbLocation");
const string CfgInternal::EMPTYDBLOCATION                   ("emptyDbLocation");
const string CfgInternal::DBDEFAULTSFILE                    ("dbDefaultsFile");
const string CfgInternal::DBBACKUPENABLED                   ("dbBackupEnabled");
const string CfgInternal::DBBACKUPLOCATION                  ("dbBackupLocation");
const string CfgInternal::DBMIRRORLOCATION                  ("dbMirrorLocation");
const string CfgInternal::DBCHECKBEFORESHUTDOWN             ("dbCheckBeforeShutdown");
const string CfgInternal::DBCHECKINDICES                    ("dbCheckIndices");
const string CfgInternal::DBCHECKINTEGRITY                  ("dbCheckIntegrity");
const string CfgInternal::DBCHECKINTEGRITYFREQUENCY         ("dbCheckIntegrityFrequency");
const string CfgInternal::DBREBUILDINDICESENABLED           ("dbRebuildIndicesEnabled");
const string CfgInternal::VERSIONINDEPENDENTBACKUPFILE      ("versionIndependentBackupFile");
const string CfgInternal::AUTOLOADVERSIONINDEPENDENTDATA    ("autoLoadVersionIndependentData");
const string CfgInternal::BACKUPVERSIONINDEPENDENTDATANOW   ("backupVersionIndependentDataNow");
const string CfgInternal::MAKECOPYOFVERSIONINDEPENDENTDATA  ("makeCopyOfVersionIndependentData");

const string CfgInternal::AUTOBACKUPDATABASELOCATION        ("autoBackupDatabaseLocation");
const string CfgInternal::AUTOBACKUPPLUDATAFILE             ("autoBackupPLUDataFile");
const string CfgInternal::AUTOBACKUPFLASHKEYDATAFILE        ("autoBackupFlashkeyDataFile");
const string CfgInternal::AUTOBACKUPLABELDATAFILE           ("autoBackupLabelDataFile");
const string CfgInternal::AUTOBACKUPOPERATORDATAFILE        ("autoBackupOperatorDataFile");
const string CfgInternal::AUTOBACKUPSTORENAMENUMBERFILE     ("autoBackupStoreNameNumberFile");
const string CfgInternal::AUTOBACKUPCONFIGFILE              ("autoBackupConfigFile");
const string CfgInternal::AUTOBACKUPSYNCHCONFIGFILE         ("autoBackupSynchConfigFile");

const string CfgInternal::PRIMARYCONSOLESESSIONPRIORITY     ("primaryConsoleSessionPriority");
const string CfgInternal::REMOTECONSOLESESSIONPRIORITY      ("remoteConsoleSessionPriority");
const string CfgInternal::MAXREMOTECONSOLESESSIONS          ("maxRemoteConsoleSessions");
const string CfgInternal::MAXCONSECUTIVERTCMESSAGES         ("maxConsecutiveRTCMessages");
const string CfgInternal::EVENTMANAGERENABLED               ("eventManagerEnabled");
const string CfgInternal::SESSIONMANAGERENABLED             ("sessionManagerEnabled");
const string CfgInternal::SESSIONMANAGERSTARTUPDELAY        ("sessionManagerStartupDelay");
const string CfgInternal::SESSIONMANAGERPERIODICTIMER       ("sessionManagerPeriodicTimer");
const string CfgInternal::REALTIMECONTROLLERENABLED         ("realTimeControllerEnabled");
const string CfgInternal::TIMERMANAGERENABLED               ("timerManagerEnabled");
const string CfgInternal::VERSIONMANAGERENABLED             ("versionManagerEnabled");
const string CfgInternal::USBENABLEDATBOOTUP                ("usbEnabledAtBootup");
const string CfgInternal::INTERRUPTXFERTIMEOUT              ("interruptXferTimeout");
const string CfgInternal::BULKXFERTIMEOUT                   ("bulkXferTimeout");
const string CfgInternal::EXPIREDRECORDSCHECKFREQUENCY      ("expiredRecordsCheckFrequency");

const string CfgInternal::FAKEWEIGHERMANAGERENABLED         ("fakeWeigherManagerEnabled");
const string CfgInternal::FAKEWEIGHERFREQUENCY              ("fakeWeigherFrequency");
const string CfgInternal::FAKEPRINTERMANAGERENABLED         ("fakePrinterManagerEnabled");
const string CfgInternal::FAKECLAMANAGERENABLED             ("fakeCLAManagerEnabled");
const string CfgInternal::FAKEWRAPPERMANAGERENABLED         ("fakeWrapperManagerEnabled");
const string CfgInternal::TESTBROWSERENABLED                ("testBrowserEnabled");
const string CfgInternal::TESTBROWSERMSGDELAY               ("testBrowserMsgDelay");
const string CfgInternal::TESTMESSAGESENABLED               ("testMessagesEnabled");

const string CfgInternal::RFIDENABLED                       ("RFIDEnabled");
const string CfgInternal::RFIDDEBOUNCETIME                  ("RFIDDebounceTime");
const string CfgInternal::RFIDCOMPORT                       ("RFIDComPort");

const string CfgInternal::DIAGNOSTICSMANAGERENABLED         ("diagnosticsManagerEnabled");
const string CfgInternal::DIAGNOSTICSDBNAME         		("diagnosticsDB");
const string CfgInternal::DIAGNOSTICSDBBACKUPNAME         	("diagnosticsDBBackup");
const string CfgInternal::DIAGNOSTICSDBOLDNAME         		("diagnosticsDBOld");

const string CfgInternal::BARCODESCANNERENABLED             ("BarcodeScannerEnabled");

const string CfgInternal::PRINTERPROBETIME                  ("printerProbeTime");
const string CfgInternal::WEIGHERPROBETIME                  ("weigherProbeTime");
const string CfgInternal::CUSTDISPPROBETIME                 ("custDispProbeTime");
const string CfgInternal::LABELAPPLIERPROBETIME             ("labelApplierProbeTime");
const string CfgInternal::WRAPPERPROBETIME                  ("wrapperProbeTime");

const string CfgInternal::USBBULKDRIVEONLYMODE              ("USBBulkDriveOnlyMode");
const string CfgInternal::USBBULKDRIVENAME                  ("USBBulkDriveName");
const string CfgInternal::COMPACTFLASHDRIVENAME             ("CompactFlashDriveName");
const string CfgInternal::PERMANENTRAMDRIVENAME             ("PermanentRamDriveName");
const string CfgInternal::TEMPORARYRAMDRIVENAME             ("TemporaryRamDriveName");
const string CfgInternal::EVOLUTIONDIRECTORY                ("evolutionDirectory");
const string CfgInternal::BOOTPARAMETERSFILENAME            ("bootParametersFileName");
const string CfgInternal::DEFAULTNETWORKCONFIGDIR           ("defaultNetworkConfigDir");
const string CfgInternal::TMPNETWORKCONFIGDIR               ("tmpNetworkConfigDir");
const string CfgInternal::FIRMWAREDIRECTORY                 ("firmwareDirectory");
const string CfgInternal::FIRMWAREGENERICFILENAME           ("firmwareGenericFilename");
const string CfgInternal::FIRMWAREFILENAMES                 ("firmwareFilenames");

const string CfgInternal::VERSIONMANAGERTEMPDIR             ("VersionManagerTempDir");
const string CfgInternal::VERSIONMANAGERFORCEUPGRADE        ("VersionManagerForceUpgrade");
const string CfgInternal::DEFAULTUPGRADEFILENAME            ("DefaultUpgradeFileName");
const string CfgInternal::INSTALLOGFILE                     ("instalLogFile");
const string CfgInternal::VERSIONINFOFILENAME               ("VersionInfoFileName");
const string CfgInternal::DEFAULTUPGRADEDIRECTORY           ("defaultUpgradeDirectory");
const string CfgInternal::UPGRADEZIPFILE                    ("upgradeZipFile");
const string CfgInternal::UPGRADEINIFILE                    ("upgradeIniFile");

const string CfgInternal::MAXGRAPHICSPERPRODUCT             ("maxGraphicsPerProduct");
const string CfgInternal::PRODUCTNAMEMAXLENGTH              ("productNameMaxLength");
const string CfgInternal::UPDATEPRINTERCONFIG               ("updatePrinterConfig");
const string CfgInternal::ONLYPRILANGFORPRODSTATS           ("onlyPriLangForProdStats");
const string CfgInternal::PERCENTAGEPRECISION               ("percentagePrecision");
const string CfgInternal::MAXLIFEDAYS                       ("maxLifeDays");
const string CfgInternal::MAXLIFEHOURS                      ("maxLifeHours");
const string CfgInternal::MAXLIFEMINUTES                    ("maxLifeMinutes");

const string CfgInternal::MAXBONUSPOINTS                    ("maxBonusPoints");
const string CfgInternal::QUARTERPOUNDFOR                   ("quarterPoundFor");
const string CfgInternal::HALFPOUNDFOR                      ("halfPoundFor");
const string CfgInternal::HUNDREDGRAMSFOR                   ("hundredGramsFor");
const string CfgInternal::CONTINUOUSSTATISTICSDELAY         ("continuousStatisticsDelay");
const string CfgInternal::LOSTPERIPHERALSCHECKTIME          ("lostPeripheralsCheckTime");

const string CfgInternal::KEYBOARDNETWEIGHTPRECISION        ("keyboardNetWeightPrecision");

const string CfgInternal::SUBTRACTDESCENT                   ("subtractDescent");
const string CfgInternal::MAXLABELNUMBER                    ("maxLabelNumber");
const string CfgInternal::LABELFEEDHEELFIRST                ("labelFeedHeelFirst");
const string CfgInternal::TESTLABELTYPE                     ("testLabelType");
const string CfgInternal::IGNORELABELFIT                    ("ignoreLabelFit");
const string CfgInternal::LABELHORIZONTALOFFSET             ("labelHorizontalOffset");
const string CfgInternal::DEFAULTNUTRILABELTEMPLATE         ("defaultNutriLabelTemplate");
const string CfgInternal::SHELFLIFEEXPLABELFAMILY           ("shelfLifeExpLabelFamily");
const string CfgInternal::EXPIRATIONLABELEXPIREDLIMIT       ("expirationLabelExpiredLimit");
const string CfgInternal::RUNTOTALSLABELFAMILY              ("runTotalsLabelFamily");

const string CfgInternal::FBWFACTIVATED                     ("FBWFActivated");
const string CfgInternal::USINGRESTRICTEDLOGINS             ("usingRestrictedLogins");
const string CfgInternal::APPINSTALLERISSERVICE             ("appInstallerIsService");
const string CfgInternal::STARTUPDELAY                      ("startupDelay");
const string CfgInternal::SIMULATEPRINTING                  ("simulatePrinting");
const string CfgInternal::MONITORMEMORYINTERVAL             ("monitorMemoryInterval");

const string CfgInternal::DUALRANGELIMITAVOIR               ("dualRangeLimitAvoir");
const string CfgInternal::DUALRANGELIMITMETRIC              ("dualRangeLimitMetric");
const string CfgInternal::REZEROWEIGHTLIMITAVOIR            ("rezeroWeightLimitAvoir");
const string CfgInternal::REZEROWEIGHTLIMITMETRIC           ("rezeroWeightLimitMetric");

const string CfgInternal::CONNECTIONIDENTIFICATIONTIMER     ("connectionIdentificationTimer");
const string CfgInternal::CONNECTIONIDENTIFICATIONDELAY     ("connectionIdentificationDelay");
const string CfgInternal::COMMSESSIONREADMESSAGETIMER       ("commSessionReadMessageTimer");
const string CfgInternal::COMMSESSIONREADMESSAGEDELAY       ("commSessionReadMessageDelay");
const string CfgInternal::WAITBEFORECONNECTINGTIMER         ("waitBeforeConnectingTimer");
const string CfgInternal::WAITBEFORECONNECTINGDELAY         ("waitBeforeConnectingDelay");

const string CfgInternal::PINGCONNECTIVITYFREQUENCY         ("pingConnectivityFrequency");
const string CfgInternal::PINGCONNECTIVITYATTEMPTS          ("pingConnectivityAttempts");
const string CfgInternal::OFFLINEPINGCONNECTIVITYFREQUENCY  ("offlinePingConnectivityFrequency");
const string CfgInternal::OFFLINEPINGCONNECTIVITYATTEMPTS   ("offlinePingConnectivityAttempts");

const string CfgInternal::SYNCHRONIZATIONENABLED               ("synchronizationEnabled");
const string CfgInternal::SYNCHRONIZEALLSECONDARIESTIMER       ("synchronizeAllSecondariesTimer");
const string CfgInternal::NOTIFYLOCALBATCHESTIMER              ("notifyLocalBatchesTimer");
const string CfgInternal::RECORDSPERSYNCITERATION              ("recordsPerSyncIteration");
const string CfgInternal::DELAYPERSYNCITERATION                ("delayPerSyncIteration");
const string CfgInternal::MAXIMUMNUMBERRETRIES                 ("maximumNumberRetries");
const string CfgInternal::RETRYCOMMANDTIMER                    ("retryCommandTimer");
const string CfgInternal::HOURSBEFOREDEREGISTERING             ("hoursBeforeDeregistering");
const string CfgInternal::DELAYPURGINGBATCHES                  ("delayPurgingBatches");
const string CfgInternal::HOURSBEFOREREMOVINGDELETEDBATCHES    ("hoursBeforeRemovingDeletedBatches");
const string CfgInternal::RECORDSPERAPPLYITERATION             ("recordsPerApplyIteration");
const string CfgInternal::DELAYPERAPPLYITERATION               ("delayPerApplyIteration");
const string CfgInternal::DELAYPERDELETEITERATION              ("delayPerDeleteIteration");
const string CfgInternal::SYNCHRONIZATIONSTATUSMESSAGESENABLED ("synchronizationStatusMessagesEnabled");

const string CfgInternal::USEPRODUCTIONPLANNINGDIALOG       ("useProductionPlanningDialog");
const string CfgInternal::HTTPREQUESTMETHOD                 ("httpRequestMethod");
const string CfgInternal::HTTPREQUESTCONTENTTYPE            ("httpRequestContentType");
const string CfgInternal::HTTPWAITINGDELAY                  ("httpWaitingDelay");
const string CfgInternal::HTTPREADRESPONSETIMER             ("httpReadResponseTimer");
const string CfgInternal::HTTPREADSOMERESPONSEDELAY         ("httpReadSomeResponseDelay");
const string CfgInternal::HTTPREADNORESPONSEDELAY           ("httpReadNoResponseDelay");
const string CfgInternal::HTTPMAXRESPONSELINELENGTH         ("httpMaxResponseLineLength");
const string CfgInternal::HTTPREPORTERRORS                  ("httpReportErrors");
const string CfgInternal::HTTPQUERYSTORE                    ("httpQueryStore");
const string CfgInternal::HTTPQUERYDEPT                     ("httpQueryDept");
const string CfgInternal::HTTPQUERYMODEL                    ("httpQueryModel");
const string CfgInternal::HTTPQUERYPLU                      ("httpQueryPLU");
const string CfgInternal::HTTPQUERYWEIGHT                   ("httpQueryWeight");
const string CfgInternal::HTTPQUERYQUANTITY                 ("httpQueryQuantity");
const string CfgInternal::HTTPQUERYLANGUAGE                 ("httpQueryLanguage");
const string CfgInternal::HTTPUSEOFFLINESPOOL               ("httpUseOfflineSpool");
const string CfgInternal::HTTPOFFLINESPOOLRETRY             ("httpOfflineSpoolRetry");
const string CfgInternal::HTTPOFFLINESPOOLMAXREQUESTAGE     ("httpOfflineSpoolMaxRequestAge");

const string CfgInternal::EXPANDEDTEXTWIDTH                 ("expandedTextWidth");
const string CfgInternal::EXPANDEDTEXTHEIGHT                ("expandedTextHeight");
const string CfgInternal::EXPANDEDTEXTFONTNAME              ("expandedTextFontName");
const string CfgInternal::EXPANDEDTEXTPOINTSIZE             ("expandedTextPointSize");
const string CfgInternal::SPECIALMESSAGEWIDTH               ("specialMessageWidth");
const string CfgInternal::SPECIALMESSAGEHEIGHT              ("specialMessageHeight");
const string CfgInternal::SPECIALMESSAGEFONTNAME            ("specialMessageFontName");
const string CfgInternal::SPECIALMESSAGEPOINTSIZE           ("specialMessagePointSize");
const string CfgInternal::PRODUCTNOTEWIDTH                  ("productNoteWidth");
const string CfgInternal::PRODUCTNOTEHEIGHT                 ("productNoteHeight");
const string CfgInternal::PRODUCTNOTEFONTNAME               ("productNoteFontName");
const string CfgInternal::PRODUCTNOTEPOINTSIZE              ("productNotePointSize");
const string CfgInternal::MARQUEETEXTWIDTH                  ("marqueeTextWidth");
const string CfgInternal::MARQUEETEXTHEIGHT                 ("marqueeTextHeight");
const string CfgInternal::MARQUEETEXTFONTNAME               ("marqueeTextFontName");
const string CfgInternal::MARQUEETEXTPOINTSIZE              ("marqueeTextPointSize");
const string CfgInternal::PRODUCTTEXT5WIDTH                 ("productText5Width");
const string CfgInternal::PRODUCTTEXT5HEIGHT                ("productText5Height");
const string CfgInternal::PRODUCTTEXT5FONTNAME              ("productText5FontName");
const string CfgInternal::PRODUCTTEXT5POINTSIZE             ("productText5PointSize");
const string CfgInternal::PRODUCTTEXT6WIDTH                 ("productText6Width");
const string CfgInternal::PRODUCTTEXT6HEIGHT                ("productText6Height");
const string CfgInternal::PRODUCTTEXT6FONTNAME              ("productText6FontName");
const string CfgInternal::PRODUCTTEXT6POINTSIZE             ("productText6PointSize");
const string CfgInternal::PRODUCTTEXT7WIDTH                 ("productText7Width");
const string CfgInternal::PRODUCTTEXT7HEIGHT                ("productText7Height");
const string CfgInternal::PRODUCTTEXT7FONTNAME              ("productText7FontName");
const string CfgInternal::PRODUCTTEXT7POINTSIZE             ("productText7PointSize");
const string CfgInternal::PRODUCTTEXT8WIDTH                 ("productText8Width");
const string CfgInternal::PRODUCTTEXT8HEIGHT                ("productText8Height");
const string CfgInternal::PRODUCTTEXT8FONTNAME              ("productText8FontName");
const string CfgInternal::PRODUCTTEXT8POINTSIZE             ("productText8PointSize");
const string CfgInternal::PRODUCTTEXT9WIDTH                 ("productText9Width");
const string CfgInternal::PRODUCTTEXT9HEIGHT                ("productText9Height");
const string CfgInternal::PRODUCTTEXT9FONTNAME              ("productText9FontName");
const string CfgInternal::PRODUCTTEXT9POINTSIZE             ("productText9PointSize");

const string CfgInternal::PRODDESCRIPTIONWIDTH              ("prodDescriptionWidth");
const string CfgInternal::PRODDESCRIPTIONHEIGHT             ("prodDescriptionHeight");
const string CfgInternal::PRODDESCRIPTIONFONTNAME           ("prodDescriptionFontName");
const string CfgInternal::PRODDESCRIPTIONPOINTSIZE          ("prodDescriptionPointSize");
const string CfgInternal::COOLTRACKINGTEXTWIDTH             ("coolTrackingTextWidth");
const string CfgInternal::COOLTRACKINGTEXTHEIGHT            ("coolTrackingTextHeight");
const string CfgInternal::COOLTRACKINGTEXTFONTNAME          ("coolTrackingTextFontName");
const string CfgInternal::COOLTRACKINGTEXTPOINTSIZE         ("coolTrackingTextPointSize");

const string CfgInternal::MAXLABELLENGTHPCT                 ("maxLabelLengthPct");
const string CfgInternal::MAXLABELLENGTHPCTEPP              ("maxLabelLengthPctEpp");

const string CfgInternal::DIMBLENGTH1_000                   ("dimBLength1_000");
const string CfgInternal::DIMBLENGTH1_500                   ("dimBLength1_500");
const string CfgInternal::DIMBLENGTH1_750                   ("dimBLength1_750");
const string CfgInternal::DIMBLENGTH2_000                   ("dimBLength2_000");
const string CfgInternal::DIMBLENGTH2_375                   ("dimBLength2_375");
const string CfgInternal::DIMBLENGTH2_500                   ("dimBLength2_500");
const string CfgInternal::DIMBLENGTH3_000                   ("dimBLength3_000");
const string CfgInternal::DIMBLENGTH3_500                   ("dimBLength3_500");
const string CfgInternal::DIMBLENGTH4_000                   ("dimBLength4_000");
const string CfgInternal::DIMBLENGTH4_500                   ("dimBLength4_500");
const string CfgInternal::DIMBLENGTH5_000                   ("dimBLength5_000");
const string CfgInternal::DIMBLENGTH5_500                   ("dimBLength5_500");
const string CfgInternal::DIMBLENGTH6_000                   ("dimBLength6_000");
const string CfgInternal::DIMBLENGTH6_500                   ("dimBLength6_500");
const string CfgInternal::DIMBLENGTH7_000                   ("dimBLength7_000");
const string CfgInternal::DIMBLENGTH7_500                   ("dimBLength7_500");
const string CfgInternal::DIMBLENGTH8_000                   ("dimBLength8_000");
const string CfgInternal::DIMBLENGTH8_500                   ("dimBLength8_500");
const string CfgInternal::DIMBLENGTH9_000                   ("dimBLength9_000");
const string CfgInternal::DIMBLENGTH9_500                   ("dimBLength9_500");
const string CfgInternal::DIMBLENGTH10_00                   ("dimBLength10_00");
const string CfgInternal::DIMBLENGTHCONTINUOUS              ("dimBLengthContinuous");

const string CfgInternal::GCDENABLED                        ("gcdEnabled");
const string CfgInternal::GCDENABLEDSTANDALONE              ("gcdEnabledStandalone");
const string CfgInternal::DESKTOPSTORELOGOPATH              ("desktopStoreLogoPath");
const string CfgInternal::DESKTOPSTORELOGOFILENAME          ("desktopStoreLogoFileName");
const string CfgInternal::DEVICESTORELOGOPATH               ("deviceStoreLogoPath");
const string CfgInternal::DEVICESTORELOGOFILENAME           ("deviceStoreLogoFileName");
const string CfgInternal::DEVICEAPPLICATIONUPDATESDIR       ("deviceApplicationUpdatesDir");
const string CfgInternal::DEVICEAPPLICATIONUPDATESSOURCEFILENAME ("deviceApplicationUpdatesSourceFileName");
const string CfgInternal::DEVICEPLAYLISTUPDATESDIR          ("devicePlaylistUpdatesDir");
const string CfgInternal::LOCALTEMPDIR                      ("localTempDir");
const string CfgInternal::ADAPTERDESCSEARCHRNDIS            ("adapterDescSearchRNDIS");
const string CfgInternal::IPSUBNETMASK                      ("ipSubnetMask");
const string CfgInternal::IPADDRESSHLX                      ("ipAddressHLX");
const string CfgInternal::IPADDRESSGCD                      ("ipAddressGCD");
const string CfgInternal::GCDSERVERPORT                     ("gcdServerPort");
const string CfgInternal::GCDDISPLAYOBJECTXMLPATHFORMAT		("gcdDisplayObjectXmlPathFormat");
const string CfgInternal::GCDDISCOVERYMESSAGE               ("gcdDiscoveryMessage");
const string CfgInternal::GCDDISCOVERYPORT                  ("gcdDiscoveryPort");

const string CfgInternal::GOODBEEPENABLED                   ("goodBeepEnabled");
const string CfgInternal::GOODBEEPFREQUENCY                 ("goodBeepFrequency");
const string CfgInternal::GOODBEEPPERIOD                    ("goodBeepPeriod");
const string CfgInternal::GOODBEEPDELAY                     ("goodBeepDelay");
const string CfgInternal::BADBEEPENABLED                    ("badBeepEnabled");
const string CfgInternal::BADBEEPFREQUENCY                  ("badBeepFrequency");
const string CfgInternal::BADBEEPPERIOD                     ("badBeepPeriod");
const string CfgInternal::BADBEEPDELAY                      ("badBeepDelay");

const string CfgInternal::UPKEYBOARDXCOORDINATE             ("UpKeyboardXCoordinate");
const string CfgInternal::UPKEYBOARDYCOORDINATE             ("UpKeyboardYCoordinate");
const string CfgInternal::UPKEYBOARDWIDTH                   ("UpKeyboardWidth");
const string CfgInternal::UPKEYBOARDHEIGHT                  ("UpKeyboardHeight");
const string CfgInternal::DOWNKEYBOARDXCOORDINATE           ("DownKeyboardXCoordinate");
const string CfgInternal::DOWNKEYBOARDYCOORDINATE           ("DownKeyboardYCoordinate");
const string CfgInternal::DOWNKEYBOARDWIDTH                 ("DownKeyboardWidth");
const string CfgInternal::DOWNKEYBOARDHEIGHT                ("DownKeyboardHeight");
const string CfgInternal::MIDDLEKEYBOARDXCOORDINATE         ("MiddleKeyboardXCoordinate");
const string CfgInternal::MIDDLEKEYBOARDYCOORDINATE         ("MiddleKeyboardYCoordinate");
const string CfgInternal::MIDDLEKEYBOARDWIDTH               ("MiddleKeyboardWidth");
const string CfgInternal::MIDDLEKEYBOARDHEIGHT              ("MiddleKeyboardHeight");
      
const string CfgInternal::PRODUCTDISCOUNTLEVEL1LABEL        ("ProductDiscountLevel1Label");
const string CfgInternal::PRODUCTDISCOUNTLEVEL2LABEL        ("ProductDiscountLevel2Label");
const string CfgInternal::PRODUCTDISCOUNTLEVEL3LABEL        ("ProductDiscountLevel3Label");

const string CfgInternal::DEFAULTFONTNAME                   ("defaultFontName");
const string CfgInternal::NOFONTSELECTEDNAME                ("noFontSelectedName");
const string CfgInternal::FONTEXTENSIONS                    ("fontExtensions");
const string CfgInternal::SYSTEMFONTSDIR                    ("systemFontsDir");
const string CfgInternal::USERFONTSDIR                      ("userFontsDir");

const string CfgInternal::NOTIFYUIOFUPDATESTATUS            ("notifyUIOfUpdateStatus");

const string CfgInternal::DELAYBETWEENMANAGERTHREADMESSAGES    ("delayBetweenManagerThreadMessages");
const string CfgInternal::DELAYBETWEENSESSIONMESSAGES          ("delayBetweenSessionMessages");

const string CfgInternal::CFGIPARAMETERNOTFOUNDTHROWSEXCEPTION ("cfgIParameterNotFoundThrowsException");

const string CfgInternal::NUMBEROFPRODUCTTEXTS                 ("numberOfProductTexts");

const string CfgInternal::ACCUMITEMSLABELFAMILYSTART           ("accumItemsLabelFamilyStart");
const string CfgInternal::ACCUMITEMSITEMLABELTYPE              ("accumItemsItemLabelType");
const string CfgInternal::ACCUMITEMSTOTALSONLYLABELFAMILYSTART ("accumItemsTotalsOnlyLabelFamilyStart");
const string CfgInternal::ACCUMITEMSITEMCOUNTRENDERNOTIFY      ("accumItemsItemCountRenderNotify");
const string CfgInternal::ACCUMITEMSSIMULATEPRINTING           ("accumItemsSimulatePrinting");

const string CfgInternal::PRINTERSTATUSNONREPEATINGMSGSTOUI    ("printerStatusNonRepeatingMsgsToUI");

const string CfgInternal::CONTINUOUSNETWORKSTATUSDELAY                       ("continuousNetworkStatusDelay");
const string CfgInternal::NETWORKCONNECTIVITYONLINEWAITPERIODSECS            ("networkConnectivityOnlineWaitPeriodSecs");
const string CfgInternal::NETWORKCONNECTIVITYOFFLINEWAITPERIODSECS           ("networkConnectivityOfflineWaitPeriodSecs");
const string CfgInternal::NETWORKCONNECTIVITYOFFLINERETRANSMISSIONPERIODSECS ("networkConnectivityOfflineRetransmissionPeriodSecs");
const string CfgInternal::NETWORKCONNECTIVITYINTERFACEDETECTIONTIME          ("networkConnectivityInterfaceDetectionTime");

const string CfgInternal::AUTOTESTERNEEDSLABELTAKEN              ("AutoTesterNeedsLabelTaken");

const string CfgInternal::PLANOGRAMSENABLED                      ("planogramsEnabled");

const string CfgInternal::MAXBESTSELLERSOFTKEYS                  ("maxBestSellerSoftkeys");
const string CfgInternal::MAXBESTSELLERSOFTKEYSFROMFEATURE       ("maxBestSellerSoftkeysFromFeature");

const string CfgInternal::SOFTKEYPLUNUMBERBEFORENAME             ("softKeyPluNumberBeforeName");
const string CfgInternal::SOFTKEYNEWLINEBETWEENPLUNUMBERANDNAME  ("softKeyNewlineBetweenPluNumberAndName");

const string CfgInternal::MESSAGELOGGINGDIRECTORY                ("messageLoggingDirectory");
const string CfgInternal::MESSAGELOGGINGINFOFILENAME             ("messageLoggingInfoFilename");
const string CfgInternal::MESSAGELOGGINGWARNINGSFILENAME         ("messageLoggingWarningsFilename");
const string CfgInternal::MESSAGELOGGINGERRORSFILENAME           ("messageLoggingErrorsFilename");
const string CfgInternal::MESSAGELOGGINGSTARTUPSHUTDOWNFILENAME  ("messageLoggingStartupShutdownFilename");
const string CfgInternal::MESSAGELOGGINGSCALEEVENTSFILENAME      ("messageLoggingScaleEventsFilename");       // Access
const string CfgInternal::MESSAGELOGGINGDATAMAINTENANCEFILENAME  ("messageLoggingDataMaintenanceFilename");   // Access
const string CfgInternal::MESSAGELOGGINGSCHEDULEDEVENTSFILENAME  ("messageLoggingScheduledEventsFilename");   // Access
const string CfgInternal::MESSAGELOGGINGMAXFILESIZE              ("messageLoggingMaxFileSize");               // Access

const string CfgInternal::REMOTESCALEMAINTENANCEENABLED          ("remoteScaleMaintenanceEnabled");           // Access
const string CfgInternal::REMOTESCALEMAINTCLROUTBOXDAYS          ("remoteScaleMaintClrOutboxDays");           // Access

const string CfgInternal::OPERACCESSLEVELSFORPRODUCTSENABLED     ("operAccessLevelsForProductsEnabled");

const string CfgInternal::RECALLSENABLED                         ("recallsEnabled");

const string CfgInternal::PRODUCTSUPPORTTEXTLABELNUM             ("productSupportTextLabelNum");
const string CfgInternal::PRODUCTUINUTRILABELNUM                 ("productUINutriLabelNum");

const string CfgInternal::UNDIMSCREENONZEROWEIGHT                ("unDimScreenOnZeroWeight");
const string CfgInternal::UNDIMSCREENONNONZEROWEIGHT             ("unDimScreenOnNonZeroWeight");
const string CfgInternal::UNDIMSCREENONTAPPLATTER                ("unDimScreenOnTapPlatter");

const string CfgInternal::COMMREQUIRESDATATYPETAG                ("commRequiresDataTypeTag");

const string CfgInternal::DOWNLOADDIR                            ("downloadDir");
const string CfgInternal::DOWNLOADFILESPECIFIERS                 ("downloadFileSpecifiers");
const string CfgInternal::DOWNLOADCHECKINGINTERVALTIMER          ("downloadCheckingIntervalTimer");
const string CfgInternal::DOWNLOADCHECKINGUPDATETIMER            ("downloadCheckingUpdateTimer");
const string CfgInternal::DOWNLOADPROCESSINGDIR                  ("downloadProcessingDir");
const string CfgInternal::DOWNLOADOUTPUTDIR                      ("downloadOutputDir");

const string CfgInternal::MERGEDIR                               ("mergedDir");
const string CfgInternal::MERGEFILESPECIFIERS                    ("mergeFileSpecifiers");

const string CfgInternal::KEEPCOPYOFPROCESSEDDATA                ("keepCopyOfProcessedData");
const string CfgInternal::MAKEPROCESSEDFILENAMEUNIQUE            ("makeProcessedFilenameUnique");
const string CfgInternal::PROCESSEDDIR                           ("processedDir");			//	Hobart Internal use only, per LP.

const string CfgInternal::PYTHONENABLED                          ("pythonEnabled");
const string CfgInternal::SMBACKENDPORT                          ("smBackendPort");
const string CfgInternal::PYTHONSCRIPTPORT                       ("pythonScriptPort");
const string CfgInternal::PYTHONSFTPUPDATEPORT                   ("pythonSftpUpdatePort");
const string CfgInternal::PYTHONDBBACKUPRESPONSEPORT             ("pythonDBBackupResponsePort");
const string CfgInternal::PYTHONDEVICEEVENTMESSAGESCRIPTNAME     ("pythonDeviceEventMessageScriptName");
const string CfgInternal::PYTHONSENDSMBACKENDMESSAGESCRIPTNAME   ("pythonSendSmBackendMessageScriptName"); 
const string CfgInternal::PYTHONGETBOARDTYPESCRIPTNAME           ("pythonGetBoardTypeScriptName");
const string CfgInternal::SCALEMANAGEMENTHANDLESUPGRADE          ("scaleManagementHandlesUpgrade");

const string CfgInternal::COREDUMPENABLED                        ("coreDumpEnabled");

const string CfgInternal::DEFAULTPINGINTERVAL                    ("defaultPingInterval");

const string CfgInternal::SWITCHTORUNSCREENONSTABLEWEIGHT        ("switchToRunScreenOnStableWeight");

const string CfgInternal::SELFSERVEALPHABETICSEARCHQUERY         ("selfServeAlphabeticSearchQuery");

const string CfgInternal::LICENSEPACKAGESTRINGS                  ("licensePackageStrings");
const string CfgInternal::LICENSEDPACKAGESTRINGS                 ("licensedPackageStrings");

const string CfgInternal::USBPRINTEROUTPUTFILENAME               ("usbPrinterOutputFilename");
const string CfgInternal::USBPRINTERXCOORDINATE                  ("usbPrinterXCoordinate");
const string CfgInternal::USBPRINTERYCOORDINATE                  ("usbPrinterYCoordinate");

const string CfgInternal::NETWORKPRINTERDRIVERSDIRECTORY         ("networkPrinterDriversDirectory");
const string CfgInternal::KNOWNUSBPRINTERS                       ("knownUsbPrinters");
const string CfgInternal::KNOWNUSBPRINTERDRIVERS                 ("knownUsbPrinterDrivers");
const string CfgInternal::KNOWNLANPRINTERS                       ("knownLanPrinters");
const string CfgInternal::KNOWNLANPRINTERDRIVERS                 ("knownLanPrinterDrivers");

const string CfgInternal::EXTPRINTERSPDFEXTENSIONS               ("extPrintersPDFExtensions");
const string CfgInternal::EXTPRINTERSDOCEXTENSIONS               ("extPrintersDOCExtensions");
const string CfgInternal::DEFAULTGHOSTSCRIPTDEVICE               ("defaultGhostScriptDevice");
const string CfgInternal::GHOSTSCRIPTDEVICES                     ("ghostScriptDevices");

const string CfgInternal::MFGTESTSERVER                          ("mfgTestServer");

const string CfgInternal::STAGINGDIR                             ("stagingDir");

const string CfgInternal::STATICPRODUCTDATATARBALLNAME           ("staticProductDataTarballName");

const string CfgInternal::MAXCUTTESTNUMBER                       ("maxCutTestNumber");
const string CfgInternal::REQUIREMEATSPECSHEET                   ("requireMeatSpecSheet");
const string CfgInternal::CAT3AUDITTRAILDEFAULTFILENAME          ("cat3AuditTrailDefaultFileName");
const string CfgInternal::CAT3WEIGHERCONFIGFILENAME              ("cat3WeigherConfigFileName");

const string CfgInternal::SELFSERVEPRINTTOSCREENFILENAME         ("selfServePrintToScreenFilename");
const string CfgInternal::SELFSERVEPRINTTOSCREENLABELTYPE        ("selfServePrintToScreenLabelType");
const string CfgInternal::SELFSERVEPRINTTOSCREENLABELTYPEFS      ("selfServePrintToScreenLabelTypeFS");

const string CfgInternal::CONSERVECONTINUOUSSTOCKBYNOTSIZING     ("conserveContinuousStockByNotSizing");

const string CfgInternal::UNITPRICEPRINTOPTIONSENABLED           ("unitPricePrintOptionsEnabled");
const string CfgInternal::UNITPRICEPRINTEXACTCALCBACKUPMETHOD    ("unitPricePrintExactCalcBackupMethod");

const string CfgInternal::SAVELABEL                              ("saveLabel");
const string CfgInternal::IGNOREVNPNTAGSWHENUPTAG                ("ignoreVendorAndProductNumTagsWhenUpcNumTag");
const string CfgInternal::FORCENONZEROBYCOUNTFORFIXEDWEIGHTITEMS ("forceNonZeroByCountForFixedWeightItems");

const string CfgInternal::ALLOWPRODUCTIONENTRYMODEONWRAPPER      ("allowProductionEntryModeOnWrapper");

const string CfgInternal::SCALESETTINGSFILE                      ("scaleSettingsFile");

const string CfgInternal::FPMMANAGERNOTATEAMPLAYER               ("fpmManagerNotATeamPlayer");

const string CfgInternal::TEMPLATELABELBARCODEOFFSET             ("templateLabelBarcodeOffset");
const string CfgInternal::TEMPLATELABELSHOWOPERATORDIALOGS       ("templateLabelShowOperatorDialogs");

const string CfgInternal::UTILIZECUSTOMERHOOKFOROPERATEMODEONLY  ("utilizeCustomerHookForOperateModeOnly");

const string CfgInternal::HFPDDIPADDRESS                         ("HFPDDIpAddress");
const string CfgInternal::HFPDDPORT                              ("HFPDDPort");
const string CfgInternal::CONSECUTIVETIMESCLOSESTTOBELOGGEDIN    ("consecutiveTimesClosestToBeLoggedIn");
const string CfgInternal::GETDEVICESINLOGINRANGEINTERVAL         ("getDevicesInLoginRangeInterval");
const string CfgInternal::GETDEVICESINLOGINRANGEAGE              ("getDevicesInLoginRangeAge");
const string CfgInternal::GETDEVICESINRANGEINTERVAL              ("getDevicesInRangeInterval");
const string CfgInternal::GETDEVICESINRANGEAGE                   ("getDevicesInRangeAge");
const string CfgInternal::IGNORELABELTAKENDIECUT                 ("ignoreLabelTakenDieCut");

const string CfgInternal::BAMNUMOFSIDESREPRESENTINGUNLIMITEDSIDES ("bamNumOfSidesRepresentingUnlimitedSides");

const string CfgInternal::BACKUPROMLABELS                         ("backupRomLabels");

const string CfgInternal::PRINTTOSCREENBARCODELABELTYPE           ("printToScreenBarcodeLabelType");
const string CfgInternal::PRINTTOSCREENBARCODEDIRNAME             ("printToScreenBarcodeDirname");
const string CfgInternal::PRINTTOSCREENBARCODEFILENAME            ("printToScreenBarcodeFilename");
const string CfgInternal::PRINTTOSCREENBARCODEEXTENSION           ("printToScreenBarcodeExtension");
const string CfgInternal::SELFSERVEWEIGHTFIELDWIDTH               ("selfServeWeightFieldWidth");
const string CfgInternal::SELFSERVEWEIGHTFIELDHEIGHT              ("selfServeWeightFieldHeight");
const string CfgInternal::SELFSERVEWEIGHTFIELDFONTSIZE            ("selfServeWeightFiel dFontSize");

const string CfgInternal::CAMERASYSTEMPREDICTWAITTIME			("cameraSystemPredictWaitTime");
const string CfgInternal::CAMERASYSTEMPREDICTLOOPS				("cameraSystemPredictLoops");
const string CfgInternal::CAMERASYSTEMALLOWEDOPERATE			("cameraSystemAllowedInOperate");
const string CfgInternal::CAMERASYSTEMALLOWEDITEMSCREEN			("cameraSystemAllowedInItemScreen");

// SOF-4216
const string CfgInternal::LIMITRUNINMAX							("limitRunInMax");

const string CfgInternal::SELFSERVETIMEOUT("selfServeTimeout");
const string CfgInternal::BARCODEFORMATTOUSEOFFLINE				 ("barcodeToUseOffline");
const string CfgInternal::BARCODEREQUESTTIMEOUT					 ("barcodeRequestTimeout");

//CK-RFID
const string CfgInternal::RFIDVOIDLABELTYPENUMBER ("RFIDVoidLabelTypeNumber");



HobString CfgInternal::getWebPagesDir() const
{
	HobString tempDir = toString(WEBPAGESROOTDIR);
#if defined ( __linux__ )
	tempDir.append("/").append(toString(LINUXWEBPAGEDIRNAME));
#else
	tempDir.append("\\").append(toString(GENERALWEBPAGEDIRNAME));
#endif
	return tempDir;
}


//double CfgInternal::getPercentageDivisor() const {int percentagePrecision = toInt(CfgInternal::PERCENTAGEPRECISION); return (percentagePrecision >= 0) ? Power::power((double)10.0, (double)percentagePrecision) : (double)1000.0;}

void CfgInternal::initIndividualCfgItems()
{
/*
#if defined(LINUX_FIX)
	reservedSoftkeyGraphicsDirectoryName = RESERVED_SOFTKEY_GRAPHICS_PATH;
#else
	reservedSoftkeyGraphicsDirectoryName = "C:\\Program Files\\Hobart\\Access Scale\\System\\Flashkey Graphics";
#endif
*/

	// RFIDHWPresent is not read from config.dat, it is set if the rfid hardware is detected at bootup
	RFIDHWPresent = false;

	cfgIDefaults_.insert(make_pair(TEMPLATELABELBARCODEOFFSET, "20"));
	cfgIDefaults_.insert(make_pair(TEMPLATELABELSHOWOPERATORDIALOGS, "F"));
	
	cfgIDefaults_.insert(make_pair(BAMNUMOFSIDESREPRESENTINGUNLIMITEDSIDES, string("99")));
	cfgIDefaults_.insert(make_pair(UTILIZECUSTOMERHOOKFOROPERATEMODEONLY, "T"));

	cfgIDefaults_.insert(make_pair(PRINTTOSCREENBARCODELABELTYPE, string("10072")));
	
	cfgIDefaults_.insert(make_pair(UTILIZECUSTOMERHOOKFOROPERATEMODEONLY, "T"));

	cfgIDefaults_.insert(make_pair(RFIDVOIDLABELTYPENUMBER, "900"));
}

void CfgInternal::loadIndividualCfgItems()
{
	scaleModel = (ScaleModel) toInt(SCALEMODEL);

	//  Make sure there is a "/" at the end of the path.
	//  It will make it easier to add a file name or wildcard extension,
	//  and we won't have to keep doing this in the code.
	
	std::string dirSeparator(Configuration::getInstance()->getDirectorySeparator());

	systemGraphicsLocation = toString(SYSTEMGRAPHICSLOCATION);
	if (systemGraphicsLocation.find_last_of(dirSeparator) != (systemGraphicsLocation.length() - 1))
		systemGraphicsLocation.append(dirSeparator);

	userGraphicsLocation = toString(USERGRAPHICSLOCATION);
	if (userGraphicsLocation.find_last_of(dirSeparator) != (userGraphicsLocation.length() - 1))
		userGraphicsLocation.append(dirSeparator);

	imagesLocation = toString(IMAGESLOCATION);
	if (imagesLocation.find_last_of(dirSeparator) != (imagesLocation.length() - 1))
		imagesLocation.append(dirSeparator);

/*
		ServiceSpeaker::setGoodBeepEnabled(Bool::valueOf(*paramValue));
		ServiceSpeaker::setGoodBeepFrequency(Integer::valueOf(*paramValue));
		ServiceSpeaker::setGoodBeepPeriod(Integer::valueOf(*paramValue));
		ServiceSpeaker::setGoodBeepDelay(Integer::valueOf(*paramValue));
		ServiceSpeaker::setBadBeepEnabled(Bool::valueOf(*paramValue));
		ServiceSpeaker::setBadBeepFrequency(Integer::valueOf(*paramValue));
		ServiceSpeaker::setBadBeepPeriod(Integer::valueOf(*paramValue));
		ServiceSpeaker::setBadBeepDelay(Integer::valueOf(*paramValue));
*/
}

void CfgInternal::loadScaleModelCfg()
{
#if defined ( __linux__ )
	std::string scaleModelCheck( "/etc/hobart/scale-type.dat" );
#else
	std::string scaleModelCheck( "scale-type.dat" ); // TODO
#endif

	// Fastrack
	// Scale Model is now auto-detected at startup depending on what peripherals (or lack of)
	// are attached. The following file will contain the information.
	util_filesystem::File scaleModelCheckFile( scaleModelCheck );

	// check for file
	if ( !scaleModelCheckFile.exists() )
		return;

	std::ifstream f;

	// open file
	f.open( scaleModelCheck.c_str() );

	if ( !f.is_open() )
		return;

	// parse file
	while (( f.good() ) && ( !f.eof() ))
	{
		// read line
		std::string line;
		getline( f, line );

		// Note:
		//
		// At bootup, startup sequence will attempt to find an FPC to indicate we are an ESC. If
		// there is no FPC then it will look for EPP peripheral devices. In practice I have seen
		// the EPP devices take a long time to attach and enumerate and cause an unknown result to
		// happen.
		//
		// ACR-25: 'unknown' now defaults to scaleModel 3 (Service Counter)
		//         Logic change from branch-ngw-2-factory
		//
		// HT-429  Had to roll back change for ACR-25.  Cannot default 'unknown' to
		//         Service Counter, as  Access console that has no peripherals and
		//         will be set to 'unknown' in scale-type.dat.  This would cause
		//         very bad day for the plant.

		// we are an EPP
		if ( (std::string::npos != line.find( "pre-pack" ) ) ||
			 (std::string::npos != line.find( "unknown" ) ) )
		{
			Debug::trace("CfgInternal", "loadScaleModelCfg(): Set SCALEMODEL TO 2" );

			cfgIProperties_->setProperty( SCALEMODEL, "2" );
			break;
		}

		// we are an ESC
		else if ( std::string::npos != line.find( "service counter" ) )
		{
			Debug::trace("CfgInternal", "loadScaleModelCfg(): Set SCALEMODEL TO 3" );

			cfgIProperties_->setProperty( SCALEMODEL, "3" );
			break;
		}
	}

	f.close();
}
