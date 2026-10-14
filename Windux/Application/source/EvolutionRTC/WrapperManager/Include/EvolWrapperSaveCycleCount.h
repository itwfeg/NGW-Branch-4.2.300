#ifndef EVOLWRAPPERSAVECYCLECOUNT
#define EVOLWRAPPERSAVECYCLECOUNT

#include <string>
#include "Wrapper.h"
#include "WrapperGeneric.h"

class EvolWrapperSaveCycleCount : public WrapperGeneric
{
	public:
	
		EvolWrapperSaveCycleCount(Hobart::CEvolMsgReceiver *pHobThread = NULL, ULONG cycleCount = 0);
		EvolWrapperSaveCycleCount(const char *buffer);
		~EvolWrapperSaveCycleCount();

		void setWrapperCycleCount(ULONG wrapperCycleCount){wrapperCycleCount_ = wrapperCycleCount;}
		ULONG getWrapperCycleCount()const {return wrapperCycleCount_;}
		const char *serialize()const;
		void show();

	private:
		ULONG wrapperCycleCount_;		

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};





#endif