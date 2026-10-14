/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HOBTOKENIZER_H
#define HOBTOKENIZER_H

#include "evolution.h"
#include "HobToken.h"

class HobTokenizer
{
	public:
		/* The maxTokens parameter indicates the maximum number of distinct
		 * tokens to parse from the input stream.  All remaining tokens are
		 * left untokenized.
		 * For example, if there would be 5 tokens in an input string, and
		 * maxTokens was 2, the first two tokens would be separated out; the
		 * remaining data would be considered the 3rd token (unparsed tokens 3, 4, 5).
		 * 10,2,abc,def,ghi ----- parsed into -----> 10  2  abc,def,ghi
		 *
		 * This parameter is nice if you know you only need the first two tokens,
		 * but the data actually contains 40 or 50 tokens; you don't want to spend
		 * the time parsing the rest of the data when it won't be used.
		 */
		HobTokenizer(const std::string *data, const char separator, int maxTokens = 0);
		HobTokenizer(const std::string *data, const char *separators, int maxTokens = 0);
		virtual ~HobTokenizer();

		void clearTokenList();

		virtual void parseInputString(const std::string *s, int maxTokens);

		virtual int first(HobString *token);
		virtual int first(HobToken *token);

		virtual int next(HobString *token);
		virtual int next(HobToken *token);

		virtual int getTokenNum(HobString *token, int num);
		virtual int getTokenNum(HobToken *token, int num);

		bool hasMoreTokens() const;

	protected:
		HobString separators;
		std::vector<HobToken *> tokenList;	//added 09232002
		std::vector<HobToken *>::iterator iter;
		int tokenCount;
		int currentTokenNum;
		HobTokenType tokenType;
		
	private:
		HobTokenizer();
		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HOBTOKENIZER_H
