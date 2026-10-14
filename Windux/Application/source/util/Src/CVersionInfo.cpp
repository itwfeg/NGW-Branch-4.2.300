// CVersionInfo.cpp: implementation of the CVersionInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "CVersionInfo.h"
#include "CSmartPtr.h"
#include "PAL_platformheader.h"
//#include <winver.h> // replaced by PAL header

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif

namespace Util
{
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


#ifdef USE_ACCOUNTANT
int CVersionInfo::dataType = 0;
std::string CVersionInfo::dataTypeName("CVersionInfo"); 
#endif

CVersionInfo::CVersionInfo(const wchar_t * pname) : m_Version(L"Not Known"),
    m_SpecialBuildDescription(L"")

{
#ifdef USE_ACCOUNTANT
    if (dataType == 0)
        dataType = util_acc::registerDataType(dataTypeName);

    util_acc::addInstance(dataType);
#endif
    try
    {
#ifdef LINUX_FIX
        DWORD dwSize = PAL_absclass::PAL_getFileVersionInfoSize(const_cast<wchar_t *>(pname),0);
#else
        DWORD dwSize = ::GetFileVersionInfoSize(const_cast<wchar_t *>(pname),0);
#endif
        Util::CSmartPtr<unsigned  char> pData(new unsigned char[dwSize]);

#ifdef PAL_WIN32_RUN        // TODO figure this out for linux later
        UINT dwLen;
#ifdef LINUX_FIX
        PAL_VS_FIXEDFILEINFO *lpInfo;
#else
        VS_FIXEDFILEINFO *lpInfo;
#endif
#endif

#ifdef LINUX_FIX
        PAL_absclass::PAL_OutputDbgStr( pname);
#else
        OutputDebugString( pname);
#endif
#ifdef LINUX_FIX
        PAL_absclass::PAL_OutputDbgStr( L"\r\n");
#else
        OutputDebugString( L"\r\n");
#endif
        if(dwSize != 0) 
        {

#ifdef LINUX_FIX
            if( PAL_absclass::PAL_getFileVersionInfo(const_cast<wchar_t *>(pname),0,dwSize,(LPVOID *)pData.get()) )
#else
            if( GetFileVersionInfo(const_cast<wchar_t *>(pname),0,dwSize,(LPVOID *)pData.get()) )
#endif
            {
#ifdef PAL_WIN32_RUN        // TODO figure this out for linux later
                if(VerQueryValue(pData,L"\\", (LPVOID*)&lpInfo, &dwLen))//lpBuffer
                {
                    OutputDebugString( L"3\r\n");
                    wchar_t wbuf[256];
                    wsprintf(wbuf,L"%d.%d.%d.%d",
                            HIWORD(lpInfo->dwProductVersionMS),
                            LOWORD(lpInfo->dwProductVersionMS),
                            HIWORD(lpInfo->dwProductVersionLS),
                            LOWORD(lpInfo->dwProductVersionLS) );

                    m_Version = std::wstring(wbuf);
                }

                /// Get the Private Build Description
                // Structure used to store enumerated languages and code pages.

                struct LANGANDCODEPAGE {
                    WORD wLanguage;
                    WORD wCodePage;
                } *lpTranslate;

                // Read the list of languages and code pages.
                UINT cbTranslate;
                VerQueryValue(pData, 
                        TEXT("\\VarFileInfo\\Translation"),
                        (LPVOID*)&lpTranslate,
                        &cbTranslate);

                // Read the file description for each language and code page.
                // Take the first

                LPVOID lpBuffer;//[256];               
                for(int i=0; i < int(cbTranslate/sizeof(struct LANGANDCODEPAGE)); i++ )
                {
                    wchar_t l_block[128];
                    wsprintf( l_block, 
                            TEXT("\\StringFileInfo\\%04x%04x\\SpecialBuild"),
                            lpTranslate[i].wLanguage,
                            lpTranslate[i].wCodePage);
                    UINT dwBytes;
                    // Retrieve file description for language and code page "i". 

                    if( VerQueryValue(pData,l_block, (LPVOID*)&lpBuffer, &dwBytes) ) 
                    {
                        if(dwBytes != 0 ) // text
                        {
                            m_SpecialBuildDescription.append((wchar_t *)lpBuffer);//,dwBytes/sizeof(wchar_t));
                        }
                        break;
                    }
                }
                //

#endif      // PAL_WIN32_RUN
            }
        }
    }
    catch(...)
    {
        LogCatchAndThrow;
    }

}

CVersionInfo::~CVersionInfo()
{
#ifdef USE_ACCOUNTANT
    util_acc::removeInstance(dataType);
#endif
}
}
