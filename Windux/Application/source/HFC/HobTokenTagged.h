/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBTOKENTAGGED_H
#define	HOBTOKENTAGGED_H

#include "HobToken.h"

typedef enum TagLength
{
	EvolTagLength = 2
} TagLength_t;


class HobTokenTagged : public HobToken
{
	public:
		HobTokenTagged(const HobString &hobString, TagLength length = EvolTagLength);
		~HobTokenTagged();
		
		const HobString getTag();
		const HobString getData();

	private:
		TagLength tagLength;
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//HOBTOKENTAGGED_H
