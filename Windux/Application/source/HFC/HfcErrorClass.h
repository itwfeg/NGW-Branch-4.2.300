/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCERRORCLASS_H
#define HFCERRORCLASS_H

#include "evolution.h"
#include "HfcObject.h"
#include "Class.h"
#include "HfcError.h"

typedef enum HfcErrorClassOffsets
{
	fieldOffsetHfcErrClassLinkStatus = endHfcErrorFieldOffsets,
	fieldOffsetHfcErrBadClass,
	fieldOffsetHfcErrBadReportingClass,
	endHfcErrorClassOffsets
} HfcErrorClassOffsets_t;

class HfcErrorClass : public HfcError
{
	public:

		HfcErrorClass();
		HfcErrorClass(long code, short style, HobRecordNum recordNum, long errorType, Class *errClass);

		~HfcErrorClass();

		virtual const char *serialize(int format) const;


	private:
		Class *errClass;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //HFCERRORCLASS_H




