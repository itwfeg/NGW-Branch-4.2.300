#ifndef HFCEVOLWRAPPERERRORHISTORYLIST
#define HFCEVOLWRAPPERERRORHISTORYLIST
#include "EvolWrapperErrorHistory.h"
#include "HobList.h"	
#include "HfcObject.h"
#include <string.h>	


class HfcEvolWrapperErrorHistoryList : public HfcObject
{

	public:
			HfcEvolWrapperErrorHistoryList();
			~HfcEvolWrapperErrorHistoryList();
			

			void addToList(EvolWrapperErrorHistory *pEvolWrapperErrorHistory);
			
			void gotoFirstPage();
			void gotoNextPage();
			void gotoPreviousPage();

			virtual const char *serialize(int format = BrowserFormat)const;	
			
			static const unsigned int maxWrapperErrorsPerGroup_ = 10;		

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

			int currentSerializeOffset;
			int totalErrors;
};





#endif