// Header file included by ANTLR UniLexer
#include "PAL_platformheader.h"

#if defined ( _WIN32 )
#include <tchar.h>
#else
#include <unistd.h>
//#endif
#endif      // _WIN32
#include "MsgTime.h"
#include "CFile.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

static bool exceptionHandling = false;

namespace Util
{
#ifdef USE_ACCOUNTANT
    int CFileInfo::dataType = 0;
    std::string CFileInfo::dataTypeName("CFileInfo");
#endif

    CFileInfo::CFileInfo(std::wstring i_FileName,
            DWORD i_CreationDistribution,
            DWORD i_DesiredAccess ,
            DWORD i_ShareMode,
            DWORD i_FlagsAndAttributes) :
        m_FileName(i_FileName),
        m_CreationDistribution(i_CreationDistribution),
        m_DesiredAccess(i_DesiredAccess),
        m_ShareMode(i_ShareMode),
        m_FlagsAndAttributes(i_FlagsAndAttributes)
    {
#ifdef USE_ACCOUNTANT
        if (dataType == 0)
            dataType = util_acc::registerDataType(dataTypeName);

        util_acc::addInstance(dataType);
#endif
    }

    CFileInfo::CFileInfo(CFileInfo& i_FileInfo) :
        m_FileName(i_FileInfo.GetFileName()),
        m_CreationDistribution(i_FileInfo.GetCreationDistribution()),
        m_DesiredAccess(i_FileInfo.GetDesiredAccess()),
        m_ShareMode(i_FileInfo.GetShareMode()),
        m_FlagsAndAttributes(i_FileInfo.GetFlagsAndAttributes())
    {
#ifdef USE_ACCOUNTANT
        if (dataType == 0)
            dataType = util_acc::registerDataType(dataTypeName);

        util_acc::addInstance(dataType);
#endif
    }

    CFileInfo::~CFileInfo()
    {
#ifdef USE_ACCOUNTANT
        util_acc::removeInstance(dataType);
#endif
    }

#ifdef USE_ACCOUNTANT
    int CFile::dataType = 0;
    std::string CFile::dataTypeName("CFile");
#endif

    CFile::CFile(CFileInfo& i_FileInfo) :
        m_FileInfo(i_FileInfo)
    {
#ifdef USE_ACCOUNTANT
        if (dataType == 0)
            dataType = util_acc::registerDataType(dataTypeName);

        util_acc::addInstance(dataType);
#endif
        if (m_FileInfo.GetFileName() == (std::wstring)_T(""))
        {
            m_FileHandle = INVALID_HANDLE_VALUE;
        }
        else
        {
            m_FileHandle = PAL_absclass::PAL_createFile((LPCTSTR)m_FileInfo.GetFileName().c_str(),
                    m_FileInfo.GetDesiredAccess(),
                    m_FileInfo.GetShareMode(),
                    NULL,
                    m_FileInfo.GetCreationDistribution(),
                    m_FileInfo.GetFlagsAndAttributes(),
                    NULL);

            // this can create an infinite loop as logger will create a new CFile and the process will repeat.
            if ( !exceptionHandling )
            {
                exceptionHandling = true;
                Assert(m_FileHandle != INVALID_HANDLE_VALUE,Util::CPlatformException());
                exceptionHandling = false;
            }
        }
    }

    CFile::CFile(CFile& i_File,HANDLE i_SourceProcessHandle)
    {
#ifdef USE_ACCOUNTANT
        if (dataType == 0)
            dataType = util_acc::registerDataType(dataTypeName);

        util_acc::addInstance(dataType);
#endif
        HANDLE l_TempHandle;
        BOOL l_Res;
        try
        {
            if (!(i_File == INVALID_HANDLE_VALUE))
            {
                if (m_FileHandle != INVALID_HANDLE_VALUE)
                {
                    PAL_absclass::PAL_closeHandle(m_FileHandle.Release());
                    m_FileInfo = CFileInfo((std::wstring) _T(""));
                }
                m_FileInfo = i_File.m_FileInfo;
                l_Res = PAL_absclass::PAL_dupHandle(i_SourceProcessHandle,
                        i_File.m_FileHandle.GetHandle(),
                        PAL_absclass::PAL_getCurrProc(),
                        &l_TempHandle,
                        NULL,
                        false,
                        DUPLICATE_SAME_ACCESS);

                // this can create an infinite loop as logger will create a new CFile and the process will repeat.
                if ( !exceptionHandling )
                {
                    exceptionHandling = true;
                    Assert(l_Res,Util::CPlatformException());
                    exceptionHandling = false;
                }

                m_FileHandle = l_TempHandle;
            }
        }
        catch(...)
        {
            throw;
        }
    }

