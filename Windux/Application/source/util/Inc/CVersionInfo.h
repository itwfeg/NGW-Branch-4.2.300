// VersionInfo.h: interface for the CVersionInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VERSIONINFO_H__259DB379_978D_4A13_858F_616E54D098A1__INCLUDED_)
#define AFX_VERSIONINFO_H__259DB379_978D_4A13_858F_616E54D098A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
namespace Util
{
    class CVersionInfo
    {
        public:
            CVersionInfo(const wchar_t * pname);
            virtual ~CVersionInfo();
            const std::wstring & Version(){ return m_Version;}
            const std::wstring & SpecialBuildDescription(){ return m_SpecialBuildDescription;}
        private:

#ifdef LINUX_FIX        // reordered to squelch a gcc warning
            std::wstring m_Version;
            std::wstring m_SpecialBuildDescription;
#else
            std::wstring m_SpecialBuildDescription;
            std::wstring m_Version;
#endif

#ifdef USE_ACCOUNTANT
            static int dataType;
            static std::string dataTypeName;
#endif

    };
}
#endif // !defined(AFX_VERSIONINFO_H__259DB379_978D_4A13_858F_616E54D098A1__INCLUDED_)
