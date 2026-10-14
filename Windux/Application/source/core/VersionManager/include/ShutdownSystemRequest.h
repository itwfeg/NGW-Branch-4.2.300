#ifndef SHUTDOWNSYSTEMREQUEST_H
#define SHUTDOWNSYSTEMREQUEST_H
#include "EvolMessage.h"
#include "CEvolMsgReceiver.h"
//class HobThread;


class ShutdownSystemRequest : public EvolMessage
{
	public:
		ShutdownSystemRequest(bool backupDB, Hobart::CEvolMsgReceiver *pHobThread = NULL);
		ShutdownSystemRequest(const char *buffer);
		~ShutdownSystemRequest();

		bool getShouldBackupDB(){return backUpDB_;}

		const char *serialize() const;
		void show();

	private:
		bool backUpDB_;

#ifdef USE_ACCOUNTANT
		static int dataType;
		static std::string dataTypeName;
#endif
};



#endif	//SHUTDOWNSYSTEMREQUEST_H
