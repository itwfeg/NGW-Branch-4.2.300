/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCGCDDEVREQUESTFILEUPLOAD_H
#define HFCGCDDEVREQUESTFILEUPLOAD_H

#include "evolution.h"
#include "HfcObject.h"

class HfcGCDDevRequestFileUpload : public HfcObject
{
	public:

		HfcGCDDevRequestFileUpload(const char *pData, int length);

		~HfcGCDDevRequestFileUpload();

		LPCSTR getFilename() const { return m_szFilename; }
		LPCSTR getUploadPath() const { return m_szUploadPath; }

private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

		char m_szFilename[MAX_PATH];
		char m_szUploadPath[MAX_PATH];

		static const DWORD FIELDINDEX_REQUESTFILEUPLOAD_FILENAME = 0;
		static const DWORD FIELDINDEX_REQUESTFILEUPLOAD_UPLOADPATH = 1;
};

#endif	//HFCGCDDEVSTATUS_H
