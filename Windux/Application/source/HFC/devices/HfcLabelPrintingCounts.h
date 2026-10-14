/* HTi Scale Project
 * COPYRIGHT (c) 2022.  All Rights Reserved.
 * ITWFEG - Hobart. Troy, Ohio  USA
 */

#ifndef HFCLABELPRINTINGCOUNTS_H
#define HFCLABELPRINTINGCOUNTS_H

#include "evolution.h"
#include "LabelPrintingCounts.h"
#include "HfcObject.h"


class HfcLabelPrintingCounts : public LabelPrintingCounts, public HfcObject
{
	public:
		HfcLabelPrintingCounts(const char *data, int len, int formatType);
		~HfcLabelPrintingCounts();
		virtual const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCLABELPRINTINGCOUNTS_H

