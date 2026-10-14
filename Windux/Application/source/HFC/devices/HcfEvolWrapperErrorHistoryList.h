#ifndef HFCEVOLWRAPPERERRORHISTORYLIST
#define HFCEVOLWRAPPERERRORHISTORYLIST
#include "EvolWrapperErrorHistory.h"
#include "HobList.h"	
#include "HfcObject.h"
#include <string.h>	

class HfcEvolWrapperErrorHistoryList
{

	public:
			HfcEvolWrapperErrorHistoryList();
			~HfcEvolWrapperErrorHistoryList();
			

			void addToList(EvolWrapperErrorHistory *pEvolWrapperErrorHistory);
			
			const char *getSerializedList(int format = BrowserFormat) const;

	private:
			HobList	errorCodeList_;
			HobList	errorDescList_;
			HobList packageLengthList_;
			HobList packageWidthList_;
			HobList packageHeightList_;
			HobList filmUsedList_;
			HobList bestFitList_;
			HobList netStretchList_;
			HobList trayNameList_;
			HobList dateTimeList_;


};





#endif