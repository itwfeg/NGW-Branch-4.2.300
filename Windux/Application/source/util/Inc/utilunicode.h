#ifndef UTILUNICODE_H
#define UTILUNICODE_H
#if defined( WIN32 )
#pragma warning(disable : 4786)
#endif
#include <map>
#include <string>
#include "CSmartPtr.h"
//
//      Some helper functions and typedefs for unicode matters.
namespace Util
{
    // Functions that return a CSmartPtr
    Util::CSmartPtr<wchar_t> chartowideptr(const char    * pChar);
    Util::CSmartPtr<char>    widetocharptr(const wchar_t * pwChar);
    // Functions that convert character pointers to strings
    void wchartostring(std::string & sStrDest, const wchar_t * pChar);
    void chartowstring(std::wstring & wStrDest,const char * pChar);

    typedef std::map<std::string,std::wstring> short_to_wide_string_map;

} // End of namespace unicode_addon

#endif
