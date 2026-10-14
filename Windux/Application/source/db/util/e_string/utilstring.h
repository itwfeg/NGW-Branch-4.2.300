#ifndef UTILSTRING_H
#define UTILSTRING_H



#include <string>
#include <vector>
#include <cstdlib>
#include <math.h>
#ifdef VXWORKS
#  include <stdio.h>
#else
#  include <sstream>
#  include "PAL_platformheader.h"
#endif


#include "../time/datetime.h"

#if defined( WIN32 )    // Alamy: only needed for Windows
class std::locale;
#endif


#ifndef NO_NAMESPACES
namespace util_string
{
#endif



const char _ESC_ = 0x1B;
const char _CR_ = 0x0D, _LF_ = 0x0A;
const char _SP_ = 0x20;


extern const char* trueStringValues;
extern const char* falseStringValues;
extern const std::string yesStringValue;
extern const std::string noStringValue;
extern const std::string wordSeparators;


int stricmp(const char* s1, const char* s2);
int strnicmp(const char* s1, const char* s2, size_t count);
const char* stristr(const char* s1, const char* s2);
inline int stringicmp(const std::string& s1, const std::string& s2) { return stricmp(s1.c_str(), s2.c_str()); }
int comparetomask(const char * s, const char* m);


std::string& toupper(std::string& s);
std::string strtoupper(const std::string& s);
std::string& tolower(std::string& s);
std::string strtolower(const std::string& s);

void toupperL(std::string& s);
void toupperL(std::string& s, const std::locale& language);
std::string* strtoupperL(const std::string& s);
std::string* strtoupperL(const std::string& s, const std::locale& language);
void tolowerL(std::string& s);
void tolowerL(std::string& s, const std::locale& language);
std::string* strtolowerL(const std::string& s);
std::string* strtolowerL(const std::string& s, const std::locale& language);


std::string ptrtostring(const void * p);

inline bool stringtobool(const std::string& s, bool slant = false)
{
	if ( s.empty() ) return slant;

	if ( slant ) return strchr(falseStringValues, s[0]) ? false : true;
	else return strchr(trueStringValues, s[0]) ? true : false;
}
inline const std::string& booltostring(const bool i)
		{ return i ? yesStringValue : noStringValue; }

std::string wchartostring(const wchar_t* wstr);
std::string wchartostring(const wchar_t* wstr, std::string::size_type length);

std::wstring stringtowstring(const std::string& str);

#if INTPTR_MAX == INT64_MAX
std::string inttostring(size_t i); /* (SJJC port 6/19/06) */
#endif

#if defined( WIN32 )
std::string inttostring(unsigned int i);
#endif
std::string inttostring(int i);
std::string inttostring(int i, int width);
inline int stringtoint(const std::string& s)
		{ return std::atoi(s.c_str()); }
int stringtoimpint(const std::string& s, int impDecimals);

std::string uinttostring(unsigned int i);

std::string inttohexstring(int i);
std::string inttohexstring(int i, int width);
int hexstringtoint(const std::string& s);

std::string uinttohexstring(unsigned int i);
std::string uinttohexstring(unsigned int i, int width);
unsigned int hexstringtouint(const std::string& s);

std::string longtostring(long l);
inline long stringtolong(const std::string& s)
		{ return std::atol(s.c_str()); }

std::string ulongtostring(unsigned long l);

std::string longlongtostring(long long l);
inline long long stringtolonglong(const std::string& s) {
#if defined( WIN32 )
    return _strtoui64( s.c_str(), NULL, 10 );
#else
    return std::atoll(s.c_str());
#endif
}

std::string ulonglongtostring(unsigned long long l);

std::string sizettostring(size_t s);

#ifndef VXWORKS
std::string dlongtostring(DWORDLONG ll);
#endif

std::string int64tostring(__int64 i);

std::string doubletostring(double d, int minWidth, int precision);
// The following convenience function uses hardcoded value of 30 since that is the value
// the function this one is replacing (Double::toString()) was using.
inline std::string doubletostring(double d) { return doubletostring(d, 30, 30); }
double stringtodouble(const std::string s);

inline std::string decimaltostring(int value, int implicitDecimals)
		{ return doubletostring((double)value/pow((double)10, implicitDecimals), 30, 30); }

util_time::DateTime stringdbtodatetime(const std::string& s);

std::string stringtocodes(const std::string& s);
std::string hexstringtostring(const std::string& s);
std::string stringtohexstring(const std::string& s);

bool isSubstringOfDBText(const std::string& key, const std::string& text);
bool isWordLead(const std::string& key, const std::string& text);

std::vector<std::string>* getCharacterSequences(const std::string& str, int minSequenceLength, int maxSequenceLength);

std::string* localeTransform(const std::string& str, const std::string& language);
void localeTransform(std::string& newstr, const std::string& str, const std::string& language);
/* #ifdef VXWORKS (SJJC port 6/19/06) */
std::string* localeTransform(const std::string& str, const std::locale& language);
void localeTransform(std::string& newstr, const std::string& str, const std::locale& language);
/* #endif (SJJC port 6/19/06) */

void duplicateCharInString(std::string& str, const char c);
bool replace(std::string& str, const std::string& from, const std::string& to);

std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string>& elems);
std::vector<std::string> split(const std::string &s, char delim);

std::string &ltrim(std::string &s);
std::string &rtrim(std::string &s);
std::string &trim(std::string &s);

#ifndef NO_NAMESPACES
} // namespace util_string
#endif



#endif // UTILSTRING_H

