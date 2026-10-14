#ifndef WEIGHERCACHEDSTATUS_H
#define WEIGHERCACHEDSTATUS_H



#include "WeigherStatus.h"
#include "CEvolMsgReceiver.h"


//class HobThread;

class WeigherCachedStatus : public WeigherStatus
{
	public:
		WeigherCachedStatus(Hobart::CEvolMsgReceiver * pHobThread = NULL);
		WeigherCachedStatus(const char* buffer);
		~WeigherCachedStatus();

		void show();	

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif /* WEIGHERCACHEDSTATUS_H */
