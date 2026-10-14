#ifndef EVOLWRAPPERSYSTEMINFOERASE_H
#define EVOLWRAPPERSYSTEMINFOERASE_H

#include "WrapperGeneric.h"

class EvolWrapperSystemInfoErase : public WrapperGeneric
{
	public:
		EvolWrapperSystemInfoErase(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		EvolWrapperSystemInfoErase(const char *buffer);
		~EvolWrapperSystemInfoErase();

	private:

#ifdef USE_ACCOUNTANT
		static int _dataType_;
		static std::string _dataTypeName_;
#endif
};

#endif