#ifndef EVOLWRAPPERREQSYSTEMRESET_H
#define EVOLWRAPPERREQSYSTEMRESET_H

#include "WrapperGeneric.h"

class EvolWrapperReqSystemReset : public WrapperGeneric
{
	public:
		EvolWrapperReqSystemReset(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperReqSystemReset(const char *buffer);
		~EvolWrapperReqSystemReset();

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};
#endif