/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#include "HobString.h"
#include "util/e_string/utilstring.h"
#include "SystemFontConversionTable.h"
#include "Debug.h"
#include "Integer.h"

using namespace std;

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int HobString::dataType = 0;
std::string HobString::dataTypeName("HobString"); 
#endif

HobString::HobString()
	: string()
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HobString::HobString( const HobString& src):string(src)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HobString::~HobString()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(dataType);
#endif
}

HobString::HobString(const string& str)
	: string(str)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HobString::HobString(const string& str, size_type str_idx)
	: string(str, str_idx)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HobString::HobString(const string& str, size_type str_idx, size_type str_num)
	: string(str, str_idx, str_num)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HobString::HobString(const char* cstr)
	: string("")
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
	if (cstr != NULL)
	{
		assign(cstr);
	}
}

HobString::HobString(const char* cstr, size_type chars_len)
	: string(cstr, chars_len)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

HobString::HobString(size_type num, char c)
	: string(num, c)
{
#ifdef USE_ACCOUNTANT
	if (dataType == 0)
		dataType = util_acc::registerDataType(dataTypeName);

	util_acc::addInstance(dataType);
#endif
}

//HobString::HobString(InputIterator beg, InputIterator end)
//	: string(beg, end)
//{}

void HobString::stripDuplicateChar(char c)
{
	if (c == 0 || size() < 2)
		return;

	for (int pos=(int)size() - 1; pos > 0; pos--)
	{
		if ( at(pos) == c && at(pos-1) == c)
		{
			erase(pos, 1);
			pos--;
		}
	}

}

void HobString::duplicateChar(char c)
{
	if (c == 0 || size() == 0)
		return;

#if defined(LINUX_FIX)
    // std::string::size_type is unsigned, >= 0 comparison is always true
	for (std::string::size_type pos=size() - 1; ; pos--)
#else
	for (std::string::size_type pos=size() - 1; pos >= 0; pos--)
#endif
	{
		if (at(pos) == c)
			insert(pos, 1, c);
		if (pos == 0)
			break;
	}

}

// Returns whether or not this HobString was quoted, used by unQuoteForCSV().
// This way, anyone can call unQuoteForCSV() whether or not the string is quoted.
// If it's not, it's left unchanged.
bool HobString::unQuote()
{
	if (size() < 2)
		return false;

	if (at(0) == '"' && at(size()-1) == '"')
	{
		erase(size()-1, 1);
		erase(0, 1);
		return true;
	}
	return false;
}

void HobString::quote()
{
	append((std::string::size_type)1, '"');
	insert((std::string::size_type)0, 1, '"');
}

void HobString::quoteForCSV()
{
	duplicateChar('"');
	quote();
}

// See comment for 'unQuote()' above.
void HobString::unQuoteForCSV()
{
	if (unQuote())
		stripDuplicateChar('"');
}

string HobString::getFromEnd(size_type str_idx)
{
	if(str_idx == 0)
		return "";
	else
	{
		size_type len = length();
		if(len > str_idx)
			return substr((len - str_idx), str_idx);
		else
			return substr(0, len);
	}
}


int HobString::countConsecutiveChars(char c)
{
	return countConsecutiveChars(c, 0);
}

int HobString::countConsecutiveChars(char c, size_type str_idx)
{
	int count = 0;
	string::iterator iter = begin() + str_idx;
	while (iter != end())
	{
		if(*iter != c)
			iter++;
		else
		{
			do
			{
				count++;
				iter++;
			} while (iter != end() && (*iter == c));
		}
	}
	return count;
}


void HobString::stripWhiteSpace(short position)
{
	if (size() > 0 && (position == LEADING || position == LEADING_TRAILING))
	{
		// Remove leading white space.
		size_type pos = 0;
		string::iterator iter = begin();
		while (iter != end() && (*iter == ' ' || *iter == '\t'))
		{
			pos++;
			iter++;
		}
		if (pos)
			erase(0, pos);
	}

	if (size() > 0 && (position == TRAILING || position == LEADING_TRAILING))
	{
		// Remove trialing white space.
		size_type pos = size();
		string::reverse_iterator iter = rbegin();
		while (iter != rend() && (*iter == ' ' || *iter == '\t'))
		{
			pos--;
			iter++;
		}
		if (pos != size())
			erase(pos);
	}
}

bool HobString::isNumeric()
{
	return (find_first_not_of("0123456789") == string::npos);
}


bool HobString::isUpper()
{
	// under construction
	return false;
}

void HobString::toUpper()
{
	// under construction
}

bool HobString::isLower()
{
	// under construction
	return false;
}

