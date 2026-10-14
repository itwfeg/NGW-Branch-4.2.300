/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCLABELAPPLIERSYSTEMREQUESTSTATUS_H
#define HFCLABELAPPLIERSYSTEMREQUESTSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "LabelApplierSystemRequestStatus.h"

class HfcLabelApplierSystemRequestStatus : LabelApplierSystemRequestStatus, public HfcObject
{
	public:

		HfcLabelApplierSystemRequestStatus();
		HfcLabelApplierSystemRequestStatus(const char *data);

		~HfcLabelApplierSystemRequestStatus();

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//	HFCLABELAPPLIERSYSTEMREQUESTSTATUS_H
