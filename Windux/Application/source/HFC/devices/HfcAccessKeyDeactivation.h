/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCACCESSKEYDEACTIVATION_H
#define HFCACCESSKEYDEACTIVATION_H

#include "evolution.h"
#include "AccessKeyDeactivation.h"
#include "HfcObject.h"


class HfcAccessKeyDeactivation : public AccessKeyDeactivation, public HfcObject
{
	public:
		HfcAccessKeyDeactivation();
		HfcAccessKeyDeactivation(const char *data);
		HfcAccessKeyDeactivation(const char *data, int len, int formatType);

		~HfcAccessKeyDeactivation();

		virtual const char *serialize(int format) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCACCESSKEYDEACTIVATION_H
