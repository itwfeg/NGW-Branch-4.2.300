/**
 * @file FpmManager.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager manages the serial flash on the HTi motherboard
 * Created on: Feb 26, 2013
 */

#ifndef FPMMANGER_H
#include <mutex>
#include <string>

#if defined (__linux__)
#include <mtd/mtd-user.h>
#endif

#include "MtdManufacturingData.h"
#include "MtdScaleSWConfig.h"
#include "MtdScaleHwCfg.h"
#include "MtdSectionStatistics.h"
#include "MtdNGWWrapperData.h"
#include "MtdScaleData.h"
#include "SessionMessage.h"

#include "CCriticalSection.h"


class FpmManager
{
	
	public:
	
	static FpmManager *getInstance();
	~FpmManager();

	bool handleWriteMessage(SessionMessage *pMsg);

	MtdManufacturingData *readMfgScaleData() {return pScaleMfgData_;}
	MtdScaleSWConfig *readScaleSWConfigData(){return pScaleSWCfg_;}
	MtdScaleHwCfg *readScaleHWConfigData(){return pScaleHwCfg_;}
	MtdNGWWrapperData *readWrapperData(){return pNGWWrapperData_;}
	MtdScaleData *readScaleData(){return pScaleData_;}
	std::string getLoaderVersion(){return loaderVersion;}
	std::string getScaleStyle(){return scaleStyle;}

	const std::string getMFGSystemStyle();
	const std::string getFullMFGSystemStyle();
	void setMFGSystemStyle(const std::string s);

	int writeMfgScaleData();
	int writeScaleSWData();
	int writeScaleHWData();
	int writeNGWWrapperData();
	int writeScaleData();

	int isWrapperLeftToRight();
	bool isWrapperTwoPrinter();
	bool isFreeStandingSelfService();
	bool isValueMaxModelFSS();
	
	typedef enum
	{
		Undefined,
		ManufacturingData,
		ScaleSWConfiguration,
		ScaleHWConfiguration,
		SoftwareKeys,
		WrapperData,
		ScaleData
	}FPMSectorTypes;


	
	private:

	static std::mutex pInstanceMutex;
	static FpmManager *pInstance;

#if defined (__linux__)
	mtd_info_t 	fpmInfo_;
#endif

	std::string fpmMFGSection;
	std::string fpmScaleSWSection;
	std::string fpmScaleHWSection;
	std::string fpmScaleKeysSection;
	std::string fpmWrapperDataSection;
	std::string fpmScaleDataSection;

	static const unsigned char sectionBlank = 0xFF;

	/*! serial flash section objects */
	MtdManufacturingData 	*pScaleMfgData_;
	MtdScaleSWConfig 		*pScaleSWCfg_;
	MtdScaleHwCfg 			*pScaleHwCfg_;
	MtdNGWWrapperData 		*pNGWWrapperData_;
	MtdScaleData 			*pScaleData_;
	unsigned long mfgSecCounter;
	unsigned long swSecCounter;
	unsigned long hwSecCounter;
	unsigned long ngwWrapperCounter;
	unsigned long scDataCounter;

	std::string loaderVersion;
	std::string versionFile;
	std::string scaleStyle;

	unsigned char *pBuffer_;

	Util::CSmartCriticalSection writeSerialFlashLock_;

	bool fpmManagerNotATeamPlayer_;

	typedef enum _FpmErrorCode
	{
		FPM_OK = 0,
		SECTION_BLANK,
		READ_FAILURE,
		ERASE_FAILURE,
		MODULE_MISSING,
		UNSUPPORTED_DEVICE,
		UNKNOWN_PARTITION,
		OPEN_PARTITION,
		WRITE_FAILURE,
		DATA_OUTOFBOUNDS,
		WRITE_BUFFER_FAILURE,
		UNSUPPORTED_OS
	}FpmErrorCode;

	FpmManager();
	void init();

	#define NAME_OF_BOARD_SITE_FILE  "/etc/hobart/boardsite"
	
	const std::string getPartition(FPMSectorTypes sector);
	FpmErrorCode readSerialFlashPartition(FPMSectorTypes sector, bool recovery = false);
	FpmErrorCode eraseSerialFlashPartition(FPMSectorTypes sector);
	FpmErrorCode writeSerialFlashPartition(FPMSectorTypes sector, const std::string &data);

	/*!methods to flash defaults */
	FpmErrorCode loadSerialFlashDefaults();
	FpmErrorCode loadMfgSectionDefaults();
	FpmErrorCode loadSWSectionDefaults();
	FpmErrorCode loadHWSectionDefaults();
	FpmErrorCode loadNGWWrapperSectionDefaults();
	FpmErrorCode loadScaleDataDefaults();


};
#endif