    CFile::~CFile()
    {
#ifdef USE_ACCOUNTANT
        util_acc::removeInstance(dataType);
#endif
        PAL_absclass::PAL_closeHandle(m_FileHandle.Release());
    }

    void CFile::Open(CFileInfo& i_FileInfo)
    {
        try
        {
            m_FileInfo = i_FileInfo;
            if (m_FileInfo.GetFileName() == (std::wstring) _T(""))
            {
                m_FileHandle = INVALID_HANDLE_VALUE;
            }
            else
            {
                m_FileHandle = PAL_absclass::PAL_createFile((LPCSTR)m_FileInfo.GetFileName().c_str(),
                        m_FileInfo.GetDesiredAccess(),
                        m_FileInfo.GetShareMode(),
                        NULL,
                        m_FileInfo.GetCreationDistribution(),
                        m_FileInfo.GetFlagsAndAttributes(),
                        NULL);

                // this can create an infinite loop as logger will create a new CFile and the process will repeat.
                if ( !exceptionHandling )
                {
                    exceptionHandling = true;
                    Assert(m_FileHandle != INVALID_HANDLE_VALUE,Util::CPlatformException());
                    exceptionHandling = false;
                }
            }
        }
        catch(...)
        {
            throw ;
        }
    }

    void CFile::Close()
    {
        BOOL l_Res;
        try
        {
            if ( m_FileHandle != INVALID_HANDLE_VALUE )
            {
                l_Res = PAL_absclass::PAL_closeHandle(m_FileHandle.Release());
                m_FileHandle = INVALID_HANDLE_VALUE;
            }
            
            Assert(l_Res,Util::CPlatformException());
        }
        catch(...)
        {
            throw;
        }
    }

    // TODO: Should these be int functions? TRS
    int CFile::Write(const BYTE* i_Buffer, int i_Length)
    {
        DWORD l_AmountWritten;
        try
        {
            Assert(m_FileHandle != INVALID_HANDLE_VALUE,Util::CPlatformException());
            PAL_absclass::PAL_writeFile(m_FileHandle.GetHandle(),i_Buffer,i_Length,&l_AmountWritten,NULL);
            Assert((unsigned long)i_Length == l_AmountWritten,Util::CPlatformException());
            return (unsigned long)l_AmountWritten;
        }
        catch(...)
        {
            throw;
        }
    }


    int CFile::Read(BYTE* o_Buffer,int i_Length)
    {
        DWORD l_AmountRead;
        try
        {
            Assert(m_FileHandle != INVALID_HANDLE_VALUE,Util::CPlatformException());
            PAL_absclass::PAL_readFile(m_FileHandle.GetHandle(),o_Buffer,i_Length,&l_AmountRead,NULL);
            Assert((unsigned long)i_Length == l_AmountRead,Util::CPlatformException());
            return (unsigned long) l_AmountRead;
        }
        catch(...)
        {
            throw;
        }
    }


    int CFile::Peek(BYTE* o_Buffer,int i_Length)
    {
        DWORD l_AmountPeeked;
        try
        {
            Assert(m_FileHandle != INVALID_HANDLE_VALUE,Util::CPlatformException());
            PAL_absclass::PAL_readFile(m_FileHandle.GetHandle(),o_Buffer,i_Length,&l_AmountPeeked,NULL);
            SeekRelative(-i_Length);
            Assert((unsigned long)i_Length == l_AmountPeeked,Util::CPlatformException());
            return (unsigned long) l_AmountPeeked;
        }
        catch(...)
        {
            throw;
        }
    }

