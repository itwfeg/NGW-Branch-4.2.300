/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCPRINTERSTATUS_H
#define HFCPRINTERSTATUS_H

#include "evolution.h"
#include "PrinterStatus.h"
#include "HfcObject.h"


class HfcPrinterStatus : public PrinterStatus, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcPrinterStatus();
		/* Don't think that you can just provide default data for len and formatType
		 * in the second constructor below to provide the functionality for the next
		 * constructor.  These two need to call different constructor for their parent
		 * class, WeigherStatus.
		 */
		HfcPrinterStatus(const char *data);
		HfcPrinterStatus(const char *data, int len, int formatType);

		~HfcPrinterStatus();

		virtual const char *serialize(int format) const;

		bool operator==(HfcPrinterStatus &hws);

		bool notificationReasonExists();
		
	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //HFCPRINTERSTATUS_H
