/* Evolution Scale Project
 * COPYRIGHT (c) 2002.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef WRITESINGLEFIELD_H
#define WRITESINGLEFIELD_H

#include "evolution.h"
#include "HfcObject.h"


typedef enum WriteSingleFieldOffsets
{
	fieldOffsetWriteSingleFieldRecordNum = 1,
	fieldOffsetWriteSingleFieldCode,
	fieldOffsetWriteSingleFieldNewValue,
	fieldOffsetWriteSingleFieldOrigValue,
	fieldOffsetWriteSingleFieldShouldAlsoPrint,
	endWriteSingleFieldOffsets
}   WriteSingleFieldOffsets_t;


class WriteSingleField : public HfcObject
{
	public:

		WriteSingleField();
		WriteSingleField(HobRecordNum recordNum, int fieldCode, HobString fieldNewValue, bool alsoPrint = false);
		WriteSingleField(const char *data, int len, int formatType);

		~WriteSingleField();

		HobRecordNum getRecordNum();
		void setRecordNum(HobRecordNum recordNum);

		int getFieldCode();
		void setFieldCode(int fieldCode);

		HobString getFieldNewValue();
		void setFieldNewValue(HobString fieldNewValue);

		HobString getFieldOrigValue();
		void setFieldOrigValue(HobString fieldOrigValue);

		bool isShouldAlsoPrint();
		void setShouldAlsoPrint(bool alsoPrint);

		virtual const char *serialize(int format) const;

		
	private:

		HobRecordNum recordNum;

		int fieldCode;

		HobString fieldNewValue;
		HobString fieldOrigValue;

		bool shouldAlsoPrint;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};

#endif //WRITESINGLEFIELD_H

