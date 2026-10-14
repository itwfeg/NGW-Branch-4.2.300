#ifndef HFCCLATESTCOMPLETE_H
#define HFCCLATESTCOMPLETE_H

#include "CLATestComplete.h"
#include "HfcObject.h"
#include "evolution.h"

typedef enum HfcCLATestCompleteOffsets
{
	fieldOffsetTest = 1,
	fieldOffsetData1,
	fieldOffsetData2,
	fieldOffsetResult,
	fieldOffsetCLATestCompleteOffsetsEnd
} HfcCLATestCompleteOffsets_t;


class HfcCLATestComplete : public CLATestComplete, public HfcObject
{
	public:

		HfcCLATestComplete(const char *data);

		~HfcCLATestComplete();

		virtual const char *serialize(int format = BrowserFormat)const;

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};


#endif	//	HFCCLATESTCOMPLETE_H
