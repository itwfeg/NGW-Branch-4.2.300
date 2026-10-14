#include "utilunicode.h"

namespace Util
{
    // A map giving a wstring version of an object's typeId.name()
    short_to_wide_string_map gwTypeIdMap;


    // Function name    : chartowideptr
    // Description          : Given a NULL terminated character string , return a Smart pointer
    //                                    to an equivalent wide (wchar_t) string.
    // Return type              : Util::CSmartPtr<wchar_t>
    // Argument         : const char * pChar
    Util::CSmartPtr<wchar_t> chartowideptr(const char * pChar)
    {
        try
        {
            if( NULL == pChar)
                return Util::CSmartPtr<wchar_t>(NULL);
            const int len = (int)(strlen(pChar)+1);
            Util::CSmartPtr<wchar_t> wCharPtr( new wchar_t[len]);
#ifdef LINUX_FIX
            int retVal = PAL_absclass::PAL_MB2WC(CP_ACP,0,pChar,-1,wCharPtr,len);
            Assert( 0 != retVal,Util::CPlatformException());
#else
            int retVal = MultiByteToWideChar(CP_ACP,0,pChar,-1,wCharPtr,len);
            Assert( 0 != retVal,Util::CNTException());
#endif
            return wCharPtr;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }


    }


    // Function name    : widetocharptr
    // Description          : Given a NULL terminated wide character string , return a Smart pointer
    //                                    to an equivalent ANSI (char) string.
    // Return type              : Util::CSmartPtr<char>
    // Argument         : const wchar_t * pwChar
    Util::CSmartPtr<char>    widetocharptr(const wchar_t * pwChar)
    {
        try
        {
            if( NULL == pwChar)
                return Util::CSmartPtr<char>(NULL);
            const int len = (int) (wcslen(pwChar)+1);
            Util::CSmartPtr<char> charPtr( new char[len]);
#ifdef LINUX_FIX
            int retVal = PAL_absclass::PAL_WC2MB(CP_ACP,0,pwChar,-1,charPtr,len,NULL,NULL);
            Assert( 0 != retVal, Util::CPlatformException());
#else
            int retVal = WideCharToMultiByte(CP_ACP,0,pwChar,-1,charPtr,len,NULL,NULL);
            Assert( 0 != retVal,Util::CNTException());
#endif
            return charPtr;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }


    }


    void wchartostring(std::string & sStrDest, const wchar_t * pChar)
    {
        Assert( NULL != pChar ,Util::CNamedException(L" wchartostring:: attempt to copy NULL pointer"));

        size_t len = wcslen(pChar);
        sStrDest.resize( len );
        sStrDest = widetocharptr(pChar).get();

    }

    void chartowstring(std::wstring & wStrDest,const char * pChar)
    {
        Assert( NULL != pChar , Util::CNamedException(L" chartowstring:: attempt to copy NULL \
                    char pointer."));

        size_t len = strlen(pChar);
        wStrDest.resize( len);
        wStrDest = chartowideptr(pChar).get();

    }



}
