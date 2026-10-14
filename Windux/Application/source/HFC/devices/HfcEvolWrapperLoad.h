/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCEVOLWRAPPERLOAD_H
#define HFCEVOLWRAPPERLOAD_H

#include "evolution.h"
#include "HfcObject.h"
#include "EvolWrapperLoad.h"

class HfcEvolWrapperLoad : public EvolWrapperLoad, public HfcObject
{
	public:

		HfcEvolWrapperLoad(const char *data);
		HfcEvolWrapperLoad(const char *data, int len, int formatType);

		~HfcEvolWrapperLoad();

		virtual const char *serialize(int format = InternalFormat) const;

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

};

#endif	//	HFCEVOLWRAPPERLOAD_H
