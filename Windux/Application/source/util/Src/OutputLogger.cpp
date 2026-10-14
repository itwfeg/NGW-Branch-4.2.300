// OutputLogger.cpp: implementation of the COutputLogger class.
//
//////////////////////////////////////////////////////////////////////
#include <PAL_platformheader.h>

#if defined ( _WIN32 )
#pragma warning ( disable: 4786)
#pragma warning ( disable: 4073)
#pragma init_seg(lib)   // The static Error logger should be destroyed AFTER the
                        // user segment exits
#endif      // _WIN32

#include "OutputLogger.h"
#include <stdlib.h>
#include "MsgTime.h"
#include "CSmartPtr.h"
#include "utilunicode.h"
#include "ErrorHandling.h"
#include "CFile.h"
#include <sstream>
#include <algorithm>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace Util
{

    CSmartPtr<COutputLogger> COutputLogger::mp_ErrorLogger(NULL);
    bool COutputLogger::mb_BlockErrorLogRedirection(false);
    Util::CSmartCriticalSection COutputLogger::ms_CritSec;

    COutputLogger & COutputLogger::GetErrorLogger(const wchar_t * pbuff,
            const wchar_t * pheader,
            bool ibBlockRedirection)
    {
        try
        {
            Util::CLockCriticalSection l_lock(ms_CritSec);
            if( !mb_BlockErrorLogRedirection)
                mb_BlockErrorLogRedirection = ibBlockRedirection;
            if( mp_ErrorLogger == NULL || ( (pbuff != NULL) && !mb_BlockErrorLogRedirection))
            {
                if( pbuff != NULL && mp_ErrorLogger != NULL)
                {
                    delete mp_ErrorLogger.Release();
                }

                wchar_t wbuff[_MAX_PATH];

                // if pbuff is NULL then use the current directory.
                if( NULL == pbuff)
                    PAL_absclass::PAL_getCurrentDirectoryW( _MAX_PATH, wbuff);
                else
                    wcsncpy(wbuff,pbuff,_MAX_PATH);

                std::wstring wstr(wbuff);
#if defined(LINUX_FIX)
                wstr += _T(PATH_SEPARATOR);
#else
                wstr += L"\\";
#endif
                //PAL_absclass::PAL_OutputDbgStr(wstr.c_str());
                //PAL_absclass::PAL_OutputDbgStr(L"\n");
                mp_ErrorLogger = new COutputLogger(wstr,(NULL == pheader) ? L"Error" : pheader);
                mp_ErrorLogger->Enable();
            }
            return *(mp_ErrorLogger.get());
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    // Function name    : COutputLogger::COutputLogger
    // Description          :
    // Return type              :
    // Argument         : const size_t maxsize
    COutputLogger::COutputLogger(const size_t maxsize,const bool iInit) :
        mb_Enabled(false),
        m_MaximumSize(maxsize),
        m_Version(0)
    {
        if( !iInit)
            return;
        mwstr_Path    = L"";
        mwstr_Header = L"Log";
        mwstr_Type   = L"log";
        mp_TheFile = NULL;
        ConstructIdent();
        MakeNewFileName();
        CheckFileStatistics(); // this actually creates the file
    }

    COutputLogger::COutputLogger(const std::wstring & path,
            const std::wstring & header,
            const std::wstring * version ,
            const std::wstring * type   ,
            const size_t maxsize
            )
        : mb_Enabled(false),
        mwstr_Path(path) ,
        mwstr_Header(header),
        m_MaximumSize(maxsize),
        m_Version(0)

    {
        try{
            if(NULL == type)
                mwstr_Type = L"log";
            else
                mwstr_Type = *type;

            mp_TheFile = NULL;

            if( NULL == version)
            {
                ConstructIdent();

            }
            else
            {

                mwstr_Ident = *version;

            }
            // TODO What is the intent of this? I comment it out because
            // gcc complains
            //mwstr_Ident;
            MakeNewFileName();
            CheckFileStatistics(); // this actually creates the file
        }
        catch(...)
        {
#ifdef _DEBUG
            OutputDebugString(L"Failed to construct file : ");
            OutputDebugString(mwstr_FullFileName.c_str());
            OutputDebugString(L"\n");
#endif

        }
    }


    // Function name    : COutputLogger::DoesFileExist
    // Description      : Return true if the file "fullpath" exist, otherwise false.
    //                    TODO::This is a static function of general utility and might well
    //                    be moved to a Util utility function.
    // Return type      : bool
    // Argument         : const std::wstring & fullpath
    bool COutputLogger::DoesFileExist( const std::wstring & UNUSED(fullpath))
    {
#if defined( _WIN32 )
        WIN32_FIND_DATA Info;
        HANDLE handle=NULL;

        try
        {
            handle = ::FindFirstFile(fullpath.c_str(), &Info);
            bool retval = true;
            if (INVALID_HANDLE_VALUE == handle)
                retval = false;;

            ::FindClose(handle);

            return retval;

        }
        catch(...) // any exception here is trouble...
        {

            ::FindClose(handle);
            LogCatchAndThrow;
        }
#else
        // TODO add FindFirstFile and FindClose to the PAL
        return false;
#endif
    }

    bool COutputLogger::Flush()
    {


        try
        {
            return mp_TheFile->Flush();

        }
        catch(Util::CBaseException & e_base)
        {
            Util::COutputLogger::GetErrorLogger().ReportToErrorLog(e_base.ExceptionMsg().c_str(),L" Util::CBaseException : ");
            LogCatchAndThrow;
        }
        catch(std::exception & e_std)
        {
            Util::COutputLogger::GetErrorLogger().ReportToErrorLog(e_std.what()," std::exception : ");
            LogCatchAndThrow;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    const std::wstring & COutputLogger::GetFullFileName()
    {
        return mwstr_FullFileName;

    }

    // Function name    : COutputLogger::~COutputLogger
    // Description          :
    // Return type              :
    COutputLogger::~COutputLogger()
    {
        CLockCriticalSection lockit(m_CritSec);
        // remove this logger from the list.

        if( mp_TheFile == NULL)
            return;
        if( false == (*mp_TheFile == INVALID_HANDLE_VALUE) )
            mp_TheFile->Close();
    }


    void COutputLogger::Close()
    {
        CLockCriticalSection lockit(m_CritSec);
        if( mp_TheFile == NULL)
            return;
        if( false == (*mp_TheFile == INVALID_HANDLE_VALUE))
            mp_TheFile->Close();
        mp_TheFile = NULL;
    }

    void  COutputLogger::ConstructFileName(std::wstring & wstr, UINT32 version)
    {

        wchar_t wbuff[40];
#if defined( _WIN32 )
        wsprintf(wbuff,L"_%0u",version);
#else
        swprintf(wbuff, 40, L"_%0u",version);
#endif
        wstr =  mwstr_Path;
        wstr += mwstr_Header;
        wstr += mwstr_Ident;
        wstr += wbuff;
        wstr += L".";
        wstr += mwstr_Type;

    }
    // Function name    : COutputLogger::ConstructVersionName
    // Description          : Default version of ContructVersionName, "ddMMyyHHmmSS" ie a timestamp.
    //                    This is a virutal function, so you can override to do what you want.
    // Return type              : void
    // Modified         :  HZ Apr 19, 2005  -- use ST time instead of DST time
    void COutputLogger::ConstructIdent()
    {
        std::wstring ident;

        CMSGTime curTime = CMSGTime::GetTheCurrentSTTime();
#if defined(LINUX_FIX)
        ident = curTime.FormatDate(PAL_DATE_FORMAT_DDMMYY_LOG)
            + curTime.FormatTime(PAL_TIME_FORMAT_HHMMSS_LOG);
#else
        ident =     curTime.FormatDate(_T( "ddMMyy"))
            + curTime.FormatTime(_T("HHmmss"));
#endif
        // make sure we are not duplicating the previous filename.
        while( ident == mwstr_Ident)
        {
            PAL_absclass::PAL_sleep(1000);
            curTime = CMSGTime::GetTheCurrentSTTime();
#if defined(LINUX_FIX)
            ident   = curTime.FormatDate(PAL_DATE_FORMAT_DDMMYY_LOG)
                + curTime.FormatTime(PAL_TIME_FORMAT_HHMMSS_LOG);
#else
            ident   = curTime.FormatDate(_T( "ddMMyy"))
                + curTime.FormatTime(_T("HHmmss"));
#endif
        }

        mwstr_Ident =  ident;
        m_LastIdentTime = CMSGTime::GetTheCurrentSTTime();

    };
    // Function name    : CSerialPortService::ReportToErrorLog
    // Description          : Customized report to Error Log
    // Return type              : void
    // Argument         : wchar_t * pbuff
    // Modified         :  HZ Apr 19, 2005  -- use ST time instead of DST time
    void COutputLogger::ReportToErrorLog(const wchar_t * pbuff , const wchar_t * header)
    {
        try
        {
            Util::COutputLogger & errlog
                = GetErrorLogger();

            std::wstring wstr;
            Util::CMSGTime theTime = Util::CMSGTime::GetTheCurrentSTTime();
            wstr = theTime.GetSQLDATETIME().c_str();
            wstr += L" : ";
            if( header != NULL)
                wstr +=  header;
            wstr += pbuff;
            wstr += L"\x0d\x0a";
            errlog.WriteWideStr(wstr);
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    // Function name    : CSerialPortService::ReportToErrorLog
    // Description          : Customized report to Error Log,
    // Return type              : void
    // Argument         : char * pbuff
    // Modified         :  HZ Apr 19, 2005  -- use ST time instead of DST time
    void COutputLogger::ReportToErrorLog(const char * pbuff , const char * header)
    {
        try
        {
            Util::COutputLogger & errlog
                = GetErrorLogger();

            std::string wstr;
            Util::CMSGTime theTime = Util::CMSGTime::GetTheCurrentSTTime();
            Util::wchartostring(wstr,theTime.GetSQLDATETIME().c_str());

            wstr += " : ";
            if( header != NULL)
                wstr +=  header;
            wstr += pbuff;
            wstr += "\x0d\x0a";
            errlog.WriteStr(wstr);
        }
        catch(...)
        {
            LogCatchAndThrow;
        }

    }

    // Function name    : COutputLogger::ReportToErrorLogF
    // Description          : Customized report to Error Log with sprintf type formatting
    // Return type              : void
    // Argument         : char * pbuff
    // Modified         :  HZ Apr 19, 2005  -- use ST time instead of DST time
    void COutputLogger::ReportToErrorLogF( const wchar_t* header, const wchar_t *format, ... )
    {
		wchar_t buffer[256];

		va_list args;
		va_start(args,format);
		vswprintf( buffer, 255, format, args );
		va_end(args);

		ReportToErrorLog( buffer, header );
	} 

    // Function name    : COutputLogger::ReportToErrorLogF
    // Description          : Customized report to Error Log with sprintf type formatting
    // Return type              : void
    // Argument         : char * pbuff
    // Modified         :  HZ Apr 19, 2005  -- use ST time instead of DST time
    void COutputLogger::ReportToErrorLogF( const char* header, const char* format, ... )
    {
		char buffer[256];

		va_list args;
		va_start(args,format);
		vsnprintf( buffer, 256, format, args );
		va_end(args);

		ReportToErrorLog( buffer, header );
	} 

	// Function name    : COutputLogger::SetHeader
    // Description          :
    // Return type              : void
    // Argument         : const std::wstring &wstr
    void COutputLogger::SetHeader(const std::wstring &wstr)
    {
        CLockCriticalSection lockit(m_CritSec);
        mwstr_Header = wstr;
        m_Version = 0;
        Close();
        MakeNewFileName();
        CheckFileStatistics();
    }

    // Function name    : COutputLogger::SetPath
    // Description          :
    // Return type              : void
    // Argument         : const std::wstring & wstr
    void COutputLogger::SetPath( const std::wstring & wstr)
    {
        CLockCriticalSection lockit(m_CritSec);
        m_Version = 0;
        mwstr_Path = wstr;
        Close();
        MakeNewFileName();
        CheckFileStatistics();
    }

    // Function name    : COutputLogger::SetIdent
    // Description          :
    // Return type              : void
    // Argument         : const std::wstring & wstr
    void COutputLogger::SetIdent( const std::wstring & wstr)
    {
        CLockCriticalSection lockit(m_CritSec);
        if( wstr != mwstr_Ident)
        {
            m_Version = 0;
            mwstr_Ident = wstr;
            Close();
            MakeNewFileName();
            CheckFileStatistics();
        }
    }

    // Function name    : void
    // Description          :
    // Return type              : COutputLogger::SetType
    // Argument         : const std::wstring & wstr
    void COutputLogger::SetType ( const std::wstring & wstr)
    {
        CLockCriticalSection lockit(m_CritSec);
        if( wstr != mwstr_Type)
        {
            mwstr_Type = wstr;
            Close();
            MakeNewFileName();
            CheckFileStatistics();
        }
    }


    // Function name    : COutputLogger::AppendBytes
    // Description          :
    // Return type              : size_t
    // Argument         :  BYTE * pBytes
    // Argument         : size_t size
    size_t COutputLogger::AppendBytes( const unsigned char * pBytes, size_t size)
    {
        CLockCriticalSection lockit(m_CritSec);
        if( false == mb_Enabled)
            return 0;
        if( mp_TheFile == INVALID_HANDLE_VALUE)
            return 0;

        size_t ret =    mp_TheFile->Write(pBytes, (int)size);
        //       mp_TheFile->Flush();
        return ret;

    };

    void COutputLogger::Clean()
    {
        CLockCriticalSection lockit(m_CritSec);
        try
        {
            if( mp_TheFile != NULL) // check if we might need to close something
            {
                if( false == (*mp_TheFile == INVALID_HANDLE_VALUE))
                    mp_TheFile->Close();
            }
            OutputLoggerVersionSetType::const_iterator citer = m_VersionSet.begin();
            while( citer != m_VersionSet.end() )
            {
                std::wstring wname = GetVersionFileName(*citer);
#if defined(LINUX_FIX)
                Assert( TRUE == PAL_absclass::PAL_deleteFile(wname.c_str()) , Util::CPlatformException());
#else
                Assert( TRUE == PAL_absclass::PAL_deleteFile(wname.c_str()) , Util::CNTException());
#endif
                citer++;

            }
            m_VersionSet.clear();
            m_Version = 0;
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    // Function name    : COutputLogger::CheckFileStatistics
    // Description          : Default version, uses the size of the output file
    //                                    to determine if a new file needs to be created.
    // Return type              : bool
    bool COutputLogger::CheckFileStatistics(size_t incr)
    {

        try
        {
            CLockCriticalSection lockit(m_CritSec);


            if(  mp_TheFile == NULL)
            {
                // Check if the current filename exists
                if( true == DoesFileExist(mwstr_FullFileName))
                    MakeNewFileName();
                CreateFile();

            }
            // If it does exist how big is it?
            else if( (mp_TheFile->FileSize()+incr) >= m_MaximumSize)
            {
                // if it is too big, then create a new m_Ident
                mp_TheFile->Close();
                MakeNewFileName();
                CreateFile();

            }
        }
        catch(...)
        {
            LogCatchAndThrow;
            return false;
        }

        return true;
    };

    void  COutputLogger::CreateFile()
    {
        CFileInfo   fileinfo(mwstr_FullFileName);
        mp_TheFile = new CFile(fileinfo);
        // the version is always one ahead
        m_VersionSet.insert( m_Version -1);
        WriteModuleStamp();
    }

    // Function name    : COutputLogger::GetVersionFileName
    // Description          : Allows the user to get the filename associated with version
    //                                    returns L"" if the version is non-existent
    // Return type              : std::wstring
    // Argument         : unsigned __int32 version
    std::wstring COutputLogger::GetVersionFileName( UINT32 version)
    {
        // The version must be in the set

        if( m_VersionSet.end() == m_VersionSet.find(version))
            return L"";
        std::wstring wstr;
        ConstructFileName(wstr,version);
        return wstr;


    }
    size_t COutputLogger::WriteByteBuffer( const unsigned char * puchar,size_t size,bool)
    {

        try
        {
            CLockCriticalSection lockit(m_CritSec);

            size_t retval;
            if( 0 == size)
                return 0;
            CheckFileStatistics(size);
            retval = AppendBytes(puchar,size);

            return retval;

        }
        catch(...)
        {
            // swallow this, hopefully it was not the error logger
            // that went down!
            LogCatch;
            return 0;
        }
        return 0;
    }

    // Function name    : COutputLogger::WriteWideStr
    // Description          :
    // Return type              : bool
    // Argument         : const std::wstring & wstr
    // Argument         : bool addLineFeed
    // Argument         : bool asAnsi
    size_t COutputLogger::WriteWideStr( const std::wstring & wstr,
            bool addLineFeed,
            bool asAnsi,
            bool
            )
    {

        try
        {
            CLockCriticalSection lockit(m_CritSec);

            size_t size = wstr.size();
            size_t retval;
            if( 0 == size)
                return 0;
            CheckFileStatistics(size);
            if( asAnsi)
            {
#if defined ( __linux__ )
                char *pChar = new char[size + 1];
                PAL_absclass::PAL_WC2MB(CP_ACP, 0, wstr.c_str(), -1, pChar, size+1, NULL, NULL);
                retval = AppendBytes((unsigned char *)pChar,size);
#else
                CSmartPtr<char> pChar = widetocharptr(wstr.c_str());
                retval = AppendBytes((unsigned char *)pChar.get(),size);
#endif      // __linux__
                if(addLineFeed)
                {
                    unsigned char * lf = (unsigned char *)"\r\n";
                    retval += AppendBytes((unsigned char *)lf,2);
                }
#if defined ( __linux__ )
                delete [] pChar;
#endif
            }
            else
            {
                unsigned char * puc = (unsigned char *)wstr.c_str();
                retval = AppendBytes(puc,sizeof(wchar_t)*size);
#ifndef LINUX_FIX
                // TODO
                if(addLineFeed)
                {
                    wchar_t * lf = L"\x0d\x0a";
                    retval += AppendBytes((unsigned char *)lf,4);
                }
#endif
            }
            return retval;

        }
        catch(...)
        {
            // swallow and report, hopefully not the error logger
            LogCatch;
            return 0;
        }
        return 0;
    }

    // Function name    : COutputLogger::WriteStr
    // Description          :
    // Return type              : bool
    // Argument         : const std::string & wstr
    // Argument         : bool addLineFeed
    // Argument         : bool asAnsi
    size_t COutputLogger::WriteStr( const std::string & wstr,
            bool addLineFeed,
            bool asAnsi
            ,bool)
    {

        try
        {
            CLockCriticalSection lockit(m_CritSec);

            size_t size = wstr.size();
            size_t retval;
            if( 0 == size)
                return 0;
            CheckFileStatistics(size);
            if( asAnsi)
            {

                retval = AppendBytes((unsigned char *)wstr.c_str(),size);
                if(addLineFeed)
                {
#if defined ( __linux__ )
                    unsigned char * lf = (unsigned char *)"\n";
                    retval += AppendBytes((unsigned char *)lf,1);
#else
                    unsigned char * lf = (unsigned char *)"\r\n";
                    retval += AppendBytes((unsigned char *)lf,2);
#endif
                }
            }
            else
            {
                unsigned char * puc = (unsigned char *)wstr.c_str();
                retval = AppendBytes(puc,sizeof(char)*size);
                if(addLineFeed)
                {
#if defined ( __linux__ )
                    unsigned char * lf = (unsigned char *)"\n";
                    retval += AppendBytes((unsigned char *)lf,1);
#else
                    char * lf = "\x0d\x0a";
                    retval += AppendBytes((unsigned char *)lf,2);
#endif
                }
            }
            return retval;

        }
        catch(...)
        {
            // swallow and report, hopefully not the error logger
            LogCatch;
            return 0;
        }
        return 0;
    }


    // Function name    : COutputLogger::MakeNewFileName
    // Description          :
    // Return type              : void
    void COutputLogger::MakeNewFileName()
    {
        ConstructFileName(mwstr_FullFileName,m_Version);
        // If the file already exists, then loop until it does not
        while( true == DoesFileExist( mwstr_FullFileName))
        {
            // sleep for 1 sec to get a different ident.
            PAL_absclass::PAL_sleep(1000);
            ConstructIdent();
            ConstructFileName(mwstr_FullFileName,m_Version);
        }
        m_Version++;
    }

    // Function name    : COutputLogger::GetImageFile
    // Description          :
    // Return type              : bool
    // Argument         : std::wstring & image
    bool COutputLogger::GetImageFile(std::wstring & image)
    {
#if defined ( __linux__ )
        // there is not a good way to get this under linux
        image = L"LINUX";
        return true;
#else
        try
        {
            PAL_HMODULE hmod = PAL_absclass::PAL_getModuleHandle(NULL);
            image = L"";
#if defined(LINUX_FIX)
            Assert(hmod != NULL,Util::CPlatformException());
#else
            Assert(hmod != NULL,Util::CNTException());
#endif
            wchar_t wbuff[_MAX_PATH+1];
#if defined(LINUX_FIX)
            Assert( 0 != PAL_absclass::PAL_getModuleFileName(hmod,wbuff,_MAX_PATH) , Util::CPlatformException());
#else
            Assert( 0 != ::GetModuleFileName(hmod,wbuff,_MAX_PATH) , Util::CNTException());
#endif
            image = wbuff;

            // this appeared to have been needed $$TRS
            return true;

        }
        catch(...)
        {

        }
        return false;
#endif
    }


    // Function name    : COutputLogger::GetImageFile
    // Description      :
    // Return type      : bool
    // Argument         : std::wstring & image
    bool COutputLogger::GetImageFile(std::string & image)
    {
#if defined ( __linux__ )
        // there is not a good way to get this under linux
        image = "LINUX";
        return true;
#else
        try
        {
            std::wstring l_string;
            GetImageFile(l_string);
            Util::wchartostring(image,l_string.c_str());
        }
        catch(...)
        {

        }
        return false;
#endif      // __linux__
    }

    void COutputLogger::HexDump(const std::string & iHeader,const std::string & iStream,const size_t iNumColumns)
    {
        try
        {
            std::ostringstream l_stream;
            l_stream.width(6);
            l_stream.fill('0');
            size_t l_num_columns(iNumColumns);
            l_stream  << std::hex << std::right << std::uppercase ;
            size_t l_num_written=0;
            std::string::const_iterator l_citer = iStream.begin() ;
            for( size_t i = 0; i < iStream.size()  ; i++)
            {
                if( (l_num_written % l_num_columns) == 0)
                {

                    if( l_num_written)
                        l_stream << "\r\n";
                    l_stream.width(6);
                    l_stream.fill('0');

                    l_stream << std::dec << i << " : " << std::hex << std::right ;

                }
                l_stream.width(2);
                l_stream.fill('0');

                l_stream << (UINT8)iStream[i] << "  ";
                l_num_written++;
            }
            WriteStr(iHeader,true);
            WriteStr(l_stream.str(),true);
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

    void COutputLogger::WriteModuleStamp()
    {
        try
        {
            std::string l_module_name;
            GetImageFile(l_module_name);
            std::string l_out("-- This file was created by : ");
            l_out += l_module_name;
            l_out += "\r\n";
            if( mp_TheFile == INVALID_HANDLE_VALUE)
                return;

            mp_TheFile->Write((const unsigned char *)l_out.c_str(),(int)l_out.size());
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
    }

} // end namespace Util
