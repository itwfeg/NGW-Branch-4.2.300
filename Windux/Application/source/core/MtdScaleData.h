/**
 * @file MtdScaleData.h
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash scale data section
 * Created on: Feb 2, 2017
 */
#ifndef MTDSCALEDATA_H
#define MTDSCALEDATA_H

#include <string>
#include <stdlib.h>


class MtdScaleData
{

public:

	MtdScaleData();

	MtdScaleData(const std::string &s);
	MtdScaleData(MtdScaleData *pScaleData);
	~MtdScaleData();


	typedef enum
	{
		XML_SC_InstallDate,
		XML_SC_PrimaryPrinterCounts,
		XML_SC_SecondaryPrinterCounts,
		XML_SC_TertiaryPrinterCounts,
		XML_EndSCElements
	}SC_XML_Elements;

	std::string getInstallDate()									{ return installDate_; }
	void setInstallDate(std::string date)							{ installDate_ = date; }

	unsigned long getPrimaryPrinterCounts()							{ return primaryPrinterCounter_; }
	void setPrimaryPrinterCounter(unsigned long count)				{ primaryPrinterCounter_ = count; }
	void incrementPrimaryPrinterCounter()							{ ++primaryPrinterCounter_; }

	unsigned long getSecondaryPrinterCounts()						{ return secondaryPrinterCounter_; }
	void setSecondaryPrinterCounter(unsigned long count)			{ secondaryPrinterCounter_ = count; }
	void incrementSecondaryPrinterCounter()							{ ++secondaryPrinterCounter_; }

	unsigned long getTertiaryPrinterCounts()						{ return tertiaryPrinterCounter_; }
	void setTertiaryPrinterCounter(unsigned long count)			 	{ tertiaryPrinterCounter_ = count; }
	void incrementTertiaryPrinterCounter()							{ ++tertiaryPrinterCounter_; }

	void setScaleDataDefaults();

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
	//   second half is scale config
	static const int SIZEOF_UBOOTBITMAPS = 0x10000;

	static const int SIZEOF_SECTION = 0x10000;


	std::string installDate_;

	unsigned long  primaryPrinterCounter_;
	unsigned long  secondaryPrinterCounter_;
	unsigned long  tertiaryPrinterCounter_;

	mutable unsigned long crc_;
	mutable std::string serializedData_;

	void init();

};

#endif

