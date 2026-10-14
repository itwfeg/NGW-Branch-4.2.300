/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCGCDDEVFILEINFO_H
#define HFCGCDDEVFILEINFO_H

#include "evolution.h"
#include "HfcObject.h"

class HfcGCDDevFileInfo : public HfcObject
{
	public:

		HfcGCDDevFileInfo(const char *pData, int length);

		~HfcGCDDevFileInfo();

		LPCSTR getPath() const { return m_szPath; }
		DWORD getFileSize() const { return m_dwFileSize; }
		DWORD getLastWriteTime() const { return m_dwLastWriteTime; }

private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

		char m_szPath[MAX_PATH];
		DWORD m_dwFileSize;
		DWORD m_dwLastWriteTime;

		static const DWORD FIELDINDEX_FILEINFO_PATH = 0;
		static const DWORD FIELDINDEX_FILEINFO_SIZE = 1;
		static const DWORD FIELDINDEX_FILEINFO_LASTWRITETIME = 2;
};

#endif	//HFCGCDDEVSTATUS_H
