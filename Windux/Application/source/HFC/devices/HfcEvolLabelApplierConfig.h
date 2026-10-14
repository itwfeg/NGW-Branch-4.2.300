/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLLABELAPPLIERCONFIG_H
#define HFCEVOLLABELAPPLIERCONFIG_H

#include "evolution.h"
#include "HfcObject.h"
#include "LabelApplier.h"


	//	Currently, this class is used for the LabelPlacement page in Operate. Right now, all it needs to know is if
	//	the applier has a second printer. Any changes made to the EvolLabelApplierConfiguration will be done via
	//	EvolWrapperConfiguration (CCD 4/24/06).

class EvolLabelApplierConfiguration;

class HfcEvolLabelApplierConfig : public HfcObject
{
	public:
		HfcEvolLabelApplierConfig(bool hasASecondPrinter, LaSideDischarge sideDischarge);

		~HfcEvolLabelApplierConfig();

		virtual const char *serialize(int format = BrowserFormat)const;

		bool hasSecondPrinter();
		void setSecondPrinter(bool value);

		LaSideDischarge getSideDischargeType() const;
		void setSideDischargeType(LaSideDischarge type);

		void show();


	private:

		bool			secondPrinter;
		LaSideDischarge	sideDischargeType;				//	RightDischarge or LeftDischarge.

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//	HFCEVOLLABELAPPLIERCONFIG_H
