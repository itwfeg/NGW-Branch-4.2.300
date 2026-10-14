/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 *  Corporation. Troy, Ohio  USA
 */

#ifndef HOBTOKENIZERCSV_H
#define HOBTOKENIZERCSV_H

#include "evolution.h"
#include "HobTokenizer.h"

class HobTokenizerCSV : public HobTokenizer
{
	public:
		HobTokenizerCSV(const std::string *data, int maxTokens = 0);
		HobTokenizerCSV(const std::string *data, const char separator, int maxTokens = 0);
		HobTokenizerCSV(const std::string *data, const char *separators, int maxTokens = 0);
		virtual ~HobTokenizerCSV();

		// override of base class implementation
		void parseInputString(const std::string *s, int maxTokens);

		int first(HobString *token);
		int next(HobString *token);
		int getTokenNum(HobString *token, int num);

		static void test1(const char *a);
		static void test();

	private:
		//HobTokenizerCSV();
		void processToken(HobString *token);

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBTOKENIZERCSV_H
