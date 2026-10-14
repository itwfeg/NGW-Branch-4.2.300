/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobTokenizer.h"
#include "Integer.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobTokenizer::dataType = 0;
string HobTokenizer::dataTypeName("HobTokenizer"); 
#endif

//private
HobTokenizer::HobTokenizer()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	init();
}

HobTokenizer::HobTokenizer(const string *data, const char separator, int maxTokens)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	separators = separator;

	if (data && !data->empty())
		parseInputString(data, maxTokens);
}

HobTokenizer::HobTokenizer(const string *data, const char *separators, int maxTokens)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif

	init();

	this->separators = separators;

	if (data && !data->empty())
		parseInputString(data, maxTokens);
}

HobTokenizer::~HobTokenizer()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif

	Debug::trace("HobTokenizer", HobString("Entering destructor - token count is: ") + HobString(Integer::toString((int)tokenList.size())));

	clearTokenList();
}

void HobTokenizer::init()
{
	tokenCount = 0;
	currentTokenNum = 0;
	tokenType = HobStringToken;
}

void HobTokenizer::clearTokenList()
{
	if (!tokenList.empty())
	{
		iter = tokenList.begin();
		while (iter != tokenList.end())
		{
			delete *iter;
			iter++;
		}
		tokenList.clear();
	}
}

void HobTokenizer::parseInputString(const string *data, int maxTokens)
{
	bool isTracing = Debug::isTracing("HobTokenizer");
	if (isTracing)
		Debug::trace("HobTokenizer", string("HobTokenizer::parseInputString - ") + ((data) ? *data : string("NULL passed in")) + "  maxTokens:" + Integer::toString(maxTokens));

	clearTokenList();
	tokenCount = 0;
	currentTokenNum = 0;

	if (data && data->length() > 0)
	{
		string::size_type pos = 0, index = 0, length = data->length();
		HobToken *token = 0;		 
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

			while (pos < length)
			{
				currentChar = data->at(pos);
				
				if ( separators.find(currentChar) != string::npos )
					break;     // found a separator.
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
						Debug::trace("HobTokenizer", HobString("token received, end of data encountered: ") + token->getRawData());
				}
				else if (isTracing)
					Debug::trace("HobTokenizer", HobString("no token received"));

				index = length;
			}
			else
			{
				token = HobToken::create(data->substr(index, pos-index), tokenType);
				index = pos + 1;
				if (isTracing && token)
					Debug::trace("HobTokenizer", string("token received, more exists: ") + token->getRawData());
			}

			if (token)
			{
				tokenList.push_back(token);
				tokenCount++;

				if (maxTokens > 0 && tokenCount == maxTokens)
				{
					// We have all the tokens asked for, so put the remaining data as the "last" token.
					if ( index < length )
					{
						token = HobToken::create(data->substr(index), tokenType);
						if (token)
						{
							tokenList.push_back(token);
							tokenCount++;
							if (isTracing)
								Debug::trace("HobTokenizer", string("max tokens received, remaining data is last token: ") + token->getRawData());
						}
					}
					index = length;
				}
			}
		}

		if (isTracing)
		{
			Debug::trace("HobTokenizer", string("token count is: ") + Integer::toString(tokenCount));
			Debug::trace("HobTokenizer", string("tokens in vector: ") + Integer::toString((int)tokenList.size()));
		}
	}
}

int HobTokenizer::first(HobString *token)
{
	if (tokenCount > 0)
	{
		currentTokenNum = 1;
		iter = tokenList.begin();
		HobToken *hobToken = *iter;
		*token = hobToken->getRawData();				//modified ats 09242002
		return 0;
	}
	return -1;
}

int HobTokenizer::first(HobToken *token)				//added ats 09252002
{
	if (tokenCount > 0)
	{
		currentTokenNum = 1;
		iter = tokenList.begin();
		*token = **iter;					
		return 0;
	}
	return -1;

}

int HobTokenizer::next(HobString *token)
{
	if (tokenCount > 0 && currentTokenNum < tokenCount)
	{
		if (currentTokenNum == 0)
			return first(token);

		currentTokenNum++;
		iter++;
		HobToken *hobToken = *iter;						//modified ats 09242002
		*token = hobToken->getRawData();					
		return 0;
	}
	return -1;
}

int HobTokenizer::next(HobToken *token)					//added ats 09252002
{
	if (tokenCount > 0 && currentTokenNum < tokenCount)
	{
		if (currentTokenNum == 0)
			return first(token);

		currentTokenNum++;
		iter++;						
		*token = **iter;					
		return 0;
	}
	return -1;
}

/* Note:  The number/position passed in is one-based, not zero-based.
 *        Thus, to get the first token, num should be 1.
 *
 * This method will also reposition the iterator so that a subsequent next() call will
 * get the token following the token retrieved by the getTokenNum() call.
 */
int HobTokenizer::getTokenNum(HobString *token, int num)
{
	if (num <= 0 || num > tokenCount)
		return -1;

	if (currentTokenNum == num)
	{
		HobToken *hobToken = *iter;						//modified ats 09242002
		*token = hobToken->getRawData();					
//		*token = *iter;
	}
	else
	{
		if (currentTokenNum > num)
			first(token);

		while (currentTokenNum != num)
			next(token);
	}
	return 0;
}

int HobTokenizer::getTokenNum(HobToken *token, int num)		//added ats 09242002
{
	if (num <= 0 || num > tokenCount)
		return -1;

	if (currentTokenNum == num)
	{						
		*token = **iter;					
	}
	else
	{
		if (currentTokenNum > num)
			first(token);

		while (currentTokenNum != num)
			next(token);
	}
	return 0;
}


bool HobTokenizer::hasMoreTokens() const
{
	return (currentTokenNum < tokenCount) ? true : false;
}
