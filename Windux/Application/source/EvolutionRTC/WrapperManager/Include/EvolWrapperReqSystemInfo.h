#ifndef EVOLWRAPPERREQSYSTEMINFO_H
#define EVOLWRAPPERREQSYSTEMINFO_H

#include "WrapperGeneric.h"

class EvolWrapperReqSystemInfo : public WrapperGeneric
{
	public:
		EvolWrapperReqSystemInfo(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperReqSystemInfo(const char *buffer);
		~EvolWrapperReqSystemInfo();

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};







#endif