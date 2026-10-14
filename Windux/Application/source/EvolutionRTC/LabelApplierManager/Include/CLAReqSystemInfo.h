#ifndef CLAREQSYSTEMINFO_H
#define CLAREQSYSTEMINFO_H

#include "LabelApplierGeneric.h"

class CLAReqSystemInfo : public LabelApplierGeneric
{
	public:
		CLAReqSystemInfo(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		CLAReqSystemInfo(const char *buffer);
		~CLAReqSystemInfo();
		
	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif