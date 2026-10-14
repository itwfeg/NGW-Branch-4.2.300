/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCLABELAPPLIERSTATUS_H
#define HFCLABELAPPLIERSTATUS_H

#include "evolution.h"
#include "LabelApplierStatus.h"
#include "HfcObject.h"


class HfcLabelApplierStatus : public LabelApplierStatus, public HfcObject
{
	public:

		// todo - Should we make these private, or at least protected, so that only the
		// HfcObject factory can create these?
		HfcLabelApplierStatus();
		/* Don't think that you can just provide default data for len and formatType
		 * in the second constructor below to provide the functionality for the next
		 * constructor.  These two need to call different constructor for their parent
		 * class, WeigherStatus.
		 */
		HfcLabelApplierStatus(const char *data);
		HfcLabelApplierStatus(const char *data, int len, int formatType);

		~HfcLabelApplierStatus();

		virtual const char *serialize(int format) const;

		bool operator==(HfcLabelApplierStatus &hlas);

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif //HFCLABELAPPLIERSTATUS_H
