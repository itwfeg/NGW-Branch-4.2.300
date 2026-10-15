#ifndef HOBVERSION_H
#define HOBVERSION_H

#include "PAL_platformheader.h"
#include <string>
#include "properties/properties.h"

class HobComplexVersion;

class HobVersion
{
	public:
		typedef enum VersionType
		{
			UnknownComponent,
			ASSSysControllerHW,
			SystemControllerSW,
			XPEImageSW,
			UNUSED4,
			UNUSED5,
			UNUSED6,
			BIOS,
			DB,
			WebPages,
			UsbWeigher15KgHW,
			UsbWeigher15KgSW,
			EssPrinterHW,
			EssPrinterSW,
			EppPrinterHW,
			EppPrinterSW,
			WrapperControllerHW,
			UNUSED17,
			EscPrinterHW,
			EscPrinterSW,
			EssPrnBootCodeSW,
			EssPrnFlashUpdateSW,
			EppPrnBootCodeSW,
			EppPrnFlashUpdateSW,
			EscPrnBootCodeSW,
			EscPrnFlashUpdateSW,
			WRMasterNVDataSW,
			WRMasterBootSW,
			WRMasterAppSW,
			WRSlave1NVDataSW,
			WRSlave1BootSW,
			WRSlave1AppSW,
			WRSlave3NVDataSW,
			WRSlave3BootSW,
			WRSlave3AppSW,
			WRSlave4NVDataSW,
			WRSlave4BootSW,
			WRSlave4AppSW,
			WRSlave5NVDataSW,
			WRSlave5BootSW,
			WRSlave5AppSW,
			WRSlave6NVDataSW,
			WRSlave6BootSW,
			WRSlave6AppSW,
			ApcPrinterHW,
			ApcPrinterSW,
			ApcPrnBootCodeSW,
			ApcPrnFlashUpdateSW,
			Epp2PrinterHW,	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
			Epp2PrinterSW,	// EPP Printing Device (Atmel-based SAM-BA device) (haberrc)
			Aws2WeigherHW,	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
			Aws2WeigherSW,	// AWS Weighing Device (Atmel-based SAM-BA device) (haberrc)
			WREMasterNVDataSW,		// EWS Wrapper components
			WREMasterBootSW,
			WREMasterAppSW,
			WRESlave1NVDataSW,
			WRESlave1BootSW,
			WRESlave1AppSW,
			WRESlave3NVDataSW,
			WRESlave3BootSW,
			WRESlave3AppSW,
			WRESlave4NVDataSW,
			WRESlave4BootSW,
			WRESlave4AppSW,
			WRESlave5NVDataSW,
			WRESlave5BootSW,
			WRESlave5AppSW,
			GCDHW,
			GCDAppSW,
			Epp2PrnBootCodeSW,		// EPP Printing Device (Atmel-based SAM-BA device) (zavalaa)  Alex Zavala's types for the new XPE printer
			Epp2PrnFlashUpdateSW,	// EPP Printing Device (Atmel-based SAM-BA device) (zavalaa)
			CLAAppSW,				//9/23 Alex Zavala's types for the CLA EPSAC2
			CLADataSW,
			CLABootSW,
			CLAControllerHW,
			FpcPrinterHW,
			FpcPrinterSW,
			KpcWeigherHW,
			KpcWeigherSW,
			KpcPrinterHW,
			KpcPrinterSW,
			KpcBootloaderSW,
			//pjr-301
			ApcWeigherSW,
			TofSW,
			TofBootloader,
            SSPrinterHW,
			SSPrinterSW,
			FSSPrinterHW,
			FSSPrinterSW,
			FSSBootloaderSW,
			EndOfComponents //This is not a real component, just a marker of the end of the list. it must always be there
		}   VersionType_t;

		HobVersion();
		HobVersion(const HobVersion &ver);
		HobVersion(const char *buffer);
		HobVersion(const std::string &fullVersion);
		HobVersion(VersionType deviceType, UINT16 majorRevision, UINT16 minorRevision, UINT32 buildNo = 0);
		HobVersion(VersionType versionType, UINT16 majorRevision, UINT16 minorRevision, UINT16 engRevision, UINT32 buildNo);
		virtual ~HobVersion();

		virtual const std::string* toString();

		void getTypeDescriptor( std::string& typeDesc) const;
		VersionType getVersionType() const {return versionType_;}
		virtual void setVersionType(VersionType vt){ versionType_ = vt; buildRevisionString();}

		UINT16 getMajorRevision() const {return majorRevision_;}
		void setMajorRevision(UINT16 majorRevision){majorRevision_ = majorRevision; buildRevisionString();}

		UINT16 getMinorRevision() const { return minorRevision_;}
		void setMinorRevision(UINT16 minorRevision){minorRevision_ = minorRevision; buildRevisionString();}

		//pjr-301
		void setEngRevision(UINT16 majorRevision){engRevision_ = majorRevision; buildRevisionString();}

		UINT16 getEngRevision() const {return engRevision_;}

		UINT32 getBuildNumber() const { return buildNumber_;}
		void setBuildNumber(UINT32 buildNo){buildNumber_ = buildNo; buildRevisionString();}

		const std::string &getRevisionString() const {return revisionString_;}
		const std::string &getRevMajMinBuildStr() const {return revMajMinBuildStr_;}
		const std::string &getRevMajMinEngStr() const { return revMajMinEngStr_; }
		const std::string &getRevMajMinStr() const {return revMajMinStr_;}
		const std::string &getRevNTEPVersionStr() const;

		virtual void saveToPropertiesFile(util_env::Properties *pINI);

		static bool isAValidFullVersionString(const std::string &fullVersion);
		static bool isAValidMajorVersionString(const std::string &majVersion);

		bool isNULL() const {return ((versionType_ == UnknownComponent) && 
									(majorRevision_ == 0) && 
									(minorRevision_ == 0) &&
									(buildNumber_ == 0));}

		virtual const char *serialize() const;
		UINT32 getSerializedLength() const { return (UINT32)serializedData_.length(); }

		HobVersion& operator = (const HobVersion& hv);
		bool operator == (const HobVersion& cv) const;
		bool operator != (const HobVersion& cv) const;
		bool operator > (const HobVersion& cv) const;
		bool operator < (const HobVersion& cv) const;

		const static char VERSION_SEPERATOR = '.';

	protected:
		mutable std::string serialized_;
		VersionType versionType_;
		mutable std::string serializedData_;

	private:

		void buildRevisionString();
		//PJR-301
		UINT16 majorRevision_;
		UINT16 minorRevision_;
		UINT16 engRevision_;
		UINT32 buildNumber_;
		std::string revisionString_;
		std::string revMajMinBuildStr_;
		std::string revMajMinEngStr_;
		std::string revMajMinStr_;
		std::string revNTEPVersionStr_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};
#endif	//HOBVERSION_H
