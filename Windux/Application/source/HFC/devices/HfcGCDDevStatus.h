/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCGCDDEVSTATUS_H
#define HFCGCDDEVSTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobVersion.h"

class HfcGCDDevStatus : public HfcObject
{
	public:

		HfcGCDDevStatus(const char *pData, int length);

		~HfcGCDDevStatus();

		bool getDDRTestPassed() const { return m_ddrTestPassed; }

		const HobVersion &getBoardVersion() const { return m_boardVersion; }

		const HobVersion &getWinCEVersion() const { return m_winCEVersion; }

		const HobVersion &getGcdMgrVersion() const { return m_gcdMgrVersion; }

		const HobVersion &getGcdDllVersion() const { return m_gcdDllVersion; }
		
	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

		bool       m_ddrTestPassed;
		HobVersion m_boardVersion;
		HobVersion m_winCEVersion;
		HobVersion m_gcdMgrVersion;
		HobVersion m_gcdDllVersion;

		static const DWORD FIELDINDEX_BOARD_MAJOR_VERSION = 0;
		static const DWORD FIELDINDEX_BOARD_MINOR_VERSION = 1;
		static const DWORD FIELDINDEX_WINCE_MAJOR_VERSION = 2;
		static const DWORD FIELDINDEX_WINCE_MINOR_VERSION = 3;
		static const DWORD FIELDINDEX_GCDDLL_MAJOR_VERSION = 4;
		static const DWORD FIELDINDEX_GCDDLL_MINOR_VERSION = 5;
		static const DWORD FIELDINDEX_GCDMGR_MAJOR_VERSION = 6;
		static const DWORD FIELDINDEX_GCDMGR_MINOR_VERSION = 7;
		static const DWORD FIELDINDEX_LANGUAGE = 8;
		static const DWORD FIELDINDEX_WEIGH_MODE = 9;
		static const DWORD FIELDINDEX_MEMORY_TEST_RESULT = 10;

		static LPCSTR SZ_MEMTSTPASS_TRUE;
};

#endif	//HFCGCDDEVSTATUS_H
