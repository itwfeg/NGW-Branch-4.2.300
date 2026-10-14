/**
 * @file HfcPrinterDefaultsComplete.h
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling the completion of saving printer configuration defaults.
 * Created on: May 10, 2018
 */
#ifndef HFCPRINTERDEFAULTSCOMPLETE_H_
#define HFCPRINTERDEFAULTSCOMPLETE_H_
#include "evolution.h"
#include "HfcObject.h"
#include "PrinterFactoryDefaultsComplete.h"

typedef enum HfcPrinterDefaultsFieldOffsets
{
	fieldOffsetPrinterStatus = 1,

	endHfcPrinterDefaultsFieldOffsets
} HfcPrinterDefaultsFieldOffsets_t;


class HfcPrinterDefaultsComplete : public PrinterFactoryDefaultsComplete, public HfcObject
{
	public:
		HfcPrinterDefaultsComplete(const char *data);
		HfcPrinterDefaultsComplete(const char *data, int len, int formatType);

		~HfcPrinterDefaultsComplete();

		virtual const char *serialize(int format = BrowserFormat) const;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif
