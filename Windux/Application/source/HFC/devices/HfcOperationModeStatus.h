/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef HFCOPERATIONMODESTATUS_H
#define HFCOPERATIONMODESTATUS_H

#include "evolution.h"
#include "HfcObject.h"
#include "OperationModeStatus.h"

class Session;

typedef enum HfcOperationModeStatusFieldOffsets
{
	fieldOffsetOpModeWrapperPressStart = 1,

	endHfcOperationModeStatusFieldOffsets		//	MUST be last!
} HfcOperationModeStatusFieldOffsets_t;

class HfcOperationModeStatus : public OperationModeStatus, public HfcObject
{
	public:
		HfcOperationModeStatus(const char *data);
		~HfcOperationModeStatus();

		virtual const char *serialize(int format = InternalFormat) const;

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif	//	HFCOPERATIONMODESTATUS_H
