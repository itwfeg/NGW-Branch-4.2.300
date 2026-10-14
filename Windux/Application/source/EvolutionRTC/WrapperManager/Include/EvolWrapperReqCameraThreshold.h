#ifndef EVOLWRAPPERREQCAMERATHRESHOLD_H
#define EVOLWRAPPERREQCAMERATHRESHOLD_H

#include "WrapperGeneric.h"

class EvolWrapperReqCameraThreshold : public WrapperGeneric
{
	public:
		EvolWrapperReqCameraThreshold(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperReqCameraThreshold(const char *buffer);
		~EvolWrapperReqCameraThreshold();

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif