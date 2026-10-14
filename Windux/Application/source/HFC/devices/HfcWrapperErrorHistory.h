/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWRAPPERERRORHISTORY_H
#define HFCWRAPPERERRORHISTORY_H

#include "WrapperErrorHistory.h"

class HobList;

class HfcWrapperErrorHistory : public WrapperErrorHistory, public HfcObject
{
	public:

		HfcWrapperErrorHistory(const char *data);

		~HfcWrapperErrorHistory();

		void init();

		const char *serialize(int format) const;
		
	private:
		HobList	*errorCodeList;
		HobList	*packCodeList;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWRAPPERERRORHISTORY_H
