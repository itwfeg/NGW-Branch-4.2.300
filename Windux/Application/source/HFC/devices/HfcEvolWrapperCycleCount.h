#ifndef HFCEVOLWRAPPERCYCLECOUNT_H
#define HFCEVOLWRAPPERCYCLECOUNT_H

#include "EvolWrapperCycleCount.h"
#include "HfcObject.h"

typedef enum HfcEvolWrapperCycleCountFieldOffsets
{
	fieldOffsetHfcEvolCycleCount = 1,
	endHfcEvolCycleCountFieldOffsets	//	MUST be last!
} HfcEvolWrapperCycleCountFieldOffsets_t;

class HfcEvolWrapperCycleCount : public EvolWrapperCycleCount, public HfcObject
{
	public:
		HfcEvolWrapperCycleCount(const char *data);

		~HfcEvolWrapperCycleCount();

		void show();

		virtual const char *serialize(int format = BrowserFormat)const;


	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};




#endif
