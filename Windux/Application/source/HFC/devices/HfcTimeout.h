/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCTIMEOUT_H
#define HFCTIMEOUT_H

#include "evolution.h"
#include "Timeout.h"
#include "HfcObject.h"

class HfcTimeout : public Timeout, public HfcObject
{
	public:

		HfcTimeout();
		HfcTimeout(const char *data);
		HfcTimeout(const char *data, int len, int formatType);

		~HfcTimeout();

		virtual const char *serialize(int format = InternalFormat) const;
		
	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCTIMEOUT_H
