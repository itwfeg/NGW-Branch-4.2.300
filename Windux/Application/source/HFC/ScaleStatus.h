/* Evolution Scale Project
 * COPYRIGHT (c) 2004.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef SCALESTATUS_H
#define SCALESTATUS_H

#include "evolution.h"
#include "HfcObject.h"
 
#include <list>

typedef enum ScaleStatusFieldOffsets
{
	fieldOffsetScaleCondition = 1,
	fieldOffsetScaleError,
	fieldOffsetScaleStatus,
	fieldOffsetStatusScaleType,
	fieldOffsetStatusScaleModel,
	endScaleStatusFieldOffsets
} ScaleStatusFieldOffsets_t;


class ScaleStatus : public HfcObject
{
	public:

		ScaleStatus();
		//ScaleStatus(const char *data, int len, int formatType);

		~ScaleStatus();

		virtual const char *serialize(int format) const;

		static void initFieldList();


	private:

		static std::list<ScaleStatusFieldOffsets> fieldList;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //SCALESTATUS_H
