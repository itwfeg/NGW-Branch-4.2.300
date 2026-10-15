#include <string>
#include <iostream>

using namespace std;

#include "HobVersion.h"
#include "HobComplexVersion.h"
#include "Integer.h"
#include "Debug.h"
#include "HobString.h"
#include "HobTokenizerCSV.h"
#include "MetSig.h"

using namespace util_string;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobVersion::dataType = 0;
std::string HobVersion::dataTypeName("HobVersion"); 
#endif


HobVersion::HobVersion() :	versionType_(UnknownComponent),
							majorRevision_(0),
							minorRevision_(0),
							engRevision_(0),
							buildNumber_(0),
							revisionString_(""),
							revMajMinBuildStr_(""),
							revMajMinEngStr_(""),
							revMajMinStr_("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	buildRevisionString();
}

HobVersion::HobVersion(const HobVersion& ver) :	versionType_(ver.versionType_),
												majorRevision_(ver.majorRevision_),
												minorRevision_(ver.minorRevision_),
												engRevision_(ver.engRevision_),
												buildNumber_(ver.buildNumber_),
												revisionString_(ver.revisionString_),
												revMajMinBuildStr_(ver.revMajMinBuildStr_),
												revMajMinEngStr_(ver.revMajMinEngStr_),
												revMajMinStr_(ver.revMajMinStr_),
												revNTEPVersionStr_(ver.revNTEPVersionStr_)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

HobVersion::HobVersion(const char *buffer)
{
unsigned int j = 0, i = 0;

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	char *pChar = (char *) &versionType_;
	for (i = 0 ; i < sizeof(versionType_); i++)
		*pChar++ = buffer[j++];

	pChar = (char *) &majorRevision_;
	for (i = 0; i < sizeof(majorRevision_); i++)
		*pChar++ = buffer[j++];

	pChar = (char *) &this->minorRevision_;
	for (i = 0; i < sizeof(this->minorRevision_); i++)
		*pChar++ = buffer[j++];

	pChar = (char *) &this->buildNumber_;
	for (i = 0; i < sizeof(this->buildNumber_); i++)
		*pChar++ = buffer[j++];

	//PJR-301
	pChar = (char *) &this->engRevision_;
	for ( i = 0; i < sizeof(this->engRevision_); i++)
	{
		*pChar++ = buffer[j++];
	}

	buildRevisionString();

}

HobVersion::HobVersion(const string &fullVersion)
{

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	versionType_ 	= HobVersion::UnknownComponent;
	majorRevision_ 	= 0;
	minorRevision_ 	= 0;
	buildNumber_ 	= 0;
	engRevision_ 	= 0;

	HobTokenizerCSV tok( &fullVersion, VERSION_SEPERATOR);
	HobString token;

	if (tok.next(&token) != -1)
	{
		versionType_ = static_cast <VersionType>(Integer::valueOf(token));
	}
	if (tok.next(&token) != -1)
	{
		majorRevision_ = static_cast <UINT16>(Integer::valueOf(token));
	}
	if (tok.next(&token) != -1)
	{
		minorRevision_ = static_cast <UINT16>(Integer::valueOf(token));
	}
	//PJR-301	
	if (tok.next(&token) != -1)
	{
		engRevision_ = static_cast <UINT16>(Integer::valueOf(token));
	}	

	if (tok.next(&token) != -1)
	{
		buildNumber_ = static_cast <UINT16>(Integer::valueOf(token));
	}


	buildRevisionString();
}

HobVersion::HobVersion(VersionType versionType, UINT16 majorRevision, UINT16 minorRevision, UINT32 buildNo):versionType_(versionType),
		majorRevision_(majorRevision), minorRevision_(minorRevision),
		buildNumber_(buildNo)
{
	buildRevisionString();

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}

//pjr-301
HobVersion::HobVersion(VersionType versionType, UINT16 majorRevision, UINT16 minorRevision, UINT16 engRevision, UINT32 buildNo):versionType_(versionType),
		majorRevision_(majorRevision), minorRevision_(minorRevision), engRevision_(engRevision), 
		buildNumber_(buildNo)
{
	buildRevisionString();

#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

}


