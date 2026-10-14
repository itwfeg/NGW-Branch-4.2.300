#ifndef WRAPPERREQUESTSETTINGS_H
#define WRAPPERREQUESTSETTINGS_H



#include "WrapperGeneric.h"

#include "CEvolMsgReceiver.h"
//class HobThread;



class WrapperRequestPackageHistory : public WrapperGeneric
{
	public:
		WrapperRequestPackageHistory(Hobart::CEvolMsgReceiver *pHobThread = NULL);
		WrapperRequestPackageHistory(const char *buffer);
		~WrapperRequestPackageHistory();

	private:
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif // WRAPPERREQUESTSETTINGS_H
