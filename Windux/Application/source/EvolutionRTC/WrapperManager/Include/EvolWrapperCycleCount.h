#ifndef EVOLWRAPPERCYCLECOUNT
#define EVOLWRAPPERCYCLECOUNT

#include "Wrapper.h"
#include "WrapperGeneric.h"
#include <string>

class HobThread;

class EvolWrapperCycleCount : public WrapperGeneric
{
	public:
	
		EvolWrapperCycleCount(Hobart::CEvolMsgReceiver *pHobThread = NULL, ULONG cycleCount = 0);
		EvolWrapperCycleCount(const char *buffer);
		~EvolWrapperCycleCount();

		ULONG getWrapperCycleCount() const {return wrapperCycleCount_;}	
		void setWrapperCycleCount(ULONG wrapperCycleCount){wrapperCycleCount_ = wrapperCycleCount;}

		
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