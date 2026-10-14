/* Evolution Scale Project
 * COPYRIGHT (c) 2003.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef READSOFTKEYPARAMS_H
#define READSOFTKEYPARAMS_H

#include "evolution.h"
#include "HfcObject.h"
#include "HobString.h"


typedef enum ReadSoftKeyParamsFieldOffsets
{
	fieldOffsetRSKPDataType = 1,
	fieldOffsetRSKPRecordNum,
	endReadSoftKeyParamsFieldOffsets
} ReadSoftKeyParamsFieldOffsets_t;


class ReadSoftKeyParams : public HfcObject
{
	public:

		ReadSoftKeyParams();
		ReadSoftKeyParams(const char *data, int len, int formatType);

		~ReadSoftKeyParams();
		
		short getDataType() const;
		void setDataType(short type);

		HobRecordNum getRecordNum() const;
		void setRecordNum(HobRecordNum num);


		const char *serialize(int formatType) const;
		

	private:

		// See PresentationMethod.h for definition of these data types; see Type_... in the .h file.
		short dataType;
		HobRecordNum recordNum;

		void init();

#ifdef USE_ACCOUNTANT
		static int dataType_;
		static std::string dataTypeName_;
#endif
};

#endif //READSOFTKEYPARAMS_H
