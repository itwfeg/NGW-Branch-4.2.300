/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCWEIGHERINQUIRE_H
#define HFCWEIGHERINQUIRE_H

#include "HfcWeigherConfig.h"

class HfcWeigherInquire : public HfcWeigherConfig
{
	public:

		HfcWeigherInquire();
		HfcWeigherInquire(const char *data);
		HfcWeigherInquire(const char *data, int len, int formatType);

		~HfcWeigherInquire();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HFCWEIGHERINQUIRE_H
