/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBTOKENIZERUS_H
#define HOBTOKENIZERUS_H

#include "evolution.h"
#include "HobTokenizer.h"
class HobToken;

class HobTokenizerUS : public HobTokenizer
{
	public:
		HobTokenizerUS(const HobString *data, const char separator, int maxTokens = 0);
		HobTokenizerUS(const HobString *data, const char *separators, int maxTokens = 0);
		virtual ~HobTokenizerUS();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBTOKENIZERUS_H