HobVersion::~HobVersion()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void HobVersion::getTypeDescriptor(string& typeDesc) const
{

	switch (versionType_)
	{
		case SystemControllerSW:
			typeDesc = "System Controller";
			break;
		case DB:
			typeDesc = "Database";
			break;
		case UsbWeigher15KgSW:
		case Aws2WeigherSW: // AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
			typeDesc = "Weigher";
			break;
		case EssPrinterSW:
			typeDesc = "ESS Printer";
			break;
	/* Alex Zavala look into this */
		case EppPrinterSW:
			typeDesc = "EPP Printer";
		break;
		/*	Alex Zavala's code for XPE version */
		case Epp2PrinterSW:
			typeDesc = "EPP2 Printer";
			break;
		case EscPrinterSW:
			typeDesc = "ESC Printer";
			break;
		case EssPrnBootCodeSW:
			typeDesc = "Ess Printer Boot";
			break;
		case EppPrnBootCodeSW:
			typeDesc = "Epp Printer Boot";
			break;
		/* Alex Zavala's code for XPE version (values do not exist) */
		case Epp2PrnBootCodeSW:
			typeDesc = "Epp2 Printer Boot";
			break;
		case EscPrnBootCodeSW:
			typeDesc = "Esc Printer Boot";
			break;
		case EssPrnFlashUpdateSW:
			typeDesc = "Ess Printer FLUD";
			break;
		case EppPrnFlashUpdateSW:
			typeDesc = "Epp Printer FLUD";
			break;
		/* Alex Zavala's code for XPE version (values do not exist) */
		case Epp2PrnFlashUpdateSW:
			typeDesc = "Epp2 Printer FLUD";
			break;
		case EscPrnFlashUpdateSW:
			typeDesc = "Esc Printer FLUD";
			break;
		case WRMasterNVDataSW:
		case WREMasterNVDataSW:
			typeDesc = "Wrapper Master Data";
			break;
		case WRMasterBootSW:
		case WREMasterBootSW:
			typeDesc = "Wrapper Master Boot";
			break;
		case WRMasterAppSW:
		case WREMasterAppSW:
			typeDesc = "Wrapper Master App";
			break;
		case WRSlave1NVDataSW:
		case WRESlave1NVDataSW:
			typeDesc = "Wrapper Slave1 Data";
			break;
		case WRSlave1BootSW:
		case WRESlave1BootSW:
			typeDesc = "Wrapper Slave1 Boot";
			break;
		case WRSlave1AppSW:
		case WRESlave1AppSW:
			typeDesc = "Wrapper Slave1 App";
			break;
		case WRSlave3NVDataSW:
		case WRESlave3NVDataSW:
			typeDesc = "Wrapper Slave3 Data";
			break;
		case WRSlave3BootSW:
		case WRESlave3BootSW:
			typeDesc = "Wrapper Slave3 Boot";
			break;
		case WRSlave3AppSW:
		case WRESlave3AppSW:
			typeDesc = "Wrapper Slave3 App";
			break;
		case WRSlave4NVDataSW:
		case WRESlave4NVDataSW:
			typeDesc = "Wrapper Slave4 Data";
			break;
		case WRSlave4BootSW:
		case WRESlave4BootSW:
			typeDesc = "Wrapper Slave4 Boot";
			break;
		case WRSlave4AppSW:
		case WRESlave4AppSW:
			typeDesc = "Wrapper Slave4 App";
			break;
		case WRSlave5NVDataSW:
		case WRESlave5NVDataSW:
			typeDesc = "Wrapper Slave5 Data";
			break;
		case WRSlave5BootSW:
		case WRESlave5BootSW:
			typeDesc = "Wrapper Slave5 Boot";
			break;
		case WRSlave5AppSW:
		case WRESlave5AppSW:
			typeDesc = "Wrapper Slave5 App";
			break;
		case FpcPrinterSW:
			typeDesc = "FPC Peripheral App";
			break;
		case ApcPrinterSW:
			typeDesc = "APC Peripheral App";
			break;
		case KpcPrinterSW:
			typeDesc = "KPC Peripheral App";
			break;
		case GCDAppSW:
			typeDesc = "GCDManager App";
			break;
		case CLAAppSW:
			typeDesc = "CLA App";
			break;
		case CLADataSW:
			typeDesc = "CLA Data";
			break;
		case CLABootSW:
			typeDesc = "CLA Boot";
			break;
		default:
				typeDesc = "None";
		break;
	}
}

