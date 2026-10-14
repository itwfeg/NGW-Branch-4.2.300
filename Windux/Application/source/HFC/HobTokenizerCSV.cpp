/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerCSV.h"
#include "Integer.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobTokenizerCSV::dataType = 0;
string HobTokenizerCSV::dataTypeName("HobTokenizerCSV"); 
#endif

HobTokenizerCSV::HobTokenizerCSV(const string *data, int maxTokens)
	: HobTokenizer(0, ',', maxTokens)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (data && !data->empty())
		parseInputString(data, maxTokens);
}

HobTokenizerCSV::HobTokenizerCSV(const string *data, const char separator, int maxTokens)
	: HobTokenizer(0, separator, maxTokens)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (data && !data->empty())
		parseInputString(data, maxTokens);
}

HobTokenizerCSV::HobTokenizerCSV(const string *data, const char *separators, int maxTokens)
	: HobTokenizer(0, separators, maxTokens)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (data && !data->empty())
		parseInputString(data, maxTokens);
}

HobTokenizerCSV::~HobTokenizerCSV()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

}

void HobTokenizerCSV::parseInputString(const string *data, int maxTokens)
{
	bool isTracing = Debug::isTracing("HobTokenizerCSV");
	if (isTracing)
		Debug::trace("HobTokenizerCSV", string("HobTokenizerCSV::parseInputString - ") + ((data) ? *data : string("NULL passed in")) + "  maxTokens:" + Integer::toString(maxTokens));

	tokenCount = 0;
	currentTokenNum = 0;

	if (data && data->length() > 0)
	{
		string::size_type pos = 0, index = 0, length = data->length();
		HobToken *token = 0;
		bool insideQuote = false;
		bool quoteEnded = false;
		char currentChar = 0;

		while (index < length)
		{
			/* todo? - if there are multiple separators, don't keep creating tokens that are empty
			*         when initially finding tokens right next to each other.  Maybe create empty
			*         tokens if find the same token next to each other, but not different ones;
			*         or maybe not, if user puts in multiple spaces to separate data.
			*/

			// Look for the first separator on this line.  The position of the separator is returned.

			pos = index;
			insideQuote = false;
			quoteEnded = false;

			while (pos < length)
			{
				currentChar = data->at(pos);

				if (currentChar == '"')
				{
					if (!quoteEnded)
					{
						if (insideQuote)
						{
							// Check for double quotes inside the quoted token
							if (pos + 1 < length && data->at(pos + 1) == '"')
								++pos;
							else
							{
								quoteEnded = true;
								insideQuote = false;
							}
						}
						else
							insideQuote = true;
					}
					pos++;
				}
				else if (separators.find(currentChar) != string::npos)
				{
					if (!insideQuote)
						break;     // found a separator.
					else
						pos++;
				}
				else
					pos++;

			}

			/* If no first separator, and the length of the data is greater than 0,
			* then the first, and only token will be the entire string.
			*/
			token = 0;
			if (pos == length)
			{
				if (index < length)
				{
					token = HobToken::create(data->substr(index), tokenType);
					if (isTracing && token)
						Debug::trace("HobTokenizerCSV", string("token received, end of data encountered: ") + token->getRawData());
				}
				else if (isTracing)
					Debug::trace("HobTokenizerCSV", "no token received");

				index = length;
			}
			else
			{
				token = HobToken::create(data->substr(index, pos - index), tokenType);
				index = pos + 1;
				if (isTracing && token)
					Debug::trace("HobTokenizerCSV", string("token received, more exists: ") + token->getRawData());
			}

			if (token)
			{
				tokenList.push_back(token);
				tokenCount++;

				if (maxTokens > 0 && tokenCount == maxTokens)
				{
					// We have all the tokens asked for, so put the remaining data as the "last" token.
					if (index < length)
					{
						token = HobToken::create(data->substr(index), tokenType);
						if (token)
						{
							tokenList.push_back(token);
							tokenCount++;
							if (isTracing)
								Debug::trace("HobTokenizerCSV", HobString("max tokens received, remaining data is last token: ") + token->getRawData());
						}
					}
					index = length;
				}
			}
		}

		if (isTracing)
		{
			Debug::trace("HobTokenizerCSV", string("token count is: ") + Integer::toString(tokenCount));
			Debug::trace("HobTokenizerCSV", string("tokens in vector: ") + Integer::toString((int)tokenList.size()));
		}
	}
}

int HobTokenizerCSV::first(HobString *token)
{
	if (HobTokenizer::first(token) != -1)
	{
		/* By default, we remove leading and trailing spaces from the 'token'. */
		processToken(token);
		return 0;
	}
	return -1;
}

int HobTokenizerCSV::next(HobString *token)
{
	if (HobTokenizer::next(token) != -1)
	{
		/* By default, we remove leading and trailing spaces from the 'token'. */
		processToken(token);
		return 0;
	}
	return -1;
}

int HobTokenizerCSV::getTokenNum(HobString *token, int num)
{
	if (HobTokenizer::getTokenNum(token, num) != -1)
	{
		/* By default, we remove leading and trailing spaces from the 'token'. */
		processToken(token);
		return 0;
	}
	return -1;
}


void HobTokenizerCSV::processToken(HobString *token)
{
	if (token)
		token->stripWhiteSpace(HobString::LEADING_TRAILING);
}

void HobTokenizerCSV::test1(const char *a)
{
	HobString *s = new HobString(a);
	HobTokenizerCSV tb(s);
	Debug::trace("HobTokenizerCSVTest", string("tokens for: '") + *s + "'");
	while (tb.next(s) != -1)
		Debug::trace("HobTokenizerCSVTest", string("     '") + *s + "'");
}
void HobTokenizerCSV::test()
{
	HobString *a = new HobString();
	test1("");
	test1("abc");
	test1("abc,");
	test1(",abc");
	test1(",abc,");
	test1("abc,def");
	test1("abc,def,");
	test1("\"abc,def\"");
	test1("\",\"");
	test1(",");
	test1("abc,\"def,\"");
	test1("\",abc\"");
	test1(",");
	test1(",,");
	test1(",,,");
}