    void CFile::SeekRelative(DWORD i_Position)
    {
        enum E_SeekBadReturnCode
        {
            e_BadCode = 0xFFFFFFFF
        };

        DWORD dwPointer;

        try
        {
            Assert(m_FileHandle != INVALID_HANDLE_VALUE,Util::CPlatformException());
            dwPointer = PAL_absclass::PAL_setFilePointer(m_FileHandle.GetHandle(), i_Position,
                    NULL, PAL_FILE_CURRENT) ;
            Assert(dwPointer != e_BadCode,Util::CPlatformException());
        }
        catch(...)
        {
            throw;
        }
    }

    void CFile::SeekAbsolute(DWORD i_Position)
    {
        enum E_SeekBadReturnCode
        {
            e_BadCode = 0xFFFFFFFF
        };
        DWORD dwPointer;

        try
        {
            Assert(m_FileHandle != INVALID_HANDLE_VALUE,Util::CPlatformException());
            dwPointer = PAL_absclass::PAL_setFilePointer(m_FileHandle.GetHandle(), i_Position,
                    NULL, PAL_FILE_BEGIN) ;
            Assert(dwPointer != e_BadCode,Util::CPlatformException());
        }
        catch(...)
        {
            throw;
        }
    }


    // CJC       1/18/01 11:45:24 AM


    // Function name    : CFile::touch
    // Description          : touch the file.
    // Return type              : void
    void CFile::touch()
    {
        try
        {
            Assert(m_FileHandle.GetHandle() != INVALID_HANDLE_VALUE,Util::CPlatformException());
            // set the last write time.
            BOOL result = PAL_absclass::PAL_touch( m_FileHandle.GetHandle() );
            Assert(TRUE == result,Util::CPlatformException());
        }
        catch(...)
        {
            throw;
        }

    }
    // CJC ADDED Jan 16/2001
    // Note: We are not using Assert in release mode here

    // void CFile::Lock(DWORD UNUSED(offset), DWORD UNUSED(buffsize))
    void CFile::Lock(DWORD offset, DWORD UNUSED(buffsize))
    {
#if defined ( _WIN32 )
#ifndef _DEBUG // no errorlogging for release mode
        if( FALSE == ::LockFile(m_FileHandle,offset,0,buffsize,0) )
        {
            throw CPlatformException();
        }
#else
        Assert( TRUE == ::LockFile(m_FileHandle,offset,0,buffsize,0) ,
                CPlatformException());
#endif
#else       // ~ linux
#ifndef _DEBUG // no errorlogging for release mode
        // 0 on success , -1 on error
        if( 0 != PAL_absclass::PAL_lockF(m_FileHandle, F_TLOCK, offset))
        {
            throw CPlatformException();
        }
#else
        Assert( 0 != PAL_absclass::PAL_lockF(m_FileHandle, F_TLOCK, offset) ,
                CPlatformException());
#endif
#endif      // _WIN32
    }

    // void CFile::UnLock(DWORD UNUSED(offset),DWORD UNUSED(buffsize))
    void CFile::UnLock(DWORD offset,DWORD UNUSED(buffsize))
    {
#if defined ( _WIN32 )
#ifndef _DEBUG // no errorlogging for release mode
        if(FALSE == ::UnlockFile(m_FileHandle,offset,0,buffsize,0) )
        {
            throw CPlatformException();
        }
#else
        Assert( TRUE == ::UnlockFile(m_FileHandle,offset,0,buffsize,0) ,
                CPlatformException());

#endif
#else  // Linux
  #ifndef _DEBUG // no errorlogging for release mode
        if(0 != PAL_absclass::PAL_unlockF(m_FileHandle,F_ULOCK, offset) )
        {
            throw CPlatformException();
        }
  #else
        Assert( 0 != PAL_absclass::PAL_unlockF(m_FileHandle,F_ULOCK, offset) ,
                CPlatformException());
  #endif
#endif      // _WIN32
    }


    DWORD CFile::FileSize()
    {
        DWORD l_FileSize;
        try
        {
            Assert(m_FileHandle != INVALID_HANDLE_VALUE,Util::CPlatformException());
            l_FileSize = PAL_absclass::PAL_getFileSize(m_FileHandle.GetHandle(),NULL);
            Assert(l_FileSize != 0xFFFFFFFF,Util::CPlatformException());
        }
        catch(...)
        {
            LogCatchAndThrow;
        }
        return l_FileSize;
    }

} // end of the namespace
