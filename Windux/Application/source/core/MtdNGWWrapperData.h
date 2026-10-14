/**
 * @file MtdNGWWrapperData.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash new generation wrapper section
 * Created on: july 21, 2016
 */

#ifndef MTDNGWWRAPPERDATA_H_
#define MTDNGWWRAPPERDATA_H_
#include <string>
#include <stdlib.h>

class MtdNGWWrapperData
{

public:

	MtdNGWWrapperData();

	MtdNGWWrapperData(const std::string &s);
	MtdNGWWrapperData(MtdNGWWrapperData *pMfgData);
	~MtdNGWWrapperData();


	typedef enum
	{
		XML_InstallDate,
		XML_PrimaryPrinterCounts,
		XML_PrimaryLabelRollCounts,
		XML_SecondaryPrinterCounts,
		XML_SecondaryLabelRollCounts,
		XML_TertiaryPrinterCounts,
		XML_TertiaryLabelRollCounts,
		XML_PowerOnOffCounts,
		XML_UpperFilmPercentUsage,
		XML_LowerFilmPercentUsage,
		XML_CompressorRunTime,
		XML_VacumRunTime,
		XML_CycleCounts,
		XML_EndManElements
	}WR_XML_Elements;



	std::string getInstallDate()									{ return installDate_; }
	void setInstallDate(std::string date)							{ installDate_ = date; }

	unsigned long getPrimaryPrinterCounts()							{ return primaryPrinterCounter_; }
	void setPrimaryPrinterCounter(unsigned long count)				{ primaryPrinterCounter_ = count; }
	void incrementPrimaryPrinterCounter()							{ ++primaryPrinterCounter_; }

	unsigned long getPrimaryLabelRollCounter()						{ return primaryLabelRollCounter_; }
	void setPrimaryLabelRollCounter(unsigned long count)			{ primaryLabelRollCounter_ = count;	};

	unsigned long getSecondaryPrinterCounts()						{ return secondaryPrinterCounter_; }
	void setSecondaryPrinterCounter(unsigned long count)			{ secondaryPrinterCounter_ = count; }
	void incrementSecondaryPrinterCounter()							{ ++secondaryPrinterCounter_; }

	unsigned long getSecondaryLabelRollCounter()					{ return secondaryLabelRollCounter_; }
	void setSecondaryLabelRollCounter(unsigned long count)			{ secondaryLabelRollCounter_ = count; }

	unsigned long getTertiaryPrinterCounts()						{ return tertiaryPrinterCounter_; }
	void setTertiaryPrinterCounter(unsigned long count)			 	{ tertiaryPrinterCounter_ = count; }
	void incrementTertiaryPrinterCounter()							{ ++tertiaryPrinterCounter_; }

	unsigned long getTertiaryLabelRollCounter()						{ return tertiaryLabelRollCounter_; }
	void setTertiaryLabelRollCounter(unsigned long count)			{ tertiaryLabelRollCounter_ = count; }

	unsigned long getPowerOnOffCounts()								{ return powerOnOffCounter_; }
	void setPowerOnOffCounter(unsigned long count)					{ powerOnOffCounter_ = count; }
	void incrementPowerOnOffCounter(unsigned long count)			{ powerOnOffCounter_ += count; }

	unsigned short getUpperFilmPercentUsage()						{ return upperFilmPercentUsage_; }
	void setUpperFilmPercentUsage(unsigned short usage)				{ upperFilmPercentUsage_ = usage; }
	void incrementUpperFilmPercentUsage(unsigned short usage)		{ upperFilmPercentUsage_ += usage; }

	unsigned short getLowerFilmPercentUsage()						{ return lowerFilmPercentUsage_; }
	void setLowerFilmPercentUsage(unsigned short usage)				{ lowerFilmPercentUsage_ = usage; }
	void incrementLowerFilmPercentUsage(unsigned short usage)		{ lowerFilmPercentUsage_ += usage; }

	unsigned long getCompressorRunTime()							{ return compressorRunTime_; }
	void setComperssorRunTime(long hours)							{ compressorRunTime_ = hours; }
	void incrementComperssorRunTime(long hours)						{ compressorRunTime_ += hours; }

	unsigned long getVacuumRunTime()								{ return VacuumRunTime_; }
	void setVacuumRunTime(long hours)								{ VacuumRunTime_ = hours; }
	void incrementVacuumRunTime(long hours)							{ VacuumRunTime_ += hours; }

	unsigned long getCycleCounts()									{ return cycleCounter_; }
	void setCycleCounter(unsigned long count)						{ cycleCounter_ = count; }
	void incrementCycleCounter()									{ ++cycleCounter_; }

	void setWrapperDataDefaults();

	unsigned long getCrc() {return crc_;}
	unsigned long calculateCrc();

	const std::string serializeXml() const;
	void show();

	static int dataOffset();
	static int dataLen() {return SIZEOF_SECTION;}

private:

	// /dev/mtd2
	// this section of flash is 0x20000 bytes
	//   first half is bitmaps
	//     0x10000 for uversion bitmap
	//   second half is ngw config
	static const int SIZEOF_UBOOTBITMAPS = 0x10000;
	static const int SIZEOF_SECTION = 0x10000;


	std::string installDate_;

	unsigned long  primaryPrinterCounter_;
	unsigned long  primaryLabelRollCounter_;
	unsigned long  secondaryPrinterCounter_;
	unsigned long  secondaryLabelRollCounter_;
	unsigned long  tertiaryPrinterCounter_;
	unsigned long  tertiaryLabelRollCounter_;
	unsigned long  powerOnOffCounter_;
	unsigned short upperFilmPercentUsage_;
	unsigned short lowerFilmPercentUsage_;
	unsigned long  compressorRunTime_;
	unsigned long  VacuumRunTime_;
	unsigned long  cycleCounter_;

	mutable unsigned long crc_;

	mutable std::string serializedData_;

	void init();

};
#endif

