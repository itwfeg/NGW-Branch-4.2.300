// Header file included by ANTLR UniLexer
#if defined(PAL_WIN32_RUN)
#include <tchar.h>
#endif
#ifndef Util_CFile_h
#define Util_CFile_h

/**************************************************************************
****   MODULE HEADER   ***

TYPE:			Concrete Type		



PURPOSE:		To allow access to an NT file



FUNCTION:			



INTERFACES:		See Below



RESOURCES:			



DATA:				



LOGIC:				



****  END MODULE HEADER   ***
**************************************************************************/


// System classes
#include <string>

//  classes
#include "SmartHandle.h"
#include "CReadWriteObject.h"

namespace Util
{
    // Pre-declaration
    class CFile;

    class CFileInfo
    {
        public:
            CFileInfo(std::wstring i_FileName = _T(""),
                    DWORD i_CreationDistribution = PAL_FILE_OPEN_CREATE, 
                    DWORD i_DesiredAccess = PAL_FILE_GEN_RD | PAL_FILE_GEN_WR , 
                    DWORD i_ShareMode = PAL_FILE_SHARE_RD, 
                    DWORD i_FlagsAndAttributes = PAL_FILE_ATTR_NORMAL
                    );

            CFileInfo(CFileInfo& i_FileInfo);

            // Do nothing as all will take care of themselves.
            virtual ~CFileInfo();

            void SetFileName(std::wstring& i_FileName)
            {m_FileName = i_FileName;}
            std::wstring& GetFileName()
            {return m_FileName;}

            void SetDesiredAccess(DWORD i_DesiredAccess = PAL_FILE_GEN_RD | PAL_FILE_GEN_WR )
            {i_DesiredAccess = m_DesiredAccess;}
            DWORD GetDesiredAccess()
            {return m_DesiredAccess;}

            void SetShareMode(DWORD i_ShareMode = PAL_FILE_SHARE_RD)
            {i_ShareMode = m_ShareMode;}
            DWORD GetShareMode()
            {return m_ShareMode;}


            void SetCreationDistribution(DWORD i_CreationDistribution = PAL_FILE_OPEN_CREATE )
            {i_CreationDistribution = m_CreationDistribution;}
            DWORD GetCreationDistribution()
            {return m_CreationDistribution;}


            void SetFlagsAndAttributes(DWORD i_FlagsAndAttributes = PAL_FILE_ATTR_NORMAL )
            {i_FlagsAndAttributes = m_FlagsAndAttributes;}
            DWORD GetFlagsAndAttributes()
            {return m_FlagsAndAttributes;}

        protected:
            std::wstring m_FileName;
            DWORD m_CreationDistribution; 
            DWORD m_DesiredAccess;
            DWORD m_ShareMode;
            DWORD m_FlagsAndAttributes;

        private:
#ifdef USE_ACCOUNTANT
            static int dataType;
            static std::string dataTypeName;
#endif
            friend class CFile;
    };
} // end of the namespace


namespace Util
{
    class CFile : public Util::CReadWriteObject
    {
        public:
            CFile(CFileInfo& i_FileInfo);

            CFile(CFile& i_File, HANDLE i_SourceProcessHandle = PAL_absclass::PAL_getCurrProc());

            virtual ~CFile();

            void Open(CFileInfo& i_FileInfo);
            void Close();
            bool Flush() { return true; };//(FALSE == ::FlushFileBuffers(m_FileHandle) ) ? false : true; };
            bool GetFileTime( 
                    PAL_FILETIME UNUSED(lpCreationTime),    // address of creation time
                    PAL_FILETIME UNUSED(lpLastAccessTime),  // address of last access time
                    PAL_FILETIME UNUSED(lpLastWriteTime)    // address of last write time
                    )
            { 
                UNREFERENCED_PARAMETER(lpLastWriteTime);
                UNREFERENCED_PARAMETER(lpLastAccessTime);
                UNREFERENCED_PARAMETER(lpCreationTime);

                return true; 
            };//(TRUE == ::GetFileTime(m_FileHandle,lpCreationTime, lpLastAccessTime, lpLastWriteTime) ) ? true : false; };

            void Lock(DWORD offset, DWORD buffsize);
            void UnLock(DWORD offset,DWORD buffsize);

            virtual int Write(const BYTE* i_Buffer, int i_Length);
            virtual int Read(BYTE* o_Buffer, int i_Length);
            virtual int Peek(BYTE* o_Buffer, int i_Length);

            void SeekRelative(DWORD i_Position);
            void SeekAbsolute(DWORD i_Position);
            void touch();
            bool operator == (HANDLE i_Handle)
            {return m_FileHandle == i_Handle;}

            DWORD FileSize();

            inline bool IsEmpty()
            {return FileSize() == 0;}

        private:
            CFileInfo m_FileInfo;
            CSmartHandle m_FileHandle;

#ifdef USE_ACCOUNTANT
            static int dataType;
            static std::string dataTypeName;
#endif
    };


}

#endif
