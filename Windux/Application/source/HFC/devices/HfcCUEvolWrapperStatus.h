/* Access Back-room Scale Project
 * COPYRIGHT (c) 2006.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCCUEVOLWRAPPERSTATUS_H
#define HFCCUEVOLWRAPPERSTATUS_H

#include "evolution.h"
#include "CUEvolWrapperStatus.h"
#include "HfcObject.h"

class HfcCUEvolWrapperStatus : public CUEvolWrapperStatus, public HfcObject
{
	public:
		HfcCUEvolWrapperStatus(const char *data);

		~HfcCUEvolWrapperStatus();

		virtual const char *serialize(int format = BrowserFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif	//HFCCUEVOLWRAPPERSTATUS_H