void HobString::toLower()
{
	// under construction
}


void HobString::copyIntoString(string::size_type maxChars, string::size_type startIndex, HobString copyFromStr)
{
	if(maxChars <= length())
	{
		string::size_type actualChars = copyFromStr.length();

		if(actualChars < maxChars)
		{
			//	Copy to the end of this string
			replace((startIndex + (maxChars - actualChars)), actualChars, copyFromStr);
		}
		else
		if(actualChars > maxChars)
		{
			//	Take the digits from the end of the copyFromStr
			replace(startIndex, maxChars, copyFromStr, (maxChars - actualChars), maxChars);
		}
		else
		{
			replace(startIndex, actualChars, copyFromStr);
		}
	}
}


void HobString::makeBinary()
{
	string::iterator iter = begin();
	while (iter != end())
	{
        *iter++ -= '0';
	}
    return;
}


void HobString::makeAscii()
{
	string::iterator iter = begin();
	while (iter != end())
	{
        *iter++ += '0';
	}
    return;
}


void HobString::getCharsWithPadding(int numChars)
{
	//	Get the right-most numChars characters.
	//	If the string is less than numChars, pad it with zeros to the left.
	int actualLength = (int)length();

	if(actualLength > numChars)
	{
		//	The string is too long. Erase the extra characters at the beginning of this string
		erase(0, (actualLength - numChars));
	}
	else if(actualLength < numChars)
	{
		//	The string is not long enough. Pad with Zeros
		insert(begin(), (numChars - actualLength), '0');
	}
}


void HobString::convertToEvolFontSequences()
{
	if (size() < 2)
		return;

	short fontsConverted = 0;
	string::size_type index = find('\x1B');

	// Stop searching through this string when we reach the end, or when we determine that
	// this string has already been converted to Evolution format font sequences; when we
	// find two ESC's in a row.
	while (index != std::string::npos && index+1 != std::string::npos && (at(index+1) != '\x1B'))
	{
		//found the escape sequence in the string
		HobString searchString(substr(index, 2));
		SystemFontConversionTable fontConvert;
		HobString evolFontSequence(fontConvert.findEvolMatch(searchString/*.c_str()*/));

		// either no match found for escape sequence, so remove the escape sequence from the text
		// or, match found, so remove ultima sequence and add the new sequence to the text
		// The following does both.
		HobString afterSequence(substr(index + 2));
		assign(substr(0, index));
		append(evolFontSequence);
		append(afterSequence);

		fontsConverted++;
		index = find('\x1B', (index + evolFontSequence.size()));		//find the next match

		Debug::trace("HobString.fontSequence", HobString("Converted font escape sequence ") + searchString + HobString( "  to ") + evolFontSequence);
	}
	Debug::trace("HobString.fontSequence", HobString("Finished converting to Evol font escape sequences.  fonts converted: ") + HobString(Integer::toString(fontsConverted)));
}


void HobString::convertToUltimaFontSequences()
{
	if (size() < 2)
		return;

	short fontsConverted = 0;

	//strip out the escape sequences and replace with Ultima equivalent
	string::size_type index = find('\x1B');
	while (index != std::string::npos)
	{
		if (at(index+1) != '\x1B')
			// This isn't an Evolution font escape sequence.  It is a possible Ultima font escape sequence,
			// so just skip on past it, leaving it in the string.
			index = find('\x1B', (index + 2));

		else
		{
			//found an Evolution font escape sequence in the string
			string::size_type sequenceEnd = find('\x1B', (index + 2));	//find the end of our escape sequence, a single '<esc>'
			if (sequenceEnd == std::string::npos)
				index = sequenceEnd;
			else
			{
				HobString ultimaFontSequence("");

				HobString searchString(substr(index, sequenceEnd - index + 1));
				SystemFontConversionTable fontConvert;
				ultimaFontSequence = fontConvert.findUltimaMatch(searchString/*.c_str()*/);

				// either no match found, so remove the escape sequence from the text
				// or match found, so remove Evol sequence and add the new Ultima sequence to the text
				// The following does both.
				HobString afterSequence(substr(sequenceEnd + 1));

				assign(substr(0, index));
				append(ultimaFontSequence);
				append(afterSequence);

				fontsConverted++;
				index = find('\x1B', (index + ultimaFontSequence.size()));		//find the next ESCAPE character

				Debug::trace("HobString.fontSequence", HobString("Converted font escape sequence ") + searchString + HobString( "  to ") + ultimaFontSequence);
			}
		}
	}
	Debug::trace("HobString.fontSequence", HobString("Finished converting to Ultima font escape sequences.  fonts converted: ") + HobString(Integer::toString(fontsConverted)));
}


