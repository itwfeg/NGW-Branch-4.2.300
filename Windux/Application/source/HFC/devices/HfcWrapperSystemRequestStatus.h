/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERSYSTEMREQUESTSTATUS_H
#define HFCWRAPPERSYSTEMREQUESTSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "WrapperSystemRequestStatus.h"

class HfcWrapperSystemRequestStatus : WrapperSystemRequestStatus, public HfcObject
{
	public:

		HfcWrapperSystemRequestStatus();
		HfcWrapperSystemRequestStatus(const char *data);

		~HfcWrapperSystemRequestStatus();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCWRAPPERSYSTEMREQUESTSTATUS_H
