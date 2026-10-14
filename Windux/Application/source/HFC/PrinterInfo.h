/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef PRINTERINFO_H
#define PRINTERINFO_H

#include "HfcObject.h"
#include "HfcPrinterSystemConfig.h"

typedef enum PrinterInfoFieldOffsets
{
	fieldOffsetPrinterInfoAddress = 1,
	fieldOffsetPrinterInfoPrinterType,
	endPrinterInfoFieldOffsets	//	MUST be last!
}   PrinterInfoFieldOffsets_t;


class PrinterInfo : public HfcObject
{
	public:

		PrinterInfo();
		PrinterInfo(const char *data);
		PrinterInfo(const char *data, int len, int formatType);

		~PrinterInfo();

		HfcPrinterSystemConfig* getPrinterSystemConfig();
		void setPrinterSystemConfig(HfcPrinterSystemConfig *weigherSystemConfig);

		DeviceAddress getStationAddress() const;
		void setStationAddress(DeviceAddress stationAddress);

		PrinterType getPrinterType() const;
		void setPrinterType(PrinterType printerType);
/*
		bool getDidStationRespond() const;
		void setDidStationRespond(bool didStationRespond);
*/
		virtual const char *serialize(int format = InternalFormat) const;

		void update(PrinterInfo *printerInfo);


	private:

		HfcPrinterSystemConfig *printerSystemConfig;

		DeviceAddress	stationAddress;
		PrinterType		printerType;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//PRINTERINFO_H
