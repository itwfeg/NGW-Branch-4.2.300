// OutputLogger.h: interface for the COutputLogger class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTPUTLOGGER_H__0B701EC3_59B4_11D4_BA59_00104B2205E1__INCLUDED_)
#define AFX_OUTPUTLOGGER_H__0B701EC3_59B4_11D4_BA59_00104B2205E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "MsgTime.h"
#include "CCriticalSection.h"
#include "CSmartPtr.h"
#include "CFile.h"
#include <set>






namespace Util
{
/*******************************************************
	class COutputLogger


*********************************************************/
class COutputLogger;

typedef std::set<COutputLogger *> OutputLoggerListType;
typedef std::set<unsigned __int32> OutputLoggerVersionSetType;

// one single object is shared by multiple threads
const size_t DEF_MAX_OUTPUTLOGGER_FILESIZE = 1400000;
class COutputLogger  
{
#if defined(LINUX_FIX)

#if defined(PAL_WIN32_RUN)
    #define PATH_SEPARATOR              "\\"

#elif   defined(__linux__)
    #define PATH_SEPARATOR              "/"
    #define __cdecl

#else
    #error  "define related VARIABLE"
#endif

#endif  // LINUX_FIX

    public:
        COutputLogger(const size_t maxsize = DEF_MAX_OUTPUTLOGGER_FILESIZE
                ,const bool iInit = true );

        COutputLogger(const std::wstring & path,
                const std::wstring & header,
                const std::wstring * version = NULL,
                const std::wstring * type    = NULL,
                const size_t maxsize = DEF_MAX_OUTPUTLOGGER_FILESIZE // fits on a floppy...
                );
        virtual ~COutputLogger();
        void HexDump(const std::string & iHeader,const std::string & iStream,const size_t iNumColumns);

        void Clean();
        void Close();
        //virtual std::wstring GetVersionFileName( unsigned __int32 version);
        virtual std::wstring GetVersionFileName( UINT32 version);
        static bool DoesFileExist( const std::wstring & fullpath);
        void SetHeader( const std::wstring & wstr);
        void SetIdent( const std::wstring & wstr);
        void SetType ( const std::wstring & wstr);
        void SetPath( const std::wstring & wstr);
        virtual size_t WriteWideStr( const std::wstring & wstr, 
                bool addLineFeed = false,
                bool asAnsi = true,
                bool bReb = false);
        virtual size_t WriteStr( const std::string & wstr, 
                bool addLineFeed = false,
                bool asAnsi = true,
                bool bReb = false);

        virtual size_t WriteByteBuffer( const unsigned char * puchar, size_t size, bool bReb=true);
        const std::wstring & GetFullFileName();
        const std::wstring & GetHeader(){ return mwstr_Header;}
        const std::wstring & GetIdent(){ return mwstr_Type;}
        const std::wstring & GetPath(){ return mwstr_Path;}
        const std::wstring & GetType(){ return mwstr_Type;}

        bool IsEnabled(){ return mb_Enabled;};
        bool Enable(){  mb_Enabled = true; return mb_Enabled;}
        bool Disable(){ mb_Enabled = false; return mb_Enabled;}
        const OutputLoggerVersionSetType & GetVersionSet(){ return m_VersionSet;}	
        // Sets up the error logger. It is static for the whole process
        static COutputLogger & GetErrorLogger(const wchar_t * pbuff = NULL, const wchar_t * pheader = NULL, bool ibBlockRedirection = false);
        // Use this methods to send output to the log
        static void ReportToErrorLog(const wchar_t * pbuff , const wchar_t * header);
        static void ReportToErrorLog(const char * pbuff , const char * header);
	static void __cdecl ReportToErrorLogF( const wchar_t* header, const wchar_t *format, ... );
	static void __cdecl ReportToErrorLogF( const char* header, const char* format, ... );
        static bool GetImageFile(std::wstring & image);
        static bool GetImageFile(std::string & image);
        bool Flush();

        static Util::CSmartPtr<COutputLogger>& GetErrorLogPtr(){ return mp_ErrorLogger;}
        static void SetErrorLogPtr( Util::CSmartPtr<COutputLogger> & ptr){ mp_ErrorLogger = ptr;}

    protected:
        void WriteModuleStamp();
        static CSmartPtr<COutputLogger> mp_ErrorLogger;
        static bool                     mb_BlockErrorLogRedirection;
        static Util::CSmartCriticalSection ms_CritSec;
        // data
        OutputLoggerVersionSetType m_VersionSet;
        CSmartPtr<CFile> mp_TheFile;
        CMSGTime m_LastIdentTime;
        CSmartCriticalSection m_CritSec;
        bool mb_Enabled;
        std::wstring mwstr_Path;
        std::wstring mwstr_Header;
        std::wstring mwstr_Ident;
        std::wstring mwstr_Type;
        std::wstring mwstr_FullFileName;
        size_t m_MaximumSize;
        //unsigned __int32 m_Version;
        UINT32 m_Version;

        // methods
        size_t AppendBytes(const unsigned char * pBytes, size_t size);
        // virtual methods
        virtual bool CheckFileStatistics(size_t incr = 0);
        virtual void ConstructIdent();
        virtual void ConstructFileName(std::wstring & wstr, UINT32 version);
        virtual void CreateFile();
        virtual void MakeNewFileName(); 

};




} // end namespace Util
#endif // !defined(AFX_OUTPUTLOGGER_H__0B701EC3_59B4_11D4_BA59_00104B2205E1__INCLUDED_)
