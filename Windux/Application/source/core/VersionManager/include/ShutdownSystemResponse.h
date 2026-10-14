#ifndef SHUTDOWNSYSTEMRESPONSE_H
#define SHUTDOWNSYSTEMRESPONSE_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"
//class HobThread;
class HobVersion;


class ShutdownSystemResponse : public EvolMessage
{
	public:
		ShutdownSystemResponse(bool OKToShutdown, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		ShutdownSystemResponse(const char *buffer);
		~ShutdownSystemResponse();

		bool getOKToShutdown(){return OKToShutdown_;}

		const char *serialize() const;
		void show();

	private:
		bool OKToShutdown_;
#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//SHUTDOWNSYSTEMRESPONSE_H