void HobString::stripFontSequences()
{
	HobString cleanString("");

	string::size_type startIndex = find('\x1B');
	string::size_type endIndex;

	// Stop searching through this string when we reach the end, or when we determine that
	// this string has already been converted to Evolution format font sequences; when we
	// find two ESC's in a row.
	while (startIndex != std::string::npos)
	{
		HobString beforeSequence(substr(0, startIndex));
		HobString afterSequence("");
		if (at(startIndex+1) == '\x1B')
		{
			// This is an Evolution font escape sequence. Find the terminating escape character.
			endIndex = find('\x1B', (startIndex + 2));
			if (endIndex == std::string::npos)
			{
				endIndex = startIndex + 2;
			}
			else
			{
				endIndex++;
			}
			afterSequence = substr(endIndex);
		}
		else
		{
			// This must be an Ultima font escape sequence. Skip over the next 2 characters.
			if ((startIndex + 3) < size())
			{
				afterSequence = substr(startIndex + 3);
			}
		}
		assign(beforeSequence);
		append(afterSequence);
		startIndex = find('\x1B', startIndex);
	}
}


//	stripChar will remove all instances of the specified character from the string
void HobString::stripChar(char c)
{
	if (c == 0 || empty())
		return;

	for (int pos=(int)size() - 1; pos > 0; pos--)
	{
		if (at(pos) == c)
		{
			erase(pos, 1);
			pos--;
		}
	}
}

//	removeTrailingNewlinesAndSpaces will remove all Newlines (\n) and spaces at the end of the string.
void HobString::removeTrailingNewlinesAndSpaces()
{
	bool traceOn = Debug::isTracing("HobString");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "removeTrailingNewlinesAndSpaces():   ";

	if (empty())
	{
		if (traceOn)
			Debug::trace("HobString", msgDebug + "The string is empty! Exiting.");
		return;
	}
	if (traceOn)
		Debug::trace("HobString", msgDebug + "Start. [" + substr() + "]");

	string::size_type lastSpace = 0, lastNonSpace = 0, lastLineFeed = 0, lastNonLineFeed = 0;
	string::size_type lastChar = length() - 1;

	//	Look for a newline char or a space at the end.
	while ((!empty()) && (lastChar != string::npos) && (((lastLineFeed = find_last_of('\n')) == lastChar) || ((lastSpace = find_last_of(' ')) == lastChar)))
	{
		if (lastLineFeed == lastChar)
		{
			HobString msgDebug2 = "";
			if (traceOn)
				msgDebug2 = msgDebug;

			//	We have a newline at the end. Remove it along with any other consecutive newlines.
			if (lastLineFeed == 0)
			{
				erase();
				if (traceOn)
					msgDebug2.append("erase()");
			}
			else if ((lastNonLineFeed = find_last_not_of('\n', (lastLineFeed - 1))) != string::npos)
			{
				erase(lastNonLineFeed + 1);
				if (traceOn)
					msgDebug2.append("erase(lastNonLineFeed + 1): lastNonLineFeed = ").append(util_string::inttostring(lastNonLineFeed));
			}
			else
			{
				erase(lastLineFeed);
				if (traceOn)
					msgDebug2.append("erase(lastLineFeed)");
			}
			if (traceOn)
			{
				Debug::trace("HobString", msgDebug2);
				Debug::trace("HobString", msgDebug + "Removed Newlines at end. length = " + util_string::inttostring(length()));
			}
		}
		else if (lastSpace == lastChar)
		{
			HobString msgDebug2 = "";
			if (traceOn)
				msgDebug2 = msgDebug;

			//	We have a space at the end. Remove it along with any other consecutive spaces.
			if (lastSpace == 0)
			{
				erase();
				if (traceOn)
					msgDebug2.append("erase()");
			}
			else if ((lastNonSpace = find_last_not_of(' ', (lastSpace - 1))) != string::npos)
			{
				erase(lastNonSpace + 1);
				if (traceOn)
					msgDebug2.append("erase(lastNonSpace + 1): lastNonSpace = ").append(util_string::inttostring(lastNonSpace));
			}
			else
			{
				erase(lastSpace);
				if (traceOn)
					msgDebug2.append("erase(lastSpace)");
			}
			if (traceOn)
			{
				Debug::trace("HobString", msgDebug2);
				Debug::trace("HobString", msgDebug + "Removed Spaces at end. length = " + util_string::inttostring(length()));
			}
		}
		lastChar = length() - 1;
		if (traceOn)
			Debug::trace("HobString", msgDebug + "Loop end:  lastChar = " + util_string::inttostring(lastChar));
	}
	if (traceOn)
		Debug::trace("HobString", msgDebug + "Exit [" + substr() + "]");
}


