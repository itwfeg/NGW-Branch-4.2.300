/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCACCESSKEYACTIVATION_H
#define HFCACCESSKEYACTIVATION_H

#include "evolution.h"
#include "AccessKeyActivation.h"
#include "HfcObject.h"


class HfcAccessKeyActivation : public AccessKeyActivation, public HfcObject
{
	public:
		HfcAccessKeyActivation();
		HfcAccessKeyActivation(const char *data);
		HfcAccessKeyActivation(const char *data, int len, int formatType);

		~HfcAccessKeyActivation();

		virtual const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCACCESSKEYACTIVATION_H
