/**
 * @file MtdScaleHwCfg.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash hardware configuration section
 * Created on: Oct 31, 2012
 */

#ifndef MTDSCALEHWCFG_H
#define MTDSCALEHWCFG_H
#include "HobString.h"

class MtdScaleHwCfg
{

public:

	MtdScaleHwCfg();
	MtdScaleHwCfg(const std::string &s);
	MtdScaleHwCfg(MtdScaleHwCfg *pMtdScaleHwCfg);
	~MtdScaleHwCfg();

	typedef enum
	{
		XML_PRIDISPLAYTYPE,
		XML_PRIDISPLAYSIZE,
		XML_PRI_INTERFACE,
		XML_SECDISPLAYTYPE,
		XML_SECDISPLAYSIZE,
		XML_SEC_INTERFACE,
		XML_SATADRIVEINS,
		XML_WIFIINS,
		XML_BLUETOOTH_INS,
		XML_POWERHOLD_INS,
		XML_STOUCH_INS,
		XML_PCIE_INS,
		XML_DOM_INS,
		XML_RFID_INS,
		XML_POWERHOLD_TIME,
		XML_SCAN_INS,
		XML_SCAN_TYPE,
		XML_SCAN_SUPPORT,
		XML_HCAPWEIGHER_INS,
		XML_SYSTEMBOARDVERSION,
		XML_HWENDELEMENTS

	}HW_XML_Elements;

	typedef enum
	{
		NO_SCANNER,
		LASER,
		IMAGE
	}HW_SCANNER_TYPES;

	typedef enum
	{
		NO_IMAGE_SUPPORT,
		ONE_D,
		TWO_D
	}SCAN_IMAGE_TYPE;

	typedef enum
	{
		LVDS_0,
		LVDS_1,
		PRGB,
		HDMI

	}HW_DISP_INTERFACE;

	const HobString getPrimaryDisplayType() const {return primaryDisplayType_;}
	void setPrimaryDisplayType(const HobString &type) {primaryDisplayType_ = type;}

	const HobString getPrimaryDisplaySize() const {return primaryDisplaySize_;}
	void setPrimaryDisplaySize(const HobString &size) {primaryDisplaySize_ = size;}

	 HW_DISP_INTERFACE getPrimaryDisplayInterface() {return primaryInterface_;}

	const HobString getSecondaryDisplayType() const {return secondaryDisplayType_;}
	void setSecondaryDisplayType(const HobString &type){secondaryDisplayType_ = type;}

	const HobString getSecondaryDisplaySize() const {return secondaryDisplaySize_;}
	void setSecondaryDisplaySize(const HobString &size) {secondaryDisplaySize_ = size;}

	HW_DISP_INTERFACE getSecondaryDisplayInterface() {return secondaryInterface_;}

	bool isSataDriveInstalled() const {return sataDriveInstalled_;}
	void setSataDriveInstalled(bool installed) { sataDriveInstalled_ = installed;}

	bool isWifiInstalled() const {return wifiInstalled_;}
	void setWifiInstalled(bool installed) {wifiInstalled_ = installed;}

	bool isBlueToothInstalled() const {return bluetoothInstalled_;}
	bool isPowerHoldUpInstalled() const {return powerHoldupInstalled_;}
	bool isSecondaryTouchInstalled() const {return secondaryTouchInstalled_;}
	bool isPcieModuleInstalled() const {return pcieModuleInstalled_;}
	bool isDiskOnModuleInstalled() const {return diskOnModuleInstalled_;}
	bool isRfidInstalled() const {return rfidInstalled_;}
	bool isScannerInstalled() const {return scannerInstalled_;}
	bool isHighCapWeigherInstalled() const {return highCapWeigherInstalled_;}

	int getPowerHoldUpTime() const {return powerHoldupTime_;}
	HW_SCANNER_TYPES getScannerType(){return scannerType_;}
	SCAN_IMAGE_TYPE getScannerImageSupport() {return scannerImageSupport_;}

	std::string getSystemBoardHWVersion() { return motherBoardHWVersion_; }
	void setSystemBoardHWVersion(std::string s) { motherBoardHWVersion_ = s; }

	void setScaleHardwareDefaults();
	unsigned long calculateCrc();
	unsigned long getCrc() {return crc_;}


	const std::string serializeXml() const;

private:

	HobString primaryDisplayType_;
	HobString primaryDisplaySize_;
	HW_DISP_INTERFACE primaryInterface_;

	HobString secondaryDisplayType_;
	HobString secondaryDisplaySize_;
	HW_DISP_INTERFACE secondaryInterface_;

	bool sataDriveInstalled_;
	bool wifiInstalled_;
	bool bluetoothInstalled_;
	bool powerHoldupInstalled_;
	bool secondaryTouchInstalled_;
	bool pcieModuleInstalled_;
	bool diskOnModuleInstalled_;
	bool rfidInstalled_;

	int powerHoldupTime_;

	bool scannerInstalled_;
	HW_SCANNER_TYPES scannerType_;
	SCAN_IMAGE_TYPE scannerImageSupport_;

	bool highCapWeigherInstalled_;

	std::string motherBoardHWVersion_;

	mutable unsigned long crc_;
	mutable std::string serializedData_;

	void init();

};




#endif