//	removeSpacesBeforeNewlines will remove all consecutive spaces before a Newline (\n).
void HobString::removeSpacesBeforeNewlines()
{
	bool traceOn = Debug::isTracing("HobString");
	HobString msgDebug;
	if (traceOn)
		msgDebug = "removeSpacesBeforeNewlines():   ";

	if (empty())
	{
		if (traceOn)
			Debug::trace("HobString", msgDebug + "The string is empty! Exiting.");
		return;
	}
	if (traceOn)
		Debug::trace("HobString", msgDebug + "Start. [" + substr() + "]");

	string::size_type idx = 0, pos = 0;
#if 0
	HobString subText = substr();
	string::size_type pos1 = 0;
	//	For show, replace all spaces with 'z'
	while ((pos1 = subText.find(' ', pos1)) != string::npos)
	{
		if (pos1 != string::npos)
			subText.replace(pos1, 1, 'z');
	}
	if (traceOn)
		Debug::trace("HobString", msgDebug + "Replacing spaces with z's: [" + subText + "]");
#endif

	//	Remove spaces before Newline char. This was done to fit fixed width text in a line.
	string::size_type lastNonSpace = 0, lastLineFeed = 0;
	pos = length();
	if (traceOn)
		Debug::trace("HobString", msgDebug + "length = " + util_string::inttostring(pos));

	int spaceCount = 0;
	while ((pos != string::npos) && ((lastLineFeed = find_last_of('\n', pos)) != string::npos))
	{
		if (traceOn)
		{
			Debug::trace("HobString", msgDebug + "lastLineFeed = " + util_string::inttostring(lastLineFeed));
			Debug::trace("HobString", msgDebug + "pos = " + util_string::inttostring(pos));
		}

		//	Is there a space before this Newline?
		if (lastLineFeed == 0)
		{
			lastNonSpace = string::npos;
			if (traceOn)
				Debug::trace("HobString", msgDebug + "lastNonSpace = " + util_string::inttostring(lastNonSpace));
		}
		else
		{
			lastNonSpace = find_last_not_of(' ', lastLineFeed - 1);
			if (traceOn)
				Debug::trace("HobString", msgDebug + "lastNonSpace = " + util_string::inttostring(lastNonSpace));
			if (lastNonSpace != string::npos)
			{
				spaceCount = (int)(lastLineFeed - lastNonSpace - 1);
				idx = lastNonSpace + 1;
			}
			else
			{
				spaceCount = (int)lastLineFeed;
				idx = 0;
			}
			if (traceOn)
			{
				Debug::trace("HobString", msgDebug + "       idx = " + util_string::inttostring(idx));
				Debug::trace("HobString", msgDebug + "spaceCount = " + util_string::inttostring(spaceCount));
			}

			if (spaceCount)
			{
				erase(idx, spaceCount);			//	Remove the spaces only.
#if 1
				if (traceOn)
					Debug::trace("HobString", msgDebug + "Removed " + util_string::inttostring(spaceCount) + " spaces. [" + substr() + "]");
#else
				//	For show, replace all spaces with 'z'
				HobString subText1 = substr();
				string::size_type pos2 = 0;
				while ((pos2 = subText1.find(' ', pos2)) != string::npos)
				{
					if (pos2 != string::npos)
						subText1.replace(pos2, 1, 'z');
				}
				if (traceOn)
					Debug::trace("HobString", msgDebug + "Removed " + util_string::inttostring(spaceCount) + " spaces. [" + subText1 + "]");
#endif
			}
		}
		pos = lastNonSpace;
		if (traceOn)
			Debug::trace("HobString", msgDebug + "Loop end:  pos = " + util_string::inttostring(pos));
	}
	if (traceOn)
		Debug::trace("HobString", msgDebug + "Exit [" + substr() + "]");
}


//	Replace char x with char y.
void HobString::replaceChar(char cOrig, char cNew)
{
	Debug::trace("HobString.test", "replaceChar():  Start [" + substr() + "]");
	if (cOrig == 0 || cNew == 0 || empty())
		return;

	for (int pos=(int)size() - 1; pos > 0; pos--)
	{
		if (at(pos) == cOrig)
		{
			replace(pos, 1, 1, cNew);
			pos--;
		}
	}
	Debug::trace("HobString.test", "replaceChar():   Exit [" + substr() + "]");
}
