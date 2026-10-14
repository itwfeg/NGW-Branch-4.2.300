/* HT Scale Project
 * COPYRIGHT (c) 2019.  All Rights Reserved.
 * ITWFEG Hobart, Troy, Ohio  USA
 */

#ifndef HOBTOKENIZERBAR_H
#define HOBTOKENIZERBAR_H

#include "evolution.h"
#include "HobTokenizer.h"
class HobToken;

class HobTokenizerBar : public HobTokenizer
{
	public:
		HobTokenizerBar(const std::string *data, int maxTokens = 0);
		virtual ~HobTokenizerBar();

		// override of base class implementation
		void parseInputString(const std::string *s, int maxTokens);

		static void test1(const char *a);
		static void test();

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBTOKENIZERBAR_H
