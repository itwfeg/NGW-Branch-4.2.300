/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCERRORINDEXNUM_H
#define HFCERRORINDEXNUM_H

#include "evolution.h"
#include "HfcObject.h"
#include "HfcError.h"

class HfcErrorIndexNum : public HfcError
{
	public:

		HfcErrorIndexNum();
		HfcErrorIndexNum(long code, short style, HobRecordNum recordNum, long errorType, int indexNum);

		~HfcErrorIndexNum();

		virtual const char *serialize(int format) const;


	private:
		int indexNum;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCERRORINDEXNUM_H




