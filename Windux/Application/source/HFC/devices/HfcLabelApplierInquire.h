/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCLABELAPPLIERINQUIRE_H
#define HFCLABELAPPLIERINQUIRE_H

#include "HfcLabelApplierConfig.h"

class HfcLabelApplierInquire : public HfcLabelApplierConfig
{
	public:

		HfcLabelApplierInquire();
		HfcLabelApplierInquire(const char *data);
		HfcLabelApplierInquire(const char *data, int len, int formatType);

		~HfcLabelApplierInquire();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCLABELAPPLIERINQUIRE_H
