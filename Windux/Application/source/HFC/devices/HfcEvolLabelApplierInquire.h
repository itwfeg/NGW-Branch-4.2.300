/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLLABELAPPLIERINQUIRE_H
#define HFCEVOLLABELAPPLIERINQUIRE_H

#include "HfcEvolLabelApplierConfig.h"

class HfcEvolLabelApplierInquire : public HfcEvolLabelApplierConfig
{
	public:

		HfcEvolLabelApplierInquire(bool hasASecondPrinter, LaSideDischarge sideDischarge);

		~HfcEvolLabelApplierInquire();

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCEVOLLABELAPPLIERINQUIRE_H