bool HobVersion::isAValidFullVersionString(const string &fullVersion)
{
	HobTokenizerCSV tok( &fullVersion, VERSION_SEPERATOR);
	HobString token;

	if (fullVersion == "")
		return false;

	if (tok.next(&token) != -1)
	{
		int tmpVT = Integer::valueOf(token);
		if ((tmpVT < 0) || (tmpVT > 255))
			return false;
	}
	if (tok.next(&token) != -1)
	{
		int tmpMaj = Integer::valueOf(token);
		if ((tmpMaj < 0) || (tmpMaj > 65536))
			return false;
	}
	//pjr301
	if (tok.next(&token) != -1)
	{
		int tmpMaj = Integer::valueOf(token);
		if ((tmpMaj < 0) || (tmpMaj > 65536))
			return false;
	}

	if (tok.next(&token) != -1)
	{
		int tmpMin = Integer::valueOf(token);

		if ((tmpMin <  0) || (tmpMin > 65536))
			return false;
	}

	return true;
}

bool HobVersion::isAValidMajorVersionString(const string &majVersion)
{
	HobTokenizerCSV tok( &majVersion, VERSION_SEPERATOR);
	HobString token;

	if (tok.next(&token) != -1)
	{
		int tmpVT = Integer::valueOf(token);
		if ((tmpVT < 0) || (tmpVT > 255))
			return false;
	}
	if (tok.next(&token) != -1)
	{
		int tmpMaj = Integer::valueOf(token);
		if ((tmpMaj < 0) || (tmpMaj > 65536))
			return false;
	}


	return true;
}

void HobVersion::buildRevisionString()
{
	Debug::trace("HobVersion", string("buildRevisionString: VersionType=") + inttostring(versionType_) + "  majorRevision_=" + inttostring(majorRevision_) +
								"  minorRevision_=" + inttostring(minorRevision_) + "  engRevision_=" + inttostring(engRevision_) + "  buildNumber_=" + inttostring(buildNumber_) );

	char tmp[20];

	sprintf(tmp, "%d", versionType_);
	revisionString_ = tmp;
	revisionString_ += ".";
	sprintf(tmp, "%d", majorRevision_);
	revisionString_ += tmp;
	revisionString_ += ".";
	sprintf(tmp, "%d", minorRevision_);
	revisionString_ += tmp;
	revisionString_ += ".";
	//pjr-301
	sprintf(tmp, "%d", engRevision_);
	revisionString_ += tmp;
	revisionString_ += ".";

	sprintf(tmp, "%d", buildNumber_);
	revisionString_ += tmp;


	sprintf(tmp, "%d", majorRevision_);
	revMajMinBuildStr_ = tmp;
	revMajMinBuildStr_ += ".";
	sprintf(tmp, "%d", minorRevision_);
	revMajMinBuildStr_ += tmp;
	revMajMinBuildStr_ += ".";
	//pjr-301
	sprintf(tmp, "%d", engRevision_);
	revMajMinBuildStr_ += tmp;
	revMajMinBuildStr_ += ".";

	sprintf(tmp, "%d", buildNumber_);
	revMajMinBuildStr_ += tmp;


	sprintf(tmp, "%d", majorRevision_);
	revMajMinEngStr_ = tmp;
	revMajMinEngStr_ += ".";
	sprintf(tmp, "%d", minorRevision_);
	revMajMinEngStr_ += tmp;
	//pjr-301
	revMajMinEngStr_ += ".";
	sprintf(tmp, "%d", engRevision_);
	revMajMinEngStr_ += tmp;

	sprintf(tmp, "%d", majorRevision_);
	revMajMinStr_ = tmp;
	revMajMinStr_ += ".";
	sprintf(tmp, "%d", minorRevision_);
	revMajMinStr_ += tmp;



	sprintf(tmp, "%d", majorRevision_);
	revNTEPVersionStr_ = tmp;
	revNTEPVersionStr_ += ".";
	sprintf(tmp, "%d", minorRevision_);
	revNTEPVersionStr_ += tmp;
	sprintf(tmp, "%02d", engRevision_);
	revNTEPVersionStr_ += tmp;
	revNTEPVersionStr_ += ".";
	revNTEPVersionStr_ += MetSig::getApplicationVersion();


	Debug::trace("HobVersion", string("buildRevisionString: revisionString_=") + revisionString_ + "  revMajMinBuildStr_=" + revMajMinBuildStr_ + "  revMajMinEngStr_=" + revMajMinEngStr_ +
								"  revMajMinStr_=" + revMajMinStr_ +  "  revNTEPVersionStr_=" + revNTEPVersionStr_);
}

