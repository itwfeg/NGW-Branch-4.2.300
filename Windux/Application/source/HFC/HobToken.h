/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBTOKEN_H
#define HOBTOKEN_H

#include "HobString.h"


typedef enum HobTokenType
{
	HobStringToken,
	HobTaggedToken
} HobTokenType_t;


class HobToken
{
	public:
		virtual ~HobToken();

		static HobToken *create(const HobString &hobString, HobTokenType type = HobStringToken);

		const HobString & getRawData() const;

		virtual void operator=(const HobToken &ht);

	protected:
 		HobString rawData;

		HobToken(const HobString &rawData);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBTOKEN_H
