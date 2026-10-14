#ifndef EVOLWRAPPERREQCYCLECOUNT_H
#define EVOLWRAPPERREQCYCLECOUNT_H

#include "WrapperGeneric.h"

class EvolWrapperReqCycleCount : public WrapperGeneric
{
	public:
		EvolWrapperReqCycleCount(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperReqCycleCount(const char *buffer);
		~EvolWrapperReqCycleCount();

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};







#endif