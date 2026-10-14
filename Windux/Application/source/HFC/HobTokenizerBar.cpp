/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizerBar.h"
#include "HobToken.h"
#include "HobTokenTagged.h"
#include "Integer.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobTokenizerBar::dataType = 0;
string HobTokenizerBar::dataTypeName("HobTokenizerBar");
#endif

HobTokenizerBar::HobTokenizerBar(const string *data, int maxTokens)
					: HobTokenizer(0, '|', maxTokens)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	if (data && !data->empty())
		parseInputString(data, maxTokens);
}

HobTokenizerBar::~HobTokenizerBar()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

void HobTokenizerBar::parseInputString(const string *data, int maxTokens)
{
	bool isTracing = Debug::isTracing("HobTokenizerBar");
	if (isTracing)
		Debug::trace("HobTokenizerBar", string("HobTokenizerBar::parseInputString - ") + ((data) ? *data : string("NULL passed in")) + "  maxTokens:" + Integer::toString(maxTokens));

	tokenCount = 0;
	currentTokenNum = 0;

	if (data && data->length() > 0)
	{
		string::size_type pos = 0, index = 0, length = data->length();
		string tokenData;
		HobToken *token = 0;
		char currentChar = 0;
		bool foundToken = false;

		while (index < length)
		{
			// Look for the first separator on this line.  The position of the separator is returned.

			pos = index;

			while (pos < length)
			{
				currentChar = data->at(pos);

				// if we find {|} in the string, then we are 'escaping' the bar, ie. there is a bar in the string, so don't consider it a separator
				if (currentChar == '{')
				{
					if ((pos + 2) < length && data->at(pos + 1) == '|' && data->at(pos + 2) == '}')
					{
						pos += 2;
						currentChar = data->at(pos);
					}
				}

				if (separators.find(currentChar) != string::npos)
					break;     // found a separator.
				else
					pos++;
			}

			/* If no first separator, and the length of the data is greater than 0,
			* then the first, and only token will be the entire string.
			*/
			foundToken = true;
			tokenData = "";
			if (pos == length)
			{
				if (index < length)
				{
					tokenData = data->substr(index);
					if ((isTracing) && (token))
						Debug::trace("HobTokenizerBar", string("token received, end of data encountered: ") +
							(token ? token->getRawData() : "NULL"));
				}
				else
				{
					foundToken = false;
					if (isTracing)
						Debug::trace("HobTokenizerBar", "no token received");
				}

				index = length;
			}
			else
			{
				tokenData = data->substr(index, pos - index);
				index = pos + 1;
				if ((isTracing) && (token))
					Debug::trace("HobTokenizerBar", string("token received, more exists: ") +
						(token ? token->getRawData() : "NULL"));
			}

			if (foundToken)
			{
				if (!tokenData.empty())
				{
					while ((pos = tokenData.find("{|}")) != string::npos)
					{
						tokenData.erase(pos + 2, 1);
						tokenData.erase(pos, 1);
					}

					while ((pos = tokenData.find("{LF}")) != string::npos)
					{
						tokenData = tokenData.substr(0, pos) + '\n' + tokenData.substr(pos + 4);
					}

					while ((pos = tokenData.find("{CRLF}")) != string::npos)
					{
						tokenData = tokenData.substr(0, pos) + '\n' + tokenData.substr(pos + 6);
					}
				}

				token = HobToken::create(tokenData, tokenType);
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
								if ((isTracing) && (token))
									Debug::trace("HobTokenizerBar", string("max tokens received, remaining data is last token: ") +
										(token ? token->getRawData() : "NULL"));
							}
						}
						index = length;
					}
				}
			}
		}

		if (isTracing)
		{
			Debug::trace("HobTokenizerBar", string("token count is: ") + Integer::toString(tokenCount));
			Debug::trace("HobTokenizerBar", string("tokens in vector: ") + Integer::toString((int)tokenList.size()));
		}
	}
}

//#include <iostream>
void HobTokenizerBar::test1(const char *a)
{
	HobString *s = new HobString(a);
	HobTokenizerBar tb(s);
	
	Debug::trace("HobTokenizerBarTest", string("tokens for: '") + *s + "'");
	while (tb.next(s) != -1)
		Debug::trace("HobTokenizerBarTest", string("     '") + *s + "'");
	
	delete s;
}
void HobTokenizerBar::test()
{
	test1("");
	test1("abc");
	test1("abc|");
	test1("|abc");
	test1("|abc|");
	test1("abc|def");
	test1("abc|def|");
	test1("abc{|}def");
	test1("abc{|def");
	test1("abc|}def");
	test1("abc{|");
	test1("{|}");
	test1("|");
	test1("abc|def{|}");
	test1("{|}abc");
	test1("|");
	test1("||");
	test1("|||");

	test1("abc{LF}def");
	test1("abc{LFdef");
	test1("abcLF}def");
	test1("abc{LF");
	test1("{LF}");
	test1("{LF}{LF}");
	test1("LF");
	test1("abc|def{LF}");
	test1("{LF}abc");

	test1("abc{CRLF}def");
	test1("abc{CRLFdef");
	test1("abcCRLF}def");
	test1("abc{CRLF");
	test1("{CRLF}");
	test1("{CRLF}{CRLF}");
	test1("CRLF");
	test1("abc|def{CRLF}");
	test1("{CRLF}abc");
	test1("{CRLF}{LF}");
	test1("{LF}{CRLF}");

	test1("abc|{LF}def");
	test1("abc{|}{LF}def");
	test1("abc{LF}|def");
	test1("abc{LF}{|}def");

	test1("abc|{CRLF}def");
	test1("abc{|}{CRLF}def");
	test1("abc{CRLF}|def");
	test1("abc{CRLF}{|}def");

}