const string* HobVersion::toString()
{
	serialized_ = "V::";
	serialized_ += revisionString_;

	return &serialized_;
}

HobVersion& HobVersion::operator = (const HobVersion& hv)
{
	versionType_ = hv.versionType_;
	majorRevision_ = hv.majorRevision_;
	minorRevision_ = hv.minorRevision_;
	buildNumber_ = hv.buildNumber_;
	engRevision_ = hv.engRevision_;

	buildRevisionString();

	return *this;
}

bool HobVersion::operator == (const HobVersion& cv) const
{
	return ((versionType_ == cv.versionType_) &&
		(majorRevision_ == cv.majorRevision_) &&
		(minorRevision_ == cv.minorRevision_) &&
		(buildNumber_ == cv.buildNumber_)	  &&
		(engRevision_ == cv.engRevision_));
}

bool HobVersion::operator != (const HobVersion& cv) const
{
	return ((versionType_ != cv.versionType_) ||
		(majorRevision_ != cv.majorRevision_) ||
		(minorRevision_ != cv.minorRevision_) ||
		(buildNumber_ != cv.buildNumber_)	  ||
		(engRevision_ != cv.engRevision_));
}

bool HobVersion::operator > (const HobVersion& cv) const
{
	if (majorRevision_ > cv.majorRevision_)
		return true;
	else if ((majorRevision_ == cv.majorRevision_) && (minorRevision_ > cv.minorRevision_))
		return true;
	else if ((majorRevision_ == cv.majorRevision_) && (minorRevision_ == cv.minorRevision_) &&
		(buildNumber_ > cv.buildNumber_))
		return true;
	else if ((majorRevision_ == cv.majorRevision_) && (minorRevision_ == cv.minorRevision_) &&
		(buildNumber_ == cv.buildNumber_) && (engRevision_ > cv.engRevision_))
		return true;
	else
		return false;
}

bool HobVersion::operator < (const HobVersion& cv) const
{
	if (majorRevision_ < cv.majorRevision_)
		return true;
	else if ((majorRevision_ == cv.majorRevision_) && (minorRevision_ < cv.minorRevision_))
		return true;
	else if ((majorRevision_ == cv.majorRevision_) && (minorRevision_ == cv.minorRevision_) &&
		(buildNumber_ < cv.buildNumber_))
		return true;
	else if ((majorRevision_ == cv.majorRevision_) && (minorRevision_ == cv.minorRevision_) &&
		(buildNumber_ == cv.buildNumber_) && (engRevision_ < cv.engRevision_))
		return true;
	else
		return false;

}

void HobVersion::saveToPropertiesFile(util_env::Properties *pINI)
{
string key;
char tmp[10];

	if (pINI != NULL)
	{
		sprintf(tmp, "%d", versionType_);

		key = "component.";
		key += tmp;
		key += ".version";

		pINI->setProperty(key, revisionString_);
	}	


}

const char *HobVersion::serialize() const
{

	serializedData_ = "";

	char *pChar = (char *) & versionType_;
	for (unsigned int i = 0; i < sizeof(versionType_); i++)
	{
		serializedData_.append(1, pChar[i]);
	}

	pChar = (char *) & majorRevision_;
	for (unsigned int i = 0; i < sizeof(majorRevision_); i++)
	{
		serializedData_.append(1, pChar[i]);
	}

	pChar = (char *) & minorRevision_;
	for (unsigned int i = 0; i < sizeof(minorRevision_); i++)
	{
		serializedData_.append(1, pChar[i]);
	}

	pChar = (char *) & buildNumber_;
	for (unsigned int i = 0; i < sizeof(buildNumber_); i++)
	{
		serializedData_.append(1, pChar[i]);
	}

	pChar = (char *) & engRevision_;//new parameter
	for (unsigned int i = 0; i < sizeof(engRevision_); i++)
	{
		serializedData_.append(1, pChar[i]);
	}


	return serializedData_.data();
}

const std::string &HobVersion::getRevNTEPVersionStr() const
{
	return revNTEPVersionStr_;
}
