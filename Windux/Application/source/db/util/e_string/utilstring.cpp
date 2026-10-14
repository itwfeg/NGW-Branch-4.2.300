#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

#ifdef WIN32_OS
	#include <iomanip>
#endif


#include "utilstring.h"
#include "edb.h"
#include PLATFORM_INC

#ifdef VXWORKS
#	include "../locale/dblocale.h"
#	include "../locale/dbcollate.h"
#	include "../locale/dbctype.h"
#endif


#ifndef NO_NAMESPACES
namespace util_string
{
#endif


using namespace std;


#if defined(LINUX_FIX)
const char* trueStringValues("YT1yt");
const char* falseStringValues("NF0nf");
const std::string yesStringValue("Y");
const std::string noStringValue("N");
const std::string wordSeparators(" \t\n/-().,");
#else
extern const char* trueStringValues("YT1yt");
extern const char* falseStringValues("NF0nf");
extern const std::string yesStringValue("Y");
extern const std::string noStringValue("N");
extern const std::string wordSeparators(" \t\n/-().,");
#endif


int stricmp(const char* s1, const char* s2)
{
	while ( *s1 && *s2 )
	{
		if ( ::toupper(*s1) != ::toupper(*s2) )
			return ::toupper(*s1) - ::toupper(*s2);

		++s1;
		++s2;
	}

	return ::toupper(*s1) - ::toupper(*s2);
}


int strnicmp(const char* s1, const char* s2, size_t count)
{
	if ( 0 == count )
		return 0;

	DWORD i = 0;
	while ( *s1 && *s2 )
	{
		if ( ::toupper(*s1) != ::toupper(*s2) )
			return ::toupper(*s1) - ::toupper(*s2);

		if ( ++i >= count )
			return 0;
		
		++s1;
		++s2;
	}

	return ::toupper(*s1) - ::toupper(*s2);
}


const char* stristr(const char* s1, const char* s2)
{
	while (*s1)
	{
		const char *s1s = s1;
		const char *s2s = s2;
		while (*s1s)
		{
			if (!*s2s)
				return s1;
			if (::toupper(*s1s) != ::toupper(*s2s))
				break;
			++ s1s;
			++ s2s;
		}
		if (!*s2s)
			return s1;
		++ s1;
	}
	return NULL;
}


int comparetomask(const char * s, const char* m)
{
	bool letpass = false;

	while ( *s && *m )
	{
		switch ( *m )
		{
			case '*':
			{
				++ m;
				letpass = true;

				if ( *m != '*' && *m != '?' )
					while ( *s && ::toupper(*s) != ::toupper(*m) ) ++ s;

				break;
			}
			case '?':
			{
				++ s;
				++ m;

				break;
			}
			default:
			{
				if ( ::toupper(*s) == ::toupper(*m) )
				{
					letpass = false;

					++ s;
					++ m;
				}
				else
				{
					if ( letpass )
						++ s;
					else
						return ::toupper(*s) - ::toupper(*m);
				}

				break;
			}
		}
	}


	if ( *s && letpass )
		return 0;
	else
	{
		while ( *m == '*' )
			++ m;
	}

	return ::toupper(*s) - ::toupper(*m);
}


std::string& toupper(std::string& s)
{
	for ( std::string::size_type i = 0; i < s.size() ; ++i )
		s[i] = (char)::toupper(s[i]);

	return s;
}


std::string strtoupper(const std::string& s)
{
	std::string copy = s;

	return toupper(copy);
}

std::string& tolower(std::string& s)
{
	for ( std::string::size_type i = 0; i < s.size() ; ++i )
		s[i] = (char)::tolower(s[i]);

	return s;
}


std::string strtolower(const std::string& s)
{
	std::string copy = s;

	return tolower(copy);
}


void toupperL(std::string& s)
{
	toupperL(s, locale());
}


void toupperL(std::string& s, const std::locale& language)
{
	if (!s.empty())
		use_facet< ctype<char> >(language).toupper(&s[0], &s[s.size()-1]);
}


std::string* strtoupperL(const std::string& s)
{
	string* local = new string(s);

	toupperL(*local, locale());

	return local;
}


std::string* strtoupperL(const std::string& s, const std::locale& language)
{
	string* local = new string(s);

	toupperL(*local, language);

	return local;
}


void tolowerL(std::string& s)
{
	tolowerL(s, locale());
}


void tolowerL(std::string& s, const std::locale& language)
{
	if (!s.empty())
		use_facet< ctype<char> >(language).tolower(&s[0], &s[s.size()-1]);
}


std::string* strtolowerL(const std::string& s)
{
	string* local = new string(s);

	tolowerL(*local, locale());

	return local;
}


std::string* strtolowerL(const std::string& s, const std::locale& language)
{
	string* local = new string(s);

	tolowerL(*local, language);

	return local;
}


std::string ptrtostring(const void * p)
{
	char buffer[110];

	sprintf(buffer, "%p", p);

	return buffer;
}


std::string wchartostring(const wchar_t* wstr)
{
	std::string str;

	while ( *wstr )
	{
		str.push_back((char)*wstr);

		++wstr;
	}

	return str;
}


std::string wchartostring(const wchar_t* wstr, std::string::size_type length)
{
	std::string str;

	while ( length-- > 0 )
	{
		str.push_back((char)*wstr);

		++wstr;
	}

	return str;
}


std::wstring stringtowstring(const std::string& str)
{
	std::wstring wstr;

	for ( std::string::size_type size = str.size(), i = 0; size > 0; --size, ++i )
		wstr.push_back(str[i]);

	return wstr;
}

#if INTPTR_MAX == INT64_MAX
std::string inttostring(size_t i)
{
	char buffer[33];

#if INTPTR_MAX == INT64_MAX
	sprintf(buffer, "%zlld", i);
#else
	sprintf(buffer, "%zd", i);
#endif

	return buffer;
}
#endif

std::string inttostring(unsigned int i)
{
	return inttostring((int) i);
}

std::string inttostring(int i)
{
	char buffer[33];

	sprintf(buffer, "%d", i);

	return buffer;
}


std::string inttostring(int i, int width)
{
	char buffer[33];

	width = (width < 32) ? width : 32;
	sprintf(buffer, "%0*d", width, i);

	return buffer;
}


int stringtoimpint(const std::string& s, int impDecimals)
{
	string newS;
	string::size_type dot = s.find('.');

	newS.assign(s, 0, dot);

	if ( dot == std::string::npos )
	{
		newS.append(impDecimals, '0');
	}
	else
	{
		if ( s.size() < dot + 1 + impDecimals )
		{
			newS.append(s, dot + 1, string::npos);
			newS.append(dot + 1 + impDecimals - s.size(), '0');
		}
		else
		{
			newS.append(s, dot + 1, impDecimals);
		}
	}

	return stringtoint(newS);
}


std::string uinttostring(unsigned int i)
{
	char buffer[33];

	sprintf(buffer, "%u", i);

	return buffer;
}


std::string inttohexstring(int i)
{
	char buffer[9];

	sprintf(buffer, "%X", i);

	return buffer;
}


std::string inttohexstring(int i, int width)
{
	char buffer[9];

	width = (0 < width && width < 8) ? width : 8;
	sprintf(buffer, "%0*X", width, i);

	return buffer;
}


int hexstringtoint(const std::string& s)
{
	int intValue;

	sscanf_s(s.c_str(), "%x", &intValue);  /* (SJJC port 6/19/06) */

	return intValue;
}


std::string uinttohexstring(unsigned int i)
{
	char buffer[9];

	sprintf(buffer, "%X", i);

	return buffer;
}


std::string uinttohexstring(unsigned int i, int width)
{
	char buffer[9];

	width = (0 < width && width < 8) ? width : 8;
	sprintf(buffer, "%0*X", width, i);

	return buffer;
}


unsigned int hexstringtouint(const std::string& s)
{
	unsigned int intValue;

	sscanf_s(s.c_str(), "%x", &intValue);

	return intValue;
}


std::string longtostring(long l)
{
	char buffer[45];

	sprintf(buffer, "%ld", l);

	return buffer;
}

std::string longlongtostring(long long l)
{
	char buffer[90];

	sprintf(buffer, "%lld", l);

	return buffer;
}

std::string ulonglongtostring(unsigned long long l)
{
	char buffer[90];

	sprintf(buffer, "%llu", l);

	return buffer;
}

std::string ulongtostring(unsigned long l)
{
	char buffer[45];

	sprintf(buffer, "%lu", l);

	return buffer;
}


std::string sizettostring(size_t s)
{
	char buffer[45];

#if defined ( __linux__ )
	sprintf(buffer, "%zd", s);
#else
	sprintf(buffer, "%Iu", s);
#endif

	return buffer;
}


#ifndef VXWORKS
std::string dlongtostring(DWORDLONG ll)
{
	char buffer[45];

#if defined ( __linux__ )
	sprintf(buffer, "%" PRIu64, ll);
#else
	sprintf(buffer, "%I64u", ll);
#endif

	return buffer;
}
#endif

std::string int64tostring(__int64 i)
{
	char buffer[45];

#if defined ( __linux__ )
	sprintf(buffer, "%" PRId64, i);
#else
	sprintf(buffer, "%I64d", i);
#endif

	return buffer;
}

std::string doubletostring(double d, int minWidth, int precision)
{
	char buffer[45];

	sprintf(buffer, "%*.*f", minWidth, precision, d);

	return buffer;
}


double stringtodouble(const std::string s)
{
	double value = 0;

	sscanf_s(s.c_str(), "%lf", &value);

	return value;
}


util_time::DateTime stringdbtodatetime(const std::string& s)
{
	int year, month, day;
	int hour, minute, second;

//	sscanf_s(s.c_str(), "%d/%d/%d,%d:%d:%d", &month, &day, &year, &hour, &minute, &second);
	sscanf_s(s.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

	util_time::Date date(year, month, day);
	util_time::Time time(hour, minute, second);

	return util_time::DateTime(date, time);
}


std::string stringtocodes(const std::string& s)
{
//#ifndef VXWORKS
#if defined(PAL_WIN32_RUN)
	int size = (int)s.size();  /* (SJJC port 6/19/06) */
	std::ostringstream oss;

	oss.setf(ios_base::uppercase);
	oss.setf(ios_base::hex, ios_base::basefield);
	oss.fill('0');
	for ( int i = 0; i < size; ++i )
		oss << std::setw(2) << s.at(i);

	return oss.str();
#elif   defined(VXWORKS) || defined( __linux__ )
	int size = s.size();
	char buffer[size*4 + 1];
	char* pBuffer = buffer;

	for ( int i = 0; i < size; ++i )
	{
		sprintf(pBuffer, "\\x%.2X", (int)((unsigned char)s.at(i)));
		pBuffer += 4;
	}

	return buffer;

#else
    #error "Implement stringtocodes()"
#endif
}


std::string hexstringtostring(const std::string& s)
{
	int charValue = 0;
	const char* p = s.data();
	std::string::size_type size = s.size() / 2;     // If the size of the source 's' is odd, we'll lose the last hex digit
	std::string result;

	for ( unsigned int i = 0; i < size; ++i, p+=2 )
	{
		sscanf_s(p, "%2x", &charValue);

		result.append(1, (char)charValue);
	}

	return result;
}


std::string stringtohexstring(const std::string& s)
{
	int size = (int)s.size();
	char buffer[13];
	std::string result;

	for ( int i = 0; i < size; ++i )
	{
    #if defined(PAL_WIN32_RUN)
		sprintf(buffer, "%0.2X", (int)((unsigned char)s[i]));
    #elif   defined( __linux__ )
		sprintf(buffer, "%02X", (int)((unsigned char)s[i]));
    #else
        #error "Implement the code for stringtohexstring()"
    #endif
		result.append(buffer, 2);
	}

	return result;
}


// This function is used to find a string of characters, parameter "key", in a
// text field from the database (which may include font change sequences or line
// breaks). It returns true if the "key" is found as is in the text field after
// "removing" any font changes and any line breaks. Also, line breaks can match
// any number of spaces in the key.
bool isSubstringOfDBText(const std::string& key, const std::string& text)
{
	string::size_type posK = 0, posT = 0;
	string::size_type totalK = key.size(), totalT = text.size();

	if ( totalT < totalK )
		return false;

	while ( posK < totalK && posT < totalT )
	{
		// If there is a font change sequence or a line break, skip it
		while ( text[posT] == _ESC_ || text[posT] == _LF_ )
		{
			// If there is a font change sequence, skip it
			if ( text[posT] == _ESC_ )
			{
				if ( posT + 1 < totalT && text[posT + 1] == _ESC_ )
				{
					// Find the end of the font change sequence
					posT += 2;
					while ( posT < totalT && text[posT] != _ESC_ )
						++posT;

					if ( posT == totalT )
						return false;

					// Pass over the last ESC
					++posT;
				}
				else
					break;
			}

			// If there is a line break sequence, skip it
#ifdef VXWORKS
			if ( text[posT] == _LF_ )
			{
				++posT;

				// Make the LF match any number of "spaces" in the key, if present
				while ( key[posK] == _SP_ )
				{
					if ( ++posK == totalK )
						return true;
				}

				if ( posT == totalT )
					return false;
			}
#else
			if ( text[posT] == _CR_ )
			{
				if ( posT + 1 < totalT && text[posT + 1] == _LF_ )
				{
					posT += 2;

					// Make the "CR+LF" match any number of "spaces" in the key, if present
					while ( key[posK] == _SP_ )
					{
						if ( ++posK == totalK )
							return true;
					}

					if ( posT == totalT )
						return false;
				}
				else
					break;
			}
#endif
		}

		if ( ::toupper(key[posK]) == ::toupper(text[posT++]) )
		{
			if ( ++posK == totalK )
				return true;
		}
		else if ( posK )
		{
			posT -= posK;
			posK = 0;
		}
	}

	return false;
}


bool isWordLead(const std::string& key, const std::string& text)
{
	string::size_type posK = 0, posT = 0;
	string::size_type totalK = key.size(), totalT = text.size();

	// The following condition also guarantees that 'totalT > 0'.
	if ( totalT < totalK || totalK == 0 )
		return false;

	char current = text[0];
	while ( posK < totalK && posT < totalT )
	{
		// Here is where we decide if the 'key' matches the 'text'. If we need to match 
		// accented vowels with non-accented values (or other locale dependent issues) 
		// on our own, here is where we need to do it.
		if ( ::toupper(key[posK]) == ::toupper(current) )
		{
			current = text[++posT];
			if ( ++posK == totalK )
				return true;
		}
		else
		{
			// Backtrack the matched characters (-1) in order to not skip any case if the key's
			// leading character(s) are repeated inside the key. (This can be optimized so
			// we don't have to go back at all.)
			if ( posK )
			{
				if ( posK > 1 )
					current = text[posT -= posK - 1];
					
				posK = 0;
			}

			// If we are inside a word skip out of it.
			while ( isalnum(current) && ++posT < totalT )
				current = text[posT];

			// Since we are outside a word go to the beginning of the next word.
			while ( !isalnum(current) && ++posT < totalT )
				current = text[posT];
		}
	}

	return false;
}


vector<string>* getCharacterSequences(const string& str, int minSequenceLength, int maxSequenceLength)
{
	int strLength = (int)str.length(), i = 0;  /* (SJJC port 6/19/06) */
	vector<string>* cont = new vector<string>();

	while ( i < strLength )
	{
		// Find the beginning of the next word
		do
		{
			if ( wordSeparators.find(str[i]) == string::npos )
				break;
			else
				++i;
		} while ( i < strLength );

		// Extract the character sequence if it has at least 'sequenceLength' characters
		if ( i < strLength )
		{
			int j = 0;

			for ( ; j < maxSequenceLength && i < strLength && (wordSeparators.find(str[i]) == string::npos); ++j, ++i ) 
				;

			if ( j >= minSequenceLength )
			{
				string seq = str.substr(i - j, j);

				// Only add this sequence if it is not already in the container.
				if ( find(cont->begin(), cont->end(), seq) == cont->end() )
					cont->push_back(seq);
			}
		}

		// Skip the rest of the current word (if any)
		while ( i < strLength )
		{
			if ( wordSeparators.find(str[i]) == string::npos )
				++i;
			else
				break;
		}
	}

	return cont;
}


#ifdef VXWORKS
std::string* localeTransform(const std::string& str, const std::locale& loc)
#else
std::string* localeTransform(const std::string& str, const std::locale& loc)
#endif
{
#ifdef VXWORKS
	// Obtain the collate facet and use it to transform the passed in string
	const collate& collateFacet = use_facet<collate>(loc);
	const char* b = str.data();
	string* res = new string(collateFacet.transform(b, b + str.size()));

	// Obtain the ctype facet and use it to uppercase the transformed key
	const ctype& ctypeFacet = use_facet<ctype>(loc);
	char* b2 = &(*res)[0];
	ctypeFacet.toupper(b2, b2 + res->size());

	return res;
#else
	const collate<char>& collateFacet = use_facet< collate<char> >(loc);
	const char* b = str.data();
	string* res = new string(collateFacet.transform(b, b + str.size()));

	// Obtain the ctype facet and use it to uppercase the transformed key
	const ctype<char>& ctypeFacet = use_facet< ctype<char> >(loc);
	char* b2 = &(*res)[0];
	ctypeFacet.toupper(b2, b2 + res->size());

	return res;
#endif
}


std::string* localeTransform(const std::string& str, const std::string& language)
{
	std::string tmpLocale("");

	util_platform::Platform::getInstance().getLocaleString(tmpLocale, language);

	return localeTransform(str, locale(tmpLocale.c_str()));
}


#ifdef VXWORKS
void localeTransform(std::string& newstr, const std::string& str, const std::locale& loc)
#else
void localeTransform(std::string& newstr, const std::string& str, const std::locale& loc)
#endif
{
	if (str.empty())
	{
		newstr = "";
		return;
	}
#ifdef VXWORKS
	// Obtain the collate facet and use it to transform the passed in string
	const collate& collateFacet = use_facet<collate>(loc);
	const char* b = str.data();
	newstr.assign(collateFacet.transform(b, b + str.size()));

	// Obtain the ctype facet and use it to uppercase the transformed key
	const ctype& ctypeFacet = use_facet<ctype>(loc);
	char* b2 = const_cast<char*>(newstr.data());
	ctypeFacet.toupper(b2, b2 + newstr.size());
#else
	// Obtain the collate facet and use it to transform the passed in string
	const collate<char>& collateFacet = use_facet< collate<char> >(loc);
	const char* b = str.data();
	newstr.assign(collateFacet.transform(b, b + str.size()));

	// Obtain the ctype facet and use it to uppercase the transformed key
	const ctype<char>& ctypeFacet = use_facet< ctype<char> >(loc);
	char* b2 = &newstr[0];
	ctypeFacet.toupper(b2, b2 + newstr.size());
#endif
}


void localeTransform(std::string& newstr, const std::string& str, const std::string& language)
{
	if (str.empty())
	{
		newstr = "";
		return;
	}
#ifdef VXWORKS
	localeTransform(newstr, str, locale(language.c_str()));
#else
	std::string tmpLocale("");

	util_platform::Platform::getInstance().getLocaleString(tmpLocale, language);

	localeTransform(newstr, str, locale(tmpLocale.c_str()));
#endif
}


void duplicateCharInString(std::string& str, const char c)
{
	if (c == 0 || str.size() == 0)
		return;

	for (std::string::size_type pos=str.size() - 1; ; pos--)
	{
		if (str.at(pos) == c)
			str.insert(pos, 1, c);
		if (pos == 0)
			break;
	}

}

bool replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

std::string &ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
	return s;
}

std::string &rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	return s;
}

std::string &trim(std::string &s)
{
	return ltrim(rtrim(s));
}

#ifndef NO_NAMESPACES
} // namespace util_string
#endif

