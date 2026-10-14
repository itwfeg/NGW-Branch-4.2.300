/* Evolution Back-room Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCGCDDEVERROR_H
#define HFCGCDDEVERROR_H

#include "evolution.h"
#include "HfcObject.h"

class HfcGCDDevError : public HfcObject
{
	public:

		enum Code
		{
			Code_ApplicationUpdateCaughtException = 1,
			Code_PlaylistUpdateCaughtException,
			Code_TimerTickCaughtException,
			Code_DirectShowException,
			Code_PlaybackError
		};

		HfcGCDDevError(const char *pData, int length);

		~HfcGCDDevError();

		Code getCode() const { return m_code; }

		const std::string &getDescription() const { return m_description; }
		
		const std::string &getFileUrl() const { return m_url; }

	private:

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif

		Code m_code;
		std::string m_description;
		std::string m_url;

		static const DWORD FIELDINDEX_CODE = 0;
		static const DWORD FIELDINDEX_DESCRIPTION = 1;
		static const DWORD FIELDINDEX_FILE_URL = 2;
};

#endif	//HFCGCDDEVSTATUS_H
