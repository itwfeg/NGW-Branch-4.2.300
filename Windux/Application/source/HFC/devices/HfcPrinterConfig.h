/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERCONFIG_H
#define HFCPRINTERCONFIG_H

#include "evolution.h"
#include "Printer.h"
#include "HfcObject.h"
#include "PrinterConfiguration.h"
#include "Configuration.h"
#include "PrinterTags.h"

typedef enum HfcPrinterConfigFieldOffsets
{
	fieldOffsetPrPrinterType = 1,
	fieldOffsetPrPrintContrast,
	fieldOffsetPrVertPrintPosition,
	fieldOffsetPrHorizPrintPosition,
	fieldOffsetPrLabelWidth,
	fieldOffsetPrExpelPosition,
/*
	fieldOffsetPrConfigType,
	fieldOffsetPrDeviceSignature,
	fieldOffsetPrLabelWidth,
	fieldOffsetPrOutOfMediaCount,
	fieldOffsetPrConfigWord0,
	fieldOffsetPrConfigWord1,
	fieldOffsetPrConfigWord2,
	fieldOffsetPrConfigWord4,
	fieldOffsetPrConfigWord5,
	fieldOffsetPrConfigWord6,
	fieldOffsetPrConfigWord7,
	fieldOffsetPrConfigWord8,
	fieldOffsetPrConfigWord9,
	fieldOffsetPrConfigWord10,
	fieldOffsetPrConfigWord11,
	fieldOffsetPrConfigWord12,
	fieldOffsetPrConfigWord13,
	fieldOffsetPrMediaSensorType,
*/
	fieldOffsetPrSlaEjectLine,
	endHfcPrinterConfigFieldOffsets		//	MUST be last!
} HfcPrinterConfigFieldOffsets_t;

class HfcPrinterConfig : public PrinterConfiguration, public HfcObject
{
	public:
		HfcPrinterConfig();
		HfcPrinterConfig(const char *data);
		HfcPrinterConfig(const char *data, int len, int formatType);
		HfcPrinterConfig(HfcPrinterConfig *hpc);

		~HfcPrinterConfig();

		virtual const char *serialize(int format) const;

		virtual void getFieldAsString(int fieldCode, HobString &result) const;

		void update(HfcPrinterConfig *printerConfig);
		void updateField(int fieldCode, const HobString &token);

		UINT16 getExpelPosition() const;
		void setExpelPosition(UINT16 expelPosition);

		UINT16 getHorizPrintPosition() const;
		void setHorizPrintPosition(UINT16 verticalPrintPosition);

		bool isEepConfigNeeded() const;
		void setEepConfigNeeded(bool eepConfigNeeded);

		bool isDbSaveNeeded() const;
		void setDbSaveNeeded(bool dbSaveNeeded);

		void show();

		static void initFieldList();			//initializes a list of field offset in either ultima, Evolution or Browser format


	protected:
		static std::list<PrinterTags::PrinterTagFieldOffsets> fieldList_FirstPrinter;
		static std::list<PrinterTags::PrinterTagFieldOffsets> fieldList_SecondPrinter;
		static std::list<PrinterTags::PrinterTagFieldOffsets> fieldList_ThirdPrinter;


	private:
		bool eepConfigNeeded;
		bool dbSaveNeeded;
		UINT16 horizPrintPosition;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCPRINTERCONFIG_H
